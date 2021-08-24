#pragma once

#include <functional>
#include <string>
#include <vector>

namespace repl
{
    using ReplCallback = std::function<bool(const std::vector<std::string>&)>;
    using ParamVec = std::vector<std::string>;
    using AliasVec = std::vector<std::string>;
}
