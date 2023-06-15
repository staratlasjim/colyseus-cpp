//
// Created by Jim Carter personal on 6/15/23.
//

#ifndef COLYSEUSCPP_CLIENT_HPP
#define COLYSEUSCPP_CLIENT_HPP

#include "Connection.hpp"
#include "Room.hpp"

#include <nlohmann/json.hpp>

typedef nlohmann::json JoinOptions;

class MatchMakeError
{
public:
    MatchMakeError(int Code, const std::string& Message) : Code(Code), Message(Message)
    {
    }

    int Code;
    std::string Message;
};

class Client {

};


#endif //COLYSEUSCPP_CLIENT_HPP
