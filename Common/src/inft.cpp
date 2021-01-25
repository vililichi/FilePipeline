#include "inft.h"
#include "primeList.h"
#include <random>

// 64bit

#if _WIN64 || __x86_64__ || __ppc64__
#define ENVIRONMENT64
#endif

//utile
unsigned long* getIntInft(const inft& val, size_t& taille)
{
	
	if (val.size() % sizeof(long) == 0)
	{
		taille = val.size() / sizeof(long);
	}
	else
	{
		taille = val.size() / sizeof(long) + 1;
	}
	unsigned long* retour = new unsigned long [taille];
	retour[taille - 1] = 0;
	unsigned char* cptr = (unsigned char*)retour;
	for (int i = 0; i < val.size(); i++)cptr[i] = val[i];

	return retour;
}

unsigned short* getShortInft(const inft& val, size_t& taille)
{

	if (val.size() % sizeof(short) == 0)
	{
		taille = val.size() / sizeof(short);
	}
	else
	{
		taille = val.size() / sizeof(short) + 1;
	}
	unsigned short* retour = new unsigned short[taille];
	retour[taille - 1] = 0;
	unsigned char* cptr = (unsigned char*)retour;
	for (int i = 0; i < val.size(); i++)cptr[i] = val[i];

	return retour;
}

void inft::cut()
{
	size_t i = nbrOctet - 1;
	while (i > 0)
	{
		if (valeur[i] == 0)
		{
			i--;
			nbrOctet -= 1;
		}
		else break;
	}
}
//constructeur et destructeur
inft::inft() : nbrOctet(1), negatif(false)
{
	valeur = new unsigned char[nbrOctet];
	valeur[0] = 0;
}
inft::inft(unsigned char* _valeur, size_t _nbrOctet, bool _negatif) : nbrOctet(_nbrOctet), negatif(_negatif)
{
	valeur = new unsigned char[nbrOctet];
	for (size_t i = 0; i < nbrOctet; i++)
	{
		valeur[i] = _valeur[i];
	}
	cut();
}
inft::inft(const inft& val)
{
	nbrOctet = val.nbrOctet;

	valeur = new unsigned char[nbrOctet];

	for (size_t i = 0; i < nbrOctet; i++)
	{
		valeur[i] = val.valeur[i];
	}

	negatif = val.negatif;
	cut();
}
inft::inft(const unsigned int val)
{
	nbrOctet = sizeof(const unsigned long);
	unsigned char* cptr = (unsigned char*)&val;

	valeur = new unsigned char[nbrOctet];

	for (size_t i = 0; i < nbrOctet; i++)
	{
		valeur[i] = cptr[i];
	}

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

	nbrOctet = sizeof(const unsigned long);
	unsigned char* cptr = (unsigned char*)&mval;

	valeur = new unsigned char[nbrOctet];

	for (size_t i = 0; i < nbrOctet; i++)
	{
		valeur[i] = cptr[i];
	}
	cut();
}

inft::~inft()
{
	delete[] valeur;
}

//opérateur d'assignation

inft& inft::operator = (const inft val)
{
	nbrOctet = val.nbrOctet;

	delete[] valeur;
	valeur = new unsigned char[nbrOctet];

	for (size_t i = 0; i < nbrOctet; i++)
	{
		valeur[i] = val.valeur[i];
	}

	negatif = val.negatif;

	cut();
	return *this;
}
inft& inft::operator = (const unsigned int val)
{
	nbrOctet = sizeof(const unsigned int);
	unsigned char* cptr = (unsigned char*)&val;

	delete[] valeur;
	valeur = new unsigned char[nbrOctet];

	for (size_t i = 0; i < nbrOctet; i++)
	{
		valeur[i] = cptr[i];
	}

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

	nbrOctet = sizeof(const int);
	unsigned char* cptr = (unsigned char*)&mval;

	delete[] valeur;
	valeur = new unsigned char[nbrOctet];

	for (size_t i = 0; i < nbrOctet; i++)
	{
		valeur[i] = cptr[i];
	}

	cut();
	return *this;

}


//pour débugage
std::ostream& operator << (std::ostream& os, const inft val)
{
	if (val.isNegatif()) os << '-';
	os << " ";
	inft cpy = val;
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
		os << (short)aff[0];

	}
	return os;
}

