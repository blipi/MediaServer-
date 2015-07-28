#include <PltHttpServer.h>
#include <vector>

#include "utils.hpp"

class MainHandler : public NPT_HttpRequestHandler
{
public:
    NPT_Result SetupResponse(NPT_HttpRequest& request,
        const NPT_HttpRequestContext& context, NPT_HttpResponse& response);

    void addHandler(NPT_HttpRequestHandler* handler, std::string path,
        bool includeChildren, bool ignoreCase);

private:
    struct Route
    {
        std::string path;
        bool includeChildren;
        bool ignoreCase;
        NPT_HttpRequestHandler* handler;
    };

    std::vector<Route> _handlers;
};

class DiscoverHandler : public NPT_HttpRequestHandler
{
public:
    NPT_Result SetupResponse(NPT_HttpRequest& request,
        const NPT_HttpRequestContext& context, NPT_HttpResponse& response);
};
