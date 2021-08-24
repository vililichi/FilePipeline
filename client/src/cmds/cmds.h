#pragma once

#include "Communication/cryptoSocket.h"
#include "repl/replTypes.h"

namespace cmd
{
    using repl::ParamVec;

    bool listCmd(CryptoSocket* cSocket, const ParamVec& command);
    bool downloadCmd(CryptoSocket* cSocket, const ParamVec& command);
    bool uploadCmd(CryptoSocket* cSocket, const ParamVec& command);
    bool renameCmd(CryptoSocket* cSocket, const ParamVec& command);
    bool removeCmd(CryptoSocket* cSocket, const ParamVec& command);
}
