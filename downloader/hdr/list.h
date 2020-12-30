#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "receive.h"
#include "general.h"

#define DOWN_PATH "download"

struct fileInfo
{
	std::string name;
	uint32_t size;
};

std::vector<fileInfo> list(sf::TcpSocket* socket_ptr)
{
	//envoie de la demande
	char ptr[1];
	ptr[0] = command::DownToUp::list;
	socket_ptr->send(ptr, 1);

	//reception de la reponse
	Packet pq = receive(socket_ptr);
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