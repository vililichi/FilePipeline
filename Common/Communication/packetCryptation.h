#include "Cryptography/AES.h"
#include "packet.h"

namespace AES
{
    // Cryptage d'un paquet à l'aide d'une clé de 240 octets, la taille du packet peu
    // augmenter de 16 octets
    void cryptage(Packet& cible_, uint8_t* cle_);

    // Décryptage d'un paquet à l'aide d'une clé de 240 octets, la taille du packet peu
    // diminuer de 16 octets
    void decryptage(Packet& cible_, uint8_t* cle_);

}
