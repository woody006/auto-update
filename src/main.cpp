#include "Updater.h"
#include <stdio.h>

// A stub for testing.
int main(void) {
	auto here = boost::filesystem::current_path();
	Updater upd(here.wstring(), "http://www.google.com/");
	upd.go();

	
	getchar();
	return 0;
}