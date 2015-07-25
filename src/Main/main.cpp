#include "PltUPnP.h"
#include "PltFileMediaServer.h"

#include <stdlib.h>

struct Options {
    const char* path;
    const char* friendly_name;
    const char* guid;
    NPT_UInt32  port;
};

int
main(int /* argc */, char** argv)
{
    // Setup options
    Options options;
    options.path     = "D:\\Media";
    options.friendly_name = "Media File";
    options.guid = NULL;
    options.port = 8081;

    // setup Neptune logging
    NPT_LogManager::GetDefault().Configure("plist:.level=FINE;.handlers=ConsoleHandler;.ConsoleHandler.colors=on;.ConsoleHandler.filter=42");

	/* for faster DLNA faster testing */
    PLT_Constants::GetInstance().SetDefaultDeviceLease(NPT_TimeInterval(60.));

    PLT_UPnP upnp;
    PLT_DeviceHostReference device(
        new PLT_FileMediaServer(
            options.path,
            options.friendly_name?options.friendly_name:"Platinum UPnP Media Server",
            false,
            options.guid, // NULL for random ID
            (NPT_UInt16)options.port)
            );

    NPT_List<NPT_IpAddress> list;
    NPT_CHECK_SEVERE(PLT_UPnPMessageHelper::GetIPAddresses(list));
    NPT_String ip = list.GetFirstItem()->ToString();

    device->m_ModelDescription = "Platinum File Media Server";
    device->m_ModelURL = "http://www.plutinosoft.com/";
    device->m_ModelNumber = "1.0";
    device->m_ModelName = "Platinum File Media Server";
    device->m_Manufacturer = "Plutinosoft";
    device->m_ManufacturerURL = "http://www.plutinosoft.com/";

    upnp.AddDevice(device);
    NPT_String uuid = device->GetUUID();

    NPT_CHECK_SEVERE(upnp.Start());
    NPT_LOG_INFO("Press 'q' to quit.");

    char buf[256];
    while (gets(buf)) {
        if (*buf == 'q')
            break;
    }

    upnp.Stop();

    return 0;
}
