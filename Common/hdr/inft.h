#pragma once

#include <iostream>



class inft
{
protected:
	bool negatif; //vrai si le nombre est négatif
	unsigned long* valeur; // valeur[0] -> octet le moins significatif, valeur[nbrOctet -1] -> octet le plus significatif
	size_t nbrLong;

	void cut(); //minimise le nombre d'octet

public:
	//constructeur et destructeur
	inft();
	//la valeur du nombre = somme(_valeur[i] * 256^i) pour tout 0<i<_nbrOctet
	inft(unsigned long* _valeur, size_t _nbrLong, bool _negatif = false);
	inft(unsigned short* _valeur, size_t _nbrShort, bool _negatif = false);
	inft(unsigned char* _valeur, size_t _nbrOctet, bool _negatif = false);
	inft(const inft& val);
	inft(inft&& val) noexcept;
	inft(const unsigned int val);
	inft(const int val);
	~inft();

	//acces aux valeurs
	const bool isNegatif() const { return negatif; }
	const size_t size() const { return nbrLong; }

	unsigned long* operator ()() { return valeur; }	//acces au pointeur
	unsigned long& operator [] (size_t index) { return *(valeur + index); } //acces à un octet précis
	const unsigned long& operator [] (size_t index) const { return *(valeur + index); } //acces à un octet précis en lecture seulement

	//opérateur d'assignation
	inft& operator = (const inft& val);
	inft& operator = (inft&& val) noexcept;
	inft& operator = (const unsigned int val);
	inft& operator = (const int val);

	//operateur de comparaison
	const bool operator < (const inft&) const;
	const bool operator > (const inft&) const;
	const bool operator == (const inft&) const;
	const bool operator <= (const inft&) const;
	const bool operator >= (const inft&) const;

	//operateur arithmetique
		//addition
	inline const inft operator + (const inft&) const;
	inline const inft operator - (const inft&) const;
	void operator -- ();
	//multiplication
	const inft operator * (const inft&) const;
	const inft operator / (const inft&) const;
	const inft operator % (const inft&) const;

	//fonctions utiles
	const inft abs() const { inft out(*this); out.negatif = false; return out; }
	const inft pow(inft val) const;
	const inft modPow(inft exposant,const inft& modulo) const;
	bool isImpair() const { return valeur[0] & 0x01; }
	bool isPrime(int precision) const;
	const inft half() const; // /2
	const inft& halfThis(); // /= 2
	const inft dbl() const;  // *2
	inline void swap(inft&);
private:
	inft(size_t _nbrLong, bool _negatif) :nbrLong(_nbrLong), negatif(_negatif) { valeur = new unsigned long[nbrLong];}; //crée un inft de taille déterminé ayant des valeurs indéterminé
	inft(bool _negatif, size_t _nbrLong) :nbrLong(_nbrLong), negatif(_negatif) { valeur = new unsigned long[nbrLong](); }; //crée un inft de taille déterminé ayant 0 comme valeur

	friend const inft  additiveComp(const inft& A, const inft& B, bool invB = false);

};

std::ostream& operator << (std::ostream& os, const inft& val);

inft randPrime(inft min, inft max);
inft randinft(inft min, inft max);
inft gdc(inft a, inft b);
inft extendedGdc(inft a, inft b, inft& x, inft& y); //ax + by = out
inft invMod(inft a, inft p); //return x pour ax%p = 1