//
// Created by Jim Carter personal on 6/14/23.
//
// MockWebSocketServer.cpp
#include "MockWebSocketServer.hpp"
#include <Poco/Net/WebSocket.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <iostream>

void MockWebSocketRequestHandler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
{
    try
    {
        Poco::Net::WebSocket ws(request, response);
        std::cout << "WebSocket connection established." << std::endl;

        // You can implement specific WebSocket server behavior for testing here
        char buffer[1024];
        int flags;
        int n;
        do
        {
            n = ws.receiveFrame(buffer, sizeof(buffer), flags);
            std::cout << Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags)) << std::endl;
            ws.sendFrame(buffer, n, flags);
        }
        while (n > 0 && (flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);
        std::cout << "WebSocket connection closed." << std::endl;
        ws.shutdown();
    }
    catch (const Poco::Net::WebSocketException& exc)
    {
        std::cout << "Error: " << exc.displayText() << " code: " << exc.code() << std::endl;
        // Handle any exceptions that occur during WebSocket handling
        switch (exc.code())
        {
            case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
                response.set("Sec-WebSocket-Version", Poco::Net::WebSocket::WEBSOCKET_VERSION);
                // fallthrough
            case Poco::Net::WebSocket::WS_ERR_NO_HANDSHAKE:
            case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
            case Poco::Net::WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
                response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_BAD_REQUEST);
                response.setContentLength(0);
                response.send();
                break;
        }
    }
}

Poco::Net::HTTPRequestHandler* MockWebSocketRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
{
    std::cout << "Request from " << request.clientAddress().toString()
              << ": "
              << request.getMethod()
              << " "
              << request.getURI()
              << " "
              << request.getVersion()
              << std::endl;

    for (auto it = request.begin(); it != request.end(); ++it)
    {
        std::cout << "request info: " << it->first << ": " << it->second << std::endl;
    }

    if(request.find("Upgrade") != request.end() && Poco::icompare(request["Upgrade"], "websocket") == 0)
        std::cout << "Web socket upgrade" << std::endl;
    else
        std::cout << "NO web socket upgrade???" << std::endl;

    return new MockWebSocketRequestHandler();
}

ConnectionTest::ConnectionTest() : _server(nullptr), _serverPort(0) {}

void ConnectionTest::SetUp()
{
    // Set up the mock WebSocket server on a separate thread
    Poco::Net::HTTPServerParams::Ptr pParams(new Poco::Net::HTTPServerParams);
    pParams->setMaxQueued(100);
    pParams->setMaxThreads(4);

    Poco::Net::ServerSocket svs(0); // Use an ephemeral port
    _serverPort = svs.address().port();

    _server = std::make_shared<Poco::Net::HTTPServer>(new MockWebSocketRequestHandlerFactory(), svs, pParams);
    _server->start();
}

void ConnectionTest::TearDown()
{
    _server->stop();
}