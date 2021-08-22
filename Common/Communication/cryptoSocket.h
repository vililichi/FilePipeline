#pragma once

#include <optional>
#include "packetCryptation.h"
#include "packetSendReceive.h"
#include "Cryptography/RSA.h"

#define THRUST_PATH "thrust"
#define UNTRUST_PATH "unthrust"

class CryptoSocket
{
public:
	sf::TcpSocket* m_socket_ptr;

	/*
	  [client] Envoie une demande de poign�e de main
	  retourne true si l'acc�s est valid�, false sinon
	*/
	bool sendHandShake( RSA::cle cle_RSA_public_, RSA::cle cle_RSA_privee_ );

	/*
	  [serveur] Receptionne une demande de poign�e de main et valide l'utilisateur
	  retourne true si l'utilisateur � un acc�s valide, false sinon
	*/
	bool getHandShake();

	/*
	  Envoie un paquet
	  [in] pq     : Paquet � envoyer
	  [out] stat_ : Status de l'op�ration
	*/
	void send( Packet pq_, std::optional<sf::Socket::Status> stat_ = std::nullopt );

	/*
	  Attend qu'un paquet soit soit r�ceptionn� et retourne ce packet
	  [out] stat_ : Status de l'op�ration
	*/
	Packet receive(std::optional <sf::Socket::Status> stat_ = std::nullopt);

	// Retourne le nom du point d'acces du CryptoSocket
	std::string getAcces() { return m_acces; }
protected:
	char m_cle[240];
	std::string m_acces;
};