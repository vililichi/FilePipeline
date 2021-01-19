#include "CliParser.h"
#include "exceptions.h"

#include <iostream>
#include <cstdlib>

/*
	TODO : Change CliParam from std::variant to a polymorphic class,
	to allow user-defined types to be used when parsing.

	CliParam should have the string-to-value conversion function,
	the value-to-string conversion function, the to-string function,
	the parsing validation function, and potentially a getter for
	the exception thrown when the parsing fails, as virtual member
	functions
*/
int main(int argc, char* argv[])
{
	CliParser cliParse;
	int i = 0;

	cliParse.addArgument("version", 'v');
	cliParse.addArgument("name", 'n', std::string());
	cliParse.addArgument("path", 'p', std::filesystem::path());
	cliParse.addArgument("port", 'P', unsigned short(0));
	cliParse.addArgument("ip", 'I', sf::IpAddress());

	try
	{
		cliParse.parse(argc, argv);
	}
	catch (cli::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return e.exitCode();
	}

	std::cout << "Printing command line..." << std::endl;
	for (int i = 1; i < argc; ++i)
	{
		std::cout << argv[i] << ' ';
	}
	std::cout << std::endl;

	std::cout << "Printing arguments..." << std::endl;
	for (auto entry : cliParse.getParsedArguments())
	{
		std::cout << ++i << " -> " << *entry << std::endl;
	}
	std::cout << "Printing arguments done!" << std::endl;

	return 0;
}