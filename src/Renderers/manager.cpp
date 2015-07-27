#include "manager.hpp"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <cctype>
#include <clocale>

#include <NptFile.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>


std::string& ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
    std::ptr_fun<int, int>(std::isgraph)));
  return s;
}

std::string& rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(),
    std::ptr_fun<int, int>(std::isgraph)).base(), s.end());
  return s;
}

std::string& trim(std::string& s) {
  return ltrim(rtrim(s));
}

Pire::NonrelocScanner CompileRegexp(const char* pattern)
{
	// Transform the pattern from UTF-8 into UCS4
	std::vector<Pire::wchar32> ucs4;
	Pire::Encodings::Utf8().FromLocal(pattern, pattern + strlen(pattern), std::back_inserter(ucs4));

	return Pire::Lexer(ucs4.begin(), ucs4.end())
		.AddFeature(Pire::Features::CaseInsensitive())	// enable case insensitivity
		.SetEncoding(Pire::Encodings::Utf8())		// set input text encoding
		.Parse() 					// create an FSM
		.Surround()					// PCRE_ANCHORED behavior
		.Compile<Pire::NonrelocScanner>();		// compile the FSM
}

bool Matches(const Pire::NonrelocScanner& scanner, const char* ptr, size_t len)
{
	return Pire::Runner(scanner)
		.Begin()	// '^'
		.Run(ptr, len)	// the text
		.End();		// '$'
		// implicitly cast to bool
}

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
			Pire::NonrelocScanner sc = CompileRegexp(useragent.search().c_str());
            if (Matches(sc, value, strlen(value)))
            {
				renderer = new Renderer(*candidate);
                return true;
            }
        }
        
		for (int i = 0; i < useragent.extraheader_size(); ++i)
        {
			const Renderer::UserAgent::ExtraHeader header = useragent.extraheader(i);

			Pire::NonrelocScanner sc = CompileRegexp(header.name().c_str());
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
