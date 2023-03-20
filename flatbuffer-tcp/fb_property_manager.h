#pragma once

#include "property.h"
#include "property_generated.h"

class FbPropertyManager
{
public:
	static FbPropertyManager& GetInstance()
	{
		static FbPropertyManager instance_;
		return instance_;
	}

	FbPropertyManager(FbPropertyManager const&) = delete;
	void operator=(FbPropertyManager const&) = delete;

	char* Serialize(flatbuffers::FlatBufferBuilder& builder, std::shared_ptr<Property> property);
	std::shared_ptr<Property> Deserialize(char* buffer);

private:
	FbPropertyManager() {}

	flatbuffers::Offset<Schema::Property> SerializeHelper(flatbuffers::FlatBufferBuilder& builder, std::shared_ptr<Property> property, unsigned int depth);
	std::shared_ptr<Property> DeserializeHelper(const Schema::Property* fb_property);

	static constexpr unsigned int MAX_DEPTH = 100;
};

