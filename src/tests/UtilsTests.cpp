//
// Created by Jim Carter personal on 6/20/23.
//
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include "Poco/JSON/Object.h"
#include "../poco/Utils.hpp"

TEST(UtilsTest, replaceAll) {
    const std::string EndPoint = "wss://localhost:2567/matchmake/ws/dummy";
    const std::string EndPointNoChange = "http://localhost:2567/matchmake/test/dummy";

    EXPECT_EQ(replaceAll(EndPoint, "ws", "http"), "https://localhost:2567/matchmake/http/dummy");
    EXPECT_EQ(replace(EndPoint, "ws", "http"), "https://localhost:2567/matchmake/ws/dummy");

    EXPECT_EQ(replace(EndPointNoChange, "ws", "http"), EndPointNoChange);
    EXPECT_EQ(replaceAll(EndPointNoChange, "ws", "http"), EndPointNoChange);
}

TEST(UtilsTest, ToPocoJsonObject) {
    nlohmann::json j = {
            {"key1", "value1"},
            {"key2", 123},
            {"key3", true},
            {"key4", {
                             {"nested_key1", "nested_value1"},
                             {"nested_key2", 456},
                             {"nested_key3", false}
                     }}
    };

    Poco::JSON::Object::Ptr pObj = toPocoJsonObject(j);

    EXPECT_EQ(pObj->getValue<std::string>("key1"), "value1");
    EXPECT_EQ(pObj->getValue<int>("key2"), 123);
    EXPECT_EQ(pObj->getValue<bool>("key3"), true);
    EXPECT_TRUE(pObj->has("key4"));

    Poco::JSON::Object::Ptr pNestedObj = pObj->getObject("key4");
    EXPECT_EQ(pNestedObj->getValue<std::string>("nested_key1"), "nested_value1");
    EXPECT_EQ(pNestedObj->getValue<int>("nested_key2"), 456);
    EXPECT_EQ(pNestedObj->getValue<bool>("nested_key3"), false);
}

TEST(UtilsTest, ToPoco) {
    nlohmann::json jArray = {1, 2, 3, 4, 5};

    Poco::Dynamic::Var var = toPoco(jArray);
    Poco::JSON::Array::Ptr pArray = var.extract<Poco::JSON::Array::Ptr>();

    EXPECT_EQ(pArray->getElement<int>(0), 1);
    EXPECT_EQ(pArray->getElement<int>(1), 2);
    EXPECT_EQ(pArray->getElement<int>(2), 3);
    EXPECT_EQ(pArray->getElement<int>(3), 4);
    EXPECT_EQ(pArray->getElement<int>(4), 5);

    nlohmann::json str = "test";
    Poco::Dynamic::Var var2 = toPoco(str);
    const auto& str2 = var2.extract<std::string>();
    EXPECT_EQ("test", str2);
}

TEST(ConversionTest, ToNlohmannJson) {
    // Create a Poco::JSON::Object::Ptr
    Poco::JSON::Object::Ptr pObj = new Poco::JSON::Object;
    pObj->set("key1", "value1");
    pObj->set("key2", 123);
    pObj->set("key3", true);

    Poco::JSON::Object::Ptr pNestedObj = new Poco::JSON::Object;
    pNestedObj->set("nested_key1", "nested_value1");
    pNestedObj->set("nested_key2", 456);
    pNestedObj->set("nested_key3", false);
    pNestedObj->set("nested_key4", 123.45);
    pNestedObj->set("nested_key5", 3.5e38);
    pObj->set("key4", pNestedObj);

    // Convert to nlohmann::json
    nlohmann::json j = toNlohmannJson(pObj);

    // Check the values
    EXPECT_EQ(j["key1"], "value1");
    EXPECT_EQ(j["key2"], 123);
    EXPECT_EQ(j["key3"], true);
    EXPECT_TRUE(j.contains("key4"));

    nlohmann::json jNested = j["key4"];
    EXPECT_EQ(jNested["nested_key1"], "nested_value1");
    EXPECT_EQ(jNested["nested_key2"], 456);
    EXPECT_EQ(jNested["nested_key3"], false);
    EXPECT_EQ(jNested["nested_key4"], 123.45);
    EXPECT_EQ(jNested["nested_key5"], 3.5e38);
}