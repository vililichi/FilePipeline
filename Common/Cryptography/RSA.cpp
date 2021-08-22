#include "RSA.h"
#include "Communication/packet.h"
#include <thread>
#include <fstream>

#define nbrI  32

void generateRandomPrimeIn(inft* container, inft min, inft max)
{
    *container = randPrime(min, max);
}

void RSA::generation(cle& clePrive, cle& clePublic)
{


    unsigned long c[nbrI];
    for (int i = 0; i < nbrI; i++) {
        c[i] = 4294967295;
    }

    inft a(c, nbrI);
    inft p;
    inft q;
    std::thread genp(&generateRandomPrimeIn, &p, a.half(), a);
    std::thread genq(&generateRandomPrimeIn, &q, a.half(), a);
    genp.join();
    genq.join();


    //n et phi
    inft n = p * q;
    inft phi = (p - inft(1)) * (q - inft(1));

    inft e;
    inft d;
    inft y;
    while (true)
    {
        e = randinft(inft(65537), inft(65538).pow(16));
        if (extendedGdc(e, phi, d, y) == inft(1))
        {
            if (d < inft()) d = d + phi;
            if (d.size() >= nbrI) break; //test de la taille de la clé
        }
    }
    

    if (!((e * d % phi) == inft(1)))
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
    packet << val.size()*4;
    packet.add( (char*) val(), val.size()*4);
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
    delete[] cptr;
    return packet;
}
Packet& operator >> (Packet& packet, RSA::cle& val)
{
    packet >> val.exposant;
    packet >> val.modulus;
    return packet;
}

//mise en place sur le disque
void RSA::storeKey(cle& cleQuelque, std::string path)
{
    Packet pq;
    pq << cleQuelque;

    std::ofstream file;
    file.open(path, std::ostream::out | std::ostream::binary);
    file << pq.size();
    file.write(pq.data(), pq.size());
    file.close();
    
}
void RSA::storeKeySet(cle& clePrive, cle& clePublic, std::string path)
{
    Packet pq;
    pq << clePrive;
    pq << clePublic;

    std::ofstream file;
    file.open(path, std::ostream::out | std::ostream::binary);
    file << pq.size();
    file.write(pq.data(), pq.size());
    file.close();
}
bool RSA::getKey(cle& cleQuelque, std::string path)
{
    size_t size;
    std::ifstream file;

    file.open(path, std::ostream::in | std::ostream::binary);
    if (!file.is_open())return false;

    file >> size;
    char* cptr = new char[size];
    file.read(cptr, size);
    file.close();

    Packet pq(size);
    pq.add(cptr, size);
    pq.move(0);
    pq >> cleQuelque;

    return true;
}
bool RSA::getKeySet(cle& clePrive, cle& clePublic, std::string path)
{
    size_t size;
    std::ifstream file;

    file.open(path, std::ostream::in | std::ostream::binary);
    if (!file.is_open())return false;

    file >> size;
    char* cptr = new char[size];
    file.read(cptr, size);
    file.close();

    Packet pq(size);
    pq.add(cptr, size);
    pq.move(0);
    pq >> clePrive;
    pq >> clePublic;

    return true;
}
