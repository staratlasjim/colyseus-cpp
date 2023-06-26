//
// Created by Jim Carter personal on 6/15/23.
//
//
// Created by Jim Carter personal on 5/25/23.
//
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/HTTPClientSession.h"
#include "src/poco/Axios.hpp"
#include "src/poco/WebSocketClient.hpp"
#include "src/core/Schema.hpp"
#include "src/core/Serializer.hpp"
#include "src/core/SchemaSerializer.hpp"
#include "src/gen/State.hpp"

#include <iostream>
#include <iomanip>
#include <memory>
#include <msgpack.hpp>

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::Net::WebSocket;
using colyseus::schema::Iterator;

enum class Protocol : int {
    // Room-related (10~19)
    HANDSHAKE = 9,
    JOIN_ROOM = 10,
    JOIN_ERROR = 11,
    LEAVE_ROOM = 12,
    ROOM_DATA = 13,
    ROOM_STATE = 14,
    ROOM_STATE_PATCH = 15,
    ROOM_DATA_SCHEMA = 16,
};

void myHandlerString(const std::string &message) {
    std::cout << "Received binary data: " << message << std::endl;
}

static std::shared_ptr<Serializer<State>> currentStateSerializer;

void myHandler(const std::vector<uint8_t> &data, IWebSocketSender* webSocketSender) {
    int size = data.size();
    std::cout << "Received binary data of size: " << size << std::endl;

    if(size <= 0) {
        return;
    }

    std::unique_ptr<Iterator> iterator = std::make_unique<Iterator>();
    iterator->offset = 0;

    std::cout << "myHandler bytes => ";
    for (uint8_t byte: data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::endl;


    // todo: Make uint8_t
    unsigned char code = data[iterator->offset++];

    std::cout << "this is a test" << std::endl;

    switch ((Protocol) code) {
        case Protocol::JOIN_ROOM: {
            std::cout << "Colyseus.Room: JOIN_ROOM" << std::endl;

            std::string serializerId = colyseus::schema::decodeString(data.data(), iterator.get());
            std::cout << "Decoded string (serializerId): " << serializerId << std::endl;

            if(serializerId.empty()) {
                std::cout << "Unknown serializer because it's empty" << std::endl;
                break;
            }

            currentStateSerializer = std::static_pointer_cast<SchemaSerializer<State>>(getSerializer(serializerId));

            if (currentStateSerializer == nullptr) {
                std::cout << "No serializer found for the given id." << std::endl;
                break;
            } else {
                std::cout << "Serializer found." << std::endl;
            }

            if(size > iterator->offset) {
                std::cout << "Includes hand shake data, ignoring for now" << std::endl;
                //currentStateSerializer->handshake(data.data(), iterator.get(), data.size());
            }
            else {
                std::cout << "no hand shake" << std::endl;
            }

            std::vector<uint8_t> data = {static_cast<uint8_t>(Protocol::JOIN_ROOM)};
            std::cout << "sending data" << std::endl;
            bool didSend = webSocketSender->send(data);

            std::cout << "Did send? " << (didSend ? "True" : "False") << std::endl;

            break;
        }
        case Protocol::JOIN_ERROR: {
            std::cout << "Colyseus.Room: ERROR" << std::endl;
            break;
        }
        case Protocol::LEAVE_ROOM: {
            std::cout << "Colyseus.Room: LEAVE_ROOM" << std::endl;
            break;
        }
        case Protocol::ROOM_DATA: {
            std::cout << "Colyseus.Room: ROOM_DATA" << std::endl;
        }
        case Protocol::ROOM_STATE:
        {
            std::cout << "Colyseus.Room: ROOM_STATE" << std::endl;
            std::vector<uint8_t> bytes(data.begin(), data.end());
            bytes.erase(bytes.begin());

            if (currentStateSerializer == nullptr) {
                std::cout << "No serializer, returning." << std::endl;
                break;
            } else {
                std::cout << "Serializer exists." << std::endl;
            }

            std::cout << "\tColyseus.Room: decoding state" << std::endl;
            currentStateSerializer->setState(bytes.data(), 0, bytes.size());
            iterator->offset += bytes.size();
            State* ptrState = currentStateSerializer->getState();
            std::cout << "\t State: \n\tLast Message: " << ptrState->lastMessage << "\n\tMsgCount: " << ptrState->msgCount << std::endl;

            Poco::Thread::sleep(500);

            // build client
            std::string schema = "schema";
            std::vector<uint8_t> sendData;
            sendData.push_back(static_cast<int>(Protocol::ROOM_DATA));

            msgpack::sbuffer sbuf;
            msgpack::pack(sbuf, schema);

            size_t size = sbuf.size();
            std::cout << "~~~ buffer size: " << size << std::endl;

            // Then, append the packed string to the vector
            sendData.insert(sendData.end(), sbuf.data(), sbuf.data() + sbuf.size());

            // send data
            bool result = webSocketSender->send(sendData);
            std::cout << "\n\t Sent data to server: " << (result ? "yes" : "no") << std::endl;
            break;
        }
        case Protocol::ROOM_STATE_PATCH: {
            std::cout << "Colyseus.Room: ROOM_STATE_PATCH" << std::endl;

            if (currentStateSerializer == nullptr) {
                std::cout << "No serializer, returning." << std::endl;
                break;
            } else {
                std::cout << "Serializer exists." << std::endl;
            }

            std::vector<uint8_t> bytes(data.begin(), data.end());
            bytes.erase(bytes.begin());
            currentStateSerializer->patch(static_cast<const unsigned char*>(bytes.data()), 0, bytes.size());
            State* ptrState = currentStateSerializer->getState();
            std::cout << "\t State: \n\tLast Message: " << ptrState->lastMessage << "\n\tMsgCount: " << ptrState->msgCount << std::endl;
            break;
        }
        case Protocol::HANDSHAKE: {
            std::cout << "Colyseus.Room: HANDSHAKE" << std::endl;
            break;
        }
        case Protocol::ROOM_DATA_SCHEMA: {
            std::cout << "Colyseus.Room: ROOM_DATA_SCHEMA" << std::endl;
            break;
        }
    }
}

int main(int args, char **argv) {

    // Create a shared_ptr to a new SchemaSerializer<Schema>
    std::shared_ptr<Serializer<State>> serializer = std::make_shared<SchemaSerializer<State>>();

    // Register it with the string "schema"
    registerSerializer("schema", serializer);

    try {
        Axios axios;

        Poco::JSON::Object::Ptr data = new Poco::JSON::Object;
        data->set("password", "xa123");
        Axios::Response res = axios.post("http://localhost:2567/matchmake/joinOrCreate/dummy", data);

        Poco::JSON::Object::Ptr room = res.json->getObject("room");
        int clients = room->getValue<int>("clients");
        std::string createdAt = room->getValue<std::string>("createdAt");
        int maxClients = room->getValue<int>("maxClients");
        Poco::JSON::Object::Ptr metadata = room->getObject("metadata");
        bool password = metadata->getValue<bool>("password");
        std::string name = room->getValue<std::string>("name");
        std::string processId = room->getValue<std::string>("processId");
        std::string roomId = room->getValue<std::string>("roomId");
        std::string sessionId = res.json->getValue<std::string>("sessionId");

        std::cout << "Room Name: " << name << ", Clients: " << clients << ", Created At: " << createdAt << std::endl;
        std::cout << "Max Clients: " << maxClients << ", Password: " << (password ? "True" : "False") << std::endl;
        std::cout << "Process ID: " << processId << ", Room ID: " << roomId << ", Session ID: " << sessionId
                  << std::endl;

        // room.connect(this.buildEndpoint(response.room, { sessionId: room.sessionId }));
        // return `${this.endpoint}/${room.processId}/${room.roomId}?${params.join('&')}`;
        std::stringstream websocketEndpoint;
        std::string protocol = "ws://";
        std::string host = "localhost";
        int port = 2567;
        std::stringstream uri;
        uri << "/" << processId << "/" << roomId << "?" << "sessionId=" << sessionId;
        websocketEndpoint << protocol << host << ":" << port << uri.str();
        std::cout << "\n\tWebsocket Room Endpoint:" << websocketEndpoint.str() << std::endl;

        try {
            WebSocketClient client(host, port, myHandler, uri.str());
            client.run();
        }
        catch (Poco::Exception &exc) {
            std::cerr << "Exception: " << exc.displayText() << std::endl;
        }
        catch (std::exception &e) {
            std::cout << "Exception " << e.what() << std::endl;
        }

        std::cout << "\n\tEnded connection to:" << websocketEndpoint.str() << std::endl;

    } catch (Poco::Exception &e) {
        // Handle JSON parsing errors
        std::cout << e.message() << std::endl;
    }


    return 0;
}

/*
            std::string schema = "schema";
            std::vector<uint8_t> sendData;
            sendData.push_back(static_cast<int>(Protocol::ROOM_DATA));

            std::stringstream sbuf;
            msgpack::pack(sbuf, schema);
            sbuf.seekg(0);

            std::string str(sbuf.str());
            std::cout << "~~~ sbuf: " << str << std::endl;

            size_t size = str.size();
            std::cout << "~~~ sbuff size: " << size << std::endl;

            // Then, append the packed string to the vector
            sendData.insert(sendData.end(), str.begin(), str.end());
 */