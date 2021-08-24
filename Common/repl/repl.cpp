#include "repl.h"

#include "General/utils/collectionUtil.h"
#include "General/utils/stringUtil.h"

#include <iostream>
#include <sstream>

using namespace repl;

std::string Repl::buildHelpMessage(const std::string& name, const AliasVec& alias,
                                   const std::string& helpMessage,
                                   const std::string& helpUsage)
{
    std::ostringstream oss;
    oss << name;

    if (!alias.empty())
    {
        oss << " {";
        oss << str::join(alias, "|");
        oss << "}";
    }

    oss << " " << helpUsage << " : " << helpMessage << "\n";

    return oss.str();
}

bool Repl::defaultDefaultCallback(const ParamVec& command)
{
    std::cout << "Unknown command [" << command[0] << "]" << std::endl;
    return true;
}

bool Repl::exitCallback(const ParamVec& command)
{
    return false;
}

bool Repl::helpCallback(const ParamVec& command)
{
    for (const auto& msg : helpMessages)
    {
        std::cout << msg << "\n";
    }

    std::cout << std::flush;

    return true;
}

Repl::Repl(const std::string& prompt)
    : prompt(prompt), helpCb([this](const ParamVec& command) -> bool
                             { return this->helpCallback(command); })
{
    callbacks.at(DEFAULT_DEFAULT_COMMAND_KEY) = &Repl::defaultDefaultCallback;
}

bool Repl::printPrompt()
{
    std::cout << prompt << std::flush;
    return true;
}

bool Repl::parse(const ParamVec& command)
{
    if (command.size() == 0)
    {
        return true;
    }
    else if (col::contains(callbacks, command[0]))
    {
        return callbacks.at(command[0])(command);
    }
    else if (!defaultCommandKey.empty() && col::contains(callbacks, defaultCommandKey))
    {
        return callbacks.at(defaultCommandKey)(command);
    }
    else
    {
        return callbacks.at(DEFAULT_DEFAULT_COMMAND_KEY)(command);
    }
}

void Repl::addCmd(const std::string& name, const std::vector<std::string>& alias,
                  const std::string& helpMessage, const std::string& helpUsage,
                  ReplCallback cb)
{
    helpMessages.push_back(buildHelpMessage(name, alias, helpMessage, helpUsage));

    callbacks.at(name) = cb;
    for (const auto& aname : alias)
    {
        callbacks.at(aname) = cb;
    }
}

void Repl::addExitCommand(const std::string& name, const std::string& helpMessage,
                          const AliasVec& alias)
{
    addCmd(name, alias, helpMessage, "", &exitCallback);
}

void Repl::addHelpCommand(const std::string& name, const std::string& helpMessage,
                          const AliasVec& alias)
{
    addCmd(name, alias, helpMessage, "", helpCb);
}

void Repl::addCommand(const std::string& name, ReplCallback cb,
                      const std::string& helpMessage, const AliasVec& alias,
                      const std::string& helpUsage)
{
    addCmd(name, alias, helpMessage, helpUsage, cb);
}

void Repl::addDefaultCommand(ReplCallback cb)
{
    callbacks.at(DEFAULT_DEFAULT_COMMAND_KEY) = cb;
}

void Repl::addDefaultCommand(const std::string& name)
{
    defaultCommandKey = name;
}
