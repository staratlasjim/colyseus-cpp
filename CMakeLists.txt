cmake_minimum_required(VERSION 3.23)
project(colyseusCpp)

set(CMAKE_CXX_STANDARD 14)
find_package(Boost REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation Net JSON NetSSL)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(msgpack-cxx CONFIG REQUIRED)

enable_testing()

find_package(GTest CONFIG REQUIRED)

# Add your main library and executable targets
add_library(ColyseusLib src/core/Schema.hpp
        src/core/PrimitiveTypes.h
        src/poco/Axios.cpp
        src/poco/Axios.hpp
        src/poco/Axios.cpp
        src/poco/WebSocketClient.cpp
        src/poco/WebSocketClient.hpp
        src/core/SchemaSerializer.hpp
        src/core/Serializer.hpp
        src/gen/State.hpp
        src/poco/Connection.hpp
        src/poco/Connection.cpp
        src/poco/Room.hpp
        src/core/Protocol.hpp
        src/poco/Client.hpp
        src/poco/Utils.hpp
        src/poco/Utils.cpp
        src/core/Serializer.cpp
        src/gen/gmp/OrderSchema.hpp
        src/gen/gmp/OverviewRoomState.hpp
        src/core/ArraySchema.cpp
        src/core/ArraySchema.hpp
        src/core/MapSchema.cpp
        src/core/MapSchema.hpp
        src/core/SchemaUtils.hpp
)
target_link_libraries(ColyseusLib PUBLIC Poco::Foundation Poco::Net Poco::NetSSL Poco::JSON Boost::boost nlohmann_json::nlohmann_json msgpack-cxx)
add_executable(colyseusCpp main.cpp
        src/poco/apps/DummyUpdater.cpp
        src/poco/apps/DummyUpdater.hpp
        src/poco/apps/MarketPlaceApp.cpp
        src/poco/apps/MarketPlaceApp.hpp)
target_link_libraries(colyseusCpp PRIVATE ColyseusLib Poco::Foundation Poco::Net Poco::NetSSL Poco::JSON Boost::boost nlohmann_json::nlohmann_json msgpack-cxx)

# Create a test executable and link it with the Google Test library
add_executable(colyseusCppTests
        src/tests/ConnectionTest.cpp
        src/tests/MockWebSocketServer.hpp
        src/tests/MockWebSocketServer.cpp
        src/tests/UtilsTests.cpp
        src/tests/ClientTest.cpp)
target_link_libraries(colyseusCppTests PRIVATE ColyseusLib GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
#add_test(NAME WebSocketExampleTests COMMAND WebSocketExampleTests)
add_test(AllTestsInMain colyseusCppTests)