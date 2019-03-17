#include <string>
#include <chrono>

#include "boost/filesystem.hpp"
#include "boost/date_time.hpp"

namespace fs = boost::filesystem;

class ManifestGenerator
{
private:
	fs::path root;
	fs::path output;
	std::string version;
	boost::gregorian::date date;
	std::string fetchPrefix;

	static uint32_t checksumFile(const fs::path& path);

public:
	ManifestGenerator(const fs::path& root,
					  const fs::path& outputFile,
					  const std::string& fetchPrefixURL);

	void setVersionString(const std::string& version);
	void setDate(const boost::gregorian::date date);

	// Runs synchronously. Throws exceptions on error. Returns on successful completion.
	void Run();
	
	~ManifestGenerator();
};