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
	//g�n�ration de cl�s de 256 octets prive:=(d,n) public:=(e,n)
	void generation(cle& clePrive_, cle& clePublic_);

	//cryptage d'un message avec un cl� public,le message et la taille de celui-ci varie beaucoup
	//message dont la taille <= 255 octets
	void cryptage(inft& message_, cle& clePublic_);

	//d�cryptage d'un message avec un cl� priv�e,le message et la taille de celui-ci varie beaucoup
	//message dont la taille <= 255 octets
	void decryptage(inft& message_, cle& clePrive_);

	// Enregistre une cl� sur le disque
	// [in] cleQuelque_ : Cl� � enregistrer
	// [in] path_       : Path du fichier o� enregister la cl�
	void storeKey(const cle& cleQuelque_, const std::string& path_);

	// Enregistre un set de cl�s sur le disque
	// [in] clePrive_   : Cl� priv� � enregistrer
	// [in] clePublic_  : Cl� public � enregistrer
	// [in] path_       : Path du fichier o� enregister la cl�
	void storeKeySet(const cle& clePrive_,const cle& clePublic_, const std::string& path_);

	// Obtient une cl� � partir du disque
    // [out] cleQuelque_ : Cl� � enregistrer
    // [in]  path_       : Path du fichier o� enregister la cl�
	bool getKey(cle& cleQuelque_, const std::string& path_);

	// Obtient un set de cl�s � partir du disque
	// [out] clePrive_   : Cl� priv� � enregistrer
	// [out] clePublic_  : Cl� public � enregistrer
	// [in]  path_       : Path du fichier o� enregister la cl�
	bool getKeySet(cle& clePrive_, cle& clePublic_, const std::string& path_);

}

Packet& operator << (Packet&, inft&);
Packet& operator << (Packet&, RSA::cle&);

Packet& operator >> (Packet&, inft&);
Packet& operator >> (Packet&, RSA::cle&);
