#include <vector>
#include <map>
#include <string>


class RendererManager;

class Renderer
{
    friend RendererManager;

private:
    std::map<std::string, std::vector<std::string> > Values;
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

private:
    std::string _rootPath;
    std::vector<Renderer*> _renderers;
};

#define gRM RendererManager::get()


void RendererManager::setRootPath(std::string rootPath)
{
    size_t last = rootPath.rfind("/");
    if (last != std::string::npos)
    {
        rootPath = rootPath.substr(0, last + 1);
    }
    else
    {
        rootPath += "/";
    }

    _rootPath = rootPath;
}
