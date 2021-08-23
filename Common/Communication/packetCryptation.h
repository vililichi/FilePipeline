#include "Cryptography/AES.h"
#include "Packet.h"

namespace AES
{
	// Cryptage d'un paquet à l'aide d'une clé de 240 octets, la taille du packet peu augmenter de 16 octets
	void cryptage(Packet& cible_, char* cle_);

	// Décryptage d'un paquet à l'aide d'une clé de 240 octets, la taille du packet peu diminuer de 16 octets
	void decryptage(Packet& cible_, char* cle_);

}