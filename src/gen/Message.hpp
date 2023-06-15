// 
// THIS FILE HAS BEEN GENERATED AUTOMATICALLY
// DO NOT CHANGE IT MANUALLY UNLESS YOU KNOW WHAT YOU'RE DOING
// 
// GENERATED USING @colyseus/schema 1.0.25
// 
#ifndef __SCHEMA_CODEGEN_MESSAGE_H__
#define __SCHEMA_CODEGEN_MESSAGE_H__ 1

#include "../core/schema.h"
#include <typeinfo>
#include <typeindex>



using namespace colyseus::schema;


class Message : public Schema {
public:
	 string str = "";
	 varint_t num = 0;

	Message() {
		this->_indexes = {{0, "str"}, {1, "num"}};
		this->_types = {{0, "string"}, {1, "number"}};
		this->_childPrimitiveTypes = {};
		this->_childSchemaTypes = {};
	}

	virtual ~Message() {
		
	}

protected:
	inline string getString(const string &field)
	{
		if (field == "str")
		{
			return this->str;

		}
		return Schema::getString(field);
	}

	inline void setString(const string &field, const string value)
	{
		if (field == "str")
		{
			this->str = value;
			return;

		}
		return Schema::setString(field, value);
	}
	inline varint_t getNumber(const string &field)
	{
		if (field == "num")
		{
			return this->num;

		}
		return Schema::getNumber(field);
	}

	inline void setNumber(const string &field, varint_t value)
	{
		if (field == "num")
		{
			this->num = value;
			return;

		}
		return Schema::setNumber(field, value);
	}


};


#endif
