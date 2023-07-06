// 
// THIS FILE HAS BEEN GENERATED AUTOMATICALLY
// DO NOT CHANGE IT MANUALLY UNLESS YOU KNOW WHAT YOU'RE DOING
// 
// GENERATED USING @colyseus/schema 1.0.25
// 
#ifndef __SCHEMA_CODEGEN_STATE_H__
#define __SCHEMA_CODEGEN_STATE_H__ 1

#include "../core/Schema.hpp"
#include <typeinfo>
#include <typeindex>



using namespace colyseus::schema;


class State : public Schema {
public:
	 string lastMessage = "";
	 varint_t msgCount = 0;
     ArraySchema<int64_t*> *arrayOfNumbers= new ArraySchema<int64_t*>();

	State() {
		this->_indexes = {{0, "lastMessage"}, {1, "msgCount"}, {1, "arrayOfNumbers"}};
		this->_types = {{0, "string"}, {1, "number"}, {2, "array"}};
		this->_childPrimitiveTypes = {{3, "number"},};
		this->_childSchemaTypes = {};
	}

	virtual ~State() {
		delete this->arrayOfNumbers;
	}

protected:
	inline string getString(const string &field)
	{
		if (field == "lastMessage")
		{
			return this->lastMessage;
		}
		return Schema::getString(field);
	}

	inline void setString(const string &field, const string &value)
	{
		if (field == "lastMessage")
		{
            std::cout << "Setting lastMessage:\t" << value << std::endl;
			this->lastMessage = value;
			return;

		}
		return Schema::setString(field, value);
	}
	inline varint_t getNumber(const string &field)
	{
		if (field == "msgCount")
		{
			return this->msgCount;

		}
		return Schema::getNumber(field);
	}

	inline void setNumber(const string &field, varint_t value)
	{
		if (field == "msgCount")
		{
			this->msgCount = value;
			return;

		}
		return Schema::setNumber(field, value);
	}

    inline ArraySchema<char*> * getArray(const string &field)
    {
        if (field == "arrayOfNumbers")
        {
            return (ArraySchema<char*> *) this->arrayOfNumbers;
        }
        return Schema::getArray(field);
    }

    inline void setArray(const string &field, ArraySchema<char*> * value)
    {
        if (field == "arrayOfNumbers")
        {
            this->arrayOfNumbers = (ArraySchema<int64_t*> *)value;
            return;

        }
        return Schema::setArray(field, value);
    }


};


#endif
