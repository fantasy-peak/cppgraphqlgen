// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "TodaySchema.h"
#include "Introspection.h"

#include <algorithm>
#include <functional>
#include <sstream>
#include <unordered_map>
#include <exception>

namespace facebook {
namespace graphql {
namespace service {

template <>
today::TaskState ModifiedArgument<today::TaskState>::convert(rapidjson::Document::AllocatorType&, const rapidjson::Value& value)
{
	static const std::unordered_map<std::string, today::TaskState> s_names = {
		{ "New", today::TaskState::New },
		{ "Started", today::TaskState::Started },
		{ "Complete", today::TaskState::Complete },
		{ "Unassigned", today::TaskState::Unassigned }
	};

	auto itr = s_names.find(value.GetString());

	if (itr == s_names.cend())
	{
		throw service::schema_exception({ "not a valid TaskState value" });
	}

	return itr->second;
}

template <>
std::future<rapidjson::Value> service::ModifiedResult<today::TaskState>::convert(std::future<today::TaskState>&& value, ResolverParams&&)
{
	static const std::string s_names[] = {
		"New",
		"Started",
		"Complete",
		"Unassigned"
	};

	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef(s_names[static_cast<size_t>(value.get())].c_str()));
	promise.set_value(std::move(result));

	return promise.get_future();
}

template <>
today::CompleteTaskInput ModifiedArgument<today::CompleteTaskInput>::convert(rapidjson::Document::AllocatorType& allocator, const rapidjson::Value& value)
{
	const auto defaultValue = []()
	{
		rapidjson::Document values(rapidjson::Type::kObjectType);
		auto& valuesAllocator = values.GetAllocator();
		rapidjson::Document parsed;
		rapidjson::Value entry;

		parsed.Parse(R"js(true)js");
		entry.CopyFrom(parsed, valuesAllocator);
		values.AddMember(rapidjson::StringRef("isComplete"), entry, valuesAllocator);

		return values;
	}();

	auto valueId = service::ModifiedArgument<std::vector<uint8_t>>::require(allocator, "id", value.GetObject());
	auto pairIsComplete = service::ModifiedArgument<bool>::find<service::TypeModifier::Nullable>(allocator, "isComplete", value.GetObject());
	auto valueIsComplete = (pairIsComplete.second
		? std::move(pairIsComplete.first)
		: service::ModifiedArgument<bool>::require<service::TypeModifier::Nullable>(allocator, "isComplete", defaultValue.GetObject()));
	auto valueClientMutationId = service::ModifiedArgument<std::string>::require<service::TypeModifier::Nullable>(allocator, "clientMutationId", value.GetObject());

	return {
		std::move(valueId),
		std::move(valueIsComplete),
		std::move(valueClientMutationId)
	};
}

} /* namespace service */

namespace today {
namespace object {

Query::Query()
	: service::Object({
		"Query"
	}, {
		{ "node", [this](service::ResolverParams&& params) { return resolveNode(std::move(params)); } },
		{ "appointments", [this](service::ResolverParams&& params) { return resolveAppointments(std::move(params)); } },
		{ "tasks", [this](service::ResolverParams&& params) { return resolveTasks(std::move(params)); } },
		{ "unreadCounts", [this](service::ResolverParams&& params) { return resolveUnreadCounts(std::move(params)); } },
		{ "appointmentsById", [this](service::ResolverParams&& params) { return resolveAppointmentsById(std::move(params)); } },
		{ "tasksById", [this](service::ResolverParams&& params) { return resolveTasksById(std::move(params)); } },
		{ "unreadCountsById", [this](service::ResolverParams&& params) { return resolveUnreadCountsById(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } },
		{ "__schema", [this](service::ResolverParams&& params) { return resolve__schema(std::move(params)); } },
		{ "__type", [this](service::ResolverParams&& params) { return resolve__type(std::move(params)); } }
	})
	, _schema(std::make_shared<introspection::Schema>())
{
	introspection::AddTypesToSchema(_schema);
	today::AddTypesToSchema(_schema);
}

std::future<rapidjson::Value> Query::resolveNode(service::ResolverParams&& params)
{
	auto argId = service::ModifiedArgument<std::vector<uint8_t>>::require(params.allocator, "id", params.arguments);
	auto result = getNode(params.requestId, std::move(argId));

	return service::ModifiedResult<service::Object>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Query::resolveAppointments(service::ResolverParams&& params)
{
	auto argFirst = service::ModifiedArgument<int>::require<service::TypeModifier::Nullable>(params.allocator, "first", params.arguments);
	auto argAfter = service::ModifiedArgument<rapidjson::Value>::require<service::TypeModifier::Nullable>(params.allocator, "after", params.arguments);
	auto argLast = service::ModifiedArgument<int>::require<service::TypeModifier::Nullable>(params.allocator, "last", params.arguments);
	auto argBefore = service::ModifiedArgument<rapidjson::Value>::require<service::TypeModifier::Nullable>(params.allocator, "before", params.arguments);
	auto result = getAppointments(params.requestId, std::move(argFirst), std::move(argAfter), std::move(argLast), std::move(argBefore));

	return service::ModifiedResult<AppointmentConnection>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Query::resolveTasks(service::ResolverParams&& params)
{
	auto argFirst = service::ModifiedArgument<int>::require<service::TypeModifier::Nullable>(params.allocator, "first", params.arguments);
	auto argAfter = service::ModifiedArgument<rapidjson::Value>::require<service::TypeModifier::Nullable>(params.allocator, "after", params.arguments);
	auto argLast = service::ModifiedArgument<int>::require<service::TypeModifier::Nullable>(params.allocator, "last", params.arguments);
	auto argBefore = service::ModifiedArgument<rapidjson::Value>::require<service::TypeModifier::Nullable>(params.allocator, "before", params.arguments);
	auto result = getTasks(params.requestId, std::move(argFirst), std::move(argAfter), std::move(argLast), std::move(argBefore));

	return service::ModifiedResult<TaskConnection>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Query::resolveUnreadCounts(service::ResolverParams&& params)
{
	auto argFirst = service::ModifiedArgument<int>::require<service::TypeModifier::Nullable>(params.allocator, "first", params.arguments);
	auto argAfter = service::ModifiedArgument<rapidjson::Value>::require<service::TypeModifier::Nullable>(params.allocator, "after", params.arguments);
	auto argLast = service::ModifiedArgument<int>::require<service::TypeModifier::Nullable>(params.allocator, "last", params.arguments);
	auto argBefore = service::ModifiedArgument<rapidjson::Value>::require<service::TypeModifier::Nullable>(params.allocator, "before", params.arguments);
	auto result = getUnreadCounts(params.requestId, std::move(argFirst), std::move(argAfter), std::move(argLast), std::move(argBefore));

	return service::ModifiedResult<FolderConnection>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Query::resolveAppointmentsById(service::ResolverParams&& params)
{
	auto argIds = service::ModifiedArgument<std::vector<uint8_t>>::require<service::TypeModifier::List>(params.allocator, "ids", params.arguments);
	auto result = getAppointmentsById(params.requestId, std::move(argIds));

	return service::ModifiedResult<Appointment>::convert<service::TypeModifier::List, service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Query::resolveTasksById(service::ResolverParams&& params)
{
	auto argIds = service::ModifiedArgument<std::vector<uint8_t>>::require<service::TypeModifier::List>(params.allocator, "ids", params.arguments);
	auto result = getTasksById(params.requestId, std::move(argIds));

	return service::ModifiedResult<Task>::convert<service::TypeModifier::List, service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Query::resolveUnreadCountsById(service::ResolverParams&& params)
{
	auto argIds = service::ModifiedArgument<std::vector<uint8_t>>::require<service::TypeModifier::List>(params.allocator, "ids", params.arguments);
	auto result = getUnreadCountsById(params.requestId, std::move(argIds));

	return service::ModifiedResult<Folder>::convert<service::TypeModifier::List, service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Query::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("Query"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

std::future<rapidjson::Value> Query::resolve__schema(service::ResolverParams&& params)
{
	std::promise<std::shared_ptr<service::Object>> promise;

	promise.set_value(std::static_pointer_cast<service::Object>(_schema));

	return service::ModifiedResult<service::Object>::convert(promise.get_future(), std::move(params));
}

std::future<rapidjson::Value> Query::resolve__type(service::ResolverParams&& params)
{
	auto argName = service::ModifiedArgument<std::string>::require(params.allocator, "name", params.arguments);
	std::promise<std::shared_ptr<introspection::object::__Type>> promise;

	promise.set_value(_schema->LookupType(argName));

	return service::ModifiedResult<introspection::object::__Type>::convert<service::TypeModifier::Nullable>(promise.get_future(), std::move(params));
}

PageInfo::PageInfo()
	: service::Object({
		"PageInfo"
	}, {
		{ "hasNextPage", [this](service::ResolverParams&& params) { return resolveHasNextPage(std::move(params)); } },
		{ "hasPreviousPage", [this](service::ResolverParams&& params) { return resolveHasPreviousPage(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> PageInfo::resolveHasNextPage(service::ResolverParams&& params)
{
	auto result = getHasNextPage(params.requestId);

	return service::ModifiedResult<bool>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> PageInfo::resolveHasPreviousPage(service::ResolverParams&& params)
{
	auto result = getHasPreviousPage(params.requestId);

	return service::ModifiedResult<bool>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> PageInfo::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("PageInfo"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

AppointmentEdge::AppointmentEdge()
	: service::Object({
		"AppointmentEdge"
	}, {
		{ "node", [this](service::ResolverParams&& params) { return resolveNode(std::move(params)); } },
		{ "cursor", [this](service::ResolverParams&& params) { return resolveCursor(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> AppointmentEdge::resolveNode(service::ResolverParams&& params)
{
	auto result = getNode(params.requestId);

	return service::ModifiedResult<Appointment>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> AppointmentEdge::resolveCursor(service::ResolverParams&& params)
{
	auto result = getCursor(params.requestId, params.allocator);

	return service::ModifiedResult<rapidjson::Value>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> AppointmentEdge::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("AppointmentEdge"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

AppointmentConnection::AppointmentConnection()
	: service::Object({
		"AppointmentConnection"
	}, {
		{ "pageInfo", [this](service::ResolverParams&& params) { return resolvePageInfo(std::move(params)); } },
		{ "edges", [this](service::ResolverParams&& params) { return resolveEdges(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> AppointmentConnection::resolvePageInfo(service::ResolverParams&& params)
{
	auto result = getPageInfo(params.requestId);

	return service::ModifiedResult<PageInfo>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> AppointmentConnection::resolveEdges(service::ResolverParams&& params)
{
	auto result = getEdges(params.requestId);

	return service::ModifiedResult<AppointmentEdge>::convert<service::TypeModifier::Nullable, service::TypeModifier::List, service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> AppointmentConnection::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("AppointmentConnection"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

TaskEdge::TaskEdge()
	: service::Object({
		"TaskEdge"
	}, {
		{ "node", [this](service::ResolverParams&& params) { return resolveNode(std::move(params)); } },
		{ "cursor", [this](service::ResolverParams&& params) { return resolveCursor(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> TaskEdge::resolveNode(service::ResolverParams&& params)
{
	auto result = getNode(params.requestId);

	return service::ModifiedResult<Task>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> TaskEdge::resolveCursor(service::ResolverParams&& params)
{
	auto result = getCursor(params.requestId, params.allocator);

	return service::ModifiedResult<rapidjson::Value>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> TaskEdge::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("TaskEdge"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

TaskConnection::TaskConnection()
	: service::Object({
		"TaskConnection"
	}, {
		{ "pageInfo", [this](service::ResolverParams&& params) { return resolvePageInfo(std::move(params)); } },
		{ "edges", [this](service::ResolverParams&& params) { return resolveEdges(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> TaskConnection::resolvePageInfo(service::ResolverParams&& params)
{
	auto result = getPageInfo(params.requestId);

	return service::ModifiedResult<PageInfo>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> TaskConnection::resolveEdges(service::ResolverParams&& params)
{
	auto result = getEdges(params.requestId);

	return service::ModifiedResult<TaskEdge>::convert<service::TypeModifier::Nullable, service::TypeModifier::List, service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> TaskConnection::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("TaskConnection"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

FolderEdge::FolderEdge()
	: service::Object({
		"FolderEdge"
	}, {
		{ "node", [this](service::ResolverParams&& params) { return resolveNode(std::move(params)); } },
		{ "cursor", [this](service::ResolverParams&& params) { return resolveCursor(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> FolderEdge::resolveNode(service::ResolverParams&& params)
{
	auto result = getNode(params.requestId);

	return service::ModifiedResult<Folder>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> FolderEdge::resolveCursor(service::ResolverParams&& params)
{
	auto result = getCursor(params.requestId, params.allocator);

	return service::ModifiedResult<rapidjson::Value>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> FolderEdge::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("FolderEdge"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

FolderConnection::FolderConnection()
	: service::Object({
		"FolderConnection"
	}, {
		{ "pageInfo", [this](service::ResolverParams&& params) { return resolvePageInfo(std::move(params)); } },
		{ "edges", [this](service::ResolverParams&& params) { return resolveEdges(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> FolderConnection::resolvePageInfo(service::ResolverParams&& params)
{
	auto result = getPageInfo(params.requestId);

	return service::ModifiedResult<PageInfo>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> FolderConnection::resolveEdges(service::ResolverParams&& params)
{
	auto result = getEdges(params.requestId);

	return service::ModifiedResult<FolderEdge>::convert<service::TypeModifier::Nullable, service::TypeModifier::List, service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> FolderConnection::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("FolderConnection"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

CompleteTaskPayload::CompleteTaskPayload()
	: service::Object({
		"CompleteTaskPayload"
	}, {
		{ "task", [this](service::ResolverParams&& params) { return resolveTask(std::move(params)); } },
		{ "clientMutationId", [this](service::ResolverParams&& params) { return resolveClientMutationId(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> CompleteTaskPayload::resolveTask(service::ResolverParams&& params)
{
	auto result = getTask(params.requestId);

	return service::ModifiedResult<Task>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> CompleteTaskPayload::resolveClientMutationId(service::ResolverParams&& params)
{
	auto result = getClientMutationId(params.requestId);

	return service::ModifiedResult<std::string>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> CompleteTaskPayload::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("CompleteTaskPayload"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

Mutation::Mutation()
	: service::Object({
		"Mutation"
	}, {
		{ "completeTask", [this](service::ResolverParams&& params) { return resolveCompleteTask(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> Mutation::resolveCompleteTask(service::ResolverParams&& params)
{
	auto argInput = service::ModifiedArgument<CompleteTaskInput>::require(params.allocator, "input", params.arguments);
	auto result = getCompleteTask(params.requestId, std::move(argInput));

	return service::ModifiedResult<CompleteTaskPayload>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Mutation::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("Mutation"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

Subscription::Subscription()
	: service::Object({
		"Subscription"
	}, {
		{ "nextAppointmentChange", [this](service::ResolverParams&& params) { return resolveNextAppointmentChange(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> Subscription::resolveNextAppointmentChange(service::ResolverParams&& params)
{
	auto result = getNextAppointmentChange(params.requestId);

	return service::ModifiedResult<Appointment>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Subscription::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("Subscription"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

Appointment::Appointment()
	: service::Object({
		"Node",
		"Appointment"
	}, {
		{ "id", [this](service::ResolverParams&& params) { return resolveId(std::move(params)); } },
		{ "when", [this](service::ResolverParams&& params) { return resolveWhen(std::move(params)); } },
		{ "subject", [this](service::ResolverParams&& params) { return resolveSubject(std::move(params)); } },
		{ "isNow", [this](service::ResolverParams&& params) { return resolveIsNow(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> Appointment::resolveId(service::ResolverParams&& params)
{
	auto result = getId(params.requestId);

	return service::ModifiedResult<std::vector<uint8_t>>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Appointment::resolveWhen(service::ResolverParams&& params)
{
	auto result = getWhen(params.requestId, params.allocator);

	return service::ModifiedResult<rapidjson::Value>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Appointment::resolveSubject(service::ResolverParams&& params)
{
	auto result = getSubject(params.requestId);

	return service::ModifiedResult<std::string>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Appointment::resolveIsNow(service::ResolverParams&& params)
{
	auto result = getIsNow(params.requestId);

	return service::ModifiedResult<bool>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Appointment::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("Appointment"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

Task::Task()
	: service::Object({
		"Node",
		"Task"
	}, {
		{ "id", [this](service::ResolverParams&& params) { return resolveId(std::move(params)); } },
		{ "title", [this](service::ResolverParams&& params) { return resolveTitle(std::move(params)); } },
		{ "isComplete", [this](service::ResolverParams&& params) { return resolveIsComplete(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> Task::resolveId(service::ResolverParams&& params)
{
	auto result = getId(params.requestId);

	return service::ModifiedResult<std::vector<uint8_t>>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Task::resolveTitle(service::ResolverParams&& params)
{
	auto result = getTitle(params.requestId);

	return service::ModifiedResult<std::string>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Task::resolveIsComplete(service::ResolverParams&& params)
{
	auto result = getIsComplete(params.requestId);

	return service::ModifiedResult<bool>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Task::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("Task"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

Folder::Folder()
	: service::Object({
		"Node",
		"Folder"
	}, {
		{ "id", [this](service::ResolverParams&& params) { return resolveId(std::move(params)); } },
		{ "name", [this](service::ResolverParams&& params) { return resolveName(std::move(params)); } },
		{ "unreadCount", [this](service::ResolverParams&& params) { return resolveUnreadCount(std::move(params)); } },
		{ "__typename", [this](service::ResolverParams&& params) { return resolve__typename(std::move(params)); } }
	})
{
}

std::future<rapidjson::Value> Folder::resolveId(service::ResolverParams&& params)
{
	auto result = getId(params.requestId);

	return service::ModifiedResult<std::vector<uint8_t>>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Folder::resolveName(service::ResolverParams&& params)
{
	auto result = getName(params.requestId);

	return service::ModifiedResult<std::string>::convert<service::TypeModifier::Nullable>(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Folder::resolveUnreadCount(service::ResolverParams&& params)
{
	auto result = getUnreadCount(params.requestId);

	return service::ModifiedResult<int>::convert(std::move(result), std::move(params));
}

std::future<rapidjson::Value> Folder::resolve__typename(service::ResolverParams&&)
{
	std::promise<rapidjson::Value> promise;
	rapidjson::Value result(rapidjson::Type::kStringType);

	result.SetString(rapidjson::StringRef("Folder"));
	promise.set_value(std::move(result));

	return promise.get_future();
}

} /* namespace object */

Operations::Operations(std::shared_ptr<object::Query> query, std::shared_ptr<object::Mutation> mutation, std::shared_ptr<object::Subscription> subscription)
	: service::Request({
		{ "query", query },
		{ "mutation", mutation },
		{ "subscription", subscription }
	})
	, _query(std::move(query))
	, _mutation(std::move(mutation))
	, _subscription(std::move(subscription))
{
}

void AddTypesToSchema(std::shared_ptr<introspection::Schema> schema)
{
	schema->AddType("ItemCursor", std::make_shared<introspection::ScalarType>("ItemCursor", R"md()md"));
	schema->AddType("DateTime", std::make_shared<introspection::ScalarType>("DateTime", R"md()md"));
	auto typeTaskState= std::make_shared<introspection::EnumType>("TaskState", R"md()md");
	schema->AddType("TaskState", typeTaskState);
	auto typeCompleteTaskInput= std::make_shared<introspection::InputObjectType>("CompleteTaskInput", R"md()md");
	schema->AddType("CompleteTaskInput", typeCompleteTaskInput);
	auto typeNode= std::make_shared<introspection::InterfaceType>("Node", R"md(Node interface for Relay support)md");
	schema->AddType("Node", typeNode);
	auto typeQuery= std::make_shared<introspection::ObjectType>("Query", R"md(Root Query type)md");
	schema->AddType("Query", typeQuery);
	auto typePageInfo= std::make_shared<introspection::ObjectType>("PageInfo", R"md()md");
	schema->AddType("PageInfo", typePageInfo);
	auto typeAppointmentEdge= std::make_shared<introspection::ObjectType>("AppointmentEdge", R"md()md");
	schema->AddType("AppointmentEdge", typeAppointmentEdge);
	auto typeAppointmentConnection= std::make_shared<introspection::ObjectType>("AppointmentConnection", R"md()md");
	schema->AddType("AppointmentConnection", typeAppointmentConnection);
	auto typeTaskEdge= std::make_shared<introspection::ObjectType>("TaskEdge", R"md()md");
	schema->AddType("TaskEdge", typeTaskEdge);
	auto typeTaskConnection= std::make_shared<introspection::ObjectType>("TaskConnection", R"md()md");
	schema->AddType("TaskConnection", typeTaskConnection);
	auto typeFolderEdge= std::make_shared<introspection::ObjectType>("FolderEdge", R"md()md");
	schema->AddType("FolderEdge", typeFolderEdge);
	auto typeFolderConnection= std::make_shared<introspection::ObjectType>("FolderConnection", R"md()md");
	schema->AddType("FolderConnection", typeFolderConnection);
	auto typeCompleteTaskPayload= std::make_shared<introspection::ObjectType>("CompleteTaskPayload", R"md()md");
	schema->AddType("CompleteTaskPayload", typeCompleteTaskPayload);
	auto typeMutation= std::make_shared<introspection::ObjectType>("Mutation", R"md()md");
	schema->AddType("Mutation", typeMutation);
	auto typeSubscription= std::make_shared<introspection::ObjectType>("Subscription", R"md()md");
	schema->AddType("Subscription", typeSubscription);
	auto typeAppointment= std::make_shared<introspection::ObjectType>("Appointment", R"md()md");
	schema->AddType("Appointment", typeAppointment);
	auto typeTask= std::make_shared<introspection::ObjectType>("Task", R"md()md");
	schema->AddType("Task", typeTask);
	auto typeFolder= std::make_shared<introspection::ObjectType>("Folder", R"md()md");
	schema->AddType("Folder", typeFolder);

	typeTaskState->AddEnumValues({
		{ "New", R"md()md", nullptr },
		{ "Started", R"md()md", nullptr },
		{ "Complete", R"md()md", nullptr },
		{ "Unassigned", R"md()md", R"md(Need to deprecate an [enum value](https://facebook.github.io/graphql/June2018/#sec-Deprecation))md" }
	});

	rapidjson::Document defaultCompleteTaskInputid;
	defaultCompleteTaskInputid.Parse(R"js(null)js");
	rapidjson::Document defaultCompleteTaskInputisComplete;
	defaultCompleteTaskInputisComplete.Parse(R"js(true)js");
	rapidjson::Document defaultCompleteTaskInputclientMutationId;
	defaultCompleteTaskInputclientMutationId.Parse(R"js(null)js");
	typeCompleteTaskInput->AddInputValues({
		std::make_shared<introspection::InputValue>("id", R"md()md", std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ID")), defaultCompleteTaskInputid),
		std::make_shared<introspection::InputValue>("isComplete", R"md()md", schema->LookupType("Boolean"), defaultCompleteTaskInputisComplete),
		std::make_shared<introspection::InputValue>("clientMutationId", R"md()md", schema->LookupType("String"), defaultCompleteTaskInputclientMutationId)
	});

	typeNode->AddFields({
		std::make_shared<introspection::Field>("id", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ID")))
	});

	rapidjson::Document defaultQuerynodeid;
	defaultQuerynodeid.Parse(R"js(null)js");
	rapidjson::Document defaultQueryappointmentsfirst;
	defaultQueryappointmentsfirst.Parse(R"js(null)js");
	rapidjson::Document defaultQueryappointmentsafter;
	defaultQueryappointmentsafter.Parse(R"js(null)js");
	rapidjson::Document defaultQueryappointmentslast;
	defaultQueryappointmentslast.Parse(R"js(null)js");
	rapidjson::Document defaultQueryappointmentsbefore;
	defaultQueryappointmentsbefore.Parse(R"js(null)js");
	rapidjson::Document defaultQuerytasksfirst;
	defaultQuerytasksfirst.Parse(R"js(null)js");
	rapidjson::Document defaultQuerytasksafter;
	defaultQuerytasksafter.Parse(R"js(null)js");
	rapidjson::Document defaultQuerytaskslast;
	defaultQuerytaskslast.Parse(R"js(null)js");
	rapidjson::Document defaultQuerytasksbefore;
	defaultQuerytasksbefore.Parse(R"js(null)js");
	rapidjson::Document defaultQueryunreadCountsfirst;
	defaultQueryunreadCountsfirst.Parse(R"js(null)js");
	rapidjson::Document defaultQueryunreadCountsafter;
	defaultQueryunreadCountsafter.Parse(R"js(null)js");
	rapidjson::Document defaultQueryunreadCountslast;
	defaultQueryunreadCountslast.Parse(R"js(null)js");
	rapidjson::Document defaultQueryunreadCountsbefore;
	defaultQueryunreadCountsbefore.Parse(R"js(null)js");
	rapidjson::Document defaultQueryappointmentsByIdids;
	defaultQueryappointmentsByIdids.Parse(R"js(null)js");
	rapidjson::Document defaultQuerytasksByIdids;
	defaultQuerytasksByIdids.Parse(R"js(null)js");
	rapidjson::Document defaultQueryunreadCountsByIdids;
	defaultQueryunreadCountsByIdids.Parse(R"js(null)js");
	typeQuery->AddFields({
		std::make_shared<introspection::Field>("node", R"md([Object Identification](https://facebook.github.io/relay/docs/en/graphql-server-specification.html#object-identification))md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("id", R"md()md", std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ID")), defaultQuerynodeid)
		}), schema->LookupType("Node")),
		std::make_shared<introspection::Field>("appointments", R"md(Appointments [Connection](https://facebook.github.io/relay/docs/en/graphql-server-specification.html#connections))md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("first", R"md()md", schema->LookupType("Int"), defaultQueryappointmentsfirst),
			std::make_shared<introspection::InputValue>("after", R"md()md", schema->LookupType("ItemCursor"), defaultQueryappointmentsafter),
			std::make_shared<introspection::InputValue>("last", R"md()md", schema->LookupType("Int"), defaultQueryappointmentslast),
			std::make_shared<introspection::InputValue>("before", R"md()md", schema->LookupType("ItemCursor"), defaultQueryappointmentsbefore)
		}), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("AppointmentConnection"))),
		std::make_shared<introspection::Field>("tasks", R"md(Tasks [Connection](https://facebook.github.io/relay/docs/en/graphql-server-specification.html#connections))md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("first", R"md()md", schema->LookupType("Int"), defaultQuerytasksfirst),
			std::make_shared<introspection::InputValue>("after", R"md()md", schema->LookupType("ItemCursor"), defaultQuerytasksafter),
			std::make_shared<introspection::InputValue>("last", R"md()md", schema->LookupType("Int"), defaultQuerytaskslast),
			std::make_shared<introspection::InputValue>("before", R"md()md", schema->LookupType("ItemCursor"), defaultQuerytasksbefore)
		}), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("TaskConnection"))),
		std::make_shared<introspection::Field>("unreadCounts", R"md(Folder unread counts [Connection](https://facebook.github.io/relay/docs/en/graphql-server-specification.html#connections))md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("first", R"md()md", schema->LookupType("Int"), defaultQueryunreadCountsfirst),
			std::make_shared<introspection::InputValue>("after", R"md()md", schema->LookupType("ItemCursor"), defaultQueryunreadCountsafter),
			std::make_shared<introspection::InputValue>("last", R"md()md", schema->LookupType("Int"), defaultQueryunreadCountslast),
			std::make_shared<introspection::InputValue>("before", R"md()md", schema->LookupType("ItemCursor"), defaultQueryunreadCountsbefore)
		}), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("FolderConnection"))),
		std::make_shared<introspection::Field>("appointmentsById", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("ids", R"md()md", std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::LIST, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ID")))), defaultQueryappointmentsByIdids)
		}), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::LIST, schema->LookupType("Appointment")))),
		std::make_shared<introspection::Field>("tasksById", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("ids", R"md()md", std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::LIST, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ID")))), defaultQuerytasksByIdids)
		}), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::LIST, schema->LookupType("Task")))),
		std::make_shared<introspection::Field>("unreadCountsById", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("ids", R"md()md", std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::LIST, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ID")))), defaultQueryunreadCountsByIdids)
		}), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::LIST, schema->LookupType("Folder"))))
	});
	typePageInfo->AddFields({
		std::make_shared<introspection::Field>("hasNextPage", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("Boolean"))),
		std::make_shared<introspection::Field>("hasPreviousPage", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("Boolean")))
	});
	typeAppointmentEdge->AddFields({
		std::make_shared<introspection::Field>("node", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("Appointment")),
		std::make_shared<introspection::Field>("cursor", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ItemCursor")))
	});
	typeAppointmentConnection->AddFields({
		std::make_shared<introspection::Field>("pageInfo", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("PageInfo"))),
		std::make_shared<introspection::Field>("edges", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::LIST, schema->LookupType("AppointmentEdge"))))
	});
	typeTaskEdge->AddFields({
		std::make_shared<introspection::Field>("node", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("Task")),
		std::make_shared<introspection::Field>("cursor", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ItemCursor")))
	});
	typeTaskConnection->AddFields({
		std::make_shared<introspection::Field>("pageInfo", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("PageInfo"))),
		std::make_shared<introspection::Field>("edges", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::LIST, schema->LookupType("TaskEdge"))))
	});
	typeFolderEdge->AddFields({
		std::make_shared<introspection::Field>("node", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("Folder")),
		std::make_shared<introspection::Field>("cursor", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ItemCursor")))
	});
	typeFolderConnection->AddFields({
		std::make_shared<introspection::Field>("pageInfo", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("PageInfo"))),
		std::make_shared<introspection::Field>("edges", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, std::make_shared<introspection::WrapperType>(introspection::__TypeKind::LIST, schema->LookupType("FolderEdge"))))
	});
	typeCompleteTaskPayload->AddFields({
		std::make_shared<introspection::Field>("task", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("Task")),
		std::make_shared<introspection::Field>("clientMutationId", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("String"))
	});
	rapidjson::Document defaultMutationcompleteTaskinput;
	defaultMutationcompleteTaskinput.Parse(R"js(null)js");
	typeMutation->AddFields({
		std::make_shared<introspection::Field>("completeTask", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>({
			std::make_shared<introspection::InputValue>("input", R"md()md", std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("CompleteTaskInput")), defaultMutationcompleteTaskinput)
		}), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("CompleteTaskPayload")))
	});
	typeSubscription->AddFields({
		std::make_shared<introspection::Field>("nextAppointmentChange", R"md()md", std::unique_ptr<std::string>(new std::string(R"md(Need to deprecate a [field](https://facebook.github.io/graphql/June2018/#sec-Deprecation))md")), std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("Appointment"))
	});
	typeAppointment->AddInterfaces({
		typeNode
	});
	typeAppointment->AddFields({
		std::make_shared<introspection::Field>("id", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ID"))),
		std::make_shared<introspection::Field>("when", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("DateTime")),
		std::make_shared<introspection::Field>("subject", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("String")),
		std::make_shared<introspection::Field>("isNow", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("Boolean")))
	});
	typeTask->AddInterfaces({
		typeNode
	});
	typeTask->AddFields({
		std::make_shared<introspection::Field>("id", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ID"))),
		std::make_shared<introspection::Field>("title", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("String")),
		std::make_shared<introspection::Field>("isComplete", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("Boolean")))
	});
	typeFolder->AddInterfaces({
		typeNode
	});
	typeFolder->AddFields({
		std::make_shared<introspection::Field>("id", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("ID"))),
		std::make_shared<introspection::Field>("name", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), schema->LookupType("String")),
		std::make_shared<introspection::Field>("unreadCount", R"md()md", std::unique_ptr<std::string>(nullptr), std::vector<std::shared_ptr<introspection::InputValue>>(), std::make_shared<introspection::WrapperType>(introspection::__TypeKind::NON_NULL, schema->LookupType("Int")))
	});

	schema->AddQueryType(typeQuery);
	schema->AddMutationType(typeMutation);
	schema->AddSubscriptionType(typeSubscription);
}

} /* namespace today */
} /* namespace graphql */
} /* namespace facebook */