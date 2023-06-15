//
// Created by Jim Carter personal on 5/26/23.
//

#ifndef COLYSEUSTEST_WEBSOCKETCLIENT_HPP
#define COLYSEUSTEST_WEBSOCKETCLIENT_HPP

#include "Poco/Net/WebSocket.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Delegate.h"
#include <iostream>
#include <functional>

class IWebSocketSender {
    public:
        virtual bool send(const std::vector<uint8_t>& data) = 0;  // Pure virtual function
};

typedef std::function<void(const std::vector<uint8_t>&, IWebSocketSender*)> MessageHandler;

class WebSocketClient: public IWebSocketSender {
public:
    WebSocketClient(const std::string& host, int port, const MessageHandler& handler, const std::string& uri = "/")
            : _cs(host, port), _handler(handler), _uri(uri), _isConnected(false) {
    }

    ~WebSocketClient() {
        disconnect();
    }

    void onConnected() {
        std::cout << "WebSocket connected!" << std::endl;
        // Handle the connection event
        _isConnected = true;
    }

    void onDisconnected() {
        std::cout << "WebSocket disconnected!" << std::endl;
        // Handle the disconnection event
        _isConnected = false;
    }

    void connect() {
        if(isConnected())
            return;

        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, _uri, Poco::Net::HTTPMessage::HTTP_1_1);
        Poco::Net::HTTPResponse response;
        _ws = std::make_shared<Poco::Net::WebSocket>(_cs, request, response);
        onConnected();
    }

    void disconnect() {
        if (_ws) {
            onDisconnected();
            _ws->shutdown();
            _ws->close();
            _ws.reset();
        }
    }

    bool isConnected() const {
        return _ws && _isConnected;
    }

    bool send(const std::vector<uint8_t>& data);
    void run();

private:
    Poco::Net::HTTPClientSession _cs;
    std::shared_ptr<Poco::Net::WebSocket> _ws;
    MessageHandler _handler;
    std::string _uri;
    bool _isConnected;
};
// example Register the callback functions
//        _ws->connected += Poco::delegate(this, &WebSocketClient::onConnected);
//        _ws->disconnected += Poco::delegate(this, &WebSocketClient::onDisconnected);
#endif //COLYSEUSTEST_WEBSOCKETCLIENT_HPP
