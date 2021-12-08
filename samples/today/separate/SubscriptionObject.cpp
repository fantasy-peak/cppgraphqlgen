// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "TodayObjects.h"

#include "graphqlservice/introspection/Introspection.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std::literals;

namespace graphql::today {
namespace object {

Subscription::Subscription(std::unique_ptr<Concept>&& pimpl) noexcept
	: service::Object{ getTypeNames(), getResolvers() }
	, _pimpl { std::move(pimpl) }
{
}

service::TypeNames Subscription::getTypeNames() const noexcept
{
	return {
		"Subscription"
	};
}

service::ResolverMap Subscription::getResolvers() const noexcept
{
	return {
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ R"gql(nodeChange)gql"sv, [this](service::ResolverParams&& params) { return resolveNodeChange(std::move(params)); } },
		{ R"gql(nextAppointmentChange)gql"sv, [this](service::ResolverParams&& params) { return resolveNextAppointmentChange(std::move(params)); } }
	};
}

void Subscription::beginSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->beginSelectionSet(params);
}

void Subscription::endSelectionSet(const service::SelectionSetParams& params) const
{
	_pimpl->endSelectionSet(params);
}

service::AwaitableResolver Subscription::resolveNextAppointmentChange(service::ResolverParams&& params) const
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getNextAppointmentChange(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<Appointment>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

service::AwaitableResolver Subscription::resolveNodeChange(service::ResolverParams&& params) const
{
	auto argId = service::ModifiedArgument<response::IdType>::require("id", params.arguments);
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = _pimpl->getNodeChange(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)), std::move(argId));
	resolverLock.unlock();

	return service::ModifiedResult<Node>::convert(std::move(result), std::move(params));
}

service::AwaitableResolver Subscription::resolve_typename(service::ResolverParams&& params) const
{
	return service::ModifiedResult<std::string>::convert(std::string{ R"gql(Subscription)gql" }, std::move(params));
}

} // namespace object

void AddSubscriptionDetails(const std::shared_ptr<schema::ObjectType>& typeSubscription, const std::shared_ptr<schema::Schema>& schema)
{
	typeSubscription->AddFields({
		schema::Field::Make(R"gql(nextAppointmentChange)gql"sv, R"md()md"sv, std::make_optional(R"md(Need to deprecate a [field](http://spec.graphql.org/June2018/#sec-Deprecation))md"sv), schema->LookupType(R"gql(Appointment)gql"sv)),
		schema::Field::Make(R"gql(nodeChange)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(Node)gql"sv)), {
			schema::InputValue::Make(R"gql(id)gql"sv, R"md()md"sv, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType(R"gql(ID)gql"sv)), R"gql()gql"sv)
		})
	});
}

} // namespace graphql::today
