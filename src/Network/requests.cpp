#include "requests.hpp"
#include "manager.hpp"


NPT_Result MainHandler::SetupResponse(NPT_HttpRequest& request,
                         const NPT_HttpRequestContext& context,
                         NPT_HttpResponse&             response)
{
    printf("Request: %s\n", request.GetHeaders().GetHeader("USER-AGENT")->GetValue().GetChars());

    Renderer* renderer = nullptr;
    bool found =
        gRM->find(context.GetRemoteAddress(), renderer) ||
        gRM->find(request.GetHeaders(), renderer);

    if (!found)
    {
        renderer = gRM->defaultRenderer(context.GetRemoteAddress());
    }

    printf("Renderer: %s\n", renderer->Name.c_str());

    NPT_HttpEntity* entity = response.GetEntity();
    entity->SetContentType("text/html;  charset=\"utf-8\"");
    entity->SetInputStream("<html><body>Bye Bye!</body></html>");

    return NPT_SUCCESS;
}
