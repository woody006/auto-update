#include "Manifest.h"
#include "tinyxml2.h"
#include "Updater.h" // for errorlog_write
#include <iostream>
#include <string>
#include <vector>
#include "boost/algorithm/string.hpp"

using namespace tinyxml2;

/*Manifest::Manifest()
{
}*/

void traverseAndPrint(XMLNode* root, int indentLevel);

std::unique_ptr<Manifest> Manifest::TryParse(const char* path)
{
	XMLDocument doc;
	XMLError xe = doc.LoadFile(path);
	if (xe != XML_SUCCESS)
	{
		errorlog_write("Error reading manifest: %s\n", doc.ErrorStr());
		return nullptr;
	}

	// todo: Traverse the XMLDocument and populate a new Manifest instance.
	XMLNode* manifestRoot = doc.FirstChild();
	if (manifestRoot == nullptr)
		return nullptr;

	auto fpNode = manifestRoot->FirstChildElement("fetch-prefix");
	if (fpNode == nullptr)
	{
		errorlog_write("Missing fetch-prefix!");
		return nullptr;
	}
	
	auto files = std::make_unique<std::vector<Manifest::FileChangeRecord>>(); // to be moved into the Manifest we create later.
	// Enumerate all <file> elements and add the to the manifest.
	auto fileNode = manifestRoot->FirstChildElement("file");
	while (fileNode != nullptr)
	{
		// Parse <action>.
		Manifest::FileChangeRecord::Action action;
		auto actionNode = fileNode->FirstChildElement("action");
		if (actionNode == nullptr)
		{
			errorlog_write("Missing <action> element inside <file>. Assuming \"add\".\n");
			action = Manifest::FileChangeRecord::Action::ADD;
		}
		else {
			auto actionText = std::string(actionNode->GetText());
			boost::algorithm::trim(actionText);
			boost::algorithm::to_lower(actionText);
			if (actionText.compare("add") == 0)
			{
				action = Manifest::FileChangeRecord::Action::ADD;
			}
			else if (actionText.compare("delete") == 0) {
				action = Manifest::FileChangeRecord::Action::DELETE;
			}
			else {
				errorlog_write("Unexpected <action> value inside <file>. Assuming \"add\".\n");
				action = Manifest::FileChangeRecord::Action::ADD;
			}
		}

		// Parse <path>.
		auto pathNode = fileNode->FirstChildElement("path");
		if (pathNode == nullptr)
		{
			errorlog_write("Missing <path> element inside <file>!\n");
			return nullptr;
		}
		auto filePath = boost::filesystem::path(pathNode->GetText());

		// Parse <size>.
		auto sizeNode = fileNode->FirstChildElement("size");
		int64_t fileSize;
		if (sizeNode == nullptr)
		{
			errorlog_write("Missing <size> element inside <file>.\n");
			fileSize = 0;
		}
		else {
			bool longParseSuccess = XMLUtil::ToInt64(sizeNode->GetText(), &fileSize);
			if (!longParseSuccess)
			{
				errorlog_write("Error parsing file size.\n");
				fileSize = 0;
			}
		}

		// Parse <hash>.
		auto hashNode = fileNode->FirstChildElement("hash");
		if (hashNode == nullptr)
		{
			errorlog_write("Missing <hash> element inside <file>!\n");
			return nullptr;
		}
		auto hashString = std::string(hashNode->GetText());
		boost::algorithm::trim(hashString);
		uint32_t hash;
		try
		{
			hash = std::stoul(hashString, nullptr, 16); // parse hash as a hex string.
		}
		catch (std::invalid_argument iae) {
			errorlog_write("Error parsing hash: %s\n", iae.what());
			return nullptr;
		}
		catch (std::out_of_range oore) {
			errorlog_write("Error parsing hash: The value was out of range: %s\n", oore.what());
			return nullptr;
		}
		
		// Now build the file record and add it to the vector.
		Manifest::FileChangeRecord fcr(filePath, action, fileSize, hash);
		files->emplace_back(fcr);
		
		fileNode = fileNode->NextSiblingElement();
	}


	// Create manifest object to be returned.
	auto result = std::make_unique<Manifest>();
	// Add fetch-prefix URL to the manifest object.
	result->fetchPrefix = fpNode->GetText();
	// Move vector created locally into instance.
	result->records = std::move(files);

	//traverseAndPrint(manifestRoot, 0);

	return result;
}

// for debug. print an XML node.
void traverseAndPrint(XMLNode* root, int indentLevel)
{
	if (root == nullptr)
		return;
	if (root->ToComment() != nullptr)
		return; // Don't proess comments.

	auto indent = std::string(indentLevel, ' ');
	// Print root node.
	std::cout << indent << root->Value() << std::endl;
	indentLevel += 2;
	//indent += ' ';
	XMLNode* child = root->FirstChild();
	while (child != nullptr)
	{
		traverseAndPrint(child, indentLevel);
		child = child->NextSibling();
	}
}


const std::vector<Manifest::FileChangeRecord>& Manifest::getFileChanges() const
{
	return *records;
}


Manifest::~Manifest()
{

}


Manifest::FileChangeRecord::FileChangeRecord(boost::filesystem::path path, Action action, int64_t fileSize, int hash)
{
	this->path = path;
	this->action = action;
	this->fileSize = fileSize;
	this->hash = hash;
}

boost::filesystem::path Manifest::FileChangeRecord::getPath() const
{
	return path;
}

Manifest::FileChangeRecord::Action Manifest::FileChangeRecord::getAction() const
{
	return action;
}

int64_t Manifest::FileChangeRecord::getFileSize() const
{
	return fileSize;
}

uint32_t Manifest::FileChangeRecord::getHash() const
{
	return hash;
}