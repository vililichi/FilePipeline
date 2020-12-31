#include "CliParser.h"

CliParser::CliParser(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		m_arguments.emplace(m_arguments.end(), argv[i]);
	}
}
