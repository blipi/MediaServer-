#include <string>
#include <thread>

#include <NptSockets.h>

namespace Upnp
{
    /** Carriage return and line feed. */
    const std::string CRLF = "\r\n";

    /** The Constant ALIVE. */
    const std::string ALIVE = "ssdp:alive";

    /**
     * IPv4 Multicast channel reserved for SSDP by Internet Assigned Numbers Authority (IANA).
     * MUST be 239.255.255.250.
     */
    const std::string IPV4_UPNP_HOST = "239.255.255.250";

    /**
     * IPv6 Multicast channel reserved for SSDP by Internet Assigned Numbers Authority (IANA).
     * MUST be [FF02::C].
     */
    const std::string IPV6_UPNP_HOST = "[FF02::C]";

    /**
     * Multicast channel reserved for SSDP by Internet Assigned Numbers Authority (IANA).
     * MUST be 1900.
     */
    const int UPNP_PORT = 1900;

    /** The Constant BYEBYE. */
    const std::string BYEBYE = "ssdp:byebye";

    class Server
    {
    private:
        static std::thread* _listenerThread;
        static std::thread* _aliveThread;

    private:
        Server() { }

        static void sendDiscover(std::string host, int port, std::string st);
        static void sendReply(std::string host, int port, std::string msg);
        static NPT_UdpMulticastSocket* getNewMulticastSocket();
        static void sleep(int delay);
        static void sendMessage(NPT_UdpSocket* socket, std::string nt, std::string message);
        static std::string buildMsg(std::string nt, std::string message);
        static NPT_IpAddress getUPNPAddress();

    public:
        static void sendAlive();
        static void sendByeBye();
        static void listen();
        static void shutDownListener();

    };
}
