#pragma once
#include <string>
#include <vector>
#include "cryptoSocket.h"

#define DOWN_PATH "download"
#define UP_PATH "upload"

struct fileInfo
{
	std::string name = "";
	uint32_t size = 0;
};

std::vector<fileInfo> list();

std::vector<fileInfo> getlist(cryptoSocket* csocket_ptr);