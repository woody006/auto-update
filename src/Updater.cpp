#define BOOST_FILESYSTEM_NO_DEPRECATED
#include "Updater.h"
#include <string>
#include "boost/filesystem.hpp"
#include "Manifest.h"

Updater::Updater(const std::wstring& path, const std::string& remoteUrl)
{
	this->path = boost::filesystem::path(path);
	this->remoteUrl = remoteUrl;
}

bool Updater::go()
{
	auto xmlPath = boost::filesystem::temp_directory_path() /= boost::filesystem::unique_path("manifest%%%%%%.xml");
	bool manifestOk = getManifest(remoteUrl.c_str(), "foo.html");
	printf("Got manifest: %s\n", manifestOk ? "yes" : "no");
	if (!manifestOk)
		return false;

	auto manifest = Manifest::TryParse(xmlPath.string().c_str());
	if (manifest == nullptr)
		return false;

	// todo: use manifest
	for (const auto &fileRecord : manifest->getFileChanges())
	{

	}

	// use boost::crc_32_type to compute hash. see https://www.boost.org/doc/libs/1_37_0/libs/crc/crc_example.cpp
}

Updater::~Updater()
{
}
