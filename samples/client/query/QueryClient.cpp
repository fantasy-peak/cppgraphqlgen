// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "QueryClient.h"

#include <algorithm>
#include <array>
#include <sstream>
#include <stdexcept>
#include <string_view>

using namespace std::literals;

namespace graphql::client {

using namespace query::Query;

static const std::array<std::string_view, 4> s_namesTaskState = {
	"New"sv,
	"Started"sv,
	"Complete"sv,
	"Unassigned"sv,
};

template <>
TaskState ModifiedResponse<TaskState>::parse(response::Value&& value)
{
	if (!value.maybe_enum())
	{
		throw std::logic_error { "not a valid TaskState value" };
	}

	const auto itr = std::find(s_namesTaskState.cbegin(), s_namesTaskState.cend(), value.release<response::StringType>());

	if (itr == s_namesTaskState.cend())
	{
		throw std::logic_error { "not a valid TaskState value" };
	}

	return static_cast<TaskState>(itr - s_namesTaskState.cbegin());
}

template <>
Response::appointments_AppointmentConnection::edges_AppointmentEdge::node_Appointment ModifiedResponse<Response::appointments_AppointmentConnection::edges_AppointmentEdge::node_Appointment>::parse(response::Value&& response)
{
	Response::appointments_AppointmentConnection::edges_AppointmentEdge::node_Appointment result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(id)js"sv)
			{
				result.id = ModifiedResponse<response::IdType>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(subject)js"sv)
			{
				result.subject = ModifiedResponse<response::StringType>::parse<TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
			if (member.first == R"js(when)js"sv)
			{
				result.when = ModifiedResponse<response::Value>::parse<TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
			if (member.first == R"js(isNow)js"sv)
			{
				result.isNow = ModifiedResponse<response::BooleanType>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(__typename)js"sv)
			{
				result._typename = ModifiedResponse<response::StringType>::parse(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

template <>
Response::appointments_AppointmentConnection::edges_AppointmentEdge ModifiedResponse<Response::appointments_AppointmentConnection::edges_AppointmentEdge>::parse(response::Value&& response)
{
	Response::appointments_AppointmentConnection::edges_AppointmentEdge result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(node)js"sv)
			{
				result.node = ModifiedResponse<Response::appointments_AppointmentConnection::edges_AppointmentEdge::node_Appointment>::parse<TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

template <>
Response::appointments_AppointmentConnection ModifiedResponse<Response::appointments_AppointmentConnection>::parse(response::Value&& response)
{
	Response::appointments_AppointmentConnection result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(edges)js"sv)
			{
				result.edges = ModifiedResponse<Response::appointments_AppointmentConnection::edges_AppointmentEdge>::parse<TypeModifier::Nullable, TypeModifier::List, TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

template <>
Response::tasks_TaskConnection::edges_TaskEdge::node_Task ModifiedResponse<Response::tasks_TaskConnection::edges_TaskEdge::node_Task>::parse(response::Value&& response)
{
	Response::tasks_TaskConnection::edges_TaskEdge::node_Task result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(id)js"sv)
			{
				result.id = ModifiedResponse<response::IdType>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(title)js"sv)
			{
				result.title = ModifiedResponse<response::StringType>::parse<TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
			if (member.first == R"js(isComplete)js"sv)
			{
				result.isComplete = ModifiedResponse<response::BooleanType>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(__typename)js"sv)
			{
				result._typename = ModifiedResponse<response::StringType>::parse(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

template <>
Response::tasks_TaskConnection::edges_TaskEdge ModifiedResponse<Response::tasks_TaskConnection::edges_TaskEdge>::parse(response::Value&& response)
{
	Response::tasks_TaskConnection::edges_TaskEdge result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(node)js"sv)
			{
				result.node = ModifiedResponse<Response::tasks_TaskConnection::edges_TaskEdge::node_Task>::parse<TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

template <>
Response::tasks_TaskConnection ModifiedResponse<Response::tasks_TaskConnection>::parse(response::Value&& response)
{
	Response::tasks_TaskConnection result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(edges)js"sv)
			{
				result.edges = ModifiedResponse<Response::tasks_TaskConnection::edges_TaskEdge>::parse<TypeModifier::Nullable, TypeModifier::List, TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

template <>
Response::unreadCounts_FolderConnection::edges_FolderEdge::node_Folder ModifiedResponse<Response::unreadCounts_FolderConnection::edges_FolderEdge::node_Folder>::parse(response::Value&& response)
{
	Response::unreadCounts_FolderConnection::edges_FolderEdge::node_Folder result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(id)js"sv)
			{
				result.id = ModifiedResponse<response::IdType>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(name)js"sv)
			{
				result.name = ModifiedResponse<response::StringType>::parse<TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
			if (member.first == R"js(unreadCount)js"sv)
			{
				result.unreadCount = ModifiedResponse<response::IntType>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(__typename)js"sv)
			{
				result._typename = ModifiedResponse<response::StringType>::parse(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

template <>
Response::unreadCounts_FolderConnection::edges_FolderEdge ModifiedResponse<Response::unreadCounts_FolderConnection::edges_FolderEdge>::parse(response::Value&& response)
{
	Response::unreadCounts_FolderConnection::edges_FolderEdge result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(node)js"sv)
			{
				result.node = ModifiedResponse<Response::unreadCounts_FolderConnection::edges_FolderEdge::node_Folder>::parse<TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

template <>
Response::unreadCounts_FolderConnection ModifiedResponse<Response::unreadCounts_FolderConnection>::parse(response::Value&& response)
{
	Response::unreadCounts_FolderConnection result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(edges)js"sv)
			{
				result.edges = ModifiedResponse<Response::unreadCounts_FolderConnection::edges_FolderEdge>::parse<TypeModifier::Nullable, TypeModifier::List, TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

template <>
Response::anyType_UnionType ModifiedResponse<Response::anyType_UnionType>::parse(response::Value&& response)
{
	Response::anyType_UnionType result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(__typename)js"sv)
			{
				result._typename = ModifiedResponse<response::StringType>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(id)js"sv)
			{
				result.id = ModifiedResponse<response::IdType>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(title)js"sv)
			{
				result.title = ModifiedResponse<response::StringType>::parse<TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
			if (member.first == R"js(isComplete)js"sv)
			{
				result.isComplete = ModifiedResponse<response::BooleanType>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(subject)js"sv)
			{
				result.subject = ModifiedResponse<response::StringType>::parse<TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
			if (member.first == R"js(when)js"sv)
			{
				result.when = ModifiedResponse<response::Value>::parse<TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
			if (member.first == R"js(isNow)js"sv)
			{
				result.isNow = ModifiedResponse<response::BooleanType>::parse(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

namespace query::Query {

const std::string& GetRequestText() noexcept
{
	static const auto s_request = R"gql(
		# Copyright (c) Microsoft Corporation. All rights reserved.
		# Licensed under the MIT License.
		
		query {
		  appointments {
		    edges {
		      node {
		        id
		        subject
		        when
		        isNow
		        __typename
		      }
		    }
		  }
		  tasks {
		    edges {
		      node {
		        id
		        title
		        isComplete
		        __typename
		      }
		    }
		  }
		  unreadCounts {
		    edges {
		      node {
		        id
		        name
		        unreadCount
		        __typename
		      }
		    }
		  }
		
		  # Read a field with an enum type
		  testTaskState
		
		  # Try a field with a union type
		  anyType(ids: ["ZmFrZVRhc2tJZA=="]) {
		    __typename
		    ...on Node {
		      id
		    }
		    ...on Task {
		      id
		      title
		      isComplete
		    }
		    ...on Appointment {
		      id
		      subject
		      when
		      isNow
		    }
		  }
		}
	)gql"s;

	return s_request;
}

const peg::ast& GetRequestObject() noexcept
{
	static const auto s_request = []() noexcept {
		auto ast = peg::parseString(GetRequestText());

		// This has already been validated against the schema by clientgen.
		ast.validated = true;

		return ast;
	}();

	return s_request;
}

Response parseResponse(response::Value&& response)
{
	Response result;

	if (response.type() == response::Type::Map)
	{
		auto members = response.release<response::MapType>();

		for (auto& member : members)
		{
			if (member.first == R"js(appointments)js"sv)
			{
				result.appointments = ModifiedResponse<Response::appointments_AppointmentConnection>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(tasks)js"sv)
			{
				result.tasks = ModifiedResponse<Response::tasks_TaskConnection>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(unreadCounts)js"sv)
			{
				result.unreadCounts = ModifiedResponse<Response::unreadCounts_FolderConnection>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(testTaskState)js"sv)
			{
				result.testTaskState = ModifiedResponse<TaskState>::parse(std::move(member.second));
				continue;
			}
			if (member.first == R"js(anyType)js"sv)
			{
				result.anyType = ModifiedResponse<Response::anyType_UnionType>::parse<TypeModifier::List, TypeModifier::Nullable>(std::move(member.second));
				continue;
			}
		}
	}

	return result;
}

} // namespace query::Query
} // namespace graphql::client
