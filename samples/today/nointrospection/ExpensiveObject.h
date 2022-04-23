// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef EXPENSIVEOBJECT_H
#define EXPENSIVEOBJECT_H

#include "TodaySchema.h"

namespace graphql::today::object {
namespace methods::ExpensiveHas {

template <class TImpl>
concept getOrderWithParams = requires (TImpl impl, service::FieldParams params)
{
	{ service::AwaitableScalar<int> { impl.getOrder(std::move(params)) } };
};

template <class TImpl>
concept getOrder = requires (TImpl impl)
{
	{ service::AwaitableScalar<int> { impl.getOrder() } };
};

template <class TImpl>
concept beginSelectionSet = requires (TImpl impl, const service::SelectionSetParams params)
{
	{ impl.beginSelectionSet(params) };
};

template <class TImpl>
concept endSelectionSet = requires (TImpl impl, const service::SelectionSetParams params)
{
	{ impl.endSelectionSet(params) };
};

} // namespace methods::ExpensiveHas

class Expensive final
	: public service::Object
{
private:
	service::AwaitableResolver resolveOrder(service::ResolverParams&& params) const;

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params) const;

	struct Concept
	{
		virtual ~Concept() = default;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;

		virtual service::AwaitableScalar<int> getOrder(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::AwaitableScalar<int> getOrder(service::FieldParams&& params) const final
		{
			if constexpr (methods::ExpensiveHas::getOrderWithParams<T>)
			{
				return { _pimpl->getOrder(std::move(params)) };
			}
			else if constexpr (methods::ExpensiveHas::getOrder<T>)
			{
				return { _pimpl->getOrder() };
			}
			else
			{
				throw std::runtime_error(R"ex(Expensive::getOrder is not implemented)ex");
			}
		}

		void beginSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::ExpensiveHas::beginSelectionSet<T>)
			{
				_pimpl->beginSelectionSet(params);
			}
		}

		void endSelectionSet(const service::SelectionSetParams& params) const final
		{
			if constexpr (methods::ExpensiveHas::endSelectionSet<T>)
			{
				_pimpl->endSelectionSet(params);
			}
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Expensive(std::unique_ptr<const Concept>&& pimpl) noexcept;

	service::TypeNames getTypeNames() const noexcept;
	service::ResolverMap getResolvers() const noexcept;

	void beginSelectionSet(const service::SelectionSetParams& params) const final;
	void endSelectionSet(const service::SelectionSetParams& params) const final;

	const std::unique_ptr<const Concept> _pimpl;

public:
	template <class T>
	Expensive(std::shared_ptr<T> pimpl) noexcept
		: Expensive { std::unique_ptr<const Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
	}

	static constexpr std::string_view getObjectType() noexcept
	{
		return { R"gql(Expensive)gql" };
	}
};

} // namespace graphql::today::object

#endif // EXPENSIVEOBJECT_H