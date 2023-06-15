//
// Created by Jim Carter personal on 6/14/23.
//

#ifndef COLYSEUSCPP_MOCKWEBSOCKETSERVER_HPP
#define COLYSEUSCPP_MOCKWEBSOCKETSERVER_HPP

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include <gtest/gtest.h>
#include <memory>
#include <thread>

class MockWebSocketRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;
};

class MockWebSocketRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
    Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;
};

class ConnectionTest : public ::testing::Test
{
public:
    ConnectionTest();

protected:
    void SetUp() override;
    void TearDown() override;

    int _serverPort;
    std::shared_ptr<Poco::Net::HTTPServer> _server;
    std::thread _serverThread;
};

#endif //COLYSEUSCPP_MOCKWEBSOCKETSERVER_HPP
