//
// Created by Jim Carter personal on 6/22/23.
//

#ifndef COLYSEUSCPP_MARKETPLACEAPP_HPP
#define COLYSEUSCPP_MARKETPLACEAPP_HPP

#include <Poco/Util/Application.h>
#include "../Client.hpp"
#include "../../gen/gmp/OverviewRoomState.hpp"

class MarketPlaceApp  : public Poco::Util::Application {

public:
    MarketPlaceApp();
    ~MarketPlaceApp();

    void exit();

protected:
    void initialize(Poco::Util::Application &self) override;
    void uninitialize() override;

    void defineOptions(Poco::Util::OptionSet &options) override;

    int main(const std::vector<std::string> &args) override;

    void handleHelp(const std::string& name, const std::string& value);
    void handleServer(const std::string& name, const std::string& value);
    void handleRoom(const std::string& name, const std::string& value);

    void displayHelp();

    std::string _serverAddress;
    std::string _roomName;
    bool _helpRequested;

    // Add a shared_ptr to a Room<State> object
    std::shared_ptr<Room<OverviewRoomState>> _room;

    // Add a protected Poco Event object
    Poco::Event _exitEvent;
};


#endif //COLYSEUSCPP_MARKETPLACEAPP_HPP
