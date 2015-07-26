#include "manager.hpp"

#include <NptFile.h>

RendererManager* RendererManager::_instance = NULL;

RendererManager::RendererManager()
{
}

void RendererManager::init()
{
    NPT_List<NPT_String> entries;
    std::string root = _rootPath + "resources/renderers";
    NPT_File::ListDir(root.c_str(), entries);

    for (auto it = entries.GetFirstItem(); it; ++it)
    {
        parse(root, (*it).GetChars());
    }
}

void parse(std::string root, std::string file)
{
    
}
