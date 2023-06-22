//
// Created by Jim Carter personal on 6/22/23.
//

#include "DummyUpdater.hpp"
#include <iostream>

#include "Poco/Util/HelpFormatter.h"
#include "Poco/Util/Option.h"


using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;

DummyUpdater::DummyUpdater() : _helpRequested(false), _roomName("dummy"), _serverAddress("ws://localhost:2567")
{
}

DummyUpdater::~DummyUpdater()
{
}

void DummyUpdater::initialize(Application& self)
{
    loadConfiguration();
    Application::initialize(self);
}

void DummyUpdater::uninitialize()
{
    Application::uninitialize();
}

void DummyUpdater::defineOptions(OptionSet& options)
{
    Application::defineOptions(options);

    options.addOption(
            Option("help", "h", "display help information")
                    .required(false)
                    .repeatable(false)
                    .callback(OptionCallback<DummyUpdater>(this, &DummyUpdater::handleHelp)));

    options.addOption(
            Option("server", "s", "specify the remote server address")
                    .required(false)
                    .repeatable(false)
                    .argument("address")
                    .callback(OptionCallback<DummyUpdater>(this, &DummyUpdater::handleServer)));

    options.addOption(
            Option("room", "r", "specify the room name to connect to")
                    .required(false)
                    .repeatable(false)
                    .argument("room")
                    .callback(OptionCallback<DummyUpdater>(this, &DummyUpdater::handleRoom)));
}


int DummyUpdater::main(const std::vector<std::string>& args)
{
    if (!_helpRequested)
    {
        Client client(_serverAddress);
        JoinOptions options;
        options["roomName"] = _roomName;

        client.JoinOrCreate<State>(_roomName, options,
                            [this](std::shared_ptr<MatchMakeError> error, std::shared_ptr<Room<State>> room)
                            {
                                if (error)
                                {
                                    std::cerr << "Error joining or creating room: " << error->message << std::endl;
                                    _exitEvent.set();
                                    return;
                                }

                                std::cout << "Connected to room: " << room->GetId() << std::endl;
                                _room = room;
                                _room->OnLeave = [this](int32_t statusCode) {
                                    std::cout << "Left the room with status code: " << statusCode << std::endl;
                                    _exitEvent.set();
                                };

                                // Set the onError handler
                                _room->OnError = [this](int32_t statusCode, const std::string& message) {
                                    std::cerr << "Room error with status code: " << statusCode << " and message: " << message << std::endl;
                                    _exitEvent.set();
                                };
                                // Add your custom room event handlers or logic here
                            });
        _exitEvent.wait();
    }
    return Application::EXIT_OK;
}

void DummyUpdater::handleHelp(const std::string& name, const std::string& value)
{
    _helpRequested = true;
    displayHelp();
    stopOptionsProcessing();
}

void DummyUpdater::handleServer(const std::string& name, const std::string& value)
{
    _serverAddress = value;
}

void DummyUpdater::handleRoom(const std::string& name, const std::string& value)
{
    _roomName = value;
}

void DummyUpdater::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("A simple updater that connects to a remote server.");
    helpFormatter.format(std::cout);
}

void DummyUpdater::exit()
{
    std::cout << "Exiting the application..." << std::endl;
    _exitEvent.set();
}
