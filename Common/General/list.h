#pragma once
#include "Communication/cryptoSocket.h"

#include <string>
#include <vector>

constexpr char DOWN_PATH[] = "operation";
constexpr char UP_PATH[] = "operation";

struct fileInfo
{
    std::string name = "";
    uint32_t size = 0;
};

std::vector<fileInfo> list(std::string path = UP_PATH);

std::vector<fileInfo> getlist(CryptoSocket* csocket_ptr);
