#pragma once

#include "flatbuffers/flatbuffers.h"

#include <memory>

class Serializable
{
public:
	Serializable() {}
	virtual ~Serializable() {}

	template<typename T>
	static char* Serialize(flatbuffers::FlatBufferBuilder& builder, std::shared_ptr<T> object)
	{
		return T::Serialize(builder, object);
	}

	template<typename T>
	static std::shared_ptr<T> Deserialize(char* buffer)
	{
		return T::Deserialize(buffer);
	}

	virtual void Print() = 0;
};