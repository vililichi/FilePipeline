#pragma once
#include <SFML/Network.hpp>
#include "packet.h"

namespace packetSender
{

	void send(Packet& pq, sf::TcpSocket* socket_ptr, sf::Socket::Status* stat = NULL);

	Packet receive(sf::TcpSocket* socket_ptr, sf::Socket::Status* stat = NULL);
}