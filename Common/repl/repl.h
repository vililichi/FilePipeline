#pragma once

#include "replTypes.h"

#include <map>

namespace repl
{
    class Repl
    {
    public:
        Repl(const std::string& prompt = "> ");

        bool printPrompt();
        bool parse(const ParamVec& command);

        void addExitCommand(const std::string& name = "exit",
                            const std::string& helpMessage = "Termine le programme",
                            const AliasVec& alias = {"e", "q", "quit"});

        void addHelpCommand(
            const std::string& name = "help",
            const std::string& helpMessage = "Liste les commandes et leur utilisation",
            const AliasVec& alias = {"h"});

        void addCommand(const std::string& name, ReplCallback cb,
                        const std::string& helpMessage = "", const AliasVec& alias = {},
                        const std::string& helpUsage = "");

        void addDefaultCommand(ReplCallback cb);
        void addDefaultCommand(const std::string& name = "help");

    private:
        std::map<std::string, ReplCallback> callbacks;
        std::vector<std::string> helpMessages;
        std::string prompt;
        std::string defaultCommandKey;

        ReplCallback helpCb;

        static constexpr char DEFAULT_DEFAULT_COMMAND_KEY[] = "__default__";

        void addCmd(const std::string& name, const std::vector<std::string>& alias,
                    const std::string& helpMessage, const std::string& helpUsage,
                    ReplCallback cb);

        static std::string buildHelpMessage(const std::string& name,
                                            const AliasVec& alias,
                                            const std::string& helpMessage,
                                            const std::string& helpUsage);

        bool helpCallback(const ParamVec& command);

        static bool defaultDefaultCallback(const ParamVec& command);
        static bool exitCallback(const ParamVec& command);
    };
}
