#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>

#include <NptLogging.h>

#include "upnp.hpp"
#include "manager.hpp"


int main(int /* argc */, char** argv)
{
    NPT_LogManager::GetDefault().Configure("plist:.level=FINE;.handlers=ConsoleHandler;.ConsoleHandler.colors=off;.ConsoleHandler.filter=42");

    gRM->setRootPath(argv[0]);
    gRM->init();

    Upnp::Server::sendAlive();
	std::this_thread::sleep_for(std::chrono::milliseconds(250));
    Upnp::Server::listen();

    char buf[256];
    while (gets(buf)) {
        if (*buf == 'q')
            break;
    }

    return 0;
}
