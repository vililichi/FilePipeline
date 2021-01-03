#include "CliParser.h"
#include <iostream>

int main(int argc, char* argv[])
{
	CliParser cliParse;
	int i = 0;

	cliParse.addArgument("version", 'v');
	cliParse.addArgument("name", 'n', std::string());
	cliParse.addArgument("path", 'p', std::filesystem::path());
	cliParse.addArgument("port", 'P', unsigned short(0));
	cliParse.addArgument("ip", 'I', sf::IpAddress());

	cliParse.parse(argc, argv);

	std::cout << "Printing command line..." << std::endl;
	for (int i = 1; i < argc; ++i)
	{
		std::cout << argv[i] << '\t';
	}
	std::cout << "Printing arguments..." << std::endl;
	for (auto& entry : cliParse.getParsedArguments())
	{
		std::cout << ++i << " -> " << entry << std::endl;
	}
	std::cout << "Printing arguments done!" << std::endl;

	return 0;
}