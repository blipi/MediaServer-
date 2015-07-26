#include "manager.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <NptFile.h>


RendererManager* RendererManager::_instance = NULL;

RendererManager::RendererManager()
{
}

void RendererManager::init()
{
    NPT_List<NPT_String> entries;
	std::string root = _rootPath + "resources" PATH_SEPARATOR "renderers" PATH_SEPARATOR;
    NPT_File::ListDir(root.c_str(), entries);

    for (auto it = entries.GetFirstItem(); it; ++it)
    {
        parse(root, (*it).GetChars());
    }
}

void RendererManager::parse(std::string root, std::string file)
{
	printf("PARSING %s\n", file.c_str());

	std::string line;
	std::ifstream fp(root + file);
	if (fp.is_open())
	{
		Renderer* renderer = new Renderer();

		while (std::getline(fp, line))
		{
			if (line.compare(0, 1, "#") == 0)
			{
				continue;
			}

			std::istringstream is_line(line);
			std::string key;
			if (std::getline(is_line, key, '='))
			{
				std::string value;
				if (std::getline(is_line, value))
				{
					registerProperty(renderer, key, value);
				}
			}
		}

		_renderers.push_back(renderer);

		fp.close();
	}
}

void RendererManager::registerProperty(Renderer* renderer, std::string key, std::string value)
{
	if (key.compare("RendererName") == 0)
	{
		renderer->Name = value;
	}
	else
	{
		auto found = renderer->Properties.find(key);
		if (found == renderer->Properties.end())
		{
			renderer->Properties.insert(std::make_pair(key, std::vector<std::string>()));
		}

		renderer->Properties[key].push_back(value);
	}
}
