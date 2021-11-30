// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef TASKCONNECTIONOBJECT_H
#define TASKCONNECTIONOBJECT_H

#include "TodaySchema.h"

namespace graphql::today::object {

class TaskConnection
	: public service::Object
{
private:
	service::AwaitableResolver resolvePageInfo(service::ResolverParams&& params);
	service::AwaitableResolver resolveEdges(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::shared_ptr<PageInfo>> getPageInfo(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<std::vector<std::shared_ptr<TaskEdge>>>> getEdges(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::shared_ptr<PageInfo>> getPageInfo(service::FieldParams&& params) const final
		{
			return _pimpl->getPageInfo(std::move(params));
		}

		service::FieldResult<std::optional<std::vector<std::shared_ptr<TaskEdge>>>> getEdges(service::FieldParams&& params) const final
		{
			return _pimpl->getEdges(std::move(params));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	TaskConnection(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	TaskConnection(std::shared_ptr<T> pimpl)
		: TaskConnection { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~TaskConnection();
};

} // namespace graphql::today::object

#endif // TASKCONNECTIONOBJECT_H
