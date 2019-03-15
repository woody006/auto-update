#include <string>
#ifdef _WIN32
#include "Windows.h"
#endif

static bool directoryExists(std::wstring path)
{
	
#ifdef _WIN32
	DWORD attrs = GetFileAttributesW(path.c_str());
	if (GetLastError() != ERROR_SUCCESS)
	{
		// failure.
	}

	return (attrs & FILE_ATTRIBUTE_DIRECTORY) != 0;

#endif
}