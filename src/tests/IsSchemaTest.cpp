//
// Created by Jim Carter personal on 7/6/23.
//
#include "gtest/gtest.h"
#include "../core/ArraySchema.hpp"
#include "../core/MapSchema.hpp"
#include "../core/Schema.hpp"

using namespace colyseus::schema;

// Test fixture for Schema, MapSchema, and ArraySchema
class IsSchemaTest : public ::testing::Test {

protected:
    Schema schema;
    MapSchema<int> mapSchema;
    ArraySchema<int> arraySchema;
};

// Test if Schema returns true for IsSchema
TEST_F(IsSchemaTest, SchemaReturnsTrueForIsSchema) {
    EXPECT_TRUE(schema.IsSchema());
}

// Test if MapSchema returns false for IsSchema
TEST_F(IsSchemaTest, MapSchemaReturnsFalseForIsSchema) {
    EXPECT_FALSE(mapSchema.IsSchema());
}

// Test if ArraySchema returns false for IsSchema
TEST_F(IsSchemaTest, ArraySchemaReturnsFalseForIsSchema) {
    EXPECT_FALSE(arraySchema.IsSchema());
}
