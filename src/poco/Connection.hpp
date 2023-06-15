//
// Created by Jim Carter personal on 6/14/23.
//

#ifndef COLYSEUSTEST_CONNECTION_HPP
#define COLYSEUSTEST_CONNECTION_HPP

#include <Poco/Net/WebSocket.h>
#include <Poco/URI.h>
#include <Poco/Runnable.h>
#include <memory>
#include <vector>
#include <string>
#include <functional>

class Connection
{
protected:
    std::unique_ptr<Poco::Net::WebSocket> _socket;
    std::atomic<bool> shouldRun;
    Poco::Thread runThread;

    void run();

public:
    Connection(): shouldRun(false) {};

    ~Connection()
    {
        // We will remove these lines as POCO automatically handles WebSocket closure
        // and the unique_ptr takes care of memory management.
        if(shouldRun) {
            Close();
        }
    }

    // Methods
    void Connect(const std::string& url);
    void Close(int code = Poco::Net::WebSocket::WS_NORMAL_CLOSE, const std::string& reason = "");

    inline void Send(const std::vector<uint8_t>& buffer) const
    {
        _socket->sendBytes(buffer.data(), buffer.size(), Poco::Net::WebSocket::FRAME_BINARY);
    }

    // Callbacks
    std::function<void()> OnOpen;
    std::function<void(int statusCode, const std::string& reason, bool wasClean)> OnClose;
    std::function<void(const std::vector<uint8_t>& data)> OnMessage;
    std::function<void(const std::string& str)> OnText;
    std::function<void(const std::string& message)> OnError;
};

#endif //COLYSEUSTEST_CONNECTION_HPP
