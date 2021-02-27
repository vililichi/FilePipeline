#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "cryptoSocket.h"
#include "general.h"

#define DOWN_PATH "download"

struct fileInfo
{
	std::string name = "";
	uint32_t size = 0;
};

std::vector<fileInfo> list(cryptoSocket* csocket_ptr)
{
	//envoie de la demande
	Packet demande;
	demande << command::DownToUp::list;
	csocket_ptr->send(demande);

	//reception de la reponse
	Packet pq = csocket_ptr->receive();
	size_t size;
	pq >> size;
	std::vector<fileInfo> retour;
	for (size_t i = 0; i < size; i++)
	{
		fileInfo fi;
		pq >> fi.name;
		pq >> fi.size;
		retour.push_back(fi);
	}
	return retour;
}