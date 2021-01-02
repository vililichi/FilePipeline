#include "packetCryptation.h"

//cryptage d'un paquet à l'aide d'une clé de 240 octets, la taille du packet peu augmenter de 16 octets
void AES::cryptage(Packet& cible, char* cle)
{
	//random
	std::random_device ran;
	//complétion
	size_t taille = cible.size();
	size_t addTaille = 16 - (taille % 16);
	char* compChaine = new char[addTaille];
	compChaine[addTaille - 1] = addTaille;
	for (int i = addTaille - 2; i >= 0; i--)compChaine[i] = ran();
	cible.add(compChaine, addTaille);
	
	//test pour debug
	taille = cible.size();
	if (taille % 16 != 0)throw "la completion ne fonctionne pas";
	

	//cryptage
	size_t cycle = taille / 16;
	for (int i = 0; i < cycle; i++)
	{
		AES::cryptage(cible.data() + (16 * i), cle);
	}

}

//décryptage d'un paquet à l'aide d'une clé de 240 octets, la taille du packet peu diminuer de 16 octets
void AES::decryptage(Packet& cible, char* cle)
{
	//test
	size_t taille = cible.size();
	if (taille % 16 != 0)throw "la taille du packet n'est pas un multiple de 16 octets";

	//decryptage
	size_t cycle = taille / 16;
	for (int i = 0; i < cycle; i++)
	{
		AES::decryptage(cible.data() + (16 * i), cle);
	}

	//decompletion
	char addTaille = cible.data()[taille - 1];
	if (addTaille > 16 || addTaille < 1)throw "dernier bytes impossible, il y a eu un probleme le packet n'est pas valide";
	cible.popBack(addTaille);
}