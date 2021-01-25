#pragma once
#include "inft.h"
#include "packet.h"

namespace RSA
{
	struct cle
	{
		inft modulus;
		inft exposant;
	};
	//génération de clés de 256 octets prive:=(d,n) public:=(e,n)
	void generation(cle& clePrive, cle& clePublic);

	//cryptage d'un message avec un clé public,le message et la taille de celui-ci varie beaucoup
	//message dont la taille <= 255 octets
	void cryptage(inft& message, cle& clePublic);

	//décryptage d'un message avec un clé privée,le message et la taille de celui-ci varie beaucoup
	//message dont la taille <= 255 octets
	void decryptage(inft& message, cle& clePrive);

	//mise en place sur le disque
	void storeKey(cle& clecleQuelque, std::string path);
	void storeKeySet(cle& clePrive,cle& clePublic, std::string path);
	bool getKey(cle& cleQuelque, std::string path);
	bool getKeySet(cle& clePrive, cle& clePublic, std::string path);

}

Packet& operator << (Packet&, inft&);
Packet& operator << (Packet&, RSA::cle&);

Packet& operator >> (Packet&, inft&);
Packet& operator >> (Packet&, RSA::cle&);
