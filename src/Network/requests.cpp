#include "requests.hpp"
#include "manager.hpp"
#include "utils.hpp"


NPT_Result MainHandler::SetupResponse(NPT_HttpRequest& request,
    const NPT_HttpRequestContext& context, NPT_HttpResponse& response)
{
    printf("Request: %s\n", request.GetHeaders().GetHeader("USER-AGENT")->GetValue().GetChars());
    printf("\tProtocol: %s\n", request.GetProtocol().GetChars());
    printf("\tMethod: %s\n", request.GetMethod().GetChars());

    Renderer* renderer = nullptr;
    bool found =
        gRM->find(context.GetRemoteAddress(), renderer) ||
        gRM->find(request.GetHeaders(), renderer);

    if (!found)
    {
        renderer = gRM->defaultRenderer(context.GetRemoteAddress());
    }

    printf("Renderer: %s\n", renderer->name().c_str());

    // Parse headers
    auto headers = request.GetHeaders().GetHeaders();
    for (auto it = headers.GetFirstItem(); it; ++it)
    {
        auto item = *it;

        // TODO: Parse SOAP headers
    }

    // Add custom header
    response.GetHeaders().AddHeader(NPT_HTTP_HEADER_SERVER, "UPnP/1.0 DLNADOC/1.50 MediaServerPP/0.1");

    // Handle route
    auto path = request.GetUrl().GetPath();
    for (auto it = _handlers.begin(); it != _handlers.end(); ++it)
    {
        Route& route = *it;

        int result = route.includeChildren ?
            path.CompareN(route.path.c_str(), route.path.length(), route.ignoreCase) :
            path.Compare(route.path.c_str(), route.ignoreCase);

        if (result == 0)
        {
            return route.handler->SetupResponse(request, context, response);
        }
    }

    return NPT_FAILURE;
}

void MainHandler::addHandler(NPT_HttpRequestHandler *handler, std::string path,
    bool includeChildren, bool ignoreCase)
{
    _handlers.push_back(Route{path, includeChildren, ignoreCase, handler});
}

NPT_Result DiscoverHandler::SetupResponse(NPT_HttpRequest &request,
    const NPT_HttpRequestContext &context, NPT_HttpResponse &response)
{
    NPT_HttpEntity* entity = response.GetEntity();

    NPT_HttpHeaders& headers = response.GetHeaders();
    headers.AddHeader(NPT_HTTP_HEADER_CONTENT_TYPE, "text/xml; charset=\"utf-8\"");
    headers.AddHeader(NPT_HTTP_HEADER_ACCEPT_RANGES, "bytes");
    headers.AddHeader(NPT_HTTP_HEADER_CONNECTION, "keep-alive");
    headers.AddHeader("Cache-Control", "no-cache");
    headers.AddHeader("Expires", "0");

    bool isDescriptionFetch = request.GetUrl().GetPath().Compare("/description/fetch") == 0;

    if (!isDescriptionFetch)
    {
        const char* input = request.GetUrl().GetPath().GetChars();
        // TODO: Use input stream
        //std::ifstream is(input)
        // TODO: sendStream(is)
    }
    else
    {
        // "resources/xml/MSpp.xml";
        // TODO: Reparse XML according to renderer
    }

    entity->SetInputStream("<html><body>Bye Bye!</body></html>");

    return NPT_SUCCESS;
}
