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
	//g�n�ration de cl�s de 256 octets prive:=(d,n) public:=(e,n)
	void generation(cle& clePrive, cle& clePublic);

	//cryptage d'un paquet de 16 octets � l'aide d'une cl� de 256 octets,le message et la taille de celui-ci varie
	//pr�voir un espace de de 128 octets pour le message crypt�
	void cryptage(inft& message, cle& clePublic);

	//d�cryptage d'un paquet de 16 octets � l'aide d'une cl� de 256 octets,le message et la taille de celui-ci varie
	//message donc la taille <= 127 octets
	void decryptage(inft& message, cle& clePrive);

}

Packet& operator << (Packet&, inft&);
Packet& operator << (Packet&, RSA::cle&);

Packet& operator >> (Packet&, inft&);
Packet& operator >> (Packet&, RSA::cle&);
