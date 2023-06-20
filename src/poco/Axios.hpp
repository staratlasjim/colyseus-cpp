//
// Created by Jim Carter personal on 5/23/23.
//

#ifndef COLYSEUSTEST_AXIOS_HPP
#define COLYSEUSTEST_AXIOS_HPP

#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/StreamCopier.h>
#include <Poco/URI.h>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <sstream>
#include <nlohmann/json.hpp>

using HTTPClientSession = Poco::Net::HTTPClientSession;
using HTTPRequest = Poco::Net::HTTPRequest;
using HTTPResponse = Poco::Net::HTTPResponse;
using Ptr = Poco::JSON::Object::Ptr;

class Axios {
public:
    enum class ResponseType {
        JSON,
        TEXT,
        HTML
    };

    struct Response {
        HTTPResponse::HTTPStatus status;
        std::string statusText;
        std::string text;
        Ptr json;
        ResponseType responseType;
    };

    Axios();
    ~Axios();

    Response get(const std::string& url, ResponseType responseType);
    Response post(const std::string& url, Ptr data);

    Response post(const std::string& url, const nlohmann::json& data);
};


#endif //COLYSEUSTEST_AXIOS_HPP
