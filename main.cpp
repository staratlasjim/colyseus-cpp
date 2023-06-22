#include <csignal>
#include <iostream>
#include "src/poco/apps/MarketPlaceApp.hpp"
#include "src/poco/apps/DummyUpdater.hpp"

void signalHandler(int signal)
{
    if (signal == SIGINT)
    {
        std::cout << "Interrupt signal (Ctrl+C) received." << std::endl;
//        dynamic_cast<DummyUpdater&>(Poco::Util::Application::instance()).exit();
        dynamic_cast<MarketPlaceApp&>(Poco::Util::Application::instance()).exit();
    }
}

int main(int argc, char** argv)
{
    // Set up the signal handler for SIGINT (Ctrl+C)
    std::signal(SIGINT, signalHandler);

    MarketPlaceApp app;
//    DummyUpdater app;
    app.init(argc, argv);
    return app.run();
}