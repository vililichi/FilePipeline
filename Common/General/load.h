#pragma once
#include "Communication/CryptoSocket.h"

#include <string>

void download(CryptoSocket* csocket_ptr, const std::string& filename,
              const std::string& folder, bool ui = false);

void upload(CryptoSocket* csocket_ptr, const std::string& filename,
            const std::string& folder, bool ui = false);

void uploadDemand(CryptoSocket* csocket_ptr, const std::string& filename,
                  const std::string& folder);
