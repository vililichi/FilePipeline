#pragma once
#include <string>
#include "cryptoSocket.h"

void download(cryptoSocket* csocket_ptr, std::string filename, std::string folder, bool ui = false);

void upload(cryptoSocket* csocket_ptr, std::string filename, std::string folder, bool ui = false);

void uploadDemand(cryptoSocket* csocket_ptr, std::string filename, std::string folder);