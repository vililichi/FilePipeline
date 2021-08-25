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
    oss << "  :: " << name;

    if (!alias.empty())
    {
        oss << " {";
        oss << str::join(alias, "|");
        oss << "}";
    }

    oss << (helpUsage.empty() ? "" : " ") << helpUsage << " : " << helpMessage << "\n";

    return oss.str();
}

bool Repl::defaultUnknownCallback(const ParamVec& command)
{
    std::cout << (unknownCommandString.empty() ? "Unknown command :"
                                               : unknownCommandString)
              << " [" << command[0] << "]" << std::endl;
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
        std::cout << msg;
    }

    std::cout << std::flush;

    return true;
}

Repl::Repl(const std::string& prompt)
    : prompt(prompt), helpCb([this](const ParamVec& command) -> bool
                             { return this->helpCallback(command); }),
      unknownCb([this](const ParamVec& command) -> bool
                { return this->defaultUnknownCallback(command); })
{
    callbacks[DEFAULT_DEFAULT_COMMAND_KEY] = unknownCb;
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
        return callbacks[command[0]](command);
    }
    else if (!defaultCommandKey.empty() && col::contains(callbacks, defaultCommandKey))
    {
        return callbacks[defaultCommandKey](command);
    }
    else
    {
        return callbacks[DEFAULT_DEFAULT_COMMAND_KEY](command);
    }
}

void Repl::addCmd(const std::string& name, const std::vector<std::string>& alias,
                  const std::string& helpMessage, const std::string& helpUsage,
                  ReplCallback cb)
{
    helpMessages.push_back(buildHelpMessage(name, alias, helpMessage, helpUsage));

    callbacks[name] = cb;
    for (const auto& aname : alias)
    {
        callbacks[aname] = cb;
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
    callbacks[DEFAULT_DEFAULT_COMMAND_KEY] = cb;
}

void Repl::addDefaultCommand(const std::string& name)
{
    defaultCommandKey = name;
}

void Repl::setUnknownCommandString(const std::string& msg)
{
    unknownCommandString = msg;
}
