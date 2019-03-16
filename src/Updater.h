#pragma once
#include <string>
#include "boost/filesystem.hpp"

#define errorlog_write(...) fprintf(stderr, __VA_ARGS__)

class Updater
{
private:
	static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* state);
	static bool getManifest(const char* URL, const char* localPath);

	boost::filesystem::path path;
	std::string remoteUrl;
public:
	bool go();
	Updater(const std::wstring& path, const std::string& remoteUrl);
	~Updater();
};
