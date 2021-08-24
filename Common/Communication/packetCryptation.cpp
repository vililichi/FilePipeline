#include "PacketCryptation.h"
#include <future>

//cryptage d'un paquet à l'aide d'une clé de 240 octets, la taille du packet peu augmenter de 16 octets
void AES::cryptage(Packet &cible_, char *cle_)
{
	//random
	std::random_device ran;
	//complétion
	size_t taille = cible_.size();
	size_t addTaille = 16 - (taille % 16);
	char *compChaine = new char[addTaille];
	compChaine[addTaille - 1] = addTaille;
	for (int i = addTaille - 2; i >= 0; i--)
		compChaine[i] = ran();
	cible_.add(compChaine, addTaille);

	//test pour debug
	taille = cible_.size();
	if (taille % 16 != 0)
		throw "la completion ne fonctionne pas";

	//cryptage
	size_t cycle = taille / 16;
	std::vector<std::future<void>> futures;
	for (int i = 0; i < cycle; i++)
	{
		futures.emplace_back(std::async(
			std::launch::async, [](char *b16, char *cle)
			{ AES::cryptage(b16, cle); },
			cible_.data() + (16 * i), cle_));
	}
	for (auto &future : futures)
		future.wait();
}

//décryptage d'un paquet à l'aide d'une clé de 240 octets, la taille du packet peu diminuer de 16 octets
void AES::decryptage(Packet &cible_, char *cle_)
{
	//test
	size_t taille = cible_.size();
	if (taille % 16 != 0)
		throw "la taille du packet n'est pas un multiple de 16 octets";

	//decryptage
	size_t cycle = taille / 16;
	std::vector<std::future<void>> futures;
	for (int i = 0; i < cycle; i++)
	{
		futures.emplace_back(std::async(
			std::launch::async, [](char *b16, char *cle)
			{ AES::decryptage(b16, cle); },
			cible_.data() + (16 * i), cle_));
	}
	for (auto &future : futures)
		future.wait();

	//decompletion
	char addTaille = cible_.data()[taille - 1];
	if (addTaille > 16 || addTaille < 1)
		throw "dernier bytes impossible, il y a eu un probleme le packet n'est pas valide";
	cible_.popBack(addTaille);
}
