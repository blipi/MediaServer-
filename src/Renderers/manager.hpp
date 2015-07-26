#include <vector>
#include <map>
#include <string>


class RendererManager;

class Renderer
{
    friend RendererManager;

private:
	std::string Name;
    std::map<std::string, std::vector<std::string> > Properties;
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

    inline void setRootPath(std::string rootPath);
    void init();

private:
	void parse(std::string root, std::string file);
	void registerProperty(Renderer* renderer, std::string key, std::string value);

private:
    std::string _rootPath;
    std::vector<Renderer*> _renderers;
};

#define gRM RendererManager::get()

#if defined(WIN32) || defined(_WIN32) 
	#define PATH_SEPARATOR "\\" 
#else 
	#define PATH_SEPARATOR "/" 
#endif 


void RendererManager::setRootPath(std::string rootPath)
{
	size_t last = rootPath.rfind(PATH_SEPARATOR);
    if (last != std::string::npos)
    {
        rootPath = rootPath.substr(0, last + 1);
    }
    else
    {
		rootPath += PATH_SEPARATOR;
    }


	printf("Root set to: %s\n", rootPath.c_str());
	_rootPath = rootPath;
}
