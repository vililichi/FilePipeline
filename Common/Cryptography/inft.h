#pragma once
#include <cinttypes>
#include <iostream>



class inft
{
protected:
	bool m_negatif; //vrai si le nombre est n�gatif
	uint32_t* m_valeur; // valeur[0] -> octet le moins significatif, valeur[nbrOctet -1] -> octet le plus significatif
	size_t m_nbrLong;

	// Minimise le nombre d'octets
	void cut();

public:
	//constructeur
	inft();

	// la valeur du nombre = somme(_valeur[i] * 256^i) pour tout 0<i<_nbrOctet

	// Cr�e un inf avec un tableau de long
	inft(uint32_t* const valeur_, size_t nbrLong_, bool negatif_ = false);
	// Cr�e un inf avec un tableau de short
	inft(uint16_t* const valeur_, size_t nbrShort_, bool negatif_ = false);
	// Cr�e un inf avec un tableau de char
	inft(uint8_t* const valeur_, size_t nbrOcte_, bool negatif_ = false);
	// Constructeur de copie
	inft(const inft& val_);
	// Constructeur de d�placement
	inft(inft&& val_) noexcept;
	// Conversion directe uint > inft
	inft(const uint32_t val_);
	// Conversion directe int > inft
	inft(const int32_t val_);
	~inft();

	// Indique si le inft est n�gatif
	const bool isNegatif() const { return m_negatif; }
	// Indique la taille du inft (nombre de paquet de 32bit )
	const size_t size() const { return m_nbrLong; }

	// Acces aux donn��s interne
	uint32_t* operator ()() { return m_valeur; }
	// Acc�s � un paquet de 32bit pr�cis
	uint32_t& operator [] (size_t index) { return *(m_valeur + index); }
	// Acc�s � un paquet de 32bit pr�cis en lecture seulement
	const uint32_t& operator [] (size_t index) const { return *(m_valeur + index); }

	//op�rateur d'assignation
	inft& operator = (const inft& val_);
	inft& operator = (inft&& val_) noexcept;
	inft& operator = (const uint32_t val_);
	inft& operator = (const int32_t  val_);

	//operateur de comparaison
	const bool operator < (const inft&) const;
	const bool operator > (const inft&) const;
	const bool operator == (const inft&) const;
	const bool operator <= (const inft&) const;
	const bool operator >= (const inft&) const;

	//operateur arithmetique
		//addition
	inline inft operator + (const inft&) const;
	inline inft operator - (const inft&) const;
	void operator -- ();
	//multiplication
	inft operator * (const inft&) const;
	inft operator / (const inft&) const;
	inft operator % (const inft&) const;

	//fonctions utiles

	// Valeur absolue
	inft abs() const { inft out(*this); out.m_negatif = false; return out; }
	// Puissance [Inf^val]
	inft pow(inft val_) const;
	// Puissance modulo [(Inf^exposant)%modulo]
	inft modPow(inft exposant,const inft& modulo) const;
	// Retourne vrai si le Inft est impaire
	bool isImpair() const { return m_valeur[0] & 0x01; }
	// Retourne vrai si le Inft est premier
	bool isPrime(uint32_t precision_) const;
	// Divise par deux [/2]
	inft half() const;
	// Divise l'object actuel par 2 [/= 2]
	inft& halfThis();
	// Multiplie par 2 [*2]
	inft dbl() const;
	// Change le contenue de deux Inft
	inline void swap(inft&);
private:
	//cr�e un inft de taille d�termin� ayant des valeurs ind�termin�
	inft(size_t nbrLong_, bool negatif_) :m_nbrLong(nbrLong_), m_negatif(negatif_) { m_valeur = new uint32_t[m_nbrLong];};
	//cr�e un inft de taille d�termin� ayant 0 comme valeur
	inft(bool negatif_, size_t nbrLong_) :m_nbrLong(nbrLong_), m_negatif(negatif_) { m_valeur = new uint32_t[m_nbrLong](); };

	friend inft  additiveComp(const inft& A_, const inft& B_, const bool invB_ = false);

};

std::ostream& operator << (std::ostream& os_, const inft& val_);

// Cr�e in Inft premier al�atoire entre min_ et max_
inft randPrime(inft min_, inft max_);
// Cr�e un Inft al�atoire entre min_ et max_
inft randinft(inft min_, inft max_);
// Retourne le plus grand commun diviseur
inft gdc(inft a_, inft b_);
// ax + by = out
inft extendedGdc(inft a_, inft b_, inft& x_, inft& y_); 
// retourne x pour ax%p = 1
inft invMod(inft a_, inft p_); 