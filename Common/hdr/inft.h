#pragma once

#include <iostream>



class inft
{
protected:
	bool negatif; //vrai si le nombre est négatif
	unsigned char* valeur; // valeur[0] -> octet le moins significatif, valeur[nbrOctet -1] -> octet le plus significatif
	size_t nbrOctet;

	void cut(); //minimise le nombre d'octet

public:
	//constructeur et destructeur
	inft();
	//la valeur du nombre = somme(_valeur[i] * 256^i) pour tout 0<i<_nbrOctet
	inft(unsigned char* _valeur, size_t _nbrOctet, bool _negatif = false);
	inft(const inft& val);
	inft(const unsigned int val);
	inft(const int val);
	~inft();

	//acces aux valeurs
	const bool isNegatif() const { return negatif; }
	const size_t size() const { return nbrOctet; }

	unsigned char* operator ()() { return valeur; }	//acces au pointeur
	unsigned char& operator [] (size_t index) { return *(valeur + index); } //acces à un octet précis
	const unsigned char& operator [] (size_t index) const { return *(valeur + index); } //acces à un octet précis en lecture seulement

	//opérateur d'assignation
	inft& operator = (const inft val);
	inft& operator = (const unsigned int val);
	inft& operator = (const int val);

	//operateur de comparaison
	const bool operator < (const inft) const;
	const bool operator > (const inft) const;
	const bool operator == (const inft) const;
	const bool operator <= (const inft) const;
	const bool operator >= (const inft) const;

	//operateur arithmetique
		//addition
	const inft operator + (const inft) const;
	const inft operator - (const inft) const;
	void operator -- ();
	//multiplication
	const inft operator * (const inft) const;
	const inft operator / (const inft) const;
	const inft operator % (const inft) const;

	//fonctions utiles
	const inft abs() const { inft out(*this); out.negatif = false; return out; }
	const inft pow(inft val) const;
	const inft modPow(inft exposant, inft modulo) const;
	bool isImpair() const { return valeur[0] & 0x01; }
	bool isPrime(int precision) const;
	const inft half() const; // /2
	const inft dbl() const;  // *2


};

std::ostream& operator << (std::ostream& os, const inft val);

inft randPrime(inft min, inft max);
inft randinft(inft min, inft max);
inft gdc(inft a, inft b);
inft extendedGdc(inft a, inft b, inft& x, inft& y); //ax + by = out
inft invMod(inft a, inft p); //return x pour ax%p = 1