// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "StarWarsObjects.h"

#include "graphqlservice/introspection/Introspection.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std::literals;

namespace graphql::learn {
namespace object {

Review::Review()
	: service::Object({
		"Review"
	}, {
		{ R"gql(stars)gql"sv, [this](service::ResolverParams&& params) { return resolveStars(std::move(params)); } },
		{ R"gql(__typename)gql"sv, [this](service::ResolverParams&& params) { return resolve_typename(std::move(params)); } },
		{ R"gql(commentary)gql"sv, [this](service::ResolverParams&& params) { return resolveCommentary(std::move(params)); } }
	})
{
}

service::FieldResult<response::IntType> Review::getStars(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Review::getStars is not implemented)ex");
}

std::future<service::ResolverResult> Review::resolveStars(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = getStars(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::IntType>::convert(std::move(result), std::move(params));
}

service::FieldResult<std::optional<response::StringType>> Review::getCommentary(service::FieldParams&&) const
{
	throw std::runtime_error(R"ex(Review::getCommentary is not implemented)ex");
}

std::future<service::ResolverResult> Review::resolveCommentary(service::ResolverParams&& params)
{
	std::unique_lock resolverLock(_resolverMutex);
	auto directives = std::move(params.fieldDirectives);
	auto result = getCommentary(service::FieldParams(service::SelectionSetParams{ params }, std::move(directives)));
	resolverLock.unlock();

	return service::ModifiedResult<response::StringType>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<service::ResolverResult> Review::resolve_typename(service::ResolverParams&& params)
{
	return service::ModifiedResult<response::StringType>::convert(response::StringType{ R"gql(Review)gql" }, std::move(params));
}

} // namespace object

void AddReviewDetails(std::shared_ptr<schema::ObjectType> typeReview, const std::shared_ptr<schema::Schema>& schema)
{
	typeReview->AddFields({
		schema::Field::Make(R"gql(stars)gql"sv, R"md()md"sv, std::nullopt, schema->WrapType(introspection::TypeKind::NON_NULL, schema->LookupType("Int"))),
		schema::Field::Make(R"gql(commentary)gql"sv, R"md()md"sv, std::nullopt, schema->LookupType("String"))
	});
}

} // namespace graphql::learn
