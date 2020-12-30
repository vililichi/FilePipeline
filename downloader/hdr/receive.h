#include <SFML/Network.hpp>
#include "packet.h"

Packet receive(sf::TcpSocket* socket_ptr)
{
	char data[PacketConst::MAXSIZE];
	size_t size;
	Packet pq;
	sf::Socket::Status stat = socket_ptr->receive(data, PacketConst::MAXSIZE, size);
	pq.add(data, size);
	pq.move(0);
	return pq;
}