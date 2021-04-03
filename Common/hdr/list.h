#pragma once
#include <string>
#include <vector>
#include "cryptoSocket.h"

#define DOWN_PATH "operation"
#define UP_PATH "operation"

struct fileInfo
{
	std::string name = "";
	uint32_t size = 0;
};

std::vector<fileInfo> list(std::string path = UP_PATH);

std::vector<fileInfo> getlist(cryptoSocket* csocket_ptr);