#include "property.h"

char* Property::Serialize(flatbuffers::FlatBufferBuilder& builder, std::shared_ptr<Property> property)
{
    builder.Finish(SerializeHelper(builder, property, 0));
    return reinterpret_cast<char*>(builder.GetBufferPointer());
}

flatbuffers::Offset<Schema::Property> Property::SerializeHelper(flatbuffers::FlatBufferBuilder& builder,
    std::shared_ptr<Property> property,
    unsigned int depth)
{
    if (depth > MAX_DEPTH)
    {
        throw "maximum depth reached";
    }

    std::vector<flatbuffers::Offset<Schema::Property>> subproperties_vector;
    for (auto& it : property->GetSubproperties())
    {
        subproperties_vector.push_back(SerializeHelper(builder, it.second, depth + 1));
    }

    return Schema::CreateProperty(builder,
        builder.CreateString(property->GetName()),
        property->GetValue(),
        property->GetType(),
        builder.CreateVector(subproperties_vector));
}

std::shared_ptr<Property> Property::Deserialize(char* buffer)
{
    return DeserializeHelper(Schema::GetProperty(buffer));
}

std::shared_ptr<Property> Property::DeserializeHelper(const Schema::Property* fb_property)
{
    std::shared_ptr<Property> property = std::make_shared<Property>(fb_property->name()->str(),
        fb_property->value(),
        fb_property->type());
    for (size_t i = 0; i < fb_property->subproperties()->size(); i++)
    {
        property->SetSubproperty(DeserializeHelper(fb_property->subproperties()->Get(i)));
    }

    return property;
}
