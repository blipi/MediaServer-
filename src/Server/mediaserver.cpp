#include "mediaserver.hpp"
#include "requests.hpp"

#include <stdlib.h>
#include <stdexcept>
#include <exception>

MediaServerPP* MediaServerPP::_instance = NULL;


MediaServerPP::MediaServerPP():
    _name("MediaServer++"),
    _serverInterface(nullptr)
{
    // HACK: Hardcoded port
    _server = new PLT_HttpServer(NPT_IpAddress::Any, 8082);
    _server->AddRequestHandler(new DescriptionFetch(), "/description/fetch");
    _server->Start();
}

NPT_NetworkInterface* MediaServerPP::serverInterface()
{
    if (!_serverInterface)
    {
        NPT_List<NPT_NetworkInterface*> interfaces;
        NPT_NetworkInterface::GetNetworkInterfaces(interfaces);

        for (auto it = interfaces.GetFirstItem(); it; ++it)
        {
            auto interface = *it;
            if (!(interface->GetFlags() & NPT_NETWORK_INTERFACE_FLAG_BROADCAST) ||
                 (interface->GetFlags() & NPT_NETWORK_INTERFACE_FLAG_LOOPBACK))
            {
                printf("Skipping %s\n", interface->GetName().GetChars());
                continue;
            }

            printf("Using %s\n", interface->GetName().GetChars());
            _serverInterface = interface;
            break;
        }

        if (!_serverInterface && interfaces.GetFirstItem() != interfaces.GetLastItem())
        {
            _serverInterface = *interfaces.GetFirstItem();
        }
        else if (!_serverInterface)
        {
            throw new std::runtime_error("No usable network interface found for UPnP multicast");
        }
    }

    return _serverInterface;
}