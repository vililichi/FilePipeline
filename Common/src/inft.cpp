#include "inft.h"
#include "primeList.h"
#include <random>
#include <algorithm>
#include <stdlib.h>


void inft::cut()
{
	size_t i = nbrLong - 1;
	while (i > 0)
	{
		if (valeur[i] == 0)
		{
			i--;
			nbrLong -= 1;
		}
		else break;
	}
}
//constructeur et destructeur
inft::inft() : nbrLong(1), negatif(false)
{
	valeur = new unsigned long[nbrLong];
	valeur[0] = 0;
}
inft::inft(unsigned long* _valeur, size_t _nbrLong, bool _negatif) : nbrLong(_nbrLong), negatif(_negatif)
{
	valeur = new unsigned long[nbrLong];
	std::copy(_valeur, _valeur + nbrLong, valeur);
	cut();
}
inft::inft(unsigned short* _valeur, size_t _nbrShort, bool _negatif)
{
	if (_nbrShort & 1)
	{
		nbrLong = (_nbrShort + 1) / 2;
		valeur = new unsigned long[nbrLong];
		valeur[nbrLong - 1] = 0;
	}
	else
	{
		nbrLong = _nbrShort / 2;
		valeur = new unsigned long[nbrLong];
	}
	unsigned short* valShort = (unsigned short*)valeur;
	std::copy(_valeur, _valeur + _nbrShort, valShort);

	negatif = _negatif;
}
inft::inft(unsigned char* _valeur, size_t _nbrOctet, bool _negatif)
{
	if (_nbrOctet % 4 != 0)
	{
		nbrLong = (_nbrOctet / 2) + 1 ;
		valeur = new unsigned long[nbrLong];
		valeur[nbrLong - 1] = 0;
	}
	else
	{
		nbrLong = _nbrOctet / 4;
		valeur = new unsigned long[nbrLong];
	}
	unsigned char* valChar = (unsigned char*)valeur;
	std::copy(_valeur, _valeur + _nbrOctet, valChar);

	negatif = _negatif;
}
inft::inft(const inft& val)
{
	nbrLong = val.nbrLong;
	valeur = new unsigned long [nbrLong];

	std::copy(val.valeur, val.valeur + nbrLong, valeur);

	negatif = val.negatif;
	cut();
}
inft::inft(inft&& val) noexcept
{
	nbrLong = val.nbrLong;

	valeur = val.valeur;
	val.valeur = nullptr;
	val.nbrLong = 0;

	negatif = val.negatif;
	cut();
}
inft::inft(const unsigned int val)
{
	nbrLong = 1;

	valeur = new unsigned long[nbrLong];

	valeur[0] = val;

	negatif = false;
	cut();
}
inft::inft(const int val)
{
	long mval = val;
	if (val < 0)
	{
		negatif = true;
		mval *= -1;
	}
	else negatif = false;

	nbrLong = 1;

	valeur = new unsigned long[nbrLong];

	valeur[0] = mval;
	cut();
}

inft::~inft()
{
	delete[] valeur;
}

//opérateur d'assignation

inft& inft::operator = (const inft& val)
{
	if (this == &val) return *this;
	nbrLong = val.nbrLong;

	delete[] valeur;
	valeur = new unsigned long[nbrLong];

	std::copy(val.valeur, val.valeur + nbrLong, valeur);

	negatif = val.negatif;
	cut();

	return *this;
}

inft& inft::operator = (inft&& val) noexcept
{
	nbrLong = val.nbrLong;

	delete[] valeur;
	valeur = val.valeur;
	val.valeur = nullptr;
	val.nbrLong = 0;

	negatif = val.negatif;

	cut();
	return *this;
}

inft& inft::operator = (const unsigned int val)
{
	nbrLong = 1;

	delete[] valeur;
	valeur = new unsigned long[nbrLong];

	valeur[0] = val;

	negatif = false;

	cut();
	return *this;
}
inft& inft::operator = (const int val)
{
	long mval = val;
	if (val < 0)
	{
		negatif = true;
		mval = (mval * -1);
	}
	else negatif = false;

	nbrLong = 1;

	delete[] valeur;
	valeur = new unsigned long[nbrLong];

	valeur[0] = val;

	cut();
	return *this;

}


//pour débugage
std::ostream& operator << (std::ostream& os, const inft& val)
{
	if (val == inft())os << "0";
	else
	{
		if (val.isNegatif()) os << '-';
		os << " ";
		inft cpy = val.abs();
		int pow10 = 0;
		while (cpy > 0)
		{
			cpy = cpy / 10;
			pow10++;
		}

		cpy = val;
		for (int i = pow10 - 1; i >= 0; i--)
		{
			inft aff = cpy / inft(10).pow(i);
			cpy = cpy % inft(10).pow(i);
			os << aff[0];

		}
	}
	return os;
}

//operateur de comparaison
const bool inft::operator < (const inft& val) const
{
	if (negatif && !val.negatif)return true;
	else if (val.negatif && !negatif) return false;
	else if (!negatif)
	{
		if (nbrLong < val.nbrLong) return true;
		else if (nbrLong > val.nbrLong) return false;
		else for (size_t i = nbrLong; i > 0; )
		{
			i--;
			if (valeur[i] < val.valeur[i]) return true;
			else if (valeur[i] > val.valeur[i]) return false;
		}
	}
	else
	{
		if (nbrLong > val.nbrLong) return true;
		else if (nbrLong < val.nbrLong) return false;
		else for (size_t i = nbrLong; i > 0; )
		{
			i--;
			if (valeur[i] > val.valeur[i]) return true;
			else if (valeur[i] < val.valeur[i]) return false;
		}
	}
	return false;
}
const bool inft::operator > (const inft& val) const
{
	if (negatif && !val.negatif)return false;
	else if (val.negatif && !negatif) return true;
	else if (negatif)
	{
		if (nbrLong < val.nbrLong) return true;
		else if (nbrLong > val.nbrLong) return false;
		else for (size_t i = nbrLong; i > 0; )
		{
			i--;
			if (valeur[i] < val.valeur[i]) return true;
			else if (valeur[i] > val.valeur[i]) return false;
		}
	}
	else
	{
		if (nbrLong > val.nbrLong) return true;
		else if (nbrLong < val.nbrLong) return false;
		else for (size_t i = nbrLong; i > 0; )
		{
			i--;
			if (valeur[i] > val.valeur[i]) return true;
			else if (valeur[i] < val.valeur[i]) return false;
		}
	}
	return false;
}
const bool inft::operator == (const inft& val) const
{
	if (negatif != val.negatif) return false;
	if (nbrLong != val.nbrLong)return false;
	for (size_t i = 0; i < nbrLong; i++) if (valeur[i] != val.valeur[i]) return false;
	return true;
}
const bool inft::operator <= (const inft& val) const
{
	return !operator > (val);
}
const bool inft::operator >= (const inft& val) const
{
	return !operator < (val);
}

//arithmétique
inft  additiveComp(const inft& A, const inft& B, bool invB)
{
	//positif + negatif ou negatif + positif
	if (A.isNegatif() != B.isNegatif())
	{
		//comparaison de taille
		const inft* big = NULL;
		const inft* small = NULL;

		const bool thisNeg = A.negatif;
		const bool valNeg = B.negatif;

		const_cast<inft*>(&A)->negatif = false;
		const_cast<inft*>(&B)->negatif = false;

		if (A == B) return inft();
		else if (A > B) { big = &A; small = &B; }
		else { big = &B; small = &A; }

		const_cast<inft*>(&A)->negatif = thisNeg;
		const_cast<inft*>(&B)->negatif = valNeg;

		//transformation int
		inft out(big->size(), big->isNegatif());

		//soustraction
		long long buffer = 0;
		for (size_t i = 0; i < big->size(); i++)
		{

			buffer += big->valeur[i];
			if (i < small->size()) buffer -= small->valeur[i];
			if (buffer < 0)
			{
				buffer += 4294967296;
				out.valeur[i] = *(unsigned long*)&buffer;
				buffer = -1;
			}
			else
			{
				out.valeur[i] = *(unsigned long*)&buffer;
				buffer = 0;
			}

		}

		out.cut();
		if (invB)const_cast<inft*>(&B)->negatif = !const_cast<inft*>(&B)->negatif;
		return out;

	}

	//positif + positif ou negatif + negatif
	else
	{
		//mesure de la taille des valeurs
		size_t max = B.nbrLong;
		if (A.nbrLong > B.nbrLong) max = A.nbrLong;

		//création de l'espace nécessaire
		inft out(max + 1, A.negatif);

		unsigned long long buffer = 0;
		for (size_t i = 0; i < max + 1; i++)
		{
			buffer = buffer >> 32;
			if (i < A.nbrLong) buffer += A.valeur[i];
			if (i < B.nbrLong) buffer += B.valeur[i];
			out.valeur[i] = *(unsigned long*)&buffer;
		}

		out.cut();
		if (invB)const_cast<inft*>(&B)->negatif = !const_cast<inft*>(&B)->negatif;
		return out;
	}
}

inft inft::operator + (const inft& val) const
{
	return additiveComp(*this, val, false);
}

inft inft::operator - (const inft& val) const
{
	const_cast<inft*>(&val)->negatif = !const_cast<inft*>(&val)->negatif;
	return additiveComp(*this, val, true);
}

void inft::operator -- ()
{
	if (*this == 0 || negatif)
	{
		*this = *this + inft(-1);
	}
	else
	{
		size_t i = 0;
		while (true)
		{
			if (valeur[i] == 0)
			{
				valeur[i] = 4294967295;
				i++;
			}
			else
			{
				valeur[i] --;
				break;
			}
		}
	}
}

inft inft::operator * (const inft& val) const
{
	inft out(isNegatif() ^ val.isNegatif(), nbrLong + val.nbrLong);

	for (size_t i = 0; i < nbrLong; i++)
		for (size_t j = 0; j < val.nbrLong; j++)
		{
			unsigned long long produit = (unsigned long long)valeur[i] * val.valeur[j];

			size_t k = 0;
			while (produit != 0)
			{
				produit += out.valeur[i + j + k];
				out.valeur[i + j + k] = *(unsigned long*)&produit;
				produit = produit >> 32;
				k++;
			}
		}

	out.cut();
	return out;
}

inft inft::operator / (const inft& val) const
{

	if (abs() < val.abs()) return inft(); //réponse triviale

	//assignation de l'espace
	inft cpy((*this).abs());
	

	//taille de la réponse
	size_t sizeThis = nbrLong << 1;
	if (((unsigned short*)valeur)[sizeThis - 1] == 0)sizeThis--;



	//dénominateur raccourcie
	size_t sizeDenom = val.nbrLong << 1;
	unsigned short* shortDenom = (unsigned short*)val.valeur;
	if (shortDenom[sizeDenom - 1] == 0)sizeDenom--;
	long long  denom = (unsigned long long)shortDenom[sizeDenom - 1] <<16;
	if (sizeDenom >= 2) denom += shortDenom[sizeDenom - 2];

	//assignation de l'espace
	size_t diffS = (sizeThis - sizeDenom) + 1;
	unsigned short* cptr = new unsigned short[diffS];	//char* contenant la réponse

	//Pour chaqu'un des éléments de la division
	for (size_t i = diffS, j = 0; i > 0;)
	{
		//décrémentation
		i--;
		//numérateur raccourcie
		size_t sizeNum = cpy.nbrLong << 1;
		unsigned short* shortNum = (unsigned short*)cpy.valeur;
		unsigned long long num = 0;
		if (sizeThis - j - 1 < sizeNum)	num += (unsigned long long)shortNum[sizeThis - j - 1] <<16;
		if (sizeThis - j < sizeNum)		num += (unsigned long long)shortNum[sizeThis - j] <<32;
		if (sizeDenom >= 2 && sizeThis - j - 2 < sizeNum)	num += shortNum[sizeThis - j - 2];

		//estimation
		unsigned long long ps = (num / denom) - 1;

		//valeur réelle
		if (ps >= 0)
		{
			//création du facteur de test
			unsigned short* facptr;
			size_t tailleFact = sizeDenom + i;
			facptr = new unsigned short[sizeDenom + i];
			for (size_t k = 0; k < i; k++) facptr[k] = 0;
			for (size_t k = 0; k < sizeDenom; k++)facptr[k + i] = shortDenom[k];
			
			inft fact(facptr, tailleFact);
			delete[] facptr;

			//test avec prédiction
			cpy = cpy - (inft( (long)ps ) * fact);

			//test exterieur à prediction
			inft limit = cpy - fact;
			if (!limit.isNegatif())
			{
				cpy.swap(limit);
				ps++;
			}
			cptr[i] = (unsigned short)ps;
		}
		else { cptr[i] = 0; }

		//incrémentation
		j++;
	}

	inft out(cptr, diffS, isNegatif() ^ val.isNegatif());
	delete[] cptr;
	return out;
}
inft inft::operator % (const inft& val) const
{
	if (abs() < val.abs()) return *this; //réponse triviale
		//assignation de l'espace
		inft cpy((*this).abs());

		//taille de la réponse
		size_t sizeThis = nbrLong << 1;
		if (((unsigned short*)valeur)[sizeThis - 1] == 0)sizeThis--;
		

		//dénominateur raccourcie
		size_t sizeDenom = val.nbrLong << 1;
		unsigned short* shortDenom = (unsigned short*)val.valeur;
		if (shortDenom[sizeDenom - 1] == 0)sizeDenom--;
		long long  denom = (unsigned long long)shortDenom[sizeDenom - 1] << 16;
		if (sizeDenom >= 2) denom += shortDenom[sizeDenom - 2];

		//taille reponse
		size_t diffS = sizeThis - sizeDenom + 1;

		//Pour chaqu'un des éléments de la division
		for (size_t i = diffS, j = 0; i > 0;)
		{
			//décrémentation
			i--;
			//numérateur raccourcie
			size_t sizeNum = cpy.nbrLong << 1;
			unsigned short* shortNum = (unsigned short*)cpy.valeur ;
			unsigned long long num = 0;
			if (sizeThis - j - 1 < sizeNum)	num += (unsigned long long)shortNum[sizeThis - j - 1] << 16;
			if (sizeThis - j < sizeNum)		num += (unsigned long long)shortNum[sizeThis - j] << 32;
			if (sizeDenom >= 2 && sizeThis - j - 2 < sizeNum)	num += shortNum[sizeThis - j - 2];

			//estimation
			unsigned long long ps = (num / denom) - 1;

			//valeur réelle
			if (ps >= 0)
			{
				//création du facteur de test
				unsigned short* facptr;
				size_t tailleFact = sizeDenom + i;
				facptr = new unsigned short[sizeDenom + i];
				for (size_t k = 0; k < i; k++) facptr[k] = 0;
				for (size_t k = 0; k < sizeDenom; k++)facptr[k + i] = shortDenom[k];

				inft fact(facptr, tailleFact);
				delete[] facptr;

				//test avec prédiction
				cpy = cpy - (inft((long)ps) * fact);

				//test exterieur à prediction
				inft limit = cpy - fact;
				if (!limit.isNegatif())
				{
					cpy.swap(limit);
				}
			}

			//incrémentation
			j++;
		}
	
	cpy.negatif = negatif;
	return cpy;
}
inft inft::half() const
{
	//assignation de l'espace
	inft out(nbrLong, false);

	//Pour chaqu'un des éléments de la réponse
	for (size_t i = 0 ; i < nbrLong- 1; i++) out.valeur[i] = valeur[i+1] << 31 | valeur[i] >> 1;
	out.valeur[nbrLong - 1] = valeur[nbrLong - 1] >> 1;

	out.cut();
	return out;
}
inft& inft::halfThis()
{
	for (size_t i = 0; i < nbrLong - 1; i++)valeur[i] = valeur[i + 1] << 31 | valeur[i] >> 1;
	valeur[nbrLong - 1] = valeur[nbrLong - 1] >> 1;
	cut();
	return *this;
}
inft inft::dbl() const
{
	//assignation de l'espace
	inft out(nbrLong + 1, false);

	//Pour chaqu'un des éléments de la réponse
	out.valeur[0] = valeur[0] << 1;
	for (size_t i = 1; i < nbrLong ; i++) out.valeur[i] = valeur[i] << 1 | valeur[i-1] >> 31;
	out.valeur[nbrLong] = valeur[nbrLong - 1] >> 31;


	out.cut();
	return out;
}

//puissance
inft inft::pow(inft val) const {
	inft out(1);
	inft driver = *this;
	while (val > 0)
	{
		if (val.isImpair())
		{
			out = out * driver;
		}

		driver = driver * driver;
		val = val.half();
		
	}
	return out;
}
inft inft::modPow(inft exposant, const inft& modulo) const {
	inft out(1);
	inft driver = *this % modulo;
	while (exposant > inft())
	{
		if (exposant.isImpair())
		{
			out = (out * driver)% modulo;
		}

		driver = (driver * driver)% modulo;
		exposant.halfThis();
	}
	return out;
}

bool inft::isPrime(int precision = 0) const
{
	std::random_device randomMachine;
	//prétest
	if (*this == inft(2) || *this == inft(3))return true;
	bool pair = !isImpair();
	bool neg = *this <= inft(1);
	if ((!isImpair()) || *this <= inft(1))return false;
	for (int i = 0; i < 256; i++)
	{
		if (*this == fistPrime[i]) return true;
		if (*this % fistPrime[i] == inft()) return false;
	}
	//recherche de d dans 2^s * d + i = *this
	inft d = *this - 1;
	inft s = inft(0);
	while (!d.isImpair())
	{
		d = d.half();
		s = s + 1;
	}

	//calcul this-1
	inft nM1 = *this - inft(1);

	//test assurément vrai
	if (precision == 0)
	{
		for (unsigned int a = 2; a < nbrLong * nbrLong * 8; a++)
		{
			//test de miller

			// a^d%n
			inft x = inft(a).modPow(d, *this);

			//test
			if (x == inft(1) || x == nM1) goto  endTest;
			for (inft k(1); k < s; k = k + 1)
			{
				x = (x * x) % *this;
				d = d.dbl();

				if (x == inft(1)) return false;
				if (x == nM1) goto  endTest;
			}
			return false;
		endTest:;
		}

		return true;
	}


	//test probabiliste
	for (int i = 0; i < precision; i++)
	{
		//test de miller
		//génération d'une valeur de test
		size_t nbrLongA = nbrLong + 1;
		unsigned long* cptrA = new unsigned long[nbrLongA];
		for (int j = 0; j < nbrLongA; j++)cptrA[j] = randomMachine();
		inft a = (inft(2) + inft(cptrA, nbrLongA)) % (*this - inft(4));
		delete[] cptrA;

		// a^d%n
		inft x = a.modPow(d, *this);

		//test
		if (x == inft(1) || x == nM1) goto  endTestP;
		for (inft k(1); k < s; k = k + 1)
		{
			x = (x * x) % *this;
			d = d.dbl();

			if (x == inft(1)) return false;
			if (x == nM1) goto  endTestP;
		}
		return false;
	endTestP:;
	}

	return true;

}


inft randPrime(inft min, inft max)
{
	std::random_device randomMachine;
	inft terrain = (max + 1) - min;
	size_t nbrLongA = terrain.size() + 1;
	inft a;

	unsigned long* cptrA = new unsigned long[nbrLongA];
	while (true)
	{
		//génération d'un nombre aléatoire
		for (int j = 0; j < nbrLongA; j++)cptrA[j] = randomMachine();
		a = (inft(cptrA, nbrLongA) % (terrain)) + min;
		if (!a.isImpair())
		{
			--a;
			if (a < min)
			{
				a = a + 2;
				if (a > max) throw;
			}
		}
		if (a.isPrime(10)) break;
	}
	delete[] cptrA;
	return a;
}

inft randinft(inft min, inft max)
{
	std::random_device randomMachine;
	inft terrain = (max + 1) - min;
	size_t nbrLongA = terrain.size() + 1;
	//génération d'un nombre aléatoire
	unsigned long* cptrA = new unsigned long[nbrLongA];
	for (int j = 0; j < nbrLongA; j++)cptrA[j] = randomMachine();
	inft a = (inft(cptrA, nbrLongA) % (terrain)) + min;
	delete[] cptrA;
	return a;
}

inft gdc(inft a, inft b)
{
	//algorythme Euclidienne
	while (!(b == inft()))
	{
		// gdc(a,b) = gdc(b, a%b);
		inft buffer = b;
		b = a % b;
		a = buffer;
	}
	return a;//gdc(a,0) = a
}

inft extendedGdc(inft a, inft b, inft& x, inft& y)
{
	inft s = 0, old_s = 1, t = 1, old_t = 0, r = b, old_r = a;
	while (!(r == inft()))
	{
		inft q = old_r / r;

		inft buffer = r;
		r = old_r - q * r;
		old_r = buffer;

		buffer = s;
		s = old_s - q * s;
		old_s = buffer;

		buffer = t;
		t = old_t - q * t;
		old_t = buffer;

	}

	x = old_s;
	y = old_t;
	if (!((x * a + y * b) == old_r))throw "algorythme euclide non fonctionnel";
	return old_r;
}

inft invMod(inft a, inft m)
{
	inft x;
	inft y;
	if (!(extendedGdc(a, m, x, y) == inft(1)))throw "a et m non premier entre eux";
	if (x < 0) x = x + m;
	return x;
}

//util
inline void inft::swap(inft& val)
{
	std::swap(negatif, val.negatif);
	std::swap(nbrLong, val.nbrLong);
	std::swap(valeur, val.valeur);

}