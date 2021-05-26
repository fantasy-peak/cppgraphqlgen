// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#include "SubscribeClient.h"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <sstream>
#include <string_view>

using namespace std::literals;

namespace graphql::subscription::TestSubscription {

const std::string& GetRequestText() noexcept
{
	static const auto s_request = R"gql(
		# Copyright (c) Microsoft Corporation. All rights reserved.
		# Licensed under the MIT License.
		
		subscription TestSubscription {
		  nextAppointment: nextAppointmentChange {
		    nextAppointmentId: id
		    when
		    subject
		    isNow
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

	// std::optional<nextAppointment_Appointment> nextAppointment;

	return result;
}

} /* namespace graphql::subscription::TestSubscription */
