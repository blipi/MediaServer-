#include "manager.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <NptFile.h>

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

        if (userAgent && candidate->_hasUserAgent)
        {
            if (Matches(candidate->_userAgentRE, value, strlen(value)))
            {
                renderer = new Renderer(candidate);
                return true;
            }
        }
        else if (candidate->_hasUserAgentEX)
        {
            const char* headerName = candidate->Properties["UserAgentAdditionalHeader"][0].c_str();
            auto headerObj = headers.GetHeader(headerName);
            const char* header = headerObj ?
                headerObj->GetValue().GetChars() :
                nullptr;

            if (header)
            {
                if (Matches(candidate->_userAgentExRE, header, strlen(header)))
                {
                    renderer = new Renderer(candidate);
                    return true;
                }
            }
        }
    }

    return false;
}

Renderer* RendererManager::defaultRenderer(NPT_SocketAddress addr)
{
    Renderer* def = new Renderer(_defaultRenderer);
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
					registerProperty(renderer, trim(key), trim(value));
				}
			}
		}

		fp.close();

        if (renderer->Name.compare("Default") == 0)
        {
            _defaultRenderer = renderer;
        }
        else
        {
            _renderers.push_back(renderer);
        }
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
        if (key.compare("UserAgentSearch") == 0)
        {
            renderer->_hasUserAgent = true;
            renderer->_userAgentRE = CompileRegexp((std::string("(") + value + ")").c_str());
        }
        else if (key.compare("UserAgentAdditionalHeaderSearch") == 0)
        {
            renderer->_hasUserAgentEX = true;
            renderer->_userAgentExRE = CompileRegexp((std::string("(") + value + ")").c_str());
        }

		auto found = renderer->Properties.find(key);
		if (found == renderer->Properties.end())
		{
			renderer->Properties.insert(std::make_pair(key, std::vector<std::string>()));
		}

		renderer->Properties[key].push_back(value);
	}
}
