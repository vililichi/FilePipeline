#include "AES.h"
#include "packet.h"

namespace AES
{
	//cryptage d'un paquet à l'aide d'une clé de 240 octets, la taille du packet peu augmenter de 16 octets
	void cryptage(Packet& cible, char* cle);

		//décryptage d'un paquet à l'aide d'une clé de 240 octets, la taille du packet peu diminuer de 16 octets
	void decryptage(Packet& cible, char* cle);

}