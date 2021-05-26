// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef QUERYCLIENT_H
#define QUERYCLIENT_H

// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "graphqlservice/GraphQLParse.h"
#include "graphqlservice/GraphQLResponse.h"

#include "graphqlservice/internal/Version.h"

// Check if the library version is compatible with clientgen 3.6.0
static_assert(graphql::internal::MajorVersion == 3, "regenerate with clientgen: major version mismatch");
static_assert(graphql::internal::MinorVersion == 6, "regenerate with clientgen: minor version mismatch");

#include <optional>
#include <string>
#include <vector>


/// <summary>
/// Operation: query (unnamed)
/// </summary>
/// <code class="language-graphql">
/// # Copyright (c) Microsoft Corporation. All rights reserved.
/// # Licensed under the MIT License.
/// 
/// query {
///   appointments {
///     edges {
///       node {
///         id
///         subject
///         when
///         isNow
///         __typename
///       }
///     }
///   }
///   tasks {
///     edges {
///       node {
///         id
///         title
///         isComplete
///         __typename
///       }
///     }
///   }
///   unreadCounts {
///     edges {
///       node {
///         id
///         name
///         unreadCount
///         __typename
///       }
///     }
///   }
/// 
///   # Read a field with an enum type
///   testTaskState
/// }
/// </code>
namespace graphql::client::query::Query {

// Return the original text of the request document.
const std::string& GetRequestText() noexcept;

// Return a pre-parsed, pre-validated request object.
const peg::ast& GetRequestObject() noexcept;

enum class TaskState
{
	New,
	Started,
	Complete,
	Unassigned,
};

struct Response
{
	struct appointments_AppointmentConnection
	{
		struct edges_AppointmentEdge
		{
			struct node_Appointment
			{
				response::IdType id;
				std::optional<response::StringType> subject;
				std::optional<response::Value> when;
				response::BooleanType isNow;
				response::StringType _typename;
			};

			std::optional<node_Appointment> node;
		};

		std::optional<std::vector<std::optional<edges_AppointmentEdge>>> edges;
	};

	appointments_AppointmentConnection appointments;

	struct tasks_TaskConnection
	{
		struct edges_TaskEdge
		{
			struct node_Task
			{
				response::IdType id;
				std::optional<response::StringType> title;
				response::BooleanType isComplete;
				response::StringType _typename;
			};

			std::optional<node_Task> node;
		};

		std::optional<std::vector<std::optional<edges_TaskEdge>>> edges;
	};

	tasks_TaskConnection tasks;

	struct unreadCounts_FolderConnection
	{
		struct edges_FolderEdge
		{
			struct node_Folder
			{
				response::IdType id;
				std::optional<response::StringType> name;
				response::IntType unreadCount;
				response::StringType _typename;
			};

			std::optional<node_Folder> node;
		};

		std::optional<std::vector<std::optional<edges_FolderEdge>>> edges;
	};

	unreadCounts_FolderConnection unreadCounts;

	std::optional<TaskState> testTaskState;
};

Response parseResponse(response::Value&& response);

} // namespace graphql::client::query::Query

#endif // QUERYCLIENT_H
