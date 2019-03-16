#pragma once
#include <vector>
#include "boost/filesystem.hpp"
#include "boost/crc.hpp"

class Manifest
{
public:
	static std::unique_ptr<Manifest> TryParse(const char* path);
	//Manifest();
	~Manifest();

	struct FileChangeRecord
	{
	public:
		enum Action {
			ADD = 1,
			DELETE = 2
		};
		FileChangeRecord(boost::filesystem::path path, Action action, int64_t fileSize, int hash);

		// Gets the path of the file described in the manifest.
		boost::filesystem::path getPath() const;

		// Gets the action that the manifest says sholuld be taken on the file.
		Action getAction() const;

		// Gets the size (in bytes) that the manifest says the file should have.
		int64_t getFileSize() const;

		// Gets the hash that the manifest says the file should have.
		uint32_t getHash() const;

	private:
		boost::filesystem::path path;
		Action action;
		int64_t fileSize;
		int hash;
	};

	const std::vector<FileChangeRecord>& getFileChanges() const;

private:
	std::string fetchPrefix;
	std::unique_ptr<std::vector<FileChangeRecord>> records;
};

