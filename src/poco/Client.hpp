//
// Created by Jim Carter personal on 6/15/23.
//

#ifndef COLYSEUSCPP_CLIENT_HPP
#define COLYSEUSCPP_CLIENT_HPP

#include "Connection.hpp"
#include "Room.hpp"
#include "Utils.hpp"
#include "Axios.hpp"
#include <nlohmann/json.hpp>
#include <utility>

typedef nlohmann::json JoinOptions;

class MatchMakeError
{
public:
    MatchMakeError(int code, std::string  message) : code(code), message(std::move(message))
    {
    }

    int code;
    std::string message;
};

class Client
{
public:
    std::string endpoint;

    explicit Client(std::string endpoint) : endpoint(std::move(endpoint))
    {
    }

    template <typename S>
    inline void JoinOrCreate(const std::string& RoomName, const JoinOptions& Options,
                             const std::function<void(std::shared_ptr<MatchMakeError>, std::shared_ptr<Room<S>>)>& Callback)
    {
        CreateMatchMakeRequest<S>("joinOrCreate", RoomName, Options, Callback);
    }

    template <typename S>
    inline void Join(const std::string& RoomName, const JoinOptions& Options,
                     const std::function<void(std::shared_ptr<MatchMakeError>, std::shared_ptr<Room<S>>)>& Callback)
    {
        CreateMatchMakeRequest<S>("join", RoomName, Options, Callback);
    }

    template <typename S>
    inline void Create(const std::string& RoomName, const JoinOptions& Options,
                       const std::function<void(std::shared_ptr<MatchMakeError>, std::shared_ptr<Room<S>>)>& Callback)
    {
        CreateMatchMakeRequest<S>("create", RoomName, Options, Callback);
    }

    template <typename S>
    inline void JoinById(const std::string& roomId, const JoinOptions& Options,
                         const std::function<void(std::shared_ptr<MatchMakeError>, std::shared_ptr<Room<S>>)>& Callback)
    {
        CreateMatchMakeRequest<S>("joinById", roomId, Options, Callback);
    }

    template <typename S>
    inline void Reconnect(const std::string& roomId, const std::string& sessionId,
                          const std::function<void(std::shared_ptr<MatchMakeError>, std::shared_ptr<Room<S>>)>& Callback)
    {
        CreateMatchMakeRequest<S>("joinById", roomId, {{"sessionId", sessionId}}, Callback);
    }

private:
    template <typename S>
    void CreateMatchMakeRequest(const std::string& Method, const std::string& RoomName, const JoinOptions& Options,
                                std::function<void(std::shared_ptr<MatchMakeError>, std::shared_ptr<Room<S>>)> Callback)
    {
        try {
            Axios axios;
            const std::string httpEndpoint = replace(this->endpoint, "ws", "http") + "/matchmake/" + Method + "/" + RoomName;

            Axios::Response response = axios.post(httpEndpoint, Options);

            // todo: Handle ERRORS!!!!
            std::shared_ptr<Room<S>> room = std::make_shared<Room<S>>(RoomName);

            Poco::JSON::Object::Ptr roomData = response.json->getObject("room");

            // check for error
            if(roomData->has("error") && !roomData->getValue<std::string>("error").empty()) {
                int Code = roomData->getValue<int>("code");
                std::string Message = roomData->getValue<std::string>("error");
                std::cerr << "Error returned in JSON from matchmaking room (code: " << Code << ") " << Message << std::endl;
                Callback(std::make_shared<MatchMakeError>(Code, Message), nullptr);
                return;
            }

            int clients = roomData->getValue<int>("clients");
            std::string createdAt = roomData->getValue<std::string>("createdAt");
            int maxClients = roomData->getValue<int>("maxClients");
            Poco::JSON::Object::Ptr metadata = roomData->getObject("metadata");

            std::string name = roomData->getValue<std::string>("name");
            std::string processId = roomData->getValue<std::string>("processId");
            std::string roomId = roomData->getValue<std::string>("roomId");
            std::string sessionId = roomData->getValue<std::string>("sessionId");

            std::cout << "Room Name: " << name << ", Clients: " << clients << ", Created At: " << createdAt << std::endl;
            std::cout << "Max Clients: " << maxClients << std::endl;
            std::cout << "Process ID: " << processId << ", Room ID: " << roomId << ", Session ID: " << sessionId
                      << std::endl;

            std::stringstream uri;
            uri << this->endpoint << "/" << processId << "/" << roomId << "?" << "sessionId=" << sessionId;
            std::string roomEndpoint = uri.str();
            std::cout << "\n\tWebsocket Room endpoint:" << roomEndpoint << std::endl;
            room->SetSessionId(sessionId);
            room->SetId(roomId);

            room->OnError = [room, Callback](const int& Code, const std::string& Message) {
                room->OnJoin = nullptr;
                std::shared_ptr<MatchMakeError> Error = std::make_shared<MatchMakeError>(Code, Message);
                Callback(Error, nullptr);
                room->OnError = nullptr;
            };

            room->OnJoin = [room, Callback]() {
                room->OnError = nullptr;
                Callback(nullptr, room);
                room->OnJoin = nullptr;
            };

            room->Connect(roomEndpoint);
        }
        catch(std::runtime_error& error) {
            std::cerr << "Error! " << error.what() << std::endl;
            Callback(std::make_shared<MatchMakeError>(400, std::string(error.what()), nullptr));
        }
    }
};


#endif //COLYSEUSCPP_CLIENT_HPP
