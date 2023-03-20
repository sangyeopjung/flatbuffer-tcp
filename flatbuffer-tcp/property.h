#pragma once

#include "serializable.h"
#include "property_generated.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

class Property : public Serializable
{
public:
	Property(std::string name, int value, int type, std::unordered_map <std::string, std::shared_ptr<Property>> subproperties)
		: name_(std::move(name)), value_(value), type_(type), subproperties_(std::move(subproperties)) {}

	Property(std::string name, int value, int type)
		: Property(std::move(name), value, type, {}) {}

	~Property() {}

	const std::string& GetName() const { return name_; }
	int GetValue() const { return value_; }
	int GetType() const { return type_; }
	std::unordered_map<std::string, std::shared_ptr<Property>> GetSubproperties() const { return subproperties_; }
	//std::shared_ptr<Property> GetSubpropertyByName(const std::string& name) const { return subproperties_.at(name); }

	void SetValue(int value) { value_ = value; }
	void SetType(int type) { type_ = type; }
	void SetSubproperty(std::shared_ptr<Property> property)
	{
		subproperties_[property->GetName()] = property;
	}
	void SetSubproperty(const std::string& name, int new_val, int new_type)
	{
		if (subproperties_.find(name) != subproperties_.end())
		{
			subproperties_[name]->SetValue(new_val);
			subproperties_[name]->SetType(new_type);
		}
		else
		{
			SetSubproperty(std::make_shared<Property>(name, new_val, new_type));
		}
	}

	static char* Serialize(flatbuffers::FlatBufferBuilder& builder, std::shared_ptr<Property> property);
	static std::shared_ptr<Property> Deserialize(char* buffer);

	virtual void Print() override
	{
		Print(0);
	}

	friend std::ostream& operator<<(std::ostream& os, const Property& property)
	{
		os << '(' << property.name_ << ',' << property.value_ << ',' << property.type_ << ")";
		return os;
	}

private:
	const std::string name_;
	int value_;
	int type_;
	std::unordered_map<std::string, std::shared_ptr<Property>> subproperties_;

	static flatbuffers::Offset<Schema::Property> SerializeHelper(flatbuffers::FlatBufferBuilder& builder,
																 std::shared_ptr<Property> property, 
																 unsigned int depth);
	static std::shared_ptr<Property> DeserializeHelper(const Schema::Property* fb_property);

	void Print(int depth)
	{
		for (int i = 0; i < depth; i++)
		{
			std::cout << '\t';
		}
		std::cout << '-' << *this << '\n';
		for (auto& it : this->subproperties_)
		{
			it.second->Print(depth + 1);
		}
	}

	static constexpr unsigned int MAX_DEPTH = 100;
};
