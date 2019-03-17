#include <iostream>
#include <fstream>
#include "ManifestGenerator.h"
//#include "tinyxml2.h"
#include "boost/crc.hpp"

ManifestGenerator::ManifestGenerator(const fs::path& root,
	const fs::path& outputFile,
	const std::string& fetchPrefixURL)
{
	this->root = root;
	output = outputFile;
	fetchPrefix = fetchPrefixURL;
}

void ManifestGenerator::Run()
{
	// I don't feel like using an actual XML library here,
	// so I'm going to just write strings to the file directly.

	// todo: add exception handling.

	std::ofstream write(output.string());
	write << "<manifest>\n";

	// Write prelude elements.
	write << "\t<version>" << version << "</version>\n";
	write << "\t<date>" << date << "</date>\n";
	write << "\t<fetch-prefix>" << fetchPrefix << "</fetch-prefix>";

	// Write file elements.
	// Recursively enumerate all files and folders in 'root'.
	// Add each file as an element to the XML document that will be output
	// (No need to structure the XML document to mirror the file system--
	// we just put the path of each file with in each file node).
	// Finally, write the XML document to the given output path.

	// This process can be made multithreaded in the future (because computing CRC is nontrivial).
	// For this first working implementation we will do everything with one thread.

	fs::recursive_directory_iterator rdi(root), end;
	while (rdi != end)
	{
		auto current = rdi->path();
		if (!fs::is_directory(current))
		{
			write << "\t<file>\n";
			write << "\t\t<action>add</action>\n";
			write << "\t\t<path>" << current << "</path>\n";
			write << "\t\t<size>" << fs::file_size(current) << "</size>\n";
			write << "\t\t<hash>" << checksumFile(current) << "</hash>\n";
			write << "\t</file>\n";
		}
		rdi++;
	}
	write << "</manifest>\n";
	write.close();
}

uint32_t ManifestGenerator::checksumFile(const fs::path& path)
{
	boost::crc_32_type crc;
	auto fileSize = fs::file_size(path); // To be used for verification after reading is done.
	const int BUFFER_SIZE = 4096;
	auto buf = std::make_unique<char[]>(BUFFER_SIZE);
	size_t totalRead = 0;
	std::ifstream read(path.c_str(), std::ios::in | std::ios::binary);
	do
	{
		read.read(buf.get(), BUFFER_SIZE);
		auto justRead = (size_t)read.gcount(); // Count of characters read by last unformatted input operation.
		if (justRead < 1)
			break;
		crc.process_bytes(buf.get(), justRead);
		totalRead += justRead;
	} while (true);
	if (totalRead != fileSize)
	{
		std::stringstream ss;
		ss << "Expected to read " << fileSize << " bytes but read " << totalRead << " instead from: "
			<< path << "\n";
		throw std::runtime_error(ss.str());
	}
	return crc.checksum();
}


void ManifestGenerator::setVersionString(const std::string& version)
{
	this->version = version;
}

void ManifestGenerator::setDate(const boost::gregorian::date date)
{
	this->date = date;
}

ManifestGenerator::~ManifestGenerator()
{
}
