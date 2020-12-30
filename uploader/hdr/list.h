#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "general.h"

#define UP_PATH "upload"

struct fileInfo
{
	std::string name;
	uint32_t size;
};

std::vector<fileInfo> list()
{
	std::vector<fileInfo> retour;
	std::filesystem::directory_iterator dirItt(UP_PATH);
	for (auto& entry : dirItt)
	{
		fileInfo fi;
		fi.size = entry.file_size();
		fi.name = split(entry.path().string(), '\\', true).back();
		retour.push_back(fi);
	}
	return retour;
}