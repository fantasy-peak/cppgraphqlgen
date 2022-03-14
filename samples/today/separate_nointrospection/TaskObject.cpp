// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "TaskObject.h"

#include "graphqlservice/introspection/Introspection.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std::literals;

namespace graphql::today {
namespace object {

Task::Task()
	: service::Object({
		R"gql(Node)gql"sv,
		R"gql(UnionType)gql"sv,
		R"gql(Task)gql"sv,
	}, {
		{ R"gql(id)gql"sv, [this](service::ResolverParams&& params) { return resolveId(std::move(params)); } },
		{ R"gql(title)gql"sv, [this](service::ResolverParams&& params) { return resolveTitle(std::move(params)); } },
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ R"gql(isComplete)gql"sv, [this](service::ResolverParams&& params) { return resolveIsComplete(std::move(params)); } }
	})
{
}

service::FieldResult<response::IdType> Task::getId(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Task::getId is not implemented)ex");
}

std::future<service::ResolverResult> Task::resolveId(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = getId(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::IdType>::convert(std::move(result), std::move(params));
}

service::FieldResult<std::optional<response::StringType>> Task::getTitle(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Task::getTitle is not implemented)ex");
}

std::future<service::ResolverResult> Task::resolveTitle(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = getTitle(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::StringType>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::FieldResult<response::BooleanType> Task::getIsComplete(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Task::getIsComplete is not implemented)ex");
}

std::future<service::ResolverResult> Task::resolveIsComplete(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = getIsComplete(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::BooleanType>::convert(std::move(result), std::move(params));
}

std::future<service::ResolverResult> Task::resolve_typename(service::ResolverParams&& params)
{
	return service::ModifiedResult<response::StringType>::convert(response::StringType{ R"gql(Task)gql" }, std::move(params));
}

} // namespace object

void AddTaskDetails(std::shared_ptr<schema::ObjectType> typeTask, const std::shared_ptr<schema::Schema>& schema)
{
	typeTask->AddInterfaces({
		std::static_pointer_cast<const schema::InterfaceType>(schema->LookupType(R"gql(Node)gql"sv))
	});
	typeTask->AddFields({
		schema::Field::Make(R"gql(id)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("ID"))),
		schema::Field::Make(R"gql(title)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType("String")),
		schema::Field::Make(R"gql(isComplete)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("Boolean")))
	});
}

} // namespace graphql::today