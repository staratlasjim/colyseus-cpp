//
// Created by Jim Carter personal on 6/15/23.
//

#ifndef COLYSEUSCPP_ROOM_HPP
#define COLYSEUSCPP_ROOM_HPP


#include "Connection.hpp"
#include "../core/Serializer.hpp"
#include "../core/Protocol.hpp"
#include "../core/SchemaSerializer.hpp"
#include "../core/schema.h"

#include <codecvt>
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <memory>
#include <functional>

#pragma push_macro("check")
#undef check

#include <msgpack.hpp>

#pragma pop_macro("check")



template <typename S>
class Room
{
public:
    Room(const std::string& Name) : Name(Name) {}

    ~Room() {
        if(bHasJoined)
            ConnectionInstance->Close();
    }

    inline const std::string& GetId() { return Id; }
    inline void SetId(const std::string& id) { this->Id = id; }
    inline const std::string& GetName() { return Name; }
    inline const std::string& GetSessionId() { return SessionId; }
    inline void SetSessionId(const std::string& sessionId) { this->SessionId = sessionId; }
    inline const std::string& GetSerializerId() { return SerializerId; }
    inline bool HasJoined() { return bHasJoined; }

    // Methods
    void Connect(const std::string& Endpoint)
    {
        ConnectionInstance = std::make_shared<Connection>();
        ConnectionInstance->OnOpen = [this]() {
            this->bHasConnected = true;
        };
        ConnectionInstance->OnClose =
                std::bind(&Room::_onClose, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        ConnectionInstance->OnError = std::bind(&Room::_onError, this, std::placeholders::_1);
        ConnectionInstance->OnMessage =
                std::bind(&Room::_onMessage, this, std::placeholders::_1);
        ConnectionInstance->Connect(Endpoint);
    }

    void Leave(bool Consent)
    {
        if (bHasJoined)
        {
            bHasJoined = false;
            if (Consent)
            {
                std::vector<uint8_t> data = {static_cast<uint8_t>(Protocol::LEAVE_ROOM)};
                ConnectionInstance->Send(data);
            }
            else {
                ConnectionInstance->Close();
                CallOnLeave(0);
            }
        }
        else {
            CallOnLeave(4000);
        }
    }

    inline void Send(unsigned char Type)
    {
        std::vector<uint8_t> data = {static_cast<uint8_t>(Protocol::ROOM_DATA), static_cast<uint8_t>(Type)};
        ConnectionInstance->Send(data);
    }

    template<typename T>
    void Send(const uint8_t& Type, T Message)
    {
        std::vector<uint8_t> sendData;
        sendData.push_back(static_cast<uint8_t>(Protocol::ROOM_DATA));
        sendData.push_back(Type);

        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, Message);

        // Then, append the packed string to the vector
        sendData.insert(sendData.end(), sbuf.data(), sbuf.data() + sbuf.size());
        ConnectionInstance->Send(sendData);
    }

    void Send(const std::string& Type)
    {
        std::vector<uint8_t> sendData;
        sendData.push_back(static_cast<uint8_t>(Protocol::ROOM_DATA));
        sendData.push_back(Type.size() | 0xa0);

        sendData.insert(sendData.end(), Type.begin(), Type.end());

        ConnectionInstance->Send(sendData);
    }

    template <typename T>
    void Send(const std::string& Type, T Message)
    {
        std::vector<uint8_t> sendData;
        sendData.push_back(static_cast<uint8_t>(Protocol::ROOM_DATA));
        sendData.push_back(Type.size() | 0xa0);
        sendData.insert(sendData.end(), Type.begin(), Type.end());

        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, Message);

        // Then, append the packed string to the vector
        sendData.insert(sendData.end(), sbuf.data(), sbuf.data() + sbuf.size());
        ConnectionInstance->Send(sendData);
    }

    inline Room<S>* OnMessage(const int Type, std::function<void(const msgpack::object&)>& Callback)
    {
        OnMessageHandlers.insert(std::make_pair(GetMessageHandlerKey(Type), Callback));
        return this;
    }

    inline Room<S>* OnMessage(const std::string& Type, std::function<void(const msgpack::object&)>& Callback)
    {
        OnMessageHandlers.insert(std::make_pair(GetMessageHandlerKey(Type), Callback));
        return this;
    }

    S* GetState()
    {
        return SerializerInstance->getState();
    }

    // Callbacks
    std::function<void()> OnJoin;
    std::function<void(int32_t StatusCode)> OnLeave;
    std::function<void(int32_t StatusCode, const std::string& Message)> OnError;
    std::function<void(S*)> OnStateChange;
    std::unordered_map<std::string, std::function<void(const msgpack::object&)>> OnMessageHandlers;
    std::function<void()> OnClose;
    // Properties
    std::shared_ptr<Connection> ConnectionInstance;



protected:
    std::string Id;
    std::string Name;
    std::string SessionId;
    std::string SerializerId;
    bool bHasJoined = false;
    bool bHasConnected = false;

    inline void CallStateChange()
    {
        if (OnStateChange)
        {
            OnStateChange(GetState());
        }
    }

    inline void SetState(std::vector<uint8_t> bytes, Iterator* iterator)
    {
        SerializerInstance->setState(bytes.data(), 0, bytes.size());
        iterator->offset += bytes.size();

        CallStateChange();
    }

    inline void ApplyPatch(std::vector<uint8_t> bytes, int Length)
    {
        SerializerInstance->patch(bytes.data(), 0, bytes.size());

        CallStateChange();
    }

    std::string GetMessageHandlerKey(const int32_t Type)
    {
        return "i" + std::to_string(Type);
    }


    std::string GetMessageHandlerKey(const std::string& Type)
    {
        return Type;
    }

    inline void CallOnLeave(int32_t StatusCode)
    {
        if (OnLeave) {
            OnLeave(StatusCode);
        }
    }

    void _onClose(int32_t StatusCode, const std::string& Reason, bool bWasClean)
    {
        // Clear the onMessageHandlers map
        OnMessageHandlers.clear();

        bHasConnected = false;

        // Call the onClose callback if it exists
        if (OnClose)
        {
            OnClose();
        }
    }

    void _onError(const std::string& Message)
    {
        if (OnError)
        {
            OnError(0, Message);
        }
    }

    void _onMessage(const std::vector<uint8_t>& data)
    {
        Poco::Thread::TID threadId = Poco::Thread::currentTid();
        std::cout << "\tRoom::_onMessage:: Current Thread ID: " << threadId << std::endl;

        int size = data.size();
        std::cout << "\tRoom::_onMessage:: Received binary data of size: " << size << std::endl;

        if(size <= 0) {
            return;
        }

        std::unique_ptr<Iterator> iterator = std::make_unique<Iterator>();
        iterator->offset = 0;

//        std::cout << "room handler bytes => ";
//        for (uint8_t byte: data) {
//            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
//        }
//        std::cout << std::endl;

        // todo: Make uint8_t
        unsigned char code = data[iterator->offset++];

        switch ((Protocol) code)
        {
            case Protocol::JOIN_ROOM:
            {
                std::cout << "Colyseus.Room: JOIN_ROOM" << std::endl;

                SerializerId = colyseus::schema::decodeString(data.data(), iterator.get());
                std::cout << "Decoded string (serializerId): " << SerializerId << std::endl;

                if(SerializerId.empty()) {
                    std::cout << "Unknown serializer because it's empty" << std::endl;
                    break;
                }

                if(!_setSerializer(SerializerId)) {
                 std::cerr << "\n\tUnknown SerializerId " << SerializerId << " setting to default" << std::endl;
                    _setSerializer("schema");
                }

                if(size > iterator->offset) {
                    std::cout << "Includes hand shake data, ignoring for now" << std::endl;
                    //currentStateSerializer->handshake(data.data(), iterator.get(), data.size());
                }
                else {
                    std::cout << "no hand shake" << std::endl;
                }

                std::cout << "~~~ OnJoin?" << std::endl;
                if (OnJoin)
                {
                    std::cout << "\t~~~ calling OnJoin" << std::endl;
                    OnJoin();
                    std::cout << "\t~~~ OnJoin Done" << std::endl;
                }

                std::cout << "\t~~~ sending JOIN_ROOM" << std::endl;

                std::vector<uint8_t> sendData = {static_cast<uint8_t>(Protocol::JOIN_ROOM)};
                std::cout << "sending data" << std::endl;
                ConnectionInstance->Send(sendData);
                break;
            }
            case Protocol::LEAVE_ROOM:
            {
                std::cout << "Colyseus.Room: LEAVE_ROOM" << std::endl;
                Leave(false);
                break;
            }
            case Protocol::ROOM_STATE:
            {
                std::cout << "Colyseus.Room: ROOM_STATE" << std::endl;
                std::vector<uint8_t> bytes(data.begin(), data.end());
                bytes.erase(bytes.begin());

                if (SerializerInstance == nullptr) {
                    std::cout << "No serializer, returning." << std::endl;
                    return;
                } else {
                    std::cout << "Serializer exists." << std::endl;
                }

                std::cout << "\tColyseus.Room: decoding state" << std::endl;
                SetState(bytes, iterator.get());
                break;
            }
            case Protocol::ROOM_STATE_PATCH:
            {
                std::cout << "Colyseus.Room: ROOM_STATE_PATCH" << std::endl;

                if (SerializerInstance == nullptr) {
                    std::cout << "No serializer, returning." << std::endl;
                    break;
                } else {
                    std::cout << "Serializer exists." << std::endl;
                }

                std::vector<uint8_t> bytes(data.begin(), data.end());
                bytes.erase(bytes.begin());
                ApplyPatch(bytes, iterator.get()->offset);
                break;
            }
            case Protocol::ROOM_DATA:
            {
                std::cout << "Colyseus.Room: ROOM_DATA" << std::endl;
                std::string Type;

                auto Bytes = data.data();
                auto Iterator = iterator.get();

                if (colyseus::schema::numberCheck(Bytes, Iterator))
                {
                    Type = GetMessageHandlerKey(colyseus::schema::decodeNumber(Bytes, Iterator));
                }
                else
                {
                    Type = GetMessageHandlerKey(colyseus::schema::decodeString(Bytes, Iterator));
                }

                auto it = OnMessageHandlers.find(Type);
                if(it == OnMessageHandlers.end()) {
                    std::cerr << "Room::onMessage() missing for type =>" << Type << std::endl;
                    return;
                }

                auto& handler = it->second;

                if(data.size() > iterator->offset) {
                    std::cout << " We have more data" << std::endl;
                    const char* bytes = reinterpret_cast<const char*>(data.data());
                    size_t dataSize = data.size();
                    auto offset = static_cast<size_t>(iterator->offset);
                    msgpack::object_handle MsgpackObjectHandle =
                            msgpack::unpack(bytes, dataSize, offset);
                    msgpack::object MsgpackObject = MsgpackObjectHandle.get();
                    handler(MsgpackObject);
                }
                else
                {
                    msgpack::object Empty;
                    handler(Empty);
                }

                break;
            }
            default:
                break;
        }
    }

    bool _setSerializer(const std::string& Id)
    {
        if (Id == "schema")
        {
            SerializerInstance = std::make_shared<SchemaSerializer<S>>();
            return true;
        }
        // ... Handle other serializer types ...
        return false;
    }

    std::shared_ptr<Serializer<S>> SerializerInstance;
};


#endif //COLYSEUSCPP_ROOM_HPP
