#include "CliParser.h"
#include "exceptions.h"

#include <assert.h>
#include <sstream>
#include <regex>

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
			pCliArg arg = m_flagLookup.at(flag);
			
			if (arg->isSet())
			{
				throw cli::duplicate_flag_exception(arg->toString());
			}

			arg->set(true);
			m_parsedArguments.insert(arg);
			if (!arg->isParamEmpty())
			{
				if (i + 1 >= argc)
				{
					throw cli::missing_parameter_exception(std::string(flag, 1));
				}

				std::string next = argv[i + 1];
				if (arg->getBool())
				{
					auto value = cli::boolFromString(next);
					if (!value)
					{
						throw cli::conversion_failed(next, "bool");
					}
					cli::CliParam param(value.value());
					arg->setParam(param);
				}
				else if (arg->getIpAddress())
				{
					std::regex regex("\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}");
					bool match = std::regex_match(next, regex);
					sf::IpAddress address(next);
					if (!match || address == sf::IpAddress::None)
					{
						throw cli::conversion_failed(next, "ipv4_address");
					}
					cli::CliParam param(address);
					arg->setParam(param);
				}
				else if (arg->getPath())
				{
					std::filesystem::path path(next);
					// NEED TO CHECK IF THE PATH IS VALID
					cli::CliParam param(path);
					arg->setParam(param);
				}
				else if (arg->getUnsignedShort())
				{
					unsigned short num;
					std::stringstream ss(next);
					ss >> num;
					if (ss.fail())
					{
						throw cli::conversion_failed(next, "unsigned short");
					}
					cli::CliParam param((unsigned short(num)));
					arg->setParam(param);
				}
				else if (arg->getString())
				{
					cli::CliParam param(next);
					// NEED TO CHECK IF THE NAME IS A VALID NAME
					arg->setParam(param);
				}
				else
				{
					// never happens
					throw cli::never_happens(arg->toString());
				}
				++i;
			}
		}
		else if (current.size() > 2
			&& current.at(0) == '-'
			&& current.at(1) == '-')
		{
			std::string option = std::regex_replace(current, std::regex("--(.*)=.*"), "$1");
			if (!m_nameLookup.count(option))
			{
				throw cli::unknown_flag_exception(option);
			}
			pCliArg arg = m_nameLookup.at(option);

			if (arg->isSet())
			{
				throw cli::duplicate_flag_exception(arg->toString());
			}

			arg->set(true);
			m_parsedArguments.insert(arg);
		}
		else
		{
			std::regex regex("=.*");
			throw cli::unknown_flag_exception(std::regex_replace(current, regex, ""));
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

	pCliArg arg = std::make_shared<cli::CliArg>(_longName, _shortFlag, _parameter);
	
	auto result = m_registeredArguments.insert(arg);
	assert(result.second);
	
	if (_shortFlag != '\0')
	{
		m_flagLookup[_shortFlag] = arg;
	}
	if (!_longName.empty())
	{
		m_nameLookup[_longName] = arg;
	}
	if (!_longName.empty() && _shortFlag != '\0' && std::isalnum(_shortFlag))
	{
		m_equivalents[_shortFlag] = _longName;
	}
}

bool CliParser::addArgument(char _shortFlag, std::string _longName, cli::CliParam _parameter)
{
	return addArgument(_longName, _shortFlag, _parameter);
}