//operateur de comparaison
const bool inft::operator < (const inft val) const
{
	if (negatif && !val.negatif)return true;
	else if (val.negatif && !negatif) return false;
	else if (!negatif)
	{
		if (nbrOctet < val.nbrOctet) return true;
		else if (nbrOctet > val.nbrOctet) return false;
		else for (size_t i = nbrOctet; i > 0; )
		{
			i--;
			if (valeur[i] < val.valeur[i]) return true;
			else if (valeur[i] > val.valeur[i]) return false;
		}
	}
	else
	{
		if (nbrOctet > val.nbrOctet) return true;
		else if (nbrOctet < val.nbrOctet) return false;
		else for (size_t i = nbrOctet; i > 0; )
		{
			i--;
			if (valeur[i] > val.valeur[i]) return true;
			else if (valeur[i] < val.valeur[i]) return false;
		}
	}
	return false;
}
const bool inft::operator > (const inft val) const
{
	if (negatif && !val.negatif)return false;
	else if (val.negatif && !negatif) return true;
	else if (negatif)
	{
		if (nbrOctet < val.nbrOctet) return true;
		else if (nbrOctet > val.nbrOctet) return false;
		else for (size_t i = nbrOctet; i > 0; )
		{
			i--;
			if (valeur[i] < val.valeur[i]) return true;
			else if (valeur[i] > val.valeur[i]) return false;
		}
	}
	else
	{
		if (nbrOctet > val.nbrOctet) return true;
		else if (nbrOctet < val.nbrOctet) return false;
		else for (size_t i = nbrOctet; i > 0; )
		{
			i--;
			if (valeur[i] > val.valeur[i]) return true;
			else if (valeur[i] < val.valeur[i]) return false;
		}
	}
	return false;
}
const bool inft::operator == (const inft val) const
{
	if (negatif != val.negatif) return false;
	if (nbrOctet != val.nbrOctet)return false;
	for (size_t i = 0; i < nbrOctet; i++) if (valeur[i] != val.valeur[i]) return false;
	return true;
}
const bool inft::operator <= (const inft val) const
{
	return !operator > (val);
}
const bool inft::operator >= (const inft val) const
{
	return !operator < (val);
}

//arithmétique
const inft inft::operator + (const inft val) const
{
#ifdef ENVIRONMENT64
	//positif + negatif ou negatif + positif
	if (isNegatif() != val.isNegatif())
	{
		//comparaison de taille
		const inft* big = NULL;
		const inft* small = NULL;
		inft absThis = abs();
		inft absVal = val.abs();
		if (absThis == absVal) return inft();
		else if (absThis > absVal) { big = this; small = &val; }
		else { big = &val; small = this; }

		//transformation int
		size_t sizeBig;
		size_t sizeSmall;
		unsigned long* bigInt = getIntInft(*big, sizeBig);
		unsigned long* smallInt = getIntInft(*small, sizeSmall);
		unsigned long* rep = new unsigned long [sizeBig];

		//soustraction
		long long buffer = 0;
		for (size_t i = 0; i < sizeBig; i++)
		{

			buffer += bigInt[i];
			if (i < sizeSmall) buffer -= smallInt[i];
			if (buffer < 0) 
			{ 
				buffer += 4294967296; 
				rep[i] = *(unsigned long*)&buffer;
				buffer = -1; 
			}
			else
			{
				rep[i] = *(unsigned long*)&buffer;
				buffer = 0;
			}
			
		}

		delete[] bigInt;
		delete[] smallInt;
		inft out((unsigned char*)rep, sizeBig*sizeof(long), big->isNegatif());
		delete[] rep;
		return out;

	}

	//positif + positif ou negatif + negatif
	else
	{
		//mesure de la taille des valeurs
		size_t max = val.nbrOctet;
		if (nbrOctet > val.nbrOctet) max = nbrOctet;

		//création de l'espace nécessaire
		unsigned char* cptr = new unsigned char[max + 1];

		unsigned short buffer = 0;
		for (size_t i = 0; i < max + 1; i++)
		{
			buffer = buffer >> 8;
			if (i < nbrOctet) buffer += valeur[i];
			if (i < val.nbrOctet) buffer += val.valeur[i];
			cptr[i] = *(char*)&buffer;
		}

		inft out(cptr, max + 1, negatif);

		delete[] cptr;
		return out;
	}

	
#else
	//mesure de la taille des valeurs
	size_t max = val.nbrOctet;
	if (nbrOctet > val.nbrOctet) max = nbrOctet;

	//création de l'espace nécessaire
	unsigned char* cptr = new unsigned char[max + 1];
	bool onegatif = 0;

	//positif + negatif ou negatif + positif
	if (isNegatif() != val.isNegatif())
	{
		//comparaison de taille
		const inft* big = NULL;
		const inft* small = NULL;
		inft absThis = abs();
		inft absVal = val.abs();
		if (absThis == absVal) return inft();
		else if (absThis > absVal) { big = this; small = &val; }
		else { big = &val; small = this; }

		//soustraction
		short buffer = 0;
		short buffer2 = 0;
		for (size_t i = 0; i < max + 1; i++)
		{
			buffer = buffer2;
			if (i < (*big).nbrOctet) buffer += (*big)[i];
			if (i < (*small).nbrOctet) buffer -= (*small)[i];
			if (buffer < 0) { buffer += 256; buffer2 = -1; }
			else buffer2 = 0;
			cptr[i] = *(char*)&buffer;
		}
		onegatif = big->isNegatif();

	}

	//positif + positif ou negatif + negatif
	else
	{
		unsigned short buffer = 0;
		for (size_t i = 0; i < max + 1; i++)
		{
			buffer = buffer >> 8;
			if (i < nbrOctet) buffer += valeur[i];
			if (i < val.nbrOctet) buffer += val.valeur[i];
			cptr[i] = *(char*)&buffer;
		}
		onegatif = negatif;
	}

	inft out(cptr, max + 1, onegatif);

	delete[] cptr;
	return out;
#endif
}

const inft inft::operator - (const inft val) const
{
	inft nval(val);
	nval.negatif = !nval.negatif;
	return operator + (nval);
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
				valeur[i] = 255;
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

const inft inft::operator * (const inft val) const
{

size_t tailleA;
size_t tailleB;
#ifdef ENVIRONMENT64
	unsigned long* longA = getIntInft(*this, tailleA);
	unsigned long* longB = getIntInft(val, tailleB);

	unsigned long* rep = new unsigned long[tailleA + tailleB];

	for (size_t i = 0; i < tailleA + tailleB; i++) rep[i] = 0;

	for (size_t i = 0; i < tailleA; i++)
		for (size_t j = 0; j < tailleB; j++)
		{
			unsigned long long produit = (unsigned long long)longA[i] * longB[j];

			size_t k = 0;
			while (produit != 0)
			{
				produit += rep[i + j + k];
				rep[i + j + k] = *(unsigned long*)&produit;
				produit = produit >> 32;
				k++;
			}
		}

	inft out((unsigned char*)rep, (tailleA + tailleB) * sizeof(long), isNegatif() ^ val.isNegatif());

	delete[] longA;
	delete[] longB;
	delete[] rep;
	return out;
#else
	unsigned short* shortA = (unsigned short*)getShortInft(*this, tailleA);
	unsigned short* shortB = (unsigned short*)getShortInft(val, tailleB);
	unsigned short* rep = new unsigned short[tailleA + tailleB];

	for (size_t i = 0; i < tailleA + tailleB; i++) rep[i] = 0;

	for (size_t i = 0; i < tailleA; i++)
		for (size_t j = 0; j < tailleB; j++)
		{
			unsigned long produit = (unsigned long)shortA[i] * shortB[j];

			size_t k = 0;
			while (produit != 0)
			{
				produit += rep[i + j + k];
				rep[i + j + k] = *(unsigned short*)&produit;
				produit = produit >> 16;
				k++;
			}
		}

	inft out((unsigned char*)rep, (tailleA + tailleB) * sizeof(short), isNegatif() ^ val.isNegatif());

	delete[] shortA;
	delete[] shortB;
	delete[] rep;
	return out;
#endif
}

const inft inft::operator / (const inft val) const
{
	if (abs() < val.abs()) return inft(); //réponse triviale

	//taille de la réponse
	size_t diffS = nbrOctet - val.nbrOctet + 1;

	//assignation de l'espace
	unsigned char* cptr = new unsigned char[diffS];	//char* contenant la réponse
	inft cpy((*this).abs());						//copy de la valeur absolue du numérateur

	//dénominateur racourcie
	long denom = val.valeur[val.nbrOctet - 1] * 256;
	if (val.nbrOctet >= 2) denom += val.valeur[val.nbrOctet - 2];

	//Pour chaqu'un des éléments de la réponse
	for (size_t i = diffS, j = 0; i > 0;)
	{
		//décrémentation
		i--;

		//numérateur racourcie
		long num = 0;
		if (nbrOctet - j - 1 < cpy.nbrOctet)num += (long)cpy.valeur[nbrOctet - j - 1] * 256;
		if (nbrOctet - j < cpy.nbrOctet) num += (long)cpy.valeur[nbrOctet - j] * 65536;
		if (val.nbrOctet >= 2 && nbrOctet - j - 2 < cpy.nbrOctet) num += cpy.valeur[nbrOctet - j - 2];

		//première estimation
		int ps = (num / denom) - 1;

		//valeur réelle
		if (ps >= 0)
		{
			//création du facteur de test
			unsigned char* facptr = new unsigned char[val.nbrOctet + i];
			for (size_t k = 0; k < i; k++) facptr[k] = 0;
			for (size_t k = 0; k < val.nbrOctet; k++)facptr[k + i] = val.valeur[k];
			inft fact(facptr, val.nbrOctet + i);
			delete[] facptr;

			//test avec prédiction
			cpy = cpy - (inft(ps) * fact);

			//test exterieur à prediction
			inft limit = cpy - fact;
			if (!limit.isNegatif())
			{
				cpy = limit;
				limit = limit - fact;
				ps++;
			}
			cptr[i] = ps;
		}
		else { cptr[i] = 0; }

		//incrémentation
		j++;
	}

	inft out(cptr, diffS, isNegatif() ^ val.isNegatif());
	delete[] cptr;
	return out;
}
const inft inft::operator % (const inft val) const
{
	if (abs() < val.abs()) return *this; //réponse triviale
		//assignation de l'espace
		inft cpy((*this).abs());

	#ifdef ENVIRONMENT64
		//taille de la réponse
		size_t sizeThis;
		delete[] getShortInft(*this, sizeThis);
		

		//dénominateur raccourcie
		size_t sizeDenom;
		unsigned short* shortDenom = getShortInft(val, sizeDenom);
		long long  denom = (unsigned long long)shortDenom[sizeDenom - 1] * 65536;
		if (sizeDenom >= 2) denom += shortDenom[sizeDenom - 2];

		//taille reponse
		size_t diffS = sizeThis - sizeDenom + 1;

		//Pour chaqu'un des éléments de la division
		for (size_t i = diffS, j = 0; i > 0;)
		{
			//décrémentation
			i--;
			//numérateur raccourcie
			size_t sizeNum;
			unsigned short* shortNum = getShortInft(cpy, sizeNum);
			unsigned long long num = 0;
			if (sizeThis - j - 1 < sizeNum)	num += (unsigned long long)shortNum[sizeThis - j - 1] * 65536;
			if (sizeThis - j < sizeNum)		num += (unsigned long long)shortNum[sizeThis - j] * 4294967296;
			if (val.nbrOctet >= 2 && sizeThis - j - 2 < sizeNum)	num += shortNum[sizeThis - j - 2];
			delete[] shortNum;

			//estimation
			unsigned long long ps = (num / denom) - 1;

			//valeur réelle
			if (ps >= 0)
			{
				//création du facteur de test
				unsigned short* facptr = new unsigned short[sizeDenom + i];
				for (size_t k = 0; k < i; k++) facptr[k] = 0;
				for (size_t k = 0; k < sizeDenom; k++)facptr[k + i] = shortDenom[k];
				inft fact((unsigned char*)facptr, (sizeDenom + i) * sizeof(short));
				delete[] facptr;

				//test avec prédiction
				cpy = cpy - (inft((long)ps) * fact);

				//test exterieur à prediction
				inft limit = cpy - fact;
				if (!limit.isNegatif())
				{
					cpy = limit;
				}
			}

			//incrémentation
			j++;
		}
		delete[] shortDenom;
	#else
		//taille de la réponse
		size_t diffS = nbrOctet - val.nbrOctet + 1;

		//dénominateur raccourcie
		long denom = val.valeur[val.nbrOctet - 1] * 256;
		if (val.nbrOctet >= 2) denom += val.valeur[val.nbrOctet - 2];

		//Pour chaqu'un des éléments de la division
		for (size_t i = diffS, j = 0; i > 0;)
		{
			//décrémentation
			i--;
			//numérateur raccourcie
			unsigned long num = 0;
			if (nbrOctet - j - 1 < cpy.nbrOctet)	num += (unsigned long)cpy.valeur[nbrOctet - j - 1] * 256;
			if (nbrOctet - j < cpy.nbrOctet)		num += (unsigned long)cpy.valeur[nbrOctet - j] * 65536;
			if (val.nbrOctet >= 2 && nbrOctet - j - 2 < cpy.nbrOctet) num += cpy.valeur[nbrOctet - j - 2];

			//estimation
			int ps = (num / denom) - 1;

			//valeur réelle
			if (ps >= 0)
			{
				//création du facteur de test
				unsigned char* facptr = new unsigned char[val.nbrOctet + i];
				for (size_t k = 0; k < i; k++) facptr[k] = 0;
				for (size_t k = 0; k < val.nbrOctet; k++)facptr[k + i] = val.valeur[k];
				inft fact(facptr, val.nbrOctet + i);
				delete[] facptr;
				//test avec prédiction
				cpy = cpy - (inft(ps) * fact);
				//test exterieur à prediction
				inft limit = cpy - fact;
				if (!limit.isNegatif())
				{
					cpy = limit;
				}
			}

			//incrémentation
			j++;
		}
	#endif
	cpy.negatif = negatif;
	return cpy;
}
const inft inft::half() const
{
	//assignation de l'espace
	unsigned char* cptr = new unsigned char[nbrOctet];	//char* contenant la réponse

	//Pour chaqu'un des éléments de la réponse
	for (size_t i = 0 ; i < nbrOctet - 1; i++) cptr[i] = valeur[i+1] << 7 | valeur[i] >> 1;
	cptr[nbrOctet - 1] = valeur[nbrOctet - 1] >> 1;

	inft out(cptr, nbrOctet, isNegatif());
	delete[] cptr;
	return out;
}
const inft inft::dbl() const
{
	//assignation de l'espace
	unsigned char* cptr = new unsigned char[nbrOctet+1];	//char* contenant la réponse

	//Pour chaqu'un des éléments de la réponse
	cptr[0] = valeur[0] << 1;
	for (size_t i = 1; i < nbrOctet ; i++) cptr[i] = valeur[i] << 1 | valeur[i-1] >> 7;
	cptr[nbrOctet] = valeur[nbrOctet - 1] >> 7;

	inft out(cptr, nbrOctet+1, isNegatif());
	delete[] cptr;
	return out;
}

//puissance
const inft inft::pow(inft val) const {
	inft out(1);
	inft driver = *this;
	while (val > 0)
	{
		if (val.isImpair())
		{
			out = out * driver;
			val = val - 1;
		}

		else
		{
			driver = driver * driver;
			val = val.half();
		}
	}
	return out;
}
const inft inft::modPow(inft exposant, inft modulo) const {
	inft out(1);
	inft driver = *this % modulo;
	while (exposant > 0)
	{
		if (exposant.isImpair())
		{
			out = (out * driver)% modulo;
			--exposant;
		}

		else
		{
			driver = (driver * driver)% modulo;
			exposant = exposant.half();
		}
	}
	return out;
}

bool inft::isPrime(int precision = 0) const
{
	std::random_device randomMachine;
	//prétest
	if (*this == inft(2) || *this == inft(3))return true;
	if ((!isImpair()) || *this <= inft(1))return false;
	for (int i = 0; i < 256; i++)
	{
		if (*this % fistPrime[i] == 0) return false;
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
		for (unsigned int a = 2; a < nbrOctet * nbrOctet * 2; a++)
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
		size_t nbrOctetsA = nbrOctet + 1;
		unsigned char* cptrA = new unsigned char[nbrOctetsA];
		for (int j = 0; j < nbrOctetsA; j++)cptrA[j] = randomMachine();
		inft a = (inft(2) + inft(cptrA, nbrOctetsA)) % (*this - inft(4));
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
	size_t nbrOctetsA = terrain.size() + 1;
	inft a;


	while (true)
	{
		//génération d'un nombre aléatoire
		unsigned char* cptrA = new unsigned char[nbrOctetsA];
		for (int j = 0; j < nbrOctetsA; j++)cptrA[j] = randomMachine();
		a = (inft(cptrA, nbrOctetsA) % (terrain)) + min;
		delete[] cptrA;
		if (a.isPrime(10)) break;
	}

	return a;
}

inft randinft(inft min, inft max)
{
	std::random_device randomMachine;
	inft terrain = (max + 1) - min;
	size_t nbrOctetsA = terrain.size() + 1;
	//génération d'un nombre aléatoire
	unsigned char* cptrA = new unsigned char[nbrOctetsA];
	for (int j = 0; j < nbrOctetsA; j++)cptrA[j] = randomMachine();
	inft a = (inft(cptrA, nbrOctetsA) % (terrain)) + min;
	delete[] cptrA;
	return a;
}

inft gdc(inft a, inft b)
{
	//algorythme Euclidienne
	while (!(b == 0))
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
	while (!(r == 0))
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
	if (!(extendedGdc(a, m, x, y) == 1))throw "a et m non premier entre eux";
	if (x < 0) x = x + m;
	return x;
}