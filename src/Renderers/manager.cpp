#include "manager.hpp"
#include "utils.hpp"

#if defined(WIN32) || defined(_WIN32)
    #include <io.h>
#endif

#include <fcntl.h>
#include <stdio.h>

#include <NptFile.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>


RendererManager* RendererManager::_instance = NULL;

RendererManager::RendererManager():
    _defaultRenderer(nullptr)
{
}

void RendererManager::init()
{
    NPT_List<NPT_String> entries;
	std::string root = _rootPath + "resources" PATH_SEPARATOR "renderers" PATH_SEPARATOR;
    NPT_File::ListDir(root.c_str(), entries);

	if (entries.GetItemCount() == 0)
	{
		throw new std::runtime_error("No renderer configuration files found.");
	}

    for (auto it = entries.GetFirstItem(); it; ++it)
    {
        parse(root, (*it).GetChars());
    }
}

bool RendererManager::find(NPT_SocketAddress addr, Renderer*& renderer)
{
    auto found = _clients.find(addr);
    if (found != _clients.end())
    {
        renderer = found->second;
        return true;
    }

    return false;
}

bool RendererManager::find(NPT_HttpHeaders& headers, Renderer*& renderer)
{
    auto userAgent = headers.GetHeader("USER-AGENT");
    userAgent = userAgent ? userAgent : headers.GetHeader("user-agent");
    const char* value = userAgent ?
        userAgent->GetValue().GetChars() :
        nullptr;

    for (auto it = _renderers.begin(); it != _renderers.end(); ++it)
	{
		Renderer* candidate = *it;
		if (!candidate->has_useragent())
			continue;

		const Renderer::UserAgent& useragent = candidate->useragent();
		if (userAgent && useragent.has_search())
        {
			Pire::NonrelocScanner sc = CompileRegexp(useragent.search());
            if (Matches(sc, value, strlen(value)))
            {
				renderer = new Renderer(*candidate);
                return true;
            }
        }

		for (int i = 0; i < useragent.extraheader_size(); ++i)
        {
			const Renderer::UserAgent::ExtraHeader header = useragent.extraheader(i);

			Pire::NonrelocScanner sc = CompileRegexp(header.name());
			if (Matches(sc, header.value().c_str(), header.value().length()))
            {
				renderer = new Renderer(*candidate);
                return true;
            }
        }
    }

    return false;
}

Renderer* RendererManager::defaultRenderer(NPT_SocketAddress addr)
{
	Renderer* def = new Renderer(*_defaultRenderer);
    _clients[addr] = def;
    return def;
}

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

void RendererManager::parse(std::string root, std::string file)
{
	int fid = open((root + file).c_str(), O_RDONLY);
	google::protobuf::io::FileInputStream fis(fid);
	fis.SetCloseOnDelete(true);

	Renderer* renderer = new Renderer();
	if (google::protobuf::TextFormat::Parse(&fis, renderer))
	{
		printf("Added: %s\n", file.c_str());

		if (renderer->name().compare("Default") == 0)
		{
			if (_defaultRenderer)
			{
				throw new std::runtime_error("Can not have more than one default renderer.");
			}

			_defaultRenderer = renderer;
		}
		else
		{
			_renderers.push_back(renderer);
		}
	}
}
