// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef FIELDOBJECT_H
#define FIELDOBJECT_H

#include "IntrospectionSchema.h"

namespace graphql::introspection::object {

class Field
	: public service::Object
{
private:
	service::AwaitableResolver resolveName(service::ResolverParams&& params) const;
	service::AwaitableResolver resolveDescription(service::ResolverParams&& params) const;
	service::AwaitableResolver resolveArgs(service::ResolverParams&& params) const;
	service::AwaitableResolver resolveType(service::ResolverParams&& params) const;
	service::AwaitableResolver resolveIsDeprecated(service::ResolverParams&& params) const;
	service::AwaitableResolver resolveDeprecationReason(service::ResolverParams&& params) const;

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::string> getName() const = 0;
		virtual service::FieldResult<std::optional<std::string>> getDescription() const = 0;
		virtual service::FieldResult<std::vector<std::shared_ptr<InputValue>>> getArgs() const = 0;
		virtual service::FieldResult<std::shared_ptr<Type>> getType() const = 0;
		virtual service::FieldResult<bool> getIsDeprecated() const = 0;
		virtual service::FieldResult<std::optional<std::string>> getDeprecationReason() const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::string> getName() const final
		{
			return { _pimpl->getName() };
		}

		service::FieldResult<std::optional<std::string>> getDescription() const final
		{
			return { _pimpl->getDescription() };
		}

		service::FieldResult<std::vector<std::shared_ptr<InputValue>>> getArgs() const final
		{
			return { _pimpl->getArgs() };
		}

		service::FieldResult<std::shared_ptr<Type>> getType() const final
		{
			return { _pimpl->getType() };
		}

		service::FieldResult<bool> getIsDeprecated() const final
		{
			return { _pimpl->getIsDeprecated() };
		}

		service::FieldResult<std::optional<std::string>> getDeprecationReason() const final
		{
			return { _pimpl->getDeprecationReason() };
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	const std::unique_ptr<Concept> _pimpl;

	service::TypeNames getTypeNames() const noexcept;
	service::ResolverMap getResolvers() const noexcept;

public:
	GRAPHQLINTROSPECTION_EXPORT Field(std::shared_ptr<introspection::Field> pimpl) noexcept;
	GRAPHQLINTROSPECTION_EXPORT ~Field();
};

} // namespace graphql::introspection::object

#endif // FIELDOBJECT_H