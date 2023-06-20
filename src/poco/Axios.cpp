//
// Created by Jim Carter personal on 5/23/23.
//

#include "Axios.hpp"
#include <Poco/Exception.h>
#include <Poco/JSON/Stringifier.h>
#include <nlohmann/json.hpp>
#include "Utils.hpp"

using Poco::JSON::Stringifier;
using Poco::Exception;
using Poco::JSON::Parser;
using Poco::JSON::Object;
using URI = Poco::URI;

Axios::Axios() = default;

Axios::~Axios() = default;

Axios::Response Axios::get(const std::string &url, ResponseType responseType) {
    URI uri(url);
    HTTPClientSession session(uri.getHost(), uri.getPort());

    HTTPRequest request(HTTPRequest::HTTP_GET, uri.getPathAndQuery(), HTTPRequest::HTTP_1_1);
    session.sendRequest(request);

    HTTPResponse res;
    std::istream &rs = session.receiveResponse(res);

    Response response;
    response.status = res.getStatus();
    response.statusText = res.getReason();
    response.responseType = responseType;

    std::stringstream ss;
    Poco::StreamCopier::copyStream(rs, ss);
    std::string str = ss.str();

    if (responseType == ResponseType::JSON) {
        try {
            Parser parser;
            Poco::Dynamic::Var result = parser.parse(str);
            response.json = result.extract<Object::Ptr>();
        } catch (Poco::Exception &e) {
            // Handle JSON parsing errors
            std::stringstream error;
            error << "JSON parsing error: " << e.displayText();
            throw Exception(error.str());
        }
    } else {
        response.text = str;
    }

    return response;
}

// todo: need way better error handling here.
Axios::Response Axios::post(const std::string &url, Ptr data) {
    URI uri(url);
    HTTPClientSession session(uri.getHost(), uri.getPort());

    std::stringstream ss;
    Stringifier::stringify(data, ss);

    HTTPRequest request(HTTPRequest::HTTP_POST, uri.getPathAndQuery(), HTTPRequest::HTTP_1_1);
    request.set("Accept", "application/json");
    request.setContentType("application/json");
    request.setContentLength(ss.str().length());

    Response response;

    try {
        std::ostream &os = session.sendRequest(request);
        os << ss.str();
        std::cout << "SENDING JSON" << std::endl << ss.str() << std::endl;

        HTTPResponse res;
        std::istream &rs = session.receiveResponse(res);

        response.status = res.getStatus();
        response.statusText = res.getReason();
        response.responseType = ResponseType::JSON;

        std::stringstream responseStream;
        Poco::StreamCopier::copyStream(rs, responseStream);
        std::string str = responseStream.str();
        std::cout << "JSON" << std::endl << str << std::endl;

        Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        response.json = result.extract<Object::Ptr>();
    } catch (Poco::Exception &e) {
        // Handle JSON parsing errors
        std::stringstream error;
        error << "JSON parsing error (" << e.code() << ") " << e.displayText() << " - " << e.message() << std::endl;
        throw std::runtime_error(error.str());
    }

    return response;
}

Axios::Response Axios::post(const std::string &url, const nlohmann::json &data) {
    Poco::JSON::Object::Ptr pObj = toPocoJsonObject(data);
    return post(url, pObj);
}