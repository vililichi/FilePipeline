#pragma once

#include "packetCryptation.h"
#include "packetSendReceive.h"
#include "RSA.h"

class cryptoSocket
{
public:
	sf::TcpSocket* socket_ptr;
	bool sendHandShake(RSA::cle cle_RSA_public, RSA::cle cle_RSA_privee);
	bool getHandShake();

	void send(Packet pq, sf::Socket::Status* stat = NULL);
	Packet receive(sf::Socket::Status* stat = NULL);
protected:
	char cle[240];
};