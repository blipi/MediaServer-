#include <PltHttpServer.h>

class DescriptionFetch : public NPT_HttpRequestHandler
{
public:
    NPT_Result SetupResponse(NPT_HttpRequest&              /*request*/,
                             const NPT_HttpRequestContext& /*context*/,
                             NPT_HttpResponse&             response);
};