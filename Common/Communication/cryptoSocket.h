#pragma once

#include "packetCryptation.h"
#include "packetSendReceive.h"
#include "Cryptography/RSA.h"

#define THRUST_PATH "thrust"
#define UNTRUST_PATH "unthrust"

class cryptoSocket
{
public:
	sf::TcpSocket* socket_ptr;
	bool sendHandShake(RSA::cle cle_RSA_public, RSA::cle cle_RSA_privee);
	bool getHandShake();

	void send(Packet pq, sf::Socket::Status* stat = NULL);
	Packet receive(sf::Socket::Status* stat = NULL);

	std::string getAcces() { return acces; }
protected:
	char cle[240];
	std::string acces;
};