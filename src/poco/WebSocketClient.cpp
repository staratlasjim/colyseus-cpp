//
// Created by Jim Carter personal on 5/26/23.
//

#include "WebSocketClient.hpp"
void WebSocketClient::run() {
    if(!isConnected()) {
        connect();
    }

    uint8_t buffer[1024];
    int flags;
    do {
        int n = _ws->receiveFrame(buffer, sizeof(buffer), flags);
        if (flags & Poco::Net::WebSocket::FRAME_BINARY) {
            std::vector<uint8_t> data(buffer, buffer + n);
            _handler(data, this);
        }
    } while ((flags & Poco::Net::WebSocket::FRAME_OP_BITMASK) != Poco::Net::WebSocket::FRAME_OP_CLOSE);

    std::cout << "\n\t RUN done" << std::endl;

    disconnect();
}

bool WebSocketClient::send(const std::vector<uint8_t>& data) {
    if (isConnected()) {
        _ws->sendFrame(data.data(), data.size(), Poco::Net::WebSocket::FRAME_BINARY);
        return true;
    }

    return false;
}