#include <iostream>
#include "boost/program_options.hpp"
#include "ManifestGenerator.h"

namespace ProgOpts = boost::program_options;

void showUsage(const ProgOpts::options_description& options);

int main(int argc, char* argv[])
{
	ProgOpts::options_description visibleOptions("Options");
	visibleOptions.add_options()
		("help", "Show usage")
		("output-file", ProgOpts::value<std::string>(), "Set output file path")
		("fetch-URL-prefix", ProgOpts::value<std::string>(), "Set URL for retrieving patch files")
		("version-string", ProgOpts::value<std::string>(), "Set an arbitrary string naming the version");

	ProgOpts::options_description hiddenOptions("Options");
	hiddenOptions.add_options()
		("input-path", ProgOpts::value<std::string>(), "The root directory of the path");

	ProgOpts::positional_options_description inputDescr;
	inputDescr.add("input-path", 1);

	ProgOpts::options_description allOptions("Options");
	allOptions.add(visibleOptions).add(hiddenOptions);

	ProgOpts::variables_map vars;
	try {
		ProgOpts::store(ProgOpts::command_line_parser(argc, argv).options(allOptions).positional(inputDescr).run(), vars);
		ProgOpts::notify(vars);
	}
	catch (ProgOpts::too_many_positional_options_error tmpoe) {
		showUsage(visibleOptions);
		return 0;
	}
	catch (ProgOpts::unknown_option uoe) {
		std::cout << "Unknown option: " << uoe.get_option_name() << "\n";
		showUsage(visibleOptions);
		return 0;
	}
	catch (ProgOpts::invalid_command_line_syntax iclse) {
		std::cout << iclse.what() << "\n";
		showUsage(visibleOptions);
		return 0;
	}

	if (vars.count("help") > 0)
	{
		showUsage(visibleOptions);
		return 0;
	}
	if (vars.count("input-path") == 0)
	{
		std::cout << "No input-path specified.\n";
		showUsage(visibleOptions);
		return 0;
	}
	if (vars.count("output-file") == 0)
	{
		std::cout << "No output-file specified.\n";
		showUsage(visibleOptions);
		return 0;
	}

	auto inputPath = vars["input-path"].as<std::string>();
	auto outputFilePath = vars["output-file"].as<std::string>();
	auto urlPrefix = (vars.count("fetch-URL") == 1) ? vars["fetch-URL"].as<std::string>() : "";
	auto versionString = (vars.count("version-string") == 1) ? vars["version-string"].as<std::string>() : "";

	ManifestGenerator mg(inputPath, outputFilePath, urlPrefix);
	auto utcToday = boost::gregorian::day_clock::universal_day();
	mg.setDate(utcToday);
	mg.setVersionString(versionString);

	mg.Run();

	return 0;
}

void showUsage(const ProgOpts::options_description& options)
{
	std::cout << "Usage:\tmake-manifest [patch-directory] [fetch-URL-prefix] {options}\n";
	std::cout << options << "\n";
}