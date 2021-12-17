// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifndef HERODATA_H
#define HERODATA_H

#include <memory>
#include <variant>

namespace graphql::learn {

class Droid;
class Human;

using SharedHero = std::variant<std::shared_ptr<Human>, std::shared_ptr<Droid>>;
using WeakHero = std::variant<std::weak_ptr<Human>, std::weak_ptr<Droid>>;

} // namespace graphql::learn

#endif // HERODATA_H
