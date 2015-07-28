#include "utils.hpp"

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

Pire::NonrelocScanner CompileRegexp(std::string pattern)
{
	// Transform the pattern from UTF-8 into UCS4
	std::vector<Pire::wchar32> ucs4;
	Pire::Encodings::Utf8().FromLocal(pattern.c_str(), pattern.c_str() + pattern.length(), std::back_inserter(ucs4));

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
