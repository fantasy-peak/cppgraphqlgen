// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING! Do not edit this file manually, your changes will be overwritten.

#pragma once

#ifndef VALIDATIONSCHEMA_H
#define VALIDATIONSCHEMA_H

#include "graphqlservice/internal/Schema.h"

// Check if the library version is compatible with schemagen 4.0.0
static_assert(graphql::internal::MajorVersion == 4, "regenerate with schemagen: major version mismatch");
static_assert(graphql::internal::MinorVersion == 0, "regenerate with schemagen: minor version mismatch");

#include <memory>
#include <string>
#include <vector>

namespace graphql {
namespace validation {

enum class DogCommand
{
	SIT,
	DOWN,
	HEEL
};

enum class CatCommand
{
	JUMP
};

struct ComplexInput
{
	std::optional<response::StringType> name;
	std::optional<response::StringType> owner;
};

namespace object {

class Query;
class Dog;
class Alien;
class Human;
class Cat;
class Mutation;
class MutateDogResult;
class Subscription;
class Message;
class Arguments;

} // namespace object

struct Sentient;
struct Pet;

struct Sentient
{
	virtual service::FieldResult<response::StringType> getName(service::FieldParams&& params) const = 0;
};

struct Pet
{
	virtual service::FieldResult<response::StringType> getName(service::FieldParams&& params) const = 0;
};

namespace object {

class Query
	: public service::Object
{
private:
	service::AwaitableResolver resolveDog(service::ResolverParams&& params);
	service::AwaitableResolver resolveHuman(service::ResolverParams&& params);
	service::AwaitableResolver resolvePet(service::ResolverParams&& params);
	service::AwaitableResolver resolveCatOrDog(service::ResolverParams&& params);
	service::AwaitableResolver resolveArguments(service::ResolverParams&& params);
	service::AwaitableResolver resolveFindDog(service::ResolverParams&& params);
	service::AwaitableResolver resolveBooleanList(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);
	service::AwaitableResolver resolve_schema(service::ResolverParams&& params);
	service::AwaitableResolver resolve_type(service::ResolverParams&& params);

	std::shared_ptr<schema::Schema> _schema;

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::shared_ptr<Dog>> getDog(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<Human>> getHuman(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<service::Object>> getPet(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<service::Object>> getCatOrDog(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<Arguments>> getArguments(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::shared_ptr<Dog>> getFindDog(service::FieldParams&& params, std::optional<ComplexInput>&& complexArg) const = 0;
		virtual service::FieldResult<std::optional<response::BooleanType>> getBooleanList(service::FieldParams&& params, std::optional<std::vector<response::BooleanType>>&& booleanListArgArg) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::shared_ptr<Dog>> getDog(service::FieldParams&& params) const final
		{
			return _pimpl->getDog(std::move(params));
		}

		service::FieldResult<std::shared_ptr<Human>> getHuman(service::FieldParams&& params) const final
		{
			return _pimpl->getHuman(std::move(params));
		}

		service::FieldResult<std::shared_ptr<service::Object>> getPet(service::FieldParams&& params) const final
		{
			return _pimpl->getPet(std::move(params));
		}

		service::FieldResult<std::shared_ptr<service::Object>> getCatOrDog(service::FieldParams&& params) const final
		{
			return _pimpl->getCatOrDog(std::move(params));
		}

		service::FieldResult<std::shared_ptr<Arguments>> getArguments(service::FieldParams&& params) const final
		{
			return _pimpl->getArguments(std::move(params));
		}

		service::FieldResult<std::shared_ptr<Dog>> getFindDog(service::FieldParams&& params, std::optional<ComplexInput>&& complexArg) const final
		{
			return _pimpl->getFindDog(std::move(params), std::move(complexArg));
		}

		service::FieldResult<std::optional<response::BooleanType>> getBooleanList(service::FieldParams&& params, std::optional<std::vector<response::BooleanType>>&& booleanListArgArg) const final
		{
			return _pimpl->getBooleanList(std::move(params), std::move(booleanListArgArg));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Query(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Query(std::shared_ptr<T> pimpl)
		: Query { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~Query();
};

class Dog
	: public service::Object
{
private:
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolveNickname(service::ResolverParams&& params);
	service::AwaitableResolver resolveBarkVolume(service::ResolverParams&& params);
	service::AwaitableResolver resolveDoesKnowCommand(service::ResolverParams&& params);
	service::AwaitableResolver resolveIsHousetrained(service::ResolverParams&& params);
	service::AwaitableResolver resolveOwner(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
		: Pet
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::optional<response::StringType>> getNickname(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<std::optional<response::IntType>> getBarkVolume(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<response::BooleanType> getDoesKnowCommand(service::FieldParams&& params, DogCommand&& dogCommandArg) const = 0;
		virtual service::FieldResult<response::BooleanType> getIsHousetrained(service::FieldParams&& params, std::optional<response::BooleanType>&& atOtherHomesArg) const = 0;
		virtual service::FieldResult<std::shared_ptr<Human>> getOwner(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<response::StringType> getName(service::FieldParams&& params) const final
		{
			return _pimpl->getName(std::move(params));
		}

		service::FieldResult<std::optional<response::StringType>> getNickname(service::FieldParams&& params) const final
		{
			return _pimpl->getNickname(std::move(params));
		}

		service::FieldResult<std::optional<response::IntType>> getBarkVolume(service::FieldParams&& params) const final
		{
			return _pimpl->getBarkVolume(std::move(params));
		}

		service::FieldResult<response::BooleanType> getDoesKnowCommand(service::FieldParams&& params, DogCommand&& dogCommandArg) const final
		{
			return _pimpl->getDoesKnowCommand(std::move(params), std::move(dogCommandArg));
		}

		service::FieldResult<response::BooleanType> getIsHousetrained(service::FieldParams&& params, std::optional<response::BooleanType>&& atOtherHomesArg) const final
		{
			return _pimpl->getIsHousetrained(std::move(params), std::move(atOtherHomesArg));
		}

		service::FieldResult<std::shared_ptr<Human>> getOwner(service::FieldParams&& params) const final
		{
			return _pimpl->getOwner(std::move(params));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Dog(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Dog(std::shared_ptr<T> pimpl)
		: Dog { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~Dog();
};

class Alien
	: public service::Object
{
private:
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolveHomePlanet(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
		: Sentient
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::optional<response::StringType>> getHomePlanet(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<response::StringType> getName(service::FieldParams&& params) const final
		{
			return _pimpl->getName(std::move(params));
		}

		service::FieldResult<std::optional<response::StringType>> getHomePlanet(service::FieldParams&& params) const final
		{
			return _pimpl->getHomePlanet(std::move(params));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Alien(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Alien(std::shared_ptr<T> pimpl)
		: Alien { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~Alien();
};

class Human
	: public service::Object
{
private:
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolvePets(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
		: Sentient
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::vector<std::shared_ptr<service::Object>>> getPets(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<response::StringType> getName(service::FieldParams&& params) const final
		{
			return _pimpl->getName(std::move(params));
		}

		service::FieldResult<std::vector<std::shared_ptr<service::Object>>> getPets(service::FieldParams&& params) const final
		{
			return _pimpl->getPets(std::move(params));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Human(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Human(std::shared_ptr<T> pimpl)
		: Human { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~Human();
};

class Cat
	: public service::Object
{
private:
	service::AwaitableResolver resolveName(service::ResolverParams&& params);
	service::AwaitableResolver resolveNickname(service::ResolverParams&& params);
	service::AwaitableResolver resolveDoesKnowCommand(service::ResolverParams&& params);
	service::AwaitableResolver resolveMeowVolume(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
		: Pet
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::optional<response::StringType>> getNickname(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<response::BooleanType> getDoesKnowCommand(service::FieldParams&& params, CatCommand&& catCommandArg) const = 0;
		virtual service::FieldResult<std::optional<response::IntType>> getMeowVolume(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<response::StringType> getName(service::FieldParams&& params) const final
		{
			return _pimpl->getName(std::move(params));
		}

		service::FieldResult<std::optional<response::StringType>> getNickname(service::FieldParams&& params) const final
		{
			return _pimpl->getNickname(std::move(params));
		}

		service::FieldResult<response::BooleanType> getDoesKnowCommand(service::FieldParams&& params, CatCommand&& catCommandArg) const final
		{
			return _pimpl->getDoesKnowCommand(std::move(params), std::move(catCommandArg));
		}

		service::FieldResult<std::optional<response::IntType>> getMeowVolume(service::FieldParams&& params) const final
		{
			return _pimpl->getMeowVolume(std::move(params));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Cat(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Cat(std::shared_ptr<T> pimpl)
		: Cat { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~Cat();
};

class Mutation
	: public service::Object
{
private:
	service::AwaitableResolver resolveMutateDog(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::shared_ptr<MutateDogResult>> applyMutateDog(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::shared_ptr<MutateDogResult>> applyMutateDog(service::FieldParams&& params) const final
		{
			return _pimpl->applyMutateDog(std::move(params));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Mutation(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Mutation(std::shared_ptr<T> pimpl)
		: Mutation { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~Mutation();
};

class MutateDogResult
	: public service::Object
{
private:
	service::AwaitableResolver resolveId(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<response::IdType> getId(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<response::IdType> getId(service::FieldParams&& params) const final
		{
			return _pimpl->getId(std::move(params));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	MutateDogResult(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	MutateDogResult(std::shared_ptr<T> pimpl)
		: MutateDogResult { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~MutateDogResult();
};

class Subscription
	: public service::Object
{
private:
	service::AwaitableResolver resolveNewMessage(service::ResolverParams&& params);
	service::AwaitableResolver resolveDisallowedSecondRootField(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::shared_ptr<Message>> getNewMessage(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<response::BooleanType> getDisallowedSecondRootField(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::shared_ptr<Message>> getNewMessage(service::FieldParams&& params) const final
		{
			return _pimpl->getNewMessage(std::move(params));
		}

		service::FieldResult<response::BooleanType> getDisallowedSecondRootField(service::FieldParams&& params) const final
		{
			return _pimpl->getDisallowedSecondRootField(std::move(params));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Subscription(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Subscription(std::shared_ptr<T> pimpl)
		: Subscription { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~Subscription();
};

class Message
	: public service::Object
{
private:
	service::AwaitableResolver resolveBody(service::ResolverParams&& params);
	service::AwaitableResolver resolveSender(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<std::optional<response::StringType>> getBody(service::FieldParams&& params) const = 0;
		virtual service::FieldResult<response::IdType> getSender(service::FieldParams&& params) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<std::optional<response::StringType>> getBody(service::FieldParams&& params) const final
		{
			return _pimpl->getBody(std::move(params));
		}

		service::FieldResult<response::IdType> getSender(service::FieldParams&& params) const final
		{
			return _pimpl->getSender(std::move(params));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Message(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Message(std::shared_ptr<T> pimpl)
		: Message { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~Message();
};

class Arguments
	: public service::Object
{
private:
	service::AwaitableResolver resolveMultipleReqs(service::ResolverParams&& params);
	service::AwaitableResolver resolveBooleanArgField(service::ResolverParams&& params);
	service::AwaitableResolver resolveFloatArgField(service::ResolverParams&& params);
	service::AwaitableResolver resolveIntArgField(service::ResolverParams&& params);
	service::AwaitableResolver resolveNonNullBooleanArgField(service::ResolverParams&& params);
	service::AwaitableResolver resolveNonNullBooleanListField(service::ResolverParams&& params);
	service::AwaitableResolver resolveBooleanListArgField(service::ResolverParams&& params);
	service::AwaitableResolver resolveOptionalNonNullBooleanArgField(service::ResolverParams&& params);

	service::AwaitableResolver resolve_typename(service::ResolverParams&& params);

	struct Concept
	{
		virtual ~Concept() = default;

		virtual service::FieldResult<response::IntType> getMultipleReqs(service::FieldParams&& params, response::IntType&& xArg, response::IntType&& yArg) const = 0;
		virtual service::FieldResult<std::optional<response::BooleanType>> getBooleanArgField(service::FieldParams&& params, std::optional<response::BooleanType>&& booleanArgArg) const = 0;
		virtual service::FieldResult<std::optional<response::FloatType>> getFloatArgField(service::FieldParams&& params, std::optional<response::FloatType>&& floatArgArg) const = 0;
		virtual service::FieldResult<std::optional<response::IntType>> getIntArgField(service::FieldParams&& params, std::optional<response::IntType>&& intArgArg) const = 0;
		virtual service::FieldResult<response::BooleanType> getNonNullBooleanArgField(service::FieldParams&& params, response::BooleanType&& nonNullBooleanArgArg) const = 0;
		virtual service::FieldResult<std::optional<std::vector<response::BooleanType>>> getNonNullBooleanListField(service::FieldParams&& params, std::optional<std::vector<response::BooleanType>>&& nonNullBooleanListArgArg) const = 0;
		virtual service::FieldResult<std::optional<std::vector<std::optional<response::BooleanType>>>> getBooleanListArgField(service::FieldParams&& params, std::vector<std::optional<response::BooleanType>>&& booleanListArgArg) const = 0;
		virtual service::FieldResult<response::BooleanType> getOptionalNonNullBooleanArgField(service::FieldParams&& params, response::BooleanType&& optionalBooleanArgArg) const = 0;
	};

	template <class T>
	struct Model
		: Concept
	{
		Model(std::shared_ptr<T>&& pimpl) noexcept
			: _pimpl { std::move(pimpl) }
		{
		}

		service::FieldResult<response::IntType> getMultipleReqs(service::FieldParams&& params, response::IntType&& xArg, response::IntType&& yArg) const final
		{
			return _pimpl->getMultipleReqs(std::move(params), std::move(xArg), std::move(yArg));
		}

		service::FieldResult<std::optional<response::BooleanType>> getBooleanArgField(service::FieldParams&& params, std::optional<response::BooleanType>&& booleanArgArg) const final
		{
			return _pimpl->getBooleanArgField(std::move(params), std::move(booleanArgArg));
		}

		service::FieldResult<std::optional<response::FloatType>> getFloatArgField(service::FieldParams&& params, std::optional<response::FloatType>&& floatArgArg) const final
		{
			return _pimpl->getFloatArgField(std::move(params), std::move(floatArgArg));
		}

		service::FieldResult<std::optional<response::IntType>> getIntArgField(service::FieldParams&& params, std::optional<response::IntType>&& intArgArg) const final
		{
			return _pimpl->getIntArgField(std::move(params), std::move(intArgArg));
		}

		service::FieldResult<response::BooleanType> getNonNullBooleanArgField(service::FieldParams&& params, response::BooleanType&& nonNullBooleanArgArg) const final
		{
			return _pimpl->getNonNullBooleanArgField(std::move(params), std::move(nonNullBooleanArgArg));
		}

		service::FieldResult<std::optional<std::vector<response::BooleanType>>> getNonNullBooleanListField(service::FieldParams&& params, std::optional<std::vector<response::BooleanType>>&& nonNullBooleanListArgArg) const final
		{
			return _pimpl->getNonNullBooleanListField(std::move(params), std::move(nonNullBooleanListArgArg));
		}

		service::FieldResult<std::optional<std::vector<std::optional<response::BooleanType>>>> getBooleanListArgField(service::FieldParams&& params, std::vector<std::optional<response::BooleanType>>&& booleanListArgArg) const final
		{
			return _pimpl->getBooleanListArgField(std::move(params), std::move(booleanListArgArg));
		}

		service::FieldResult<response::BooleanType> getOptionalNonNullBooleanArgField(service::FieldParams&& params, response::BooleanType&& optionalBooleanArgArg) const final
		{
			return _pimpl->getOptionalNonNullBooleanArgField(std::move(params), std::move(optionalBooleanArgArg));
		}

	private:
		const std::shared_ptr<T> _pimpl;
	};

	Arguments(std::unique_ptr<Concept>&& pimpl);

	const std::unique_ptr<Concept> _pimpl;

public:
	template <class T>
	Arguments(std::shared_ptr<T> pimpl)
		: Arguments { std::make_unique<Model<T>>(std::move(pimpl)) }
	{
	}

	~Arguments();
};

} // namespace object

class Operations
	: public service::Request
{
public:
	explicit Operations(std::shared_ptr<object::Query> query, std::shared_ptr<object::Mutation> mutation, std::shared_ptr<object::Subscription> subscription);

	template <class TQuery, class TMutation, class TSubscription>
	explicit Operations(std::shared_ptr<TQuery> query, std::shared_ptr<TMutation> mutation, std::shared_ptr<TSubscription> subscription)
		: Operations { std::make_shared<object::Query>(std::move(query)), std::make_shared<object::Mutation>(std::move(mutation)), std::make_shared<object::Subscription>(std::move(subscription)) }
	{
	}

private:
	std::shared_ptr<object::Query> _query;
	std::shared_ptr<object::Mutation> _mutation;
	std::shared_ptr<object::Subscription> _subscription;
};

std::shared_ptr<schema::Schema> GetSchema();

} // namespace validation
} // namespace graphql

#endif // VALIDATIONSCHEMA_H
