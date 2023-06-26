//
// Created by Jim Carter personal on 6/26/23.
//


#include "../core/Schema.hpp"
#include "../core/utils/HasIsSchema.hpp"
#include <gtest/gtest.h>

TEST(SchemaTest, TestIsSchema) {
    using namespace colyseus::schema;

    Schema schema;
    ArraySchema<int> arraySchema;
    MapSchema<int> mapSchema;

    ASSERT_TRUE(has_IsSchema<decltype(schema)>::value);
    ASSERT_TRUE(has_IsSchema<decltype(arraySchema)>::value);
    ASSERT_TRUE(has_IsSchema<decltype(mapSchema)>::value);

    ASSERT_TRUE(schema.IsSchema());
    ASSERT_FALSE(arraySchema.IsSchema());
    ASSERT_FALSE(mapSchema.IsSchema());

    // Compile-time check if a class doesn't have IsSchema method
    class NoSchema {};
    NoSchema notHasIsSchema;
    ASSERT_FALSE(has_IsSchema<decltype(notHasIsSchema)>::value) << "NoSchema should not have IsSchema" << std::endl;
}