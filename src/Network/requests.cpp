#include "requests.hpp"


NPT_Result DescriptionFetch::SetupResponse(NPT_HttpRequest& request,
                         const NPT_HttpRequestContext& /*context*/,
                         NPT_HttpResponse&             response)
{
    printf("Request: %s\n", request.GetHeaders().GetHeader("USER-AGENT")->GetValue().GetChars());

    NPT_HttpEntity* entity = response.GetEntity();
    entity->SetContentType("text/html;  charset=\"utf-8\"");
    entity->SetInputStream("<html><body>Bye Bye!</body></html>");

    return NPT_SUCCESS;
}
