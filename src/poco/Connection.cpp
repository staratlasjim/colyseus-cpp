//
// Created by Jim Carter personal on 6/14/23.
//
#include "Connection.hpp"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/WebSocket.h"
#include <iostream>
#include <functional>

void Connection::Connect(const std::string& url)
{
    Poco::URI uri(url);
    std::string scheme = uri.getScheme();
    Poco::Net::HTTPClientSession* pSession = nullptr;

    if (scheme == "https" || scheme == "wss")
    {
        Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> pCertHandler = new Poco::Net::AcceptCertificateHandler(false);
        Poco::Net::Context::Ptr pContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
        Poco::Net::SSLManager::instance().initializeClient(0, pCertHandler, pContext);
        pSession = new Poco::Net::HTTPSClientSession(uri.getHost(), uri.getPort(), pContext);
    }
    else
    {
        pSession = new Poco::Net::HTTPClientSession(uri.getHost(), uri.getPort());
    }

    std::string path(uri.getPathAndQuery());
    if (path.empty()) path = "/";

    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, path, Poco::Net::HTTPMessage::HTTP_1_1);
    Poco::Net::HTTPResponse response;
    _socket = std::make_unique<Poco::Net::WebSocket>(*pSession, request, response);

    /*
     * 		HTTP_CONTINUE                        = 100,
		HTTP_SWITCHING_PROTOCOLS             = 101,
		HTTP_PROCESSING                      = 102,
		HTTP_OK                              = 200,
		HTTP_CREATED                         = 201,
		HTTP_ACCEPTED                        = 202,
     */
    int status = response.getStatus();
    std::cout << "\n\t Status from server: " << status << std::endl;
    if (status >= Poco::Net::HTTPResponse::HTTP_CONTINUE && status <= Poco::Net::HTTPResponse::HTTP_ACCEPTED)
    {
        shouldRun = true;
        if (OnOpen) OnOpen();
        runThread.startFunc([this]() { this->run(); });
    }
    else
    {
        if (OnError) OnError("Failed to connect to the WebSocket server.");
    }
}

void Connection::Close(int code, const std::string& reason)
{
//    std::cout << "\n\t close called \n\n" << (shouldRun ? "Yes" : "No") << std::endl;
    if (_socket)
    {
        shouldRun = false;
        _socket->shutdown(code, reason);
        if (OnClose) OnClose(code, reason, true);
        runThread.join();
    }
}

void Connection::run() {
//    uint8_t buffer[1024];
    Poco::Buffer<uint8_t> buffer(64000);
    int flags = 0;

    std::cout << "\n\t Running \n\n" << std::endl;

    _socket->setReceiveTimeout(std::chrono::seconds(1));

    Poco::Thread::TID threadId = Poco::Thread::currentTid();
    std::cout << "Connection::run Current Thread ID: " << threadId << std::endl;

    while (shouldRun)
    {
        try
        {
            std::cout << "\n\t shouldRun " << (shouldRun ? "Yes" : "No") << std::endl;
            int n = _socket->receiveFrame(buffer.begin(), static_cast<int>(buffer.size()), flags);
            int opcode = flags & Poco::Net::WebSocket::FRAME_OP_BITMASK;
            std::cout << "\n\t Received frame " << n << " bytes of data" << std::endl;
            if(n <= 0)
                continue;

            if (opcode == Poco::Net::WebSocket::FRAME_OP_TEXT && OnText)
            {
                std::cout << "\n\t FRAME_OP_TEXT " << n << " bytes of data" << std::endl;
                OnText(std::string(buffer.begin(), buffer.begin() + n));
            }
            if (opcode == Poco::Net::WebSocket::FRAME_OP_BINARY && OnMessage)
            {
                std::cout << "\n\t FRAME_OP_BINARY " << n << " bytes of data" << std::endl;
                std::vector<uint8_t> vec(buffer.begin(), buffer.begin() + n);
                OnMessage(vec);
            }
            else if (opcode == Poco::Net::WebSocket::FRAME_OP_CLOSE)
            {
                std::cout << "\n\t Close called " << std::endl;
                shouldRun = false;
                break;
            }
        }
        catch (Poco::TimeoutException&) {
            std::cout << "time out" << std::endl;
        }
        catch (const Poco::Exception& e)
        {
            std::cerr << "Exception: " << e.displayText() << std::endl;
            break;
        }
    }
//    std::cout << "\n\t thread done!!! should run: " << (shouldRun ? "Yes" : "No") << std::endl;
}
