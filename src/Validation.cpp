// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "graphqlservice/GraphQLGrammar.h"
#include "graphqlservice/IntrospectionSchema.h"

#include "Validation.h"

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std::literals;

namespace graphql::service {

bool ValidateArgumentVariable::operator==(const ValidateArgumentVariable& other) const
{
	return name == other.name;
}

bool ValidateArgumentEnumValue::operator==(const ValidateArgumentEnumValue& other) const
{
	return value == other.value;
}

bool ValidateArgumentValuePtr::operator==(const ValidateArgumentValuePtr& other) const
{
	return (!value ? !other.value : (other.value && value->data == other.value->data));
}

bool ValidateArgumentList::operator==(const ValidateArgumentList& other) const
{
	return values == other.values;
}

bool ValidateArgumentMap::operator==(const ValidateArgumentMap& other) const
{
	return values == other.values;
}

ValidateArgumentValue::ValidateArgumentValue(ValidateArgumentVariable&& value)
	: data(std::move(value))
{
}

ValidateArgumentValue::ValidateArgumentValue(response::IntType value)
	: data(value)
{
}

ValidateArgumentValue::ValidateArgumentValue(response::FloatType value)
	: data(value)
{
}

ValidateArgumentValue::ValidateArgumentValue(response::StringType&& value)
	: data(std::move(value))
{
}

ValidateArgumentValue::ValidateArgumentValue(response::BooleanType value)
	: data(value)
{
}

ValidateArgumentValue::ValidateArgumentValue(ValidateArgumentEnumValue&& value)
	: data(std::move(value))
{
}

ValidateArgumentValue::ValidateArgumentValue(ValidateArgumentList&& value)
	: data(std::move(value))
{
}

ValidateArgumentValue::ValidateArgumentValue(ValidateArgumentMap&& value)
	: data(std::move(value))
{
}

ValidateArgumentValueVisitor::ValidateArgumentValueVisitor(std::vector<schema_error>& errors)
	: _errors(errors)
{
}

ValidateArgumentValuePtr ValidateArgumentValueVisitor::getArgumentValue()
{
	auto result = std::move(_argumentValue);

	return result;
}

void ValidateArgumentValueVisitor::visit(const peg::ast_node& value)
{
	if (value.is_type<peg::variable_value>())
	{
		visitVariable(value);
	}
	else if (value.is_type<peg::integer_value>())
	{
		visitIntValue(value);
	}
	else if (value.is_type<peg::float_value>())
	{
		visitFloatValue(value);
	}
	else if (value.is_type<peg::string_value>())
	{
		visitStringValue(value);
	}
	else if (value.is_type<peg::true_keyword>() || value.is_type<peg::false_keyword>())
	{
		visitBooleanValue(value);
	}
	else if (value.is_type<peg::null_keyword>())
	{
		visitNullValue(value);
	}
	else if (value.is_type<peg::enum_value>())
	{
		visitEnumValue(value);
	}
	else if (value.is_type<peg::list_value>())
	{
		visitListValue(value);
	}
	else if (value.is_type<peg::object_value>())
	{
		visitObjectValue(value);
	}
}

void ValidateArgumentValueVisitor::visitVariable(const peg::ast_node& variable)
{
	ValidateArgumentVariable value { variable.string_view().substr(1) };
	auto position = variable.begin();

	_argumentValue.value = std::make_unique<ValidateArgumentValue>(std::move(value));
	_argumentValue.position = { position.line, position.column };
}

void ValidateArgumentValueVisitor::visitIntValue(const peg::ast_node& intValue)
{
	response::IntType value { std::atoi(intValue.string().c_str()) };
	auto position = intValue.begin();

	_argumentValue.value = std::make_unique<ValidateArgumentValue>(value);
	_argumentValue.position = { position.line, position.column };
}

void ValidateArgumentValueVisitor::visitFloatValue(const peg::ast_node& floatValue)
{
	response::FloatType value { std::atof(floatValue.string().c_str()) };
	auto position = floatValue.begin();

	_argumentValue.value = std::make_unique<ValidateArgumentValue>(value);
	_argumentValue.position = { position.line, position.column };
}

void ValidateArgumentValueVisitor::visitStringValue(const peg::ast_node& stringValue)
{
	response::StringType value { stringValue.unescaped };
	auto position = stringValue.begin();

	_argumentValue.value = std::make_unique<ValidateArgumentValue>(std::move(value));
	_argumentValue.position = { position.line, position.column };
}

void ValidateArgumentValueVisitor::visitBooleanValue(const peg::ast_node& booleanValue)
{
	response::BooleanType value { booleanValue.is_type<peg::true_keyword>() };
	auto position = booleanValue.begin();

	_argumentValue.value = std::make_unique<ValidateArgumentValue>(value);
	_argumentValue.position = { position.line, position.column };
}

void ValidateArgumentValueVisitor::visitNullValue(const peg::ast_node& nullValue)
{
	auto position = nullValue.begin();

	_argumentValue.value.reset();
	_argumentValue.position = { position.line, position.column };
}

void ValidateArgumentValueVisitor::visitEnumValue(const peg::ast_node& enumValue)
{
	ValidateArgumentEnumValue value { enumValue.string_view() };
	auto position = enumValue.begin();

	_argumentValue.value = std::make_unique<ValidateArgumentValue>(std::move(value));
	_argumentValue.position = { position.line, position.column };
}

void ValidateArgumentValueVisitor::visitListValue(const peg::ast_node& listValue)
{
	ValidateArgumentList value;
	auto position = listValue.begin();

	value.values.reserve(listValue.children.size());

	for (const auto& child : listValue.children)
	{
		ValidateArgumentValueVisitor visitor(_errors);

		visitor.visit(*child);
		value.values.emplace_back(visitor.getArgumentValue());
	}

	_argumentValue.value = std::make_unique<ValidateArgumentValue>(std::move(value));
	_argumentValue.position = { position.line, position.column };
}

void ValidateArgumentValueVisitor::visitObjectValue(const peg::ast_node& objectValue)
{
	ValidateArgumentMap value;
	auto position = objectValue.begin();

	for (const auto& field : objectValue.children)
	{
		auto name = field->children.front()->string_view();

		if (value.values.find(name) != value.values.end())
		{
			// http://spec.graphql.org/June2018/#sec-Input-Object-Field-Uniqueness
			auto position = field->begin();
			std::ostringstream message;

			message << "Conflicting input field name: " << name;

			_errors.push_back({ message.str(), { position.line, position.column } });
			continue;
		}

		ValidateArgumentValueVisitor visitor(_errors);

		visitor.visit(*field->children.back());
		value.values[std::move(name)] = visitor.getArgumentValue();
	}

	_argumentValue.value = std::make_unique<ValidateArgumentValue>(std::move(value));
	_argumentValue.position = { position.line, position.column };
}

ValidateField::ValidateField(std::string_view returnType,
	std::optional<std::string_view> objectType, std::string_view fieldName,
	ValidateFieldArguments&& arguments)
	: returnType(returnType)
	, objectType(objectType)
	, fieldName(fieldName)
	, arguments(std::move(arguments))
{
}

bool ValidateField::operator==(const ValidateField& other) const
{
	return returnType == other.returnType
		&& ((objectType && other.objectType && *objectType != *other.objectType)
			|| (fieldName == other.fieldName && arguments == other.arguments));
}

ValidateVariableTypeVisitor::ValidateVariableTypeVisitor(
	const schema::Schema& schema, const ValidateTypeKinds& typeKinds)
	: _schema(schema)
	, _typeKinds(typeKinds)
{
}

void ValidateVariableTypeVisitor::visit(const peg::ast_node& typeName)
{
	if (typeName.is_type<peg::nonnull_type>())
	{
		visitNonNullType(typeName);
	}
	else if (typeName.is_type<peg::list_type>())
	{
		visitListType(typeName);
	}
	else if (typeName.is_type<peg::named_type>())
	{
		visitNamedType(typeName);
	}
}

void ValidateVariableTypeVisitor::visitNamedType(const peg::ast_node& namedType)
{
	auto name = namedType.string_view();
	auto itrKind = _typeKinds.find(name);

	if (itrKind == _typeKinds.end())
	{
		return;
	}

	switch (itrKind->second)
	{
		case introspection::TypeKind::SCALAR:
		case introspection::TypeKind::ENUM:
		case introspection::TypeKind::INPUT_OBJECT:
			_isInputType = true;
			_variableType = _schema.LookupType(name);
			break;

		default:
			break;
	}
}

void ValidateVariableTypeVisitor::visitListType(const peg::ast_node& listType)
{
	ValidateVariableTypeVisitor visitor(_schema, _typeKinds);

	visitor.visit(*listType.children.front());
	_isInputType = visitor.isInputType();
	_variableType =
		std::make_shared<schema::WrapperType>(introspection::TypeKind::LIST, visitor.getType());
}

void ValidateVariableTypeVisitor::visitNonNullType(const peg::ast_node& nonNullType)
{
	ValidateVariableTypeVisitor visitor(_schema, _typeKinds);

	visitor.visit(*nonNullType.children.front());
	_isInputType = visitor.isInputType();
	_variableType =
		std::make_shared<schema::WrapperType>(introspection::TypeKind::NON_NULL, visitor.getType());
}

bool ValidateVariableTypeVisitor::isInputType() const
{
	return _isInputType;
}

ValidateType ValidateVariableTypeVisitor::getType()
{
	auto result = std::move(_variableType);

	return result;
}

ValidateExecutableVisitor::ValidateExecutableVisitor(const std::shared_ptr<schema::Schema>& schema)
	: _schema(schema)
{
	const auto& queryType = _schema->queryType();
	const auto& mutationType = _schema->mutationType();
	const auto& subscriptionType = _schema->subscriptionType();

	if (queryType)
	{
		_operationTypes[strQuery] = queryType->name();
	}

	if (mutationType)
	{
		_operationTypes[strMutation] = mutationType->name();
	}

	if (subscriptionType)
	{
		_operationTypes[strSubscription] = subscriptionType->name();
	}

	const auto& types = _schema->types();

	for (const auto& entry : types)
	{
		const auto name = entry.first;
		const auto kind = entry.second->kind();

		if (!isScalarType(kind))
		{
			if (kind == introspection::TypeKind::OBJECT)
			{
				_matchingTypes[name].insert(name);
			}
			else
			{
				const auto& possibleTypes = entry.second->possibleTypes();
				std::set<std::string_view> matchingTypes;

				for (const auto& possibleType : possibleTypes)
				{
					const auto spType = possibleType.lock();

					if (spType)
					{
						matchingTypes.insert(spType->name());
					}
				}

				if (!matchingTypes.empty())
				{
					_matchingTypes[name] = std::move(matchingTypes);
				}
			}
		}
		else if (kind == introspection::TypeKind::ENUM)
		{
			const auto& enumValues = entry.second->enumValues();
			std::set<std::string_view> values;

			for (const auto& value : enumValues)
			{
				if (value)
				{
					values.insert(value->name());
				}
			}

			if (!enumValues.empty())
			{
				_enumValues[name] = std::move(values);
			}
		}
		else if (kind == introspection::TypeKind::SCALAR)
		{
			_scalarTypes.insert(name);
		}

		_typeKinds[std::move(name)] = kind;
	}

	const auto& directives = _schema->directives();

	for (const auto& directive : directives)
	{
		const auto name = directive->name();
		const auto& locations = directive->locations();
		const auto& args = directive->args();
		ValidateDirective validateDirective;

		for (const auto location : locations)
		{
			validateDirective.locations.insert(location);
		}

		validateDirective.arguments = getArguments(args);
		_directives[name] = std::move(validateDirective);
	}
}

void ValidateExecutableVisitor::visit(const peg::ast_node& root)
{
	// Visit all of the fragment definitions and check for duplicates.
	peg::for_each_child<peg::fragment_definition>(root,
		[this](const peg::ast_node& fragmentDefinition) {
			const auto& fragmentName = fragmentDefinition.children.front();
			const auto inserted =
				_fragmentDefinitions.insert({ fragmentName->string_view(), fragmentDefinition });

			if (!inserted.second)
			{
				// http://spec.graphql.org/June2018/#sec-Fragment-Name-Uniqueness
				auto position = fragmentDefinition.begin();
				std::ostringstream error;

				error << "Duplicate fragment name: " << inserted.first->first;

				_errors.push_back({ error.str(), { position.line, position.column } });
			}
		});

	// Visit all of the operation definitions and check for duplicates.
	peg::for_each_child<
		peg::operation_definition>(root, [this](const peg::ast_node& operationDefinition) {
		std::string_view operationName;

		peg::on_first_child<peg::operation_name>(operationDefinition,
			[&operationName](const peg::ast_node& child) {
				operationName = child.string_view();
			});

		const auto inserted = _operationDefinitions.insert({ operationName, operationDefinition });

		if (!inserted.second)
		{
			// http://spec.graphql.org/June2018/#sec-Operation-Name-Uniqueness
			auto position = operationDefinition.begin();
			std::ostringstream error;

			error << "Duplicate operation name: " << inserted.first->first;

			_errors.push_back({ error.str(), { position.line, position.column } });
		}
	});

	// Check for lone anonymous operations.
	if (_operationDefinitions.size() > 1)
	{
		auto itr = std::find_if(_operationDefinitions.cbegin(),
			_operationDefinitions.cend(),
			[](const auto& entry) noexcept {
				return entry.first.empty();
			});

		if (itr != _operationDefinitions.cend())
		{
			// http://spec.graphql.org/June2018/#sec-Lone-Anonymous-Operation
			auto position = itr->second.begin();

			_errors.push_back(
				{ "Anonymous operation not alone", { position.line, position.column } });
		}
	}

	// Visit the executable definitions recursively.
	for (const auto& child : root.children)
	{
		if (child->is_type<peg::fragment_definition>())
		{
			visitFragmentDefinition(*child);
		}
		else if (child->is_type<peg::operation_definition>())
		{
			visitOperationDefinition(*child);
		}
		else
		{
			// http://spec.graphql.org/June2018/#sec-Executable-Definitions
			auto position = child->begin();

			_errors.push_back({ "Unexpected type definition", { position.line, position.column } });
		}
	}

	if (!_fragmentDefinitions.empty())
	{
		// http://spec.graphql.org/June2018/#sec-Fragments-Must-Be-Used
		const size_t originalSize = _errors.size();
		auto unreferencedFragments = std::move(_fragmentDefinitions);

		for (const auto& name : _referencedFragments)
		{
			unreferencedFragments.erase(name);
		}

		_errors.resize(originalSize + unreferencedFragments.size());
		std::transform(unreferencedFragments.cbegin(),
			unreferencedFragments.cend(),
			_errors.begin() + originalSize,
			[](const auto& fragmentDefinition) noexcept {
				auto position = fragmentDefinition.second.begin();
				std::ostringstream message;

				message << "Unused fragment definition name: " << fragmentDefinition.first;

				return schema_error { message.str(), { position.line, position.column } };
			});
	}
}

std::vector<schema_error> ValidateExecutableVisitor::getStructuredErrors()
{
	auto errors = std::move(_errors);

	// Reset all of the state for this query, but keep the Introspection schema information.
	_fragmentDefinitions.clear();
	_operationDefinitions.clear();
	_referencedFragments.clear();
	_fragmentCycles.clear();

	return errors;
}

void ValidateExecutableVisitor::visitFragmentDefinition(const peg::ast_node& fragmentDefinition)
{
	peg::on_first_child<peg::directives>(fragmentDefinition, [this](const peg::ast_node& child) {
		visitDirectives(introspection::DirectiveLocation::FRAGMENT_DEFINITION, child);
	});

	const auto name = fragmentDefinition.children.front()->string_view();
	const auto& selection = *fragmentDefinition.children.back();
	const auto& typeCondition = fragmentDefinition.children[1];
	auto innerType = typeCondition->children.front()->string_view();

	auto itrKind = _typeKinds.find(innerType);

	if (itrKind == _typeKinds.end() || isScalarType(itrKind->second))
	{
		// http://spec.graphql.org/June2018/#sec-Fragment-Spread-Type-Existence
		// http://spec.graphql.org/June2018/#sec-Fragments-On-Composite-Types
		auto position = typeCondition->begin();
		std::ostringstream message;

		message << (itrKind == _typeKinds.end() ? "Undefined target type on fragment definition: "
												: "Scalar target type on fragment definition: ")
				<< name << " name: " << innerType;

		_errors.push_back({ message.str(), { position.line, position.column } });
		return;
	}

	_fragmentStack.insert(name);
	_scopedType = std::move(innerType);

	visitSelection(selection);

	_scopedType = {};
	_fragmentStack.clear();
	_selectionFields.clear();
}

void ValidateExecutableVisitor::visitOperationDefinition(const peg::ast_node& operationDefinition)
{
	auto operationType = strQuery;

	peg::on_first_child<peg::operation_type>(operationDefinition,
		[&operationType](const peg::ast_node& child) {
			operationType = child.string_view();
		});

	std::string_view operationName;

	peg::on_first_child<peg::operation_name>(operationDefinition,
		[&operationName](const peg::ast_node& child) {
			operationName = child.string_view();
		});

	_operationVariables = std::make_optional<VariableTypes>();

	peg::for_each_child<peg::variable>(operationDefinition,
		[this, operationName](const peg::ast_node& variable) {
			std::string_view variableName;
			ValidateArgument variableArgument;

			for (const auto& child : variable.children)
			{
				if (child->is_type<peg::variable_name>())
				{
					// Skip the $ prefix
					variableName = child->string_view().substr(1);

					if (_operationVariables->find(variableName) != _operationVariables->end())
					{
						// http://spec.graphql.org/June2018/#sec-Variable-Uniqueness
						auto position = child->begin();
						std::ostringstream message;

						message << "Conflicting variable";

						if (!operationName.empty())
						{
							message << " operation: " << operationName;
						}

						message << " name: " << variableName;

						_errors.push_back({ message.str(), { position.line, position.column } });
						return;
					}
				}
				else if (child->is_type<peg::named_type>() || child->is_type<peg::list_type>()
					|| child->is_type<peg::nonnull_type>())
				{
					ValidateVariableTypeVisitor visitor(*_schema, _typeKinds);

					visitor.visit(*child);

					if (!visitor.isInputType())
					{
						// http://spec.graphql.org/June2018/#sec-Variables-Are-Input-Types
						auto position = child->begin();
						std::ostringstream message;

						message << "Invalid variable type";

						if (!operationName.empty())
						{
							message << " operation: " << operationName;
						}

						message << " name: " << variableName;

						_errors.push_back({ message.str(), { position.line, position.column } });
						return;
					}

					variableArgument.type = visitor.getType();
				}
				else if (child->is_type<peg::default_value>())
				{
					ValidateArgumentValueVisitor visitor(_errors);

					visitor.visit(*child->children.back());

					auto argument = visitor.getArgumentValue();

					if (!validateInputValue(false, argument, variableArgument.type))
					{
						// http://spec.graphql.org/June2018/#sec-Values-of-Correct-Type
						auto position = child->begin();
						std::ostringstream message;

						message << "Incompatible variable default value";

						if (!operationName.empty())
						{
							message << " operation: " << operationName;
						}

						message << " name: " << variableName;

						_errors.push_back({ message.str(), { position.line, position.column } });
						return;
					}

					variableArgument.defaultValue = true;
					variableArgument.nonNullDefaultValue = argument.value != nullptr;
				}
			}

			_variableDefinitions.insert({ variableName, variable });
			_operationVariables->insert({ variableName, std::move(variableArgument) });
		});

	peg::on_first_child<peg::directives>(operationDefinition,
		[this, &operationType](const peg::ast_node& child) {
			auto location = introspection::DirectiveLocation::QUERY;

			if (operationType == strMutation)
			{
				location = introspection::DirectiveLocation::MUTATION;
			}
			else if (operationType == strSubscription)
			{
				location = introspection::DirectiveLocation::SUBSCRIPTION;
			}

			visitDirectives(location, child);
		});

	auto itrType = _operationTypes.find(operationType);

	if (itrType == _operationTypes.cend())
	{
		auto position = operationDefinition.begin();
		std::ostringstream error;

		error << "Unsupported operation type: " << operationType;

		_errors.push_back({ error.str(), { position.line, position.column } });
		return;
	}

	_scopedType = itrType->second;
	_fieldCount = 0;

	const auto& selection = *operationDefinition.children.back();

	visitSelection(selection);

	if (_fieldCount > 1 && operationType == strSubscription)
	{
		// http://spec.graphql.org/June2018/#sec-Single-root-field
		auto position = operationDefinition.begin();
		std::ostringstream error;

		error << "Subscription with more than one root field";

		if (!operationName.empty())
		{
			error << " name: " << operationName;
		}

		_errors.push_back({ error.str(), { position.line, position.column } });
	}

	_scopedType = {};
	_fragmentStack.clear();
	_selectionFields.clear();

	for (const auto& variable : _variableDefinitions)
	{
		if (_referencedVariables.find(variable.first) == _referencedVariables.end())
		{
			// http://spec.graphql.org/June2018/#sec-All-Variables-Used
			auto position = variable.second.begin();
			std::ostringstream error;

			error << "Unused variable name: " << variable.first;

			_errors.push_back({ error.str(), { position.line, position.column } });
		}
	}

	_operationVariables.reset();
	_variableDefinitions.clear();
	_referencedVariables.clear();
}

void ValidateExecutableVisitor::visitSelection(const peg::ast_node& selection)
{
	for (const auto& child : selection.children)
	{
		if (child->is_type<peg::field>())
		{
			visitField(*child);
		}
		else if (child->is_type<peg::fragment_spread>())
		{
			visitFragmentSpread(*child);
		}
		else if (child->is_type<peg::inline_fragment>())
		{
			visitInlineFragment(*child);
		}
	}
}

ValidateTypeFieldArguments ValidateExecutableVisitor::getArguments(
	const std::vector<std::shared_ptr<schema::InputValue>>& args)
{
	ValidateTypeFieldArguments result;

	for (const auto& arg : args)
	{
		if (!arg)
		{
			continue;
		}

		ValidateArgument argument;

		argument.defaultValue = !arg->defaultValue().empty();
		argument.nonNullDefaultValue =
			argument.defaultValue && arg->defaultValue() != R"gql(null)gql"sv;
		argument.type = arg->type().lock();

		result[arg->name()] = std::move(argument);
	}

	return result;
}

std::optional<introspection::TypeKind> ValidateExecutableVisitor::getTypeKind(
	std::string_view name) const
{
	auto itrKind = _typeKinds.find(name);

	return (itrKind == _typeKinds.cend() ? std::nullopt : std::make_optional(itrKind->second));
}

std::optional<introspection::TypeKind> ValidateExecutableVisitor::getScopedTypeKind() const
{
	return getTypeKind(_scopedType);
}

constexpr bool ValidateExecutableVisitor::isScalarType(introspection::TypeKind kind)
{
	switch (kind)
	{
		case introspection::TypeKind::OBJECT:
		case introspection::TypeKind::INTERFACE:
		case introspection::TypeKind::UNION:
			return false;

		default:
			return true;
	}
}

bool ValidateExecutableVisitor::matchesScopedType(std::string_view name) const
{
	if (name == _scopedType)
	{
		return true;
	}

	auto itrScoped = _matchingTypes.find(_scopedType);
	auto itrNamed = _matchingTypes.find(name);

	if (itrScoped != _matchingTypes.end() && itrNamed != _matchingTypes.end())
	{
		auto itrMatch = std::find_if(itrScoped->second.begin(),
			itrScoped->second.end(),
			[this, itrNamed](std::string_view matchingType) noexcept {
				return itrNamed->second.find(matchingType) != itrNamed->second.end();
			});

		return itrMatch != itrScoped->second.end();
	}

	return false;
}

bool ValidateExecutableVisitor::validateInputValue(
	bool hasNonNullDefaultValue, const ValidateArgumentValuePtr& argument, const ValidateType& type)
{
	if (!type)
	{
		_errors.push_back({ "Unknown input type", argument.position });
		return false;
	}

	if (argument.value && std::holds_alternative<ValidateArgumentVariable>(argument.value->data))
	{
		if (_operationVariables)
		{
			const auto& variable = std::get<ValidateArgumentVariable>(argument.value->data);
			auto itrVariable = _operationVariables->find(variable.name);

			if (itrVariable == _operationVariables->end())
			{
				// http://spec.graphql.org/June2018/#sec-All-Variable-Uses-Defined
				std::ostringstream message;

				message << "Undefined variable name: " << variable.name;

				_errors.push_back({ message.str(), argument.position });
				return false;
			}

			_referencedVariables.insert(variable.name);

			return validateVariableType(
				hasNonNullDefaultValue || itrVariable->second.nonNullDefaultValue,
				itrVariable->second.type,
				argument.position,
				type);
		}
		else
		{
			// In fragment definitions, variables can hold any type. It's only when we are
			// transitively visiting them through an operation definition that they are assigned a
			// type, and the type may not be exactly the same in all operations definitions which
			// reference the fragment.
			return true;
		}
	}

	const auto kind = type->kind();

	if (!argument.value)
	{
		// The null literal matches any nullable type and does not match a non-nullable type.
		if (kind == introspection::TypeKind::NON_NULL && !hasNonNullDefaultValue)
		{
			_errors.push_back({ "Expected Non-Null value", argument.position });
			return false;
		}

		return true;
	}

	switch (kind)
	{
		case introspection::TypeKind::NON_NULL:
		{
			// Unwrap and check the next one.
			const auto ofType = type->ofType().lock();

			if (!ofType)
			{
				_errors.push_back({ "Unknown Non-Null type", argument.position });
				return false;
			}

			return validateInputValue(hasNonNullDefaultValue, argument, ofType);
		}

		case introspection::TypeKind::LIST:
		{
			if (!std::holds_alternative<ValidateArgumentList>(argument.value->data))
			{
				_errors.push_back({ "Expected List value", argument.position });
				return false;
			}

			const auto ofType = type->ofType().lock();

			if (!ofType)
			{
				_errors.push_back({ "Unknown List type", argument.position });
				return false;
			}

			// Check every value against the target type.
			for (const auto& value : std::get<ValidateArgumentList>(argument.value->data).values)
			{
				if (!validateInputValue(false, value, ofType))
				{
					// Error messages are added in the recursive call, so just bubble up the result.
					return false;
				}
			}

			return true;
		}

		case introspection::TypeKind::INPUT_OBJECT:
		{
			const auto name = type->name();

			if (name.empty())
			{
				_errors.push_back({ "Unknown Input Object type", argument.position });
				return false;
			}

			if (!std::holds_alternative<ValidateArgumentMap>(argument.value->data))
			{
				std::ostringstream message;

				message << "Expected Input Object value name: " << name;

				_errors.push_back({ message.str(), argument.position });
				return false;
			}

			auto itrFields = getInputTypeFields(name);

			if (itrFields == _inputTypeFields.end())
			{
				std::ostringstream message;

				message << "Expected Input Object fields name: " << name;

				_errors.push_back({ message.str(), argument.position });
				return false;
			}

			const auto& values = std::get<ValidateArgumentMap>(argument.value->data).values;
			std::set<std::string_view> subFields;

			// Check every value against the target type.
			for (const auto& entry : values)
			{
				auto itrField = itrFields->second.find(entry.first);

				if (itrField == itrFields->second.end())
				{
					// http://spec.graphql.org/June2018/#sec-Input-Object-Field-Names
					std::ostringstream message;

					message << "Undefined Input Object field type: " << name
							<< " name: " << entry.first;

					_errors.push_back({ message.str(), entry.second.position });
					return false;
				}

				if (entry.second.value || !itrField->second.defaultValue)
				{
					if (!validateInputValue(itrField->second.nonNullDefaultValue,
							entry.second,
							itrField->second.type))
					{
						// Error messages are added in the recursive call, so just bubble up the
						// result.
						return false;
					}
				}

				subFields.insert(entry.first);
			}

			// See if all required fields were specified.
			for (const auto& entry : itrFields->second)
			{
				if (entry.second.defaultValue || subFields.find(entry.first) != subFields.end())
				{
					continue;
				}

				if (!entry.second.type)
				{
					std::ostringstream message;

					message << "Unknown Input Object field type: " << name
							<< " name: " << entry.first;

					_errors.push_back({ message.str(), argument.position });
					return false;
				}

				const auto fieldKind = entry.second.type->kind();

				if (fieldKind == introspection::TypeKind::NON_NULL)
				{
					// http://spec.graphql.org/June2018/#sec-Input-Object-Required-Fields
					std::ostringstream message;

					message << "Missing Input Object field type: " << name
							<< " name: " << entry.first;

					_errors.push_back({ message.str(), argument.position });
					return false;
				}
			}

			return true;
		}

		case introspection::TypeKind::ENUM:
		{
			const auto name = type->name();

			if (name.empty())
			{
				_errors.push_back({ "Unknown Enum value", argument.position });
				return false;
			}

			if (!std::holds_alternative<ValidateArgumentEnumValue>(argument.value->data))
			{
				std::ostringstream message;

				message << "Expected Enum value name: " << name;

				_errors.push_back({ message.str(), argument.position });
				return false;
			}

			const auto& value = std::get<ValidateArgumentEnumValue>(argument.value->data).value;
			auto itrEnumValues = _enumValues.find(name);

			if (itrEnumValues == _enumValues.end()
				|| itrEnumValues->second.find(value) == itrEnumValues->second.end())
			{
				std::ostringstream message;

				message << "Undefined Enum value type: " << name << " name: " << value;

				_errors.push_back({ message.str(), argument.position });
				return false;
			}

			return true;
		}

		case introspection::TypeKind::SCALAR:
		{
			const auto name = type->name();

			if (name.empty())
			{
				_errors.push_back({ "Unknown Scalar value", argument.position });
				return false;
			}

			if (name == R"gql(Int)gql"sv)
			{
				if (!std::holds_alternative<response::IntType>(argument.value->data))
				{
					_errors.push_back({ "Expected Int value", argument.position });
					return false;
				}
			}
			else if (name == R"gql(Float)gql"sv)
			{
				if (!std::holds_alternative<response::FloatType>(argument.value->data)
					&& !std::holds_alternative<response::IntType>(argument.value->data))
				{
					_errors.push_back({ "Expected Float value", argument.position });
					return false;
				}
			}
			else if (name == R"gql(String)gql"sv)
			{
				if (!std::holds_alternative<response::StringType>(argument.value->data))
				{
					_errors.push_back({ "Expected String value", argument.position });
					return false;
				}
			}
			else if (name == R"gql(ID)gql"sv)
			{
				if (std::holds_alternative<response::StringType>(argument.value->data))
				{
					try
					{
						const auto& value = std::get<response::StringType>(argument.value->data);
						auto decoded = Base64::fromBase64(value.data(), value.size());

						return true;
					}
					catch (const schema_exception&)
					{
						// Eat the exception and fail validation
					}
				}

				_errors.push_back({ "Expected ID value", argument.position });
				return false;
			}
			else if (name == R"gql(Boolean)gql"sv)
			{
				if (!std::holds_alternative<response::BooleanType>(argument.value->data))
				{
					_errors.push_back({ "Expected Boolean value", argument.position });
					return false;
				}
			}

			if (_scalarTypes.find(name) == _scalarTypes.end())
			{
				std::ostringstream message;

				message << "Undefined Scalar type name: " << name;

				_errors.push_back({ message.str(), argument.position });
				return false;
			}

			return true;
		}

		default:
		{
			_errors.push_back({ "Unexpected value type", argument.position });
			return false;
		}
	}
}

bool ValidateExecutableVisitor::validateVariableType(bool isNonNull,
	const ValidateType& variableType, const schema_location& position,
	const ValidateType& inputType)
{
	if (!variableType)
	{
		_errors.push_back({ "Unknown variable type", position });
		return false;
	}

	const auto variableKind = variableType->kind();

	if (variableKind == introspection::TypeKind::NON_NULL)
	{
		const auto ofType = variableType->ofType().lock();

		if (!ofType)
		{
			_errors.push_back({ "Unknown Non-Null variable type", position });
			return false;
		}

		return validateVariableType(true, ofType, position, inputType);
	}

	if (!inputType)
	{
		_errors.push_back({ "Unknown input type", position });
		return false;
	}

	const auto inputKind = inputType->kind();

	switch (inputKind)
	{
		case introspection::TypeKind::NON_NULL:
		{
			if (!isNonNull)
			{
				// http://spec.graphql.org/June2018/#sec-All-Variable-Usages-are-Allowed
				_errors.push_back({ "Expected Non-Null variable type", position });
				return false;
			}

			// Unwrap and check the next one.
			const auto ofType = inputType->ofType().lock();

			if (!ofType)
			{
				_errors.push_back({ "Unknown Non-Null input type", position });
				return false;
			}

			return validateVariableType(false, variableType, position, ofType);
		}

		case introspection::TypeKind::LIST:
		{
			if (variableKind != inputKind)
			{
				// http://spec.graphql.org/June2018/#sec-All-Variable-Usages-are-Allowed
				_errors.push_back({ "Expected List variable type", position });
				return false;
			}

			// Unwrap and check the next one.
			const auto variableOfType = variableType->ofType().lock();

			if (!variableOfType)
			{
				_errors.push_back({ "Unknown List variable type", position });
				return false;
			}

			const auto inputOfType = inputType->ofType().lock();

			if (!inputOfType)
			{
				_errors.push_back({ "Unknown List input type", position });
				return false;
			}

			return validateVariableType(false, variableOfType, position, inputOfType);
		}

		case introspection::TypeKind::INPUT_OBJECT:
		{
			if (variableKind != inputKind)
			{
				// http://spec.graphql.org/June2018/#sec-All-Variable-Usages-are-Allowed
				_errors.push_back({ "Expected Input Object variable type", position });
				return false;
			}

			break;
		}

		case introspection::TypeKind::ENUM:
		{
			if (variableKind != inputKind)
			{
				// http://spec.graphql.org/June2018/#sec-All-Variable-Usages-are-Allowed
				_errors.push_back({ "Expected Enum variable type", position });
				return false;
			}

			break;
		}

		case introspection::TypeKind::SCALAR:
		{
			if (variableKind != inputKind)
			{
				// http://spec.graphql.org/June2018/#sec-All-Variable-Usages-are-Allowed
				_errors.push_back({ "Expected Scalar variable type", position });
				return false;
			}

			break;
		}

		default:
		{
			// http://spec.graphql.org/June2018/#sec-All-Variable-Usages-are-Allowed
			_errors.push_back({ "Unexpected input type", position });
			return false;
		}
	}

	const auto variableName = variableType->name();

	if (variableName.empty())
	{
		_errors.push_back({ "Unknown variable type", position });
		return false;
	}

	const auto inputName = inputType->name();

	if (inputName.empty())
	{
		_errors.push_back({ "Unknown input type", position });
		return false;
	}

	if (variableName != inputName)
	{
		// http://spec.graphql.org/June2018/#sec-All-Variable-Usages-are-Allowed
		std::ostringstream message;

		message << "Incompatible variable type: " << variableName << " name: " << inputName;

		_errors.push_back({ message.str(), position });
		return false;
	}

	return true;
}

ValidateExecutableVisitor::TypeFields::const_iterator ValidateExecutableVisitor::
	getScopedTypeFields()
{
	auto typeKind = getScopedTypeKind();
	auto itrType = _typeFields.find(_scopedType);

	if (itrType == _typeFields.cend() && typeKind && !isScalarType(*typeKind))
	{
		const auto& type = _schema->LookupType(_scopedType);

		if (type)
		{
			const auto& fields = type->fields();
			std::map<std::string_view, ValidateTypeField> validateFields;

			for (auto& entry : fields)
			{
				if (!entry)
				{
					continue;
				}

				const auto fieldName = entry->name();
				ValidateTypeField subField;

				subField.returnType = entry->type().lock();

				if (fieldName.empty() || !subField.returnType)
				{
					continue;
				}

				subField.arguments = getArguments(entry->args());

				validateFields[fieldName] = std::move(subField);
			}

			if (_scopedType == _operationTypes[strQuery])
			{
				ValidateTypeField schemaField;

				schemaField.returnType =
					std::make_shared<schema::WrapperType>(introspection::TypeKind::NON_NULL,
						_schema->LookupType(R"gql(__Schema)gql"sv));
				validateFields[R"gql(__schema)gql"sv] = std::move(schemaField);

				ValidateTypeField typeField;
				ValidateArgument nameArgument;

				typeField.returnType = _schema->LookupType(R"gql(__Type)gql"sv);

				nameArgument.type =
					std::make_shared<schema::WrapperType>(introspection::TypeKind::NON_NULL,
						_schema->LookupType(R"gql(String)gql"sv));
				typeField.arguments[R"gql(name)gql"sv] = std::move(nameArgument);

				validateFields[R"gql(__type)gql"sv] = std::move(typeField);
			}

			ValidateTypeField typenameField;

			typenameField.returnType =
				std::make_shared<schema::WrapperType>(introspection::TypeKind::NON_NULL,
					_schema->LookupType(R"gql(String)gql"sv));
			validateFields[R"gql(__typename)gql"sv] = std::move(typenameField);

			itrType = _typeFields.insert({ _scopedType, std::move(validateFields) }).first;
		}
	}

	return itrType;
}

ValidateExecutableVisitor::InputTypeFields::const_iterator ValidateExecutableVisitor::
	getInputTypeFields(std::string_view name)
{
	auto typeKind = getTypeKind(name);
	auto itrType = _inputTypeFields.find(name);

	if (itrType == _inputTypeFields.cend() && typeKind
		&& *typeKind == introspection::TypeKind::INPUT_OBJECT)
	{
		const auto& type = _schema->LookupType(name);

		if (type)
		{
			itrType = _inputTypeFields.insert({ name, getArguments(type->inputFields()) }).first;
		}
	}

	return itrType;
}

template <class _FieldTypes>
std::string_view ValidateExecutableVisitor::getFieldType(
	const _FieldTypes& fields, std::string_view name)
{
	std::string_view result;
	auto itrType = fields.find(name);

	if (itrType == fields.end())
	{
		return result;
	}

	// Iteratively expand nested types till we get the underlying field type.
	auto fieldType = getValidateFieldType(itrType->second);

	do
	{
		const auto name = fieldType->name();
		const auto ofType = fieldType->ofType().lock();

		if (!name.empty())
		{
			result = name;
		}
		else if (ofType)
		{
			fieldType = ofType;
		}
		else
		{
			break;
		}
	} while (result.empty());

	return result;
}

const ValidateType& ValidateExecutableVisitor::getValidateFieldType(
	const FieldTypes::mapped_type& value)
{
	return value.returnType;
}

const ValidateType& ValidateExecutableVisitor::getValidateFieldType(
	const InputFieldTypes::mapped_type& value)
{
	return value.type;
}

template <class _FieldTypes>
std::string ValidateExecutableVisitor::getWrappedFieldType(
	const _FieldTypes& fields, std::string_view name)
{
	std::string result;
	auto itrType = fields.find(name);

	if (itrType == fields.end())
	{
		return result;
	}

	result = getWrappedFieldType(getValidateFieldType(itrType->second));

	return result;
}

std::string ValidateExecutableVisitor::getWrappedFieldType(const ValidateType& returnType)
{
	// Recursively expand nested types till we get the underlying field type.
	const auto name = returnType->name();

	if (!name.empty())
	{
		return std::string { name };
	}

	std::ostringstream oss;
	const auto kind = returnType->kind();
	const auto ofType = returnType->ofType().lock();

	if (ofType)
	{
		switch (kind)
		{
			case introspection::TypeKind::LIST:
				oss << '[' << getWrappedFieldType(ofType) << ']';
				break;

			case introspection::TypeKind::NON_NULL:
				oss << getWrappedFieldType(ofType) << '!';
				break;

			default:
				break;
		}
	}

	return oss.str();
}

void ValidateExecutableVisitor::visitField(const peg::ast_node& field)
{
	peg::on_first_child<peg::directives>(field, [this](const peg::ast_node& child) {
		visitDirectives(introspection::DirectiveLocation::FIELD, child);
	});

	std::string_view name;

	peg::on_first_child<peg::field_name>(field, [&name](const peg::ast_node& child) {
		name = child.string_view();
	});

	auto kind = getScopedTypeKind();

	if (!kind)
	{
		// http://spec.graphql.org/June2018/#sec-Leaf-Field-Selections
		auto position = field.begin();
		std::ostringstream message;

		message << "Field on unknown type: " << _scopedType << " name: " << name;

		_errors.push_back({ message.str(), { position.line, position.column } });
		return;
	}

	std::string_view innerType;
	std::string wrappedType;
	auto itrType = getScopedTypeFields();

	if (itrType == _typeFields.cend())
	{
		// http://spec.graphql.org/June2018/#sec-Leaf-Field-Selections
		auto position = field.begin();
		std::ostringstream message;

		message << "Field on scalar type: " << _scopedType << " name: " << name;

		_errors.push_back({ message.str(), { position.line, position.column } });
		return;
	}

	switch (*kind)
	{
		case introspection::TypeKind::OBJECT:
		case introspection::TypeKind::INTERFACE:
		{
			// http://spec.graphql.org/June2018/#sec-Field-Selections-on-Objects-Interfaces-and-Unions-Types
			innerType = getFieldType(itrType->second, name);
			wrappedType = getWrappedFieldType(itrType->second, name);
			break;
		}

		case introspection::TypeKind::UNION:
		{
			if (name != R"gql(__typename)gql")
			{
				// http://spec.graphql.org/June2018/#sec-Leaf-Field-Selections
				auto position = field.begin();
				std::ostringstream message;

				message << "Field on union type: " << _scopedType << " name: " << name;

				_errors.push_back({ message.str(), { position.line, position.column } });
				return;
			}

			// http://spec.graphql.org/June2018/#sec-Field-Selections-on-Objects-Interfaces-and-Unions-Types
			innerType = "String";
			wrappedType = "String!";
			break;
		}

		default:
			break;
	}

	if (innerType.empty())
	{
		// http://spec.graphql.org/June2018/#sec-Field-Selections-on-Objects-Interfaces-and-Unions-Types
		auto position = field.begin();
		std::ostringstream message;

		message << "Undefined field type: " << _scopedType << " name: " << name;

		_errors.push_back({ message.str(), { position.line, position.column } });
		return;
	}

	std::string_view alias;

	peg::on_first_child<peg::alias_name>(field, [&alias](const peg::ast_node& child) {
		alias = child.string_view();
	});

	if (alias.empty())
	{
		alias = name;
	}

	ValidateFieldArguments validateArguments;
	std::map<std::string_view, schema_location> argumentLocations;
	std::queue<std::string_view> argumentNames;

	peg::on_first_child<peg::arguments>(field,
		[this, &name, &validateArguments, &argumentLocations, &argumentNames](
			const peg::ast_node& child) {
			for (auto& argument : child.children)
			{
				auto argumentName = argument->children.front()->string_view();
				auto position = argument->begin();

				if (validateArguments.find(argumentName) != validateArguments.end())
				{
					// http://spec.graphql.org/June2018/#sec-Argument-Uniqueness
					std::ostringstream message;

					message << "Conflicting argument type: " << _scopedType << " field: " << name
							<< " name: " << argumentName;

					_errors.push_back({ message.str(), { position.line, position.column } });
					continue;
				}

				ValidateArgumentValueVisitor visitor(_errors);

				visitor.visit(*argument->children.back());
				validateArguments[argumentName] = visitor.getArgumentValue();
				argumentLocations[argumentName] = { position.line, position.column };
				argumentNames.push(std::move(argumentName));
			}
		});

	std::optional<std::string_view> objectType =
		(*kind == introspection::TypeKind::OBJECT ? std::make_optional(_scopedType) : std::nullopt);
	ValidateField validateField(wrappedType, objectType, name, std::move(validateArguments));
	auto itrValidateField = _selectionFields.find(alias);

	if (itrValidateField != _selectionFields.end())
	{
		if (itrValidateField->second == validateField)
		{
			// We already validated this field.
			return;
		}
		else
		{
			// http://spec.graphql.org/June2018/#sec-Field-Selection-Merging
			auto position = field.begin();
			std::ostringstream message;

			message << "Conflicting field type: " << _scopedType << " name: " << name;

			_errors.push_back({ message.str(), { position.line, position.column } });
		}
	}

	auto itrField = itrType->second.find(name);

	if (itrField != itrType->second.end())
	{
		while (!argumentNames.empty())
		{
			auto argumentName = std::move(argumentNames.front());

			argumentNames.pop();

			auto itrArgument = itrField->second.arguments.find(argumentName);

			if (itrArgument == itrField->second.arguments.end())
			{
				// http://spec.graphql.org/June2018/#sec-Argument-Names
				std::ostringstream message;

				message << "Undefined argument type: " << _scopedType << " field: " << name
						<< " name: " << argumentName;

				_errors.push_back({ message.str(), argumentLocations[argumentName] });
			}
		}

		for (auto& argument : itrField->second.arguments)
		{
			auto itrArgument = validateField.arguments.find(argument.first);
			const bool missing = itrArgument == validateField.arguments.end();

			if (!missing && itrArgument->second.value)
			{
				// The value was not null.
				if (!validateInputValue(argument.second.nonNullDefaultValue,
						itrArgument->second,
						argument.second.type))
				{
					// http://spec.graphql.org/June2018/#sec-Values-of-Correct-Type
					std::ostringstream message;

					message << "Incompatible argument type: " << _scopedType << " field: " << name
							<< " name: " << argument.first;

					_errors.push_back({ message.str(), argumentLocations[argument.first] });
				}

				continue;
			}
			else if (argument.second.defaultValue)
			{
				// The argument has a default value.
				continue;
			}

			// See if the argument is wrapped in NON_NULL
			if (argument.second.type
				&& introspection::TypeKind::NON_NULL == argument.second.type->kind())
			{
				// http://spec.graphql.org/June2018/#sec-Required-Arguments
				auto position = field.begin();
				std::ostringstream message;

				message << (missing ? "Missing argument type: "
									: "Required non-null argument type: ")
						<< _scopedType << " field: " << name << " name: " << argument.first;

				_errors.push_back({ message.str(), { position.line, position.column } });
			}
		}
	}

	_selectionFields.insert({ alias, std::move(validateField) });

	const peg::ast_node* selection = nullptr;

	peg::on_first_child<peg::selection_set>(field, [&selection](const peg::ast_node& child) {
		selection = &child;
	});

	size_t subFieldCount = 0;

	if (selection != nullptr)
	{
		auto outerType = std::move(_scopedType);
		auto outerFields = std::move(_selectionFields);
		auto outerFieldCount = _fieldCount;

		_fieldCount = 0;
		_selectionFields.clear();
		_scopedType = std::move(innerType);

		visitSelection(*selection);

		innerType = std::move(_scopedType);
		_scopedType = std::move(outerType);
		_selectionFields = std::move(outerFields);
		subFieldCount = _fieldCount;
		_fieldCount = outerFieldCount;
	}

	if (subFieldCount == 0)
	{
		auto itrInnerKind = _typeKinds.find(innerType);

		if (itrInnerKind != _typeKinds.end() && !isScalarType(itrInnerKind->second))
		{
			// http://spec.graphql.org/June2018/#sec-Leaf-Field-Selections
			auto position = field.begin();
			std::ostringstream message;

			message << "Missing fields on non-scalar type: " << innerType;

			_errors.push_back({ message.str(), { position.line, position.column } });
			return;
		}
	}

	++_fieldCount;
}

void ValidateExecutableVisitor::visitFragmentSpread(const peg::ast_node& fragmentSpread)
{
	peg::on_first_child<peg::directives>(fragmentSpread, [this](const peg::ast_node& child) {
		visitDirectives(introspection::DirectiveLocation::FRAGMENT_SPREAD, child);
	});

	const auto name = fragmentSpread.children.front()->string_view();
	auto itr = _fragmentDefinitions.find(name);

	if (itr == _fragmentDefinitions.cend())
	{
		// http://spec.graphql.org/June2018/#sec-Fragment-spread-target-defined
		auto position = fragmentSpread.begin();
		std::ostringstream message;

		message << "Undefined fragment spread name: " << name;

		_errors.push_back({ message.str(), { position.line, position.column } });
		return;
	}

	if (_fragmentStack.find(name) != _fragmentStack.cend())
	{
		if (_fragmentCycles.insert(name).second)
		{
			// http://spec.graphql.org/June2018/#sec-Fragment-spreads-must-not-form-cycles
			auto position = fragmentSpread.begin();
			std::ostringstream message;

			message << "Cyclic fragment spread name: " << name;

			_errors.push_back({ message.str(), { position.line, position.column } });
		}

		return;
	}

	const auto& selection = *itr->second.children.back();
	const auto& typeCondition = itr->second.children[1];
	const auto innerType = typeCondition->children.front()->string_view();

	if (!matchesScopedType(innerType))
	{
		// http://spec.graphql.org/June2018/#sec-Fragment-spread-is-possible
		auto position = fragmentSpread.begin();
		std::ostringstream message;

		message << "Incompatible fragment spread target type: " << innerType << " name: " << name;

		_errors.push_back({ message.str(), { position.line, position.column } });
		return;
	}

	auto outerType = std::move(_scopedType);

	_fragmentStack.insert(name);
	_scopedType = std::move(innerType);

	visitSelection(selection);

	_scopedType = std::move(outerType);
	_fragmentStack.erase(name);

	_referencedFragments.insert(name);
}

void ValidateExecutableVisitor::visitInlineFragment(const peg::ast_node& inlineFragment)
{
	peg::on_first_child<peg::directives>(inlineFragment, [this](const peg::ast_node& child) {
		visitDirectives(introspection::DirectiveLocation::INLINE_FRAGMENT, child);
	});

	std::string_view innerType;
	schema_location typeConditionLocation;

	peg::on_first_child<peg::type_condition>(inlineFragment,
		[&innerType, &typeConditionLocation](const peg::ast_node& child) {
			auto position = child.begin();

			innerType = child.children.front()->string_view();
			typeConditionLocation = { position.line, position.column };
		});

	if (innerType.empty())
	{
		innerType = _scopedType;
	}
	else
	{
		auto itrKind = _typeKinds.find(innerType);

		if (itrKind == _typeKinds.end() || isScalarType(itrKind->second))
		{
			// http://spec.graphql.org/June2018/#sec-Fragment-Spread-Type-Existence
			// http://spec.graphql.org/June2018/#sec-Fragments-On-Composite-Types
			std::ostringstream message;

			message << (itrKind == _typeKinds.end()
					? "Undefined target type on inline fragment name: "
					: "Scalar target type on inline fragment name: ")
					<< innerType;

			_errors.push_back({ message.str(), std::move(typeConditionLocation) });
			return;
		}

		if (!matchesScopedType(innerType))
		{
			// http://spec.graphql.org/June2018/#sec-Fragment-spread-is-possible
			std::ostringstream message;

			message << "Incompatible target type on inline fragment name: " << innerType;

			_errors.push_back({ message.str(), std::move(typeConditionLocation) });
			return;
		}
	}

	peg::on_first_child<peg::selection_set>(inlineFragment,
		[this, &innerType](const peg::ast_node& selection) {
			auto outerType = std::move(_scopedType);

			_scopedType = std::move(innerType);

			visitSelection(selection);

			_scopedType = std::move(outerType);
		});
}

void ValidateExecutableVisitor::visitDirectives(
	introspection::DirectiveLocation location, const peg::ast_node& directives)
{
	std::set<std::string_view> uniqueDirectives;

	for (const auto& directive : directives.children)
	{
		std::string_view directiveName;

		peg::on_first_child<peg::directive_name>(*directive,
			[&directiveName](const peg::ast_node& child) {
				directiveName = child.string_view();
			});

		if (!uniqueDirectives.insert(directiveName).second)
		{
			// http://spec.graphql.org/June2018/#sec-Directives-Are-Unique-Per-Location
			auto position = directive->begin();
			std::ostringstream message;

			message << "Conflicting directive name: " << directiveName;

			_errors.push_back({ message.str(), { position.line, position.column } });
			continue;
		}

		auto itrDirective = _directives.find(directiveName);

		if (itrDirective == _directives.end())
		{
			// http://spec.graphql.org/June2018/#sec-Directives-Are-Defined
			auto position = directive->begin();
			std::ostringstream message;

			message << "Undefined directive name: " << directiveName;

			_errors.push_back({ message.str(), { position.line, position.column } });
			continue;
		}

		if (itrDirective->second.locations.find(location) == itrDirective->second.locations.end())
		{
			// http://spec.graphql.org/June2018/#sec-Directives-Are-In-Valid-Locations
			auto position = directive->begin();
			std::ostringstream message;

			message << "Unexpected location for directive: " << directiveName;

			switch (location)
			{
				case introspection::DirectiveLocation::QUERY:
					message << " name: QUERY";
					break;

				case introspection::DirectiveLocation::MUTATION:
					message << " name: MUTATION";
					break;

				case introspection::DirectiveLocation::SUBSCRIPTION:
					message << " name: SUBSCRIPTION";
					break;

				case introspection::DirectiveLocation::FIELD:
					message << " name: FIELD";
					break;

				case introspection::DirectiveLocation::FRAGMENT_DEFINITION:
					message << " name: FRAGMENT_DEFINITION";
					break;

				case introspection::DirectiveLocation::FRAGMENT_SPREAD:
					message << " name: FRAGMENT_SPREAD";
					break;

				case introspection::DirectiveLocation::INLINE_FRAGMENT:
					message << " name: INLINE_FRAGMENT";
					break;

				default:
					break;
			}

			_errors.push_back({ message.str(), { position.line, position.column } });
			continue;
		}

		peg::on_first_child<peg::arguments>(*directive,
			[this, &directive, &directiveName, itrDirective](const peg::ast_node& child) {
				ValidateFieldArguments validateArguments;
				std::map<std::string_view, schema_location> argumentLocations;
				std::queue<std::string_view> argumentNames;

				for (auto& argument : child.children)
				{
					auto position = argument->begin();
					auto argumentName = argument->children.front()->string_view();

					if (validateArguments.find(argumentName) != validateArguments.end())
					{
						// http://spec.graphql.org/June2018/#sec-Argument-Uniqueness
						std::ostringstream message;

						message << "Conflicting argument directive: " << directiveName
								<< " name: " << argumentName;

						_errors.push_back({ message.str(), { position.line, position.column } });
						continue;
					}

					ValidateArgumentValueVisitor visitor(_errors);

					visitor.visit(*argument->children.back());
					validateArguments[argumentName] = visitor.getArgumentValue();
					argumentLocations[argumentName] = { position.line, position.column };
					argumentNames.push(std::move(argumentName));
				}

				while (!argumentNames.empty())
				{
					auto argumentName = std::move(argumentNames.front());

					argumentNames.pop();

					auto itrArgument = itrDirective->second.arguments.find(argumentName);

					if (itrArgument == itrDirective->second.arguments.end())
					{
						// http://spec.graphql.org/June2018/#sec-Argument-Names
						std::ostringstream message;

						message << "Undefined argument directive: " << directiveName
								<< " name: " << argumentName;

						_errors.push_back({ message.str(), argumentLocations[argumentName] });
					}
				}

				for (auto& argument : itrDirective->second.arguments)
				{
					auto itrArgument = validateArguments.find(argument.first);
					const bool missing = itrArgument == validateArguments.end();

					if (!missing && itrArgument->second.value)
					{
						// The value was not null.
						if (!validateInputValue(argument.second.nonNullDefaultValue,
								itrArgument->second,
								argument.second.type))
						{
							// http://spec.graphql.org/June2018/#sec-Values-of-Correct-Type
							std::ostringstream message;

							message << "Incompatible argument directive: " << directiveName
									<< " name: " << argument.first;

							_errors.push_back({ message.str(), argumentLocations[argument.first] });
						}

						continue;
					}
					else if (argument.second.defaultValue)
					{
						// The argument has a default value.
						continue;
					}

					// See if the argument is wrapped in NON_NULL
					if (argument.second.type
						&& introspection::TypeKind::NON_NULL == argument.second.type->kind())
					{
						// http://spec.graphql.org/June2018/#sec-Required-Arguments
						auto position = directive->begin();
						std::ostringstream message;

						message << (missing ? "Missing argument directive: "
											: "Required non-null argument directive: ")
								<< directiveName << " name: " << argument.first;

						_errors.push_back({ message.str(), { position.line, position.column } });
					}
				}
			});
	}
}

} /* namespace graphql::service */
