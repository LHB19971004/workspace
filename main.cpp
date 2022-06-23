#include "app.h"
#include "utils.h"
#include "sys.h"
#include <boost/program_options.hpp>
#define E6_JT_VERSION_MAJOR 0
#define E6_JT_VERSION_MINOR 1
#define E6_JT_BUILD_DATE __DATE__ " " __TIME__
#define E6_JT_TO_STR(str) #str
#define E6_JT_MAKE_VERSION(major, minor) E6_JT_TO_STR(major) "." E6_JT_TO_STR(minor)
#define E6_JT_VERSION E6_JT_MAKE_VERSION(E6_JT_VERSION_MAJOR, E6_JT_VERSION_MINOR)
// return root folder
std::string parseCommandLine(int argc, char *argv[])
{
	std::string rootFolder = ".";
	namespace po = boost::program_options;

	// Declare the supported options.
	po::options_description desc("Usage e6_jt [OPTIONS] [cmd [arg [arg ...]]]");
	desc.add_options()
		("help,h", "Output help and exit.");
	desc.add_options()
		("root,r", po::value<std::string>()->default_value(rootFolder), "Set program folder.");
	desc.add_options()
		("version,v", "Output version and exit.");

	po::variables_map vm;
	try
	{
		po::store(parse_command_line(argc, argv, desc, po::command_line_style::default_style), vm);
	}
	catch (const std::exception &/*e*/)
	{
		std::cout << desc << std::endl;
		exit(-1);
	}

	if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		exit(0);
	}

	if (vm.count("version"))
	{
		std::cout << "Version: " << E6_JT_VERSION << std::endl;
		std::cout << "Build date: " << E6_JT_BUILD_DATE << std::endl;
		exit(0);
	}

	try
	{
		if (vm.count("root"))
		{
			rootFolder = vm["root"].as<std::string>();
		}
	}
	catch (const std::exception &/*e*/)
	{
	}
	return Utils::absolute(rootFolder);
}

int main(int argc, char *argv[])
{
	sysInit();
	auto root_path = parseCommandLine(argc, argv);
	App app(root_path);
	return app.exec();
}