// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef RESOURCEOBJECT_H
#define RESOURCEOBJECT_H

#include "ValidationSchema.h"

namespace graphql::validation::object {

class Resource
	: public service::Object
{
private:
	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::TypeNames getTypeNames() const noexcept = 0;
		virtual service::ResolverMap getResolvers() const noexcept = 0;

		virtual void beginSelectionSet(const service::SelectionSetParams& params) const = 0;
		virtual void endSelectionSet(const service::SelectionSetParams& params) const = 0;		
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::TypeNames getTypeNames() const noexcept final
		{
			return _pimpl->getTypeNames();
		}

		service::ResolverMap getResolvers() const noexcept final
		{
			return _pimpl->getResolvers();
		}

		void beginSelectionSet(const service::SelectionSetParams& params) const final
		{
			_pimpl->beginSelectionSet(params);
		}

		void endSelectionSet(const service::SelectionSetParams& params) const final
		{
			_pimpl->endSelectionSet(params);
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Resource(std::unique_ptr<Concept>&& pimpl) noexcept;

	void beginSelectionSet(const service::SelectionSetParams& params) const final;
	void endSelectionSet(const service::SelectionSetParams& params) const final;

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Resource(std::shared_ptr<T> pimpl) noexcept
		: Resource { std::unique_ptr<Concept> { std::make_unique<Model<T>>(std::move(pimpl)) } }
	{
		static_assert(T::template implements<Resource>(), "Resource is not implemented");
	}
};

} // namespace graphql::validation::object

#endif // RESOURCEOBJECT_H
