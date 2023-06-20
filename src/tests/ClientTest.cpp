//
// Created by Jim Carter personal on 6/20/23.
//
/*
 * Axios axios;

        Poco::JSON::Object::Ptr data = new Poco::JSON::Object;
        data->set("password", "xa123");
        Axios::Response res = axios.post("http://localhost:2567/matchmake/joinOrCreate/dummy", data);
 */

#include "gtest/gtest.h"
#include "nlohmann/json.hpp"
#include "Poco/JSON/Object.h"
#include "../poco/Utils.hpp"
#include "../poco/Axios.hpp"

const std::string mustHaveTestServerRunningError = "~~ These integration tests require that you have Colysues server running on port 2567 with the 'dummy room' registered";

TEST(ClientTest, MustHaveServerRunning) {
    const std::string EndPoint = "http://localhost:2567/matchmake/joinOrCreate/dummy";

    Axios axios;
    Poco::JSON::Object::Ptr data = new Poco::JSON::Object;


    try {
        Axios::Response res = axios.post("http://localhost:2567/matchmake/joinOrCreate/dummy", data);
        ASSERT_EQ(res.status, HTTPResponse::HTTPStatus::HTTP_OK) << mustHaveTestServerRunningError << std::endl;
    }
    catch(std::runtime_error& error) {
        ASSERT_FALSE(true) << mustHaveTestServerRunningError << std::endl;
    }

}