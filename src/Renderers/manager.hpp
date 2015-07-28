#include <algorithm>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include <NptSockets.h>
#include <NptHttp.h>

#include "Renderer.pb.h"


struct SocketAddressComparator
{
    bool operator()(const NPT_SocketAddress& a, const NPT_SocketAddress& b) const
    {
        if (a.GetIpAddress().AsLong() < b.GetIpAddress().AsLong())
        {
            return a.GetPort() < b.GetPort();
        }

        return false;
    }
};

class RendererManager
{
private:
    static RendererManager* _instance;
    RendererManager();

public:
    static RendererManager* get()
    {
        if (!_instance)
        {
            _instance = new RendererManager();
        }
        return _instance;
    }

    void setRootPath(std::string rootPath);
    void init();

	bool find(NPT_SocketAddress addr, Renderer*& renderer);
	bool find(NPT_HttpHeaders& headers, Renderer*& renderer);
	Renderer* defaultRenderer(NPT_SocketAddress addr);

private:
	void parse(std::string root, std::string file);

private:
    std::string _rootPath;
	Renderer* _defaultRenderer;
	std::vector<Renderer*> _renderers;
	std::map<NPT_SocketAddress, Renderer*, SocketAddressComparator> _clients;
};

#define gRM RendererManager::get()

#if defined(WIN32) || defined(_WIN32)
	#define PATH_SEPARATOR "\\"
#else
	#define PATH_SEPARATOR "/"
#endif
