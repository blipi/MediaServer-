#include <string>
#include <sstream>
#include <iostream>
#include <cctype>
#include <clocale>

#include <pire/pire.h>

std::string& ltrim(std::string& s);
std::string& rtrim(std::string& s);
std::string& trim(std::string& s);

Pire::NonrelocScanner CompileRegexp(std::string pattern);
bool Matches(const Pire::NonrelocScanner& scanner, const char* ptr, size_t len);
