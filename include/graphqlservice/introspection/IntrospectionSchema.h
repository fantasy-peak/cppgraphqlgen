// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef INTROSPECTIONSCHEMA_H
#define INTROSPECTIONSCHEMA_H

#include "graphqlservice/internal/Schema.h"

// Check if the library version is compatible with schemagen 4.0.0
static_assert(graphql::internal::MajorVersion == 4, "regenerate with schemagen: major version mismatch");
static_assert(graphql::internal::MinorVersion == 0, "regenerate with schemagen: minor version mismatch");

// clang-format off
#ifdef GRAPHQL_DLLEXPORTS
	#ifdef IMPL_GRAPHQLINTROSPECTION_DLL
		#define GRAPHQLINTROSPECTION_EXPORT __declspec(dllexport)
	#else // !IMPL_GRAPHQLINTROSPECTION_DLL
		#define GRAPHQLINTROSPECTION_EXPORT __declspec(dllimport)
	#endif // !IMPL_GRAPHQLINTROSPECTION_DLL
#else // !GRAPHQL_DLLEXPORTS
	#define GRAPHQLINTROSPECTION_EXPORT
#endif // !GRAPHQL_DLLEXPORTS
// clang-format on

#include <memory>
#include <string>
#include <vector>

namespace graphql {
namespace introspection {

enum class TypeKind
{
	SCALAR,
	OBJECT,
	INTERFACE,
	UNION,
	ENUM,
	INPUT_OBJECT,
	LIST,
	NON_NULL
};

enum class DirectiveLocation
{
	QUERY,
	MUTATION,
	SUBSCRIPTION,
	FIELD,
	FRAGMENT_DEFINITION,
	FRAGMENT_SPREAD,
	INLINE_FRAGMENT,
	SCHEMA,
	SCALAR,
	OBJECT,
	FIELD_DEFINITION,
	ARGUMENT_DEFINITION,
	INTERFACE,
	UNION,
	ENUM,
	ENUM_VALUE,
	INPUT_OBJECT,
	INPUT_FIELD_DEFINITION
};

namespace object {

class Schema;
class Type;
class Field;
class InputValue;
class EnumValue;
class Directive;

} // namespace object

class Schema;
class Type;
class Field;
class InputValue;
class EnumValue;
class Directive;

namespace object {

class Schema
	: public service::Object
{
private:
	service::AwaitableResolver resolveTypes(service::ResolverParams&& params);
	service::AwaitableResolver resolveQueryType(service::ResolverParams&& params);
	service::AwaitableResolver resolveMutationType(service::ResolverParams&& params);
	service::AwaitableResolver resolveSubscriptionType(service::ResolverParams&& params);
	service::AwaitableResolver resolveDirectives(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::vector<std::shared_ptr<Type>>> getTypes(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<Type>> getQueryType(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<Type>> getMutationType(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<Type>> getSubscriptionType(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::vector<std::shared_ptr<Directive>>> getDirectives(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::vector<std::shared_ptr<Type>>> getTypes(service::FieldParams&&) const final
		{
			return { _pimpl->getTypes() };
		}

		service::FieldResult<std::shared_ptr<Type>> getQueryType(service::FieldParams&&) const final
		{
			return { _pimpl->getQueryType() };
		}

		service::FieldResult<std::shared_ptr<Type>> getMutationType(service::FieldParams&&) const final
		{
			return { _pimpl->getMutationType() };
		}

		service::FieldResult<std::shared_ptr<Type>> getSubscriptionType(service::FieldParams&&) const final
		{
			return { _pimpl->getSubscriptionType() };
		}

		service::FieldResult<std::vector<std::shared_ptr<Directive>>> getDirectives(service::FieldParams&&) const final
		{
			return { _pimpl->getDirectives() };
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	const std::unique_ptr<Concept> _pimpl;

public:
	GRAPHQLINTROSPECTION_EXPORT Schema(std::shared_ptr<introspection::Schema> pimpl);
	GRAPHQLINTROSPECTION_EXPORT ~Schema();
};

class Type
	: public service::Object
{
private:
	service::AwaitableResolver resolveKind(service::ResolverParams&& params);
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolveDescription(service::ResolverParams&& params);
	service::AwaitableResolver resolveFields(service::ResolverParams&& params);
	service::AwaitableResolver resolveInterfaces(service::ResolverParams&& params);
	service::AwaitableResolver resolvePossibleTypes(service::ResolverParams&& params);
	service::AwaitableResolver resolveEnumValues(service::ResolverParams&& params);
	service::AwaitableResolver resolveInputFields(service::ResolverParams&& params);
	service::AwaitableResolver resolveOfType(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<TypeKind> getKind(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::string>> getName(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::string>> getDescription(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::vector<std::shared_ptr<Field>>>> getFields(service::FieldParams&& params, std::optional<bool>&& includeDeprecatedArg) const = 0;
		virtual service::FieldResult<std::optional<std::vector<std::shared_ptr<Type>>>> getInterfaces(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::vector<std::shared_ptr<Type>>>> getPossibleTypes(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::vector<std::shared_ptr<EnumValue>>>> getEnumValues(service::FieldParams&& params, std::optional<bool>&& includeDeprecatedArg) const = 0;
		virtual service::FieldResult<std::optional<std::vector<std::shared_ptr<InputValue>>>> getInputFields(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<Type>> getOfType(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<TypeKind> getKind(service::FieldParams&&) const final
		{
			return { _pimpl->getKind() };
		}

		service::FieldResult<std::optional<std::string>> getName(service::FieldParams&&) const final
		{
			return { _pimpl->getName() };
		}

		service::FieldResult<std::optional<std::string>> getDescription(service::FieldParams&&) const final
		{
			return { _pimpl->getDescription() };
		}

		service::FieldResult<std::optional<std::vector<std::shared_ptr<Field>>>> getFields(service::FieldParams&&, std::optional<bool>&& includeDeprecatedArg) const final
		{
			return { _pimpl->getFields(std::move(includeDeprecatedArg)) };
		}

		service::FieldResult<std::optional<std::vector<std::shared_ptr<Type>>>> getInterfaces(service::FieldParams&&) const final
		{
			return { _pimpl->getInterfaces() };
		}

		service::FieldResult<std::optional<std::vector<std::shared_ptr<Type>>>> getPossibleTypes(service::FieldParams&&) const final
		{
			return { _pimpl->getPossibleTypes() };
		}

		service::FieldResult<std::optional<std::vector<std::shared_ptr<EnumValue>>>> getEnumValues(service::FieldParams&&, std::optional<bool>&& includeDeprecatedArg) const final
		{
			return { _pimpl->getEnumValues(std::move(includeDeprecatedArg)) };
		}

		service::FieldResult<std::optional<std::vector<std::shared_ptr<InputValue>>>> getInputFields(service::FieldParams&&) const final
		{
			return { _pimpl->getInputFields() };
		}

		service::FieldResult<std::shared_ptr<Type>> getOfType(service::FieldParams&&) const final
		{
			return { _pimpl->getOfType() };
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	const std::unique_ptr<Concept> _pimpl;

public:
	GRAPHQLINTROSPECTION_EXPORT Type(std::shared_ptr<introspection::Type> pimpl);
	GRAPHQLINTROSPECTION_EXPORT ~Type();
};

class Field
	: public service::Object
{
private:
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolveDescription(service::ResolverParams&& params);
	service::AwaitableResolver resolveArgs(service::ResolverParams&& params);
	service::AwaitableResolver resolveType(service::ResolverParams&& params);
	service::AwaitableResolver resolveIsDeprecated(service::ResolverParams&& params);
	service::AwaitableResolver resolveDeprecationReason(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::string> getName(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::string>> getDescription(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::vector<std::shared_ptr<InputValue>>> getArgs(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<Type>> getType(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<bool> getIsDeprecated(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::string>> getDeprecationReason(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::string> getName(service::FieldParams&&) const final
		{
			return { _pimpl->getName() };
		}

		service::FieldResult<std::optional<std::string>> getDescription(service::FieldParams&&) const final
		{
			return { _pimpl->getDescription() };
		}

		service::FieldResult<std::vector<std::shared_ptr<InputValue>>> getArgs(service::FieldParams&&) const final
		{
			return { _pimpl->getArgs() };
		}

		service::FieldResult<std::shared_ptr<Type>> getType(service::FieldParams&&) const final
		{
			return { _pimpl->getType() };
		}

		service::FieldResult<bool> getIsDeprecated(service::FieldParams&&) const final
		{
			return { _pimpl->getIsDeprecated() };
		}

		service::FieldResult<std::optional<std::string>> getDeprecationReason(service::FieldParams&&) const final
		{
			return { _pimpl->getDeprecationReason() };
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	const std::unique_ptr<Concept> _pimpl;

public:
	GRAPHQLINTROSPECTION_EXPORT Field(std::shared_ptr<introspection::Field> pimpl);
	GRAPHQLINTROSPECTION_EXPORT ~Field();
};

class InputValue
	: public service::Object
{
private:
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolveDescription(service::ResolverParams&& params);
	service::AwaitableResolver resolveType(service::ResolverParams&& params);
	service::AwaitableResolver resolveDefaultValue(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::string> getName(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::string>> getDescription(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<Type>> getType(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::string>> getDefaultValue(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::string> getName(service::FieldParams&&) const final
		{
			return { _pimpl->getName() };
		}

		service::FieldResult<std::optional<std::string>> getDescription(service::FieldParams&&) const final
		{
			return { _pimpl->getDescription() };
		}

		service::FieldResult<std::shared_ptr<Type>> getType(service::FieldParams&&) const final
		{
			return { _pimpl->getType() };
		}

		service::FieldResult<std::optional<std::string>> getDefaultValue(service::FieldParams&&) const final
		{
			return { _pimpl->getDefaultValue() };
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	const std::unique_ptr<Concept> _pimpl;

public:
	GRAPHQLINTROSPECTION_EXPORT InputValue(std::shared_ptr<introspection::InputValue> pimpl);
	GRAPHQLINTROSPECTION_EXPORT ~InputValue();
};

class EnumValue
	: public service::Object
{
private:
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolveDescription(service::ResolverParams&& params);
	service::AwaitableResolver resolveIsDeprecated(service::ResolverParams&& params);
	service::AwaitableResolver resolveDeprecationReason(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::string> getName(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::string>> getDescription(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<bool> getIsDeprecated(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::string>> getDeprecationReason(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::string> getName(service::FieldParams&&) const final
		{
			return { _pimpl->getName() };
		}

		service::FieldResult<std::optional<std::string>> getDescription(service::FieldParams&&) const final
		{
			return { _pimpl->getDescription() };
		}

		service::FieldResult<bool> getIsDeprecated(service::FieldParams&&) const final
		{
			return { _pimpl->getIsDeprecated() };
		}

		service::FieldResult<std::optional<std::string>> getDeprecationReason(service::FieldParams&&) const final
		{
			return { _pimpl->getDeprecationReason() };
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	const std::unique_ptr<Concept> _pimpl;

public:
	GRAPHQLINTROSPECTION_EXPORT EnumValue(std::shared_ptr<introspection::EnumValue> pimpl);
	GRAPHQLINTROSPECTION_EXPORT ~EnumValue();
};

class Directive
	: public service::Object
{
private:
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolveDescription(service::ResolverParams&& params);
	service::AwaitableResolver resolveLocations(service::ResolverParams&& params);
	service::AwaitableResolver resolveArgs(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::string> getName(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::string>> getDescription(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::vector<DirectiveLocation>> getLocations(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::vector<std::shared_ptr<InputValue>>> getArgs(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::string> getName(service::FieldParams&&) const final
		{
			return { _pimpl->getName() };
		}

		service::FieldResult<std::optional<std::string>> getDescription(service::FieldParams&&) const final
		{
			return { _pimpl->getDescription() };
		}

		service::FieldResult<std::vector<DirectiveLocation>> getLocations(service::FieldParams&&) const final
		{
			return { _pimpl->getLocations() };
		}

		service::FieldResult<std::vector<std::shared_ptr<InputValue>>> getArgs(service::FieldParams&&) const final
		{
			return { _pimpl->getArgs() };
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	const std::unique_ptr<Concept> _pimpl;

public:
	GRAPHQLINTROSPECTION_EXPORT Directive(std::shared_ptr<introspection::Directive> pimpl);
	GRAPHQLINTROSPECTION_EXPORT ~Directive();
};

} // namespace object

GRAPHQLINTROSPECTION_EXPORT void AddTypesToSchema(const std::shared_ptr<schema::Schema>& schema);

} // namespace introspection

namespace service {

#ifdef GRAPHQL_DLLEXPORTS
// Export all of the built-in converters
template <>
GRAPHQLINTROSPECTION_EXPORT introspection::TypeKind ModifiedArgument<introspection::TypeKind>::convert(
	const response::Value& value);
template <>
GRAPHQLINTROSPECTION_EXPORT AwaitableResolver ModifiedResult<introspection::TypeKind>::convert(
	FieldResult<introspection::TypeKind> result, ResolverParams params);
template <>
GRAPHQLINTROSPECTION_EXPORT introspection::DirectiveLocation ModifiedArgument<introspection::DirectiveLocation>::convert(
	const response::Value& value);
template <>
GRAPHQLINTROSPECTION_EXPORT AwaitableResolver ModifiedResult<introspection::DirectiveLocation>::convert(
	FieldResult<introspection::DirectiveLocation> result, ResolverParams params);
#endif // GRAPHQL_DLLEXPORTS

} // namespace service
} // namespace graphql

#endif // INTROSPECTIONSCHEMA_H
