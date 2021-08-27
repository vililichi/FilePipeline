#pragma once
#include <string>
#include <vector>


std::vector<std::string> split(const std::string& chaine, const char separateur,
                               bool separateurMultiple = false);

std::vector<std::string> splitCommand(const std::string& commande);

void createFolder(const std::string& path);

void ignoreLine();

void hideCursor( const bool hide_);

void ManageEndian( uint8_t* const Bytes, const size_t nbrBytes);
