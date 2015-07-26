#include <guid.h>

#include <string>
#include <iostream>
#include <sstream>

#include <NptNetwork.h>
#include <PltHttpServer.h>


class MediaServerPP
{
private:
    static MediaServerPP* _instance;
    MediaServerPP();

public:
    static MediaServerPP* get()
    {
        if (!_instance)
        {
            _instance = new MediaServerPP();
        }
        return _instance;
    }

    inline std::string name() { return _name; }
    inline Guid udn() { return _udn; }
    inline std::string udnString();

    NPT_NetworkInterface* serverInterface();

private:
    std::string _name;
    Guid _udn;

    NPT_NetworkInterface* _serverInterface;
    PLT_HttpServer* _server;
};

#define gMS MediaServerPP::get()


std::string MediaServerPP::udnString()
{
    std::stringstream ss;
    ss << _udn;
    return ss.str();
}
