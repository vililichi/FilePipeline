#pragma once
#include <SFML/Network.hpp>
#include "packet.h"

void send(Packet& pq, sf::TcpSocket* socket_ptr, sf::Socket::Status* stat = NULL)
{
	char* data = new char[pq.size() + 2];
	short size = pq.size();
	char* csize = (char*)&size;
	data[0] = csize[0];
	data[1] = csize[1];
	pq.move(0);
	pq.read(data + 2, pq.size());
	if(stat)*stat = socket_ptr->send(data, pq.size() + 2);
	else socket_ptr->send(data, pq.size() + 2);
	delete[] data;
}

Packet receive(sf::TcpSocket* socket_ptr, sf::Socket::Status* stat = NULL)
{
	size_t size = 0;
	Packet pq;
	//reception de la taille
	char cpSize[2];
	if(stat)
		while (size != 2) {
			size_t addSize = 0;
			*stat = socket_ptr->receive(cpSize+ size, 2 - size, addSize);
			size += addSize;
			if (*stat == sf::Socket::Status::Disconnected || *stat == sf::Socket::Status::Error) return pq;
		}
	else
		while (size != 2) {
			size_t addSize = 0;
			sf::Socket::Status stat = socket_ptr->receive(cpSize + size, 2 - size, addSize);
			size += addSize;
			if (stat == sf::Socket::Status::Disconnected || stat == sf::Socket::Status::Error) return pq;
		}
	//reception du packet
	
	short pSize = *(short*)cpSize;
	size = 0;
	char* data = new char[pSize];
	if (stat)
		while (size != pSize) {
			size_t addSize = 0;
			*stat = socket_ptr->receive(data + size, pSize - size, addSize);
			size += addSize;
			if (*stat == sf::Socket::Status::Disconnected || *stat == sf::Socket::Status::Error) return pq;
			}
	else
		while (size != pSize) {
			size_t addSize = 0;
			sf::Socket::Status stat = socket_ptr->receive(data + size, pSize - size, addSize);
			size += addSize;
			if (stat == sf::Socket::Status::Disconnected || stat == sf::Socket::Status::Error) return pq;
		}
	pq.add(data, size);
	pq.move(0);
	delete[] data;
	return pq;
}