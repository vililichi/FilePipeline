#pragma once
#include "Inft.h"

class Packet;

namespace RSA
{
	struct cle
	{
		inft modulus;
		inft exposant;
	};
	//génération de clés de 256 octets prive:=(d,n) public:=(e,n)
	void generation(cle& clePrive_, cle& clePublic_);

	//cryptage d'un message avec un clé public,le message et la taille de celui-ci varie beaucoup
	//message dont la taille <= 255 octets
	void cryptage(inft& message_, cle& clePublic_);

	//décryptage d'un message avec un clé privée,le message et la taille de celui-ci varie beaucoup
	//message dont la taille <= 255 octets
	void decryptage(inft& message_, cle& clePrive_);

	// Enregistre une clé sur le disque
	// [in] cleQuelque_ : Clé à enregistrer
	// [in] path_       : Path du fichier où enregister la clé
	void storeKey(const cle& cleQuelque_, const std::string& path_);

	// Enregistre un set de clés sur le disque
	// [in] clePrive_   : Clé privé à enregistrer
	// [in] clePublic_  : Clé public à enregistrer
	// [in] path_       : Path du fichier où enregister la clé
	void storeKeySet(const cle& clePrive_,const cle& clePublic_, const std::string& path_);

	// Obtient une clé à partir du disque
    // [out] cleQuelque_ : Clé à enregistrer
    // [in]  path_       : Path du fichier où enregister la clé
	bool getKey(cle& cleQuelque_, const std::string& path_);

	// Obtient un set de clés à partir du disque
	// [out] clePrive_   : Clé privé à enregistrer
	// [out] clePublic_  : Clé public à enregistrer
	// [in]  path_       : Path du fichier où enregister la clé
	bool getKeySet(cle& clePrive_, cle& clePublic_, const std::string& path_);

}

Packet& operator << (Packet&, inft&);
Packet& operator << (Packet&, RSA::cle&);

Packet& operator >> (Packet&, inft&);
Packet& operator >> (Packet&, RSA::cle&);
