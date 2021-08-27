#pragma once

#include "Communication/cryptoSocket.h"
#include "repl/replTypes.h"

#include <mutex>

namespace cmd
{
    using repl::ParamVec;

    bool listCmd(const ParamVec& command);
    bool userCmd(std::vector<CryptoSocket**>& sockets, std::mutex& mutex,
                 const ParamVec& command);
    bool keyCmd(const ParamVec& command);
    bool trustCmd(const ParamVec& command);
    bool untrustCmd(const ParamVec& command);
    bool kickCmd(std::vector<CryptoSocket**>& sockets, std::mutex& mutex,
                 const ParamVec& command);
}
