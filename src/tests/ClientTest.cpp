//
// Created by Jim Carter personal on 6/20/23.
//
/*
 * Axios axios;

        Poco::JSON::Object::Ptr data = new Poco::JSON::Object;
        data->set("password", "xa123");
        Axios::Response res = axios.post("http://localhost:2567/matchmake/joinOrCreate/dummy", data);
 */
#include <future>
#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include "Poco/JSON/Object.h"
#include "../poco/Utils.hpp"
#include "../poco/Axios.hpp"
#include "../poco/Client.hpp"
#include "../gen/State.hpp"

const std::string mustHaveTestServerRunningError = "!!! These integration tests require that you have Colysues server running on port 2567 with the 'dummy room' registered !!!";

TEST(ClientTest, MustHaveServerRunning) {
    const std::string EndPoint = "http://localhost:2567/matchmake/joinOrCreate/dummy";

    Axios axios;
    Poco::JSON::Object::Ptr data = new Poco::JSON::Object;


    try {
        Axios::Response res = axios.post("http://localhost:2567/matchmake/joinOrCreate/dummy", data);
        ASSERT_EQ(res.status, HTTPResponse::HTTPStatus::HTTP_OK) << mustHaveTestServerRunningError << std::endl;
    }
    catch(std::runtime_error& error) {
        ASSERT_FALSE(true) << mustHaveTestServerRunningError << std::endl;
    }
}

TEST(ClientTest, JoinDummyRoom) {
    using namespace std::chrono;

    Poco::Thread::TID threadId = Poco::Thread::currentTid();
    std::cout << "ClientTest, JoinDummyRoom Current Thread ID: " << threadId << std::endl;

    std::shared_ptr<Client> client = std::make_shared<Client>("ws://localhost:2567");

    std::shared_ptr<Room<State>> mainRoom;

    JoinOptions options;

    bool didCall = false;

    // Define a promise and future
    std::promise<void> promise;
    std::future<void> future = promise.get_future();

    // Define the lambda function
    auto callback = [&](std::shared_ptr<MatchMakeError> error, std::shared_ptr<Room<State>> room) {
        Poco::Thread::TID threadId = Poco::Thread::currentTid();
        std::cout << "ClientTest, JoinDummyRoom, callback Current Thread ID: " << threadId << std::endl;
        std::cout << "On Join" << std::endl;
        EXPECT_TRUE(error == nullptr) << "Received an error from the JoinOrCreate dummy room " << error->code << " " << error->message << std::endl;
        EXPECT_TRUE(room != nullptr) << "Room was not created";

        // hold a ref to the room;
        mainRoom = room;

        State* state = room->GetState();
        EXPECT_TRUE(state != nullptr) << "Dummy Room should have state" << std::endl;
        EXPECT_TRUE(state->msgCount == 0) << "Msg Count should be zero " << std::endl;

        room->OnStateChange = [&](State* state) {
            std::cout << "On State Change" << std::endl;
            EXPECT_TRUE(state != nullptr) << "Dummy Room should have state" << std::endl;
            EXPECT_TRUE(state->msgCount > 0) << "Msg Count should be greater than zero " << std::endl;

            didCall = true;
            promise.set_value();
        };

        std::cout << "On Join done" << std::endl;
    };

    // Call the method with the lambda function as the callback
    client->JoinOrCreate<State>("dummy", options, callback);

    std::cout << "\t~~~ JoinOrCreate wait" << std::endl;

    auto currentMs = getTimeSinceEpochMs();

    future.wait_for(seconds(5));

    auto afterWait = getTimeSinceEpochMs();

    auto duration = milliseconds(afterWait - currentMs);

    auto secondsPassed = duration_cast<seconds>(duration);
    auto ms = duration_cast<milliseconds>(duration) % 1000;

    std::cout.precision(3);
    std::cout << "\n~~~ wait took: " << secondsPassed.count() << "." << std::fixed << ms.count() << std::endl;

    EXPECT_TRUE(didCall) << "Expected that our OnStateChange did get called" << std::endl;

    std::cout << "\n\n\tdone" << std::endl;
}