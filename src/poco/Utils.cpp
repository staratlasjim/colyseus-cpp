//
// Created by Jim Carter personal on 6/20/23.
//
#include "Utils.hpp"


Poco::JSON::Object::Ptr toPocoJsonObject(const nlohmann::json &j) {
    if(j.empty()) {
        return {};
    }
    if (j.is_object()) {
        return toPoco(j).extract<Poco::JSON::Object::Ptr>();
    } else {
        throw std::runtime_error("Input JSON is not an object");
    }
}

Poco::Dynamic::Var toPoco(const nlohmann::json &j) {
    switch (j.type()) {
        case nlohmann::json::value_t::object: {
            Poco::JSON::Object::Ptr pObj = new Poco::JSON::Object;
            for (auto& item : j.items()) {
                pObj->set(item.key(), toPoco(item.value()));
            }
            return pObj;
        }
        case nlohmann::json::value_t::array: {
            Poco::JSON::Array::Ptr pArray = new Poco::JSON::Array;
            for (auto& item : j) {
                pArray->add(toPoco(item));
            }
            return pArray;
        }
        case nlohmann::json::value_t::string:
            return j.get<std::string>();
        case nlohmann::json::value_t::boolean:
            return j.get<bool>();
        case nlohmann::json::value_t::number_integer:
            return j.get<int64_t>();
        case nlohmann::json::value_t::number_unsigned:
            return j.get<uint64_t>();
        case nlohmann::json::value_t::number_float:
            return j.get<double>();
        case nlohmann::json::value_t::null:
        default:
            return nullptr;
    }
}

nlohmann::json toNlohmannJson(const Poco::JSON::Object::Ptr &pObj) {
    return toNlohmannJson(Poco::Dynamic::Var(pObj));
}

// todo: performance issues here, need to fix at some point (or remove?); e.g. this copies the json on return...
nlohmann::json toNlohmannJson(const Poco::Dynamic::Var &var) {
    std::cout << "Parsing: " << var.toString() << std::endl;

    if (var.isEmpty()) {
        return "";
    } else if (var.isStruct()) {
        nlohmann::json j;
        Poco::JSON::Object::Ptr pObj = var.extract<Poco::JSON::Object::Ptr>();
        for (Poco::JSON::Object::ConstIterator it = pObj->begin(); it != pObj->end(); ++it) {
            j[it->first] = toNlohmannJson(it->second);
        }
        return j;
    } else if (var.isArray()) {
        nlohmann::json j;
        Poco::JSON::Array::Ptr pArray = var.extract<Poco::JSON::Array::Ptr>();
        for (int i = 0; i < pArray->size(); i++) {
            j.push_back(toNlohmannJson(pArray->get(i)));
        }
        return j;
    } else if (var.isBoolean()) {
        return var.extract<bool>();
    } else if (var.isInteger()) {
        return var.extract<int>();
    } else if (var.isNumeric()) {
        return var.extract<double>();
    } else if (var.isString()) {
        return var.extract<std::string>();
    } else {
        try {
            Poco::JSON::Object::Ptr jsonObject = var.extract<Poco::JSON::Object::Ptr>();
            nlohmann::json j;
            for (Poco::JSON::Object::ConstIterator it = jsonObject->begin(); it != jsonObject->end(); ++it) {
                j[it->first] = toNlohmannJson(it->second);
            }
            return j;
        } catch (Poco::Exception& e) {
            // The Var does not contain a type we support.
            throw std::runtime_error("Unsupported type in Poco::JSON:: " + e.message() + " :: " + var.toString());
        }

    }
}

std::future<void> setTimeout(const std::function<void()> &func, int delay) {
    return std::async(std::launch::async, [func, delay]() {
        // Sleep for the required interval
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));

        // Call the function
        func();
    });
}

std::int64_t getTimeSinceEpochMs() {
    using namespace std::chrono;
    using Clock = std::chrono::steady_clock;

    if (Clock::period::num == 1 && Clock::period::den == 1000) {
        return duration_cast<milliseconds>(Clock::now().time_since_epoch()).count();
    } else {
        return time_point_cast<milliseconds>(Clock::now()).time_since_epoch().count();
    }
}




