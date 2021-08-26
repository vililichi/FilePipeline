#pragma once
#include <string>
#include <vector>


std::vector <std::string> split(const std::string& chaine, const char separateur, bool separateurMultiple = false);

std::vector<std::string> splitCommand(const std::string& commande);

void createFolder(std::string path);

void IgnoreLine();

