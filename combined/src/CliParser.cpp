#include "CliParser.h"

CliParser::CliParser()
{

}

bool CliParser::hasArgument(char _shortFlag)
{
	return false;
}

bool CliParser::parse(int argc, char* argv[])
{
	if (argc == 1)
	{
		return true;
	}
	int i = 1;
	if (m_registeredSubcommands.count(std::string(argv[1])))
	{
		m_parsedSubcommand = argv[1];
		++i;
	}
	else
	{
		m_parsedSubcommand = std::string();
	}

	for (/* i */; i < argc; ++i)
	{
		std::string current = argv[i];
		if (current.size() == 2
			&& current.at(0) == '-'
			&& m_flagLookup.count(current.at(1)))
		{
			char flag = current.at(1);
			cli::CliArg& arg = *m_flagLookup[flag];
			
			if (arg.isSet())
			{
				throw "AlreadySet";
			}

			arg.set(true);
			if (!arg.isParamEmpty())
			{
				++i;
				arg.set(true);
				m_parsedArguments.insert(&arg);
			}
			else
			{
				//cli::CliArg arg(flag);
				//m_parsedArguments.emplace(m_parsedArguments.end(), flag);
			}
			//cli::CliArg arg(current.at(1));
		}
	}
	return true;
}

bool CliParser::addArgument(std::string _longName, char _shortFlag, cli::CliParam _parameter)
{
	if (_shortFlag != '\0' && !std::isalnum(_shortFlag))
	{
		return false;
	}

	cli::CliArg arg(_longName, _shortFlag, _parameter);

	if (_shortFlag != '\0')
	{
		m_flagLookup[_shortFlag] = &arg;
	}
	if (!_longName.empty())
	{
		m_nameLookup[_longName] = &arg;
	}
	
	m_registeredArguments.insert(m_registeredArguments.end(), std::move(arg));
	
	if (!_longName.empty() && _shortFlag != '\0' && std::isalnum(_shortFlag))
	{
		m_equivalents[_shortFlag] = _longName;
	}

}

bool CliParser::addArgument(char _shortFlag, std::string _longName, cli::CliParam _parameter)
{
	return addArgument(_longName, _shortFlag, _parameter);
}