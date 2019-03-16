#include <stdio.h>

#include "Updater.h"
#include "Manifest.h"

#include "boost/dll/runtime_symbol_info.hpp"

// A stub for testing.
int main(void) {

	//auto here = boost::filesystem::current_path();
	auto here = boost::dll::program_location().parent_path();

	//Updater upd(here.wstring(), "http://www.google.com/");
	//upd.go();

	
	auto xmlPath = here /= "test.xml";
	auto m = Manifest::TryParse(xmlPath.string().c_str());
	if (m == nullptr)
	{
		printf("TryParse manifest failed!\n");
		goto end;
	}



end:
	printf("Program exiting.\n");
	getchar();
	return 0;
}
