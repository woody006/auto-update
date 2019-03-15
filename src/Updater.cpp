#define BOOST_FILESYSTEM_NO_DEPRECATED
#include "Updater.h"
#include <string>
#include "boost/filesystem.hpp"

Updater::Updater(const std::wstring& path, const std::string& remoteUrl)
{
	this->path = boost::filesystem::path(path);
	this->remoteUrl = remoteUrl;
}

void Updater::go()
{
	auto tempPath = boost::filesystem::temp_directory_path() /= boost::filesystem::unique_path("manifest%%%%%%.xml");
	bool manifestOk = getManifest(remoteUrl.c_str(), "foo.html");
	printf("Got manifest: %s\n", manifestOk ? "yes" : "no");
}

Updater::~Updater()
{
}
