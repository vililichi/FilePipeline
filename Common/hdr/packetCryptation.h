#include "AES.h"
#include "packet.h"

namespace AES
{
	//cryptage d'un paquet � l'aide d'une cl� de 240 octets, la taille du packet peu augmenter de 16 octets
	void cryptage(Packet& cible, char* cle);

		//d�cryptage d'un paquet � l'aide d'une cl� de 240 octets, la taille du packet peu diminuer de 16 octets
	void decryptage(Packet& cible, char* cle);

}