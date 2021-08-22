#pragma once
#include <string>
#include "Communication/CryptoSocket.h"

void download(CryptoSocket* csocket_ptr, std::string filename, std::string folder, bool ui = false);

void upload(CryptoSocket* csocket_ptr, std::string filename, std::string folder, bool ui = false);

void uploadDemand(CryptoSocket* csocket_ptr, std::string filename, std::string folder);