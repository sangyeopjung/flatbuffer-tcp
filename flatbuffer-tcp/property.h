#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

class Property
{
public:
	Property(std::string name, int value, int type, std::unordered_map <std::string, std::shared_ptr<Property >> subproperties)
		: name_(std::move(name)), value_(value), type_(type), subproperties_(std::move(subproperties)) {}

	Property(std::string name, int value, int type)
		: Property(name, value, type, {}) {}

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

	static void Print(const Property& property, int depth = 0)
	{
		for (int i = 0; i < depth; i++)
		{
			std::cout << '\t';
		}
		std::cout << '-' << property << '\n';
		for (auto& it : property.subproperties_)
		{
			Property::Print(*it.second, depth + 1);
		}
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
};
