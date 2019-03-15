#pragma once
#include <string>
#include "boost/filesystem.hpp"

class Updater
{
private:
	
	boost::filesystem::path path;
	std::string remoteUrl;
public:
	void go();
	Updater(const std::wstring& path, const std::string& remoteUrl);
	~Updater();
};

void errorlog_write(const char* message, ...);

size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* state);

bool getManifest(const char* URL, const char* localPath);