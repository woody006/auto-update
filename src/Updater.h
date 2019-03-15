#pragma once
#include <string>
#include "boost/filesystem.hpp"

class Updater
{
private:
	static void errorlog_write(const char* message, ...);
	static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* state);
	static bool getManifest(const char* URL, const char* localPath);

	boost::filesystem::path path;
	std::string remoteUrl;
public:
	void go();
	Updater(const std::wstring& path, const std::string& remoteUrl);
	~Updater();
};
