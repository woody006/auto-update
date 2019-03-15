#include "boost/filesystem.hpp"
#include "curl/curl.h"

#include "Updater.h"

/* References
 * Example on using curl with write callback: https://curl.haxx.se/libcurl/c/getinmemory.html
 * Standard on user agent strings: https://tools.ietf.org/html/rfc7231#page-46
 * CURLOPT_WRITEDATA documentation: https://curl.haxx.se/libcurl/c/CURLOPT_WRITEDATA.html
 *		"If you're using libcurl as a win32 DLL, you MUST use a CURLOPT_WRITEFUNCTION if you set this option or you will experience crashes."
 */

void Updater::errorlog_write(const char* message, ...)
{
	printf(message);
}

bool Updater::getManifest(const char* URL, const char* localPath)
{
	FILE* file = nullptr;
	errno_t fopenResult = fopen_s(&file, localPath, "w");
	if (fopenResult != ERROR_SUCCESS || file == nullptr)
	{
		errorlog_write("Failed to create and open a temporary file.");
		return false;
	}
	CURL* curlCtx = curl_easy_init();
	CURLcode res;
	if (curlCtx == nullptr)
	{
		errorlog_write("Error creating curl context.");
		return false;
	}
	bool success = false;
	char* curlErrorString = new char[CURL_ERROR_SIZE];
	res = curl_easy_setopt(curlCtx, CURLOPT_ERRORBUFFER, curlErrorString);
	if (res != CURLE_OK)
	{
		errorlog_write("Unexpected curl error: %s\n", curl_easy_strerror(res));
		goto cleanup;
	}
	res = curl_easy_setopt(curlCtx, CURLOPT_URL, URL);
	if (res != CURLE_OK)
	{
		errorlog_write("Unexpected curl error. %s\n", curlErrorString);
		goto cleanup;
	}
	res = curl_easy_setopt(curlCtx, CURLOPT_WRITEDATA, file);
	if (res != CURLE_OK)
	{
		errorlog_write("Unexpected curl error. %s\n", curlErrorString);
		goto cleanup;
	}
	res = curl_easy_setopt(curlCtx, CURLOPT_WRITEFUNCTION, writeCallback);
	if (res != CURLE_OK)
	{
		errorlog_write("Unexpected curl error. %s\n", curlErrorString);
		goto cleanup;
	}
	res = curl_easy_setopt(curlCtx, CURLOPT_USERAGENT, "libcurl-agent/1.0 updater/0.1");
	if (res != CURLE_OK)
	{
		errorlog_write("Unexpected curl error. %s\n", curlErrorString);
		goto cleanup;
	}

	res = curl_easy_perform(curlCtx);
	if (res != CURLE_OK)
	{
		errorlog_write("Curl error while downloading manifest: %s\n", curlErrorString);
		goto cleanup;
	}

	success = true;

	cleanup:
	curl_easy_cleanup(curlCtx);
	delete curlErrorString;
	fclose(file);
	if (!success)
	{
		remove(localPath);
	}
	
	return success;
}

size_t Updater::writeCallback(char* ptr, size_t size, size_t nmemb, void* state)
{
	//size_t recvBytes = size * nmemb;
	return fwrite(ptr, size, nmemb, (FILE*)state);
}