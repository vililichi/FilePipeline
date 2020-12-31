#include "CliParser.h"
#include <iostream>

int main(int argc, char* argv[])
{
	CliParser cliParse(argc, argv);
	int i = 0;

	std::cout << "Printing arguments..." << std::endl;
	for (auto& entry : cliParse.getVector())
	{
		std::cout << ++i << " -> " << entry << std::endl;
	}
	std::cout << "Printing arguments done!" << std::endl;

	return 0;
}