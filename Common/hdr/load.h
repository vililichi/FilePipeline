#pragma once
#include <string>
#include "cryptoSocket.h"

void download(cryptoSocket* csocket_ptr, std::string filename, std::string folder);

void upload(cryptoSocket* csocket_ptr, std::string filename, std::string folder);