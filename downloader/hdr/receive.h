#include <SFML/Network.hpp>
#include "packet.h"

Packet receive(sf::TcpSocket* socket_ptr)
{
	char data[INIT_PACKET_SIZE];
	size_t size;
	Packet pq;
	sf::Socket::Status stat = socket_ptr->receive(data, INIT_PACKET_SIZE, size);
	pq.add(data, size);
	pq.move(0);
	return pq;
}