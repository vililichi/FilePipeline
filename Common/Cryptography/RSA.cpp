#include "RSA.h"

#include "Communication/packet.h"

#include <fstream>
#include <thread>
#include <utility>

constexpr size_t nbrI = 32;
constexpr uint32_t Fermat4 = 65537;

// Fonction pour les threads de RSA::generation
static void generateRandomPrimeInft(inft* container_, inft min_, inft max_)
{
    *container_ = randPrime(std::move(min_), std::move(max_));
}

std::pair<RSA::cle, RSA::cle> RSA::generation()
{
    uint32_t c[nbrI];
    for (int i = 0; i < nbrI; i++)
    {
        c[i] = std::numeric_limits<uint32_t>::max();
    }

    inft a(c, nbrI);
    inft p;
    inft q;
    std::thread genp(&generateRandomPrimeInft, &p, a.half(), a);
    std::thread genq(&generateRandomPrimeInft, &q, a.half(), a);
    genp.join();
    genq.join();

    // n et phi
    inft n = p * q;
    inft phi = (p - inft(1)) * (q - inft(1));

    inft e;
    inft d;
    inft y;
    while (true)
    {
        e = randinft(inft(Fermat4), inft(Fermat4 + 1).pow(16));
        if (extendedGdc(e, phi, d, y) == inft(1))
        {
            if (d < inft())
                d = d + phi;
            if (d.size() >= nbrI)
                break; // test de la taille de la clé
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

    RSA::cle clePublic;
    RSA::cle clePrive;

    clePublic.modulus = n;
    clePrive.modulus = n;
    clePublic.exposant = e;
    clePrive.exposant = d;

    return {clePublic, clePrive};
}

// cryptage d'un paquet de 16 octets à l'aide d'une clé de 256 octets,le message et la
// taille de celui-ci varie
void RSA::cryptage(inft& message_, const cle& clePublic_)
{
    message_ = message_.modPow(clePublic_.exposant, clePublic_.modulus);
}

// décryptage d'un paquet de 16 octets à l'aide d'une clé de 256 octets,le message et la
// taille de celui-ci varie
void RSA::decryptage(inft& message_, const cle& clePrive_)
{
    message_ = message_.modPow(clePrive_.exposant, clePrive_.modulus);
}

// stockage dans packets
Packet& operator<<(Packet& packet_, const inft& val_)
{
    packet_ << val_.size() * 4;
    packet_.add(reinterpret_cast<const uint8_t*>(val_()), val_.size() * 4);
    packet_ << val_.isNegatif();
    return packet_;
}
Packet& operator<<(Packet& packet_, const RSA::cle& val_)
{
    packet_ << val_.exposant;
    packet_ << val_.modulus;
    return packet_;
}

Packet& operator>>(Packet& packet_, inft& val_)
{
    size_t taille;
    bool negative;
    packet_ >> taille;
    uint8_t* cptr = new uint8_t[taille];
    packet_.read(cptr, taille);
    packet_ >> negative;

    val_ = inft(cptr, taille, negative);
    delete[] cptr;
    return packet_;
}
Packet& operator>>(Packet& packet_, RSA::cle& val_)
{
    packet_ >> val_.exposant;
    packet_ >> val_.modulus;
    return packet_;
}

// mise en place sur le disque
void RSA::storeKey(const cle& cleQuelque_, const std::string& path_)
{
    Packet pq;
    pq << cleQuelque_;

    std::ofstream file;
    file.open(path_, std::ostream::out | std::ostream::binary);
    file << pq.size();
    file.write(reinterpret_cast<char*>(pq.data()), pq.size());
    file.close();
}
void RSA::storeKeySet(const cle& clePrive_, const cle& clePublic_,
                      const std::string& path_)
{
    Packet pq;
    pq << clePrive_;
    pq << clePublic_;

    std::ofstream file;
    file.open(path_, std::ostream::out | std::ostream::binary);
    file << pq.size();
    file.write(reinterpret_cast<char*>(pq.data()), pq.size());
    file.close();
}
bool RSA::getKey(cle& cleQuelque_, const std::string& path_)
{
    size_t size;
    std::ifstream file;

    file.open(path_, std::ostream::in | std::ostream::binary);
    if (!file.is_open())
        return false;

    file >> size;
    char* cptr = new char[size];
    file.read(cptr, size);
    file.close();

    Packet pq(size);
    pq.add(reinterpret_cast<uint8_t*>(cptr), size);
    pq.move(0);
    pq >> cleQuelque_;

    return true;
}
bool RSA::getKeySet(cle& clePrive_, cle& clePublic_, const std::string& path_)
{
    size_t size;
    std::ifstream file;

    file.open(path_, std::ostream::in | std::ostream::binary);
    if (!file.is_open())
        return false;

    file >> size;
    char* cptr = new char[size];
    file.read(cptr, size);
    file.close();

    Packet pq(size);
    pq.add(reinterpret_cast<uint8_t*>(cptr), size);
    pq.move(0);
    pq >> clePrive_;
    pq >> clePublic_;

    return true;
}
