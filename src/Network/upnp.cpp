#include "upnp.hpp"
#include "mediaserver.hpp"

#include <Neptune.h>
#include <NptTime.h>

namespace Upnp {

std::thread* Server::_listenerThread = NULL;
std::thread* Server::_aliveThread = NULL;

void Server::sendDiscover(std::string host, int port, std::string st)
{
    std::string udn = gMS->udnString();
    std::string serverHost = "192.168.1.103";
    int serverPort = 8082;

    NPT_DateTime date;
    NPT_TimeStamp now;

    NPT_System::GetCurrentTimeStamp(now);
    date.FromTimeStamp(now);
    NPT_String date_str = date.ToString(NPT_DateTime::FORMAT_RFC_1123);

    if (st.compare(udn) == 0)
    {
        udn = "";
    }
    else
    {
        udn += "::";
    }

    std::string discovery;
    discovery.reserve(1024);
    discovery.append("HTTP/1.1 200 OK").append(CRLF);
	discovery.append("CACHE-CONTROL: max-age=1200").append(CRLF);
	discovery.append("DATE: ").append(date_str.GetChars()).append(" GMT").append(CRLF);
	discovery.append("LOCATION: http://").append(serverHost).append(":").append(std::to_string(serverPort)).append("/description/fetch").append(CRLF);
	discovery.append("SERVER: ").append(gMS->name()).append(CRLF);
	discovery.append("ST: ").append(st).append(CRLF);
	discovery.append("EXT: ").append(CRLF);
	discovery.append("USN: ").append(udn).append(st).append(CRLF);
	discovery.append("Content-Length: 0").append(CRLF).append(CRLF);

    sendReply(host, port, discovery);
}

void Server::sendReply(std::string host, int port, std::string msg)
{
    NPT_UdpSocket socket;
    NPT_DataBuffer buffer;

    NPT_IpAddress ip;
    NPT_SocketAddress address;

    ip.Parse(host.c_str());
    address.SetIpAddress(ip);
    address.SetPort(port);

    buffer.SetData((NPT_Byte*)msg.c_str(), msg.length());
    socket.Send(buffer, &address);
}

void Server::sendAlive()
{
    NPT_UdpMulticastSocket* multicastSocket = getNewMulticastSocket();
    NPT_IpAddress upnpAddress = getUPNPAddress();

    multicastSocket->JoinGroup(upnpAddress);
    sendMessage(multicastSocket, "upnp:rootdevice", ALIVE);
	sendMessage(multicastSocket, gMS->udnString(), ALIVE);
	sendMessage(multicastSocket, "urn:schemas-upnp-org:device:MediaServer:1", ALIVE);
	sendMessage(multicastSocket, "urn:schemas-upnp-org:service:ContentDirectory:1", ALIVE);
	sendMessage(multicastSocket, "urn:schemas-upnp-org:service:ConnectionManager:1", ALIVE);
}

NPT_UdpMulticastSocket* Server::getNewMulticastSocket()
{
    NPT_NetworkInterface* serverInterface = gMS->serverInterface();

    auto addresses = serverInterface->GetAddresses();
    if (!addresses.GetFirstItem())
    {
        throw new std::runtime_error("No usable addresses found for UPnP multicast");
    }

    NPT_SocketAddress localAddress((*addresses.GetFirstItem()).GetPrimaryAddress(), 0);

    NPT_UdpMulticastSocket* ssdpSocket = new NPT_UdpMulticastSocket();
    ssdpSocket->Bind(localAddress, true);
    ssdpSocket->SetTimeToLive(32);
    return ssdpSocket;
}

void Server::sendByeBye()
{
    NPT_UdpMulticastSocket* multicastSocket = getNewMulticastSocket();
    NPT_IpAddress upnpAddress = getUPNPAddress();

    multicastSocket->JoinGroup(upnpAddress);
    sendMessage(multicastSocket, "upnp:rootdevice", BYEBYE);
	sendMessage(multicastSocket, "urn:schemas-upnp-org:device:MediaServer:1", BYEBYE);
	sendMessage(multicastSocket, "urn:schemas-upnp-org:service:ContentDirectory:1", BYEBYE);
	sendMessage(multicastSocket, "urn:schemas-upnp-org:service:ConnectionManager:1", BYEBYE);
}

void Server::sleep(int delay)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void Server::sendMessage(NPT_UdpSocket* socket, std::string nt, std::string message)
{
    std::string msg = buildMsg(nt, message);

    NPT_IpAddress upnpAddress = getUPNPAddress();
    NPT_DataBuffer buffer;
    NPT_SocketAddress address;

    address.SetIpAddress(upnpAddress);
    address.SetPort(UPNP_PORT);

    buffer.SetData((NPT_Byte*)msg.c_str(), msg.length());
    socket->Send(buffer, &address);
}


void Server::listen()
{
    _aliveThread = new std::thread([] () {
        int delay = 10000;

        while (true)
        {
            sleep(delay);
            printf("SENDING ALIVE\n");
            sendAlive();

            delay = (delay == 10000 ? 20000 : 180000);
        }
    });

    _listenerThread = new std::thread([] () {
        bool bindErrorReported = false;

        while (true)
        {
            NPT_UdpMulticastSocket socket;
            NPT_IpAddress upnpAddress = getUPNPAddress();

            // TODO: Setup local port and network interface
            socket.SetTimeToLive(4);
            socket.JoinGroup(upnpAddress);

            while (true)
            {
                char* buf = new char[1024];

                NPT_DataBuffer receivePacket;
                NPT_SocketAddress address;
                receivePacket.SetBuffer((NPT_Byte*)buf, 1024);

                socket.Receive(receivePacket, &address);

                std::string s((char*)receivePacket.GetData(), receivePacket.GetDataSize());
                if (s.compare(0, 8, "M-SEARCH") == 0)
                {
                    NPT_String remoteAddr = address.GetIpAddress().ToString();
                    int remotePort = address.GetPort();

                    // if (gMS->isAddressAllowed(address))
                    {
                        if (s.find_first_of("urn:schemas-upnp-org:service:ContentDirectory:1") > 0)
                        {
                            sendDiscover(remoteAddr.GetChars(), remotePort, "urn:schemas-upnp-org:service:ContentDirectory:1");
                        }

                        if (s.find_first_of("upnp:rootdevice") > 0)
                        {
							sendDiscover(remoteAddr.GetChars(), remotePort, "upnp:rootdevice");
						}

                        if (s.find_first_of("urn:schemas-upnp-org:device:MediaServer:1") > 0)
                        {
							sendDiscover(remoteAddr.GetChars(), remotePort, "urn:schemas-upnp-org:device:MediaServer:1");
						}

						if (s.find_first_of("ssdp:all") > 0)
                        {
							sendDiscover(remoteAddr.GetChars(), remotePort, "urn:schemas-upnp-org:device:MediaServer:1");
						}

						if (s.find_first_of(gMS->udnString()) > 0)
                        {
							sendDiscover(remoteAddr.GetChars(), remotePort, gMS->udnString());
						}
                    }
                }
                else if (s.compare(0, 6, "NOTIFY") == 0)
                {
                    /*
                    String remoteAddr = address.getHostAddress();
					int remotePort = receivePacket.getPort();

					logger.trace("Receiving a NOTIFY from [" + remoteAddr + ":" + remotePort + "]");
                    */
                }
            }
        }
    });
}

void Server::shutDownListener()
{
    // TODO: Stop both threads
}

std::string Server::buildMsg(std::string nt, std::string message)
{
    std::string sb;
    sb.reserve(1024);

    sb.append("NOTIFY * HTTP/1.1").append(CRLF);
	sb.append("HOST: ").append(IPV4_UPNP_HOST).append(":").append(std::to_string(UPNP_PORT)).append(CRLF);
	sb.append("NT: ").append(nt).append(CRLF);
	sb.append("NTS: ").append(message).append(CRLF);

	if (message.compare(ALIVE) == 0)
    {
        // HACK: Host and Port are hardcoded
		sb.append("LOCATION: http://").append("192.168.1.103").append(":").append("8082").append("/description/fetch").append(CRLF);
	}

	sb.append("USN: ").append(gMS->udnString());

	if (nt.compare(gMS->udnString()) != 0)
    {
		sb.append("::").append(nt);
	}

	sb.append(CRLF);

	if (message.compare(ALIVE) == 0)
    {
		sb.append("CACHE-CONTROL: max-age=1800").append(CRLF);
	}

	if (message.compare(ALIVE) == 0)
    {
		sb.append("SERVER: ").append(gMS->name()).append(CRLF);
	}

	sb.append(CRLF);
	return sb;
}

NPT_IpAddress Server::getUPNPAddress()
{
    NPT_IpAddress ip;
    ip.Parse(IPV4_UPNP_HOST.c_str());
    return ip;
}

}
