//
// Created by Jim Carter personal on 6/14/23.
//
#include "../poco/Connection.hpp"
#include "MockWebSocketServer.hpp"
#include <gtest/gtest.h>
#include <thread>


TEST_F(ConnectionTest, ConnectSuccess)
{
    Connection connection;
    bool isOpen = false;
    bool isClosed = false;
    std::stringstream closeReason;
    int _statusCode = 0;
    bool _wasClean = false;

    connection.OnOpen = [&isOpen] { isOpen = true; };
    connection.OnError = [](const std::string& error) { FAIL() << "Unexpected error: " << error; };
    connection.OnClose =
            [&isClosed, &closeReason, &_statusCode, &_wasClean](int statusCode, const std::string& reason, bool wasClean) {
        _statusCode = statusCode;
        _wasClean = wasClean;
        isClosed = true;
        closeReason << reason;
    };
    connection.Connect("ws://localhost:" + std::to_string(_serverPort));
    // You can add a timeout or use a condition variable to wait for the connection to open
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // close the connection
    connection.Close(Poco::Net::WebSocket::WS_NORMAL_CLOSE, "test");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(isOpen);
    ASSERT_TRUE(isClosed);
    ASSERT_EQ(_statusCode, Poco::Net::WebSocket::WS_NORMAL_CLOSE);
    ASSERT_EQ(closeReason.str(), "test");
    ASSERT_TRUE(_wasClean);
}

TEST_F(ConnectionTest, MessageSendSuccess)
{
    Connection connection;
    bool isOpen = false;
    bool isClosed = false;
    bool gotMessage = false;
    std::string message;
    connection.OnOpen = [&isOpen] { isOpen = true; };
    connection.OnError = [](const std::string& error) { FAIL() << "Unexpected error: " << error; };
    connection.OnClose =
            [&isClosed](int statusCode, const std::string& reason, bool wasClean) {
                isClosed = true;
            };
    connection.OnMessage = [&gotMessage, &message](const std::vector<uint8_t>& data) {
        gotMessage = true;
        std::string str(data.begin(), data.end());
        message = str;
    };
    connection.Connect("ws://localhost:" + std::to_string(_serverPort));
    // You can add a timeout or use a condition variable to wait for the connection to open
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(isOpen);

    std::string test = "test";
    std::vector<uint8_t> sendData;
    // Then, append the packed string to the vector
    sendData.insert(sendData.end(), test.begin(), test.end());
    connection.Send(sendData);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    ASSERT_TRUE(gotMessage);
    ASSERT_EQ(test, message);

    // close the connection
    connection.Close();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_TRUE(isClosed);

}