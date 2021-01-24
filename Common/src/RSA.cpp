#include "RSA.h"
#include <thread>

#define nbrO  128

void generateRandomPrimeIn(inft* container, inft min, inft max)
{
    *container = randPrime(min, max);
}

void RSA::generation(cle& clePrive, cle& clePublic)
{


    unsigned char c[nbrO];
    for (int i = 0; i < nbrO; i++) {
        c[i] = 255;
    }

    inft a(c, nbrO);

    inft p;
    inft q;
    std::thread genp(&generateRandomPrimeIn, &p, a / 2, a);
    std::thread genq(&generateRandomPrimeIn, &q, a / 2, a);
    genp.join();
    genq.join();


    //n et phi
    inft n = p * q;
    inft phi = (p - 1) * (q - 1);

    inft e;
    inft d;
    inft y;
    while (true)
    {
        e = randinft(a / 2, a);
        if (extendedGdc(e, phi, d, y) == 1) break;
    }
    if (d < 0) d = d + phi;

    if (!((e * d % phi) == 1))
    {
        std::cout << "inversion non valide\n";
    }
    inft test = 10245896;
    if (!(test.modPow(e, n).modPow(d, n) == inft(10245896)))
    {
        std::cout << "inversion non valide 2\n";
    }

    clePublic.modulus = n;
    clePrive.modulus = n;
    clePublic.exposant = e;
    clePrive.exposant = d;

}

//cryptage d'un paquet de 16 octets à l'aide d'une clé de 256 octets,le message et la taille de celui-ci varie
void RSA::cryptage(inft& message, cle& clePublic)
{
    message = message.modPow(clePublic.exposant, clePublic.modulus);
}

//décryptage d'un paquet de 16 octets à l'aide d'une clé de 256 octets,le message et la taille de celui-ci varie
void RSA::decryptage(inft& message, cle& clePrive)
{
    message = message.modPow(clePrive.exposant, clePrive.modulus);
}

//stockage dans packets
Packet& operator << (Packet& packet, inft& val)
{
    packet << val.size();
    packet.add( (char*) val(), val.size());
    packet << val.isNegatif();
    return packet;
}
Packet& operator << (Packet& packet, RSA::cle& val)
{
    packet << val.exposant;
    packet << val.modulus;
    return packet;
}

Packet& operator >> (Packet& packet, inft& val)
{
    size_t taille;
    bool negative;
    packet >> taille;
    char* cptr = new char[taille];
    packet.read(cptr, taille);
    packet >> negative;

    val = inft((unsigned char*)cptr, taille, negative);
    return packet;
}
Packet& operator >> (Packet& packet, RSA::cle& val)
{
    packet >> val.exposant;
    packet >> val.modulus;
    return packet;
}
