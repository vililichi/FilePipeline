#include "inft.h"
#include "primeList.h"
#include <random>
#include <algorithm>


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

//op�rateur d'assignation

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


//pour d�bugage
std::ostream& operator << (std::ostream& os, const inft& val)
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

//arithm�tique
const inft inft::operator + (const inft& val) const
{
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
		unsigned long* rep = new unsigned long [big->size()];

		//soustraction
		long long buffer = 0;
		for (size_t i = 0; i < big->size(); i++)
		{

			buffer += big->valeur[i];
			if (i < small->size()) buffer -= small->valeur[i];
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

		inft out(rep, big->size(), big->isNegatif());
		delete[] rep;
		return out;

	}

	//positif + positif ou negatif + negatif
	else
	{
		//mesure de la taille des valeurs
		size_t max = val.nbrLong;
		if (nbrLong > val.nbrLong) max = nbrLong;

		//cr�ation de l'espace n�cessaire
		unsigned long* cptr = new unsigned long[max + 1];

		unsigned long long buffer = 0;
		for (size_t i = 0; i < max + 1; i++)
		{
			buffer = buffer >> 32;
			if (i < nbrLong) buffer += valeur[i];
			if (i < val.nbrLong) buffer += val.valeur[i];
			cptr[i] = *(unsigned long*)&buffer;
		}

		inft out(cptr, max + 1, negatif);

		delete[] cptr;
		return out;
	}
}

const inft inft::operator - (const inft& val) const
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

const inft inft::operator * (const inft& val) const
{
	unsigned long* rep = new unsigned long[nbrLong + val.nbrLong];

	for (size_t i = 0; i < nbrLong + val.nbrLong; i++) rep[i] = 0;

	for (size_t i = 0; i < nbrLong; i++)
		for (size_t j = 0; j < val.nbrLong; j++)
		{
			unsigned long long produit = (unsigned long long)valeur[i] * val.valeur[j];

			size_t k = 0;
			while (produit != 0)
			{
				produit += rep[i + j + k];
				rep[i + j + k] = *(unsigned long*)&produit;
				produit = produit >> 32;
				k++;
			}
		}

	inft out(rep, (nbrLong + val.nbrLong), isNegatif() ^ val.isNegatif());

	delete[] rep;
	return out;
}

const inft inft::operator / (const inft& val) const
{

	if (abs() < val.abs()) return inft(); //r�ponse triviale

	//assignation de l'espace
	inft cpy((*this).abs());
	

	//taille de la r�ponse
	size_t sizeThis = nbrLong << 1;
	if (((unsigned short*)valeur)[sizeThis - 1] == 0)sizeThis--;



	//d�nominateur raccourcie
	size_t sizeDenom = val.nbrLong << 1;
	unsigned short* shortDenom = (unsigned short*)val.valeur;
	if (shortDenom[sizeDenom - 1] == 0)sizeDenom--;
	long long  denom = (unsigned long long)shortDenom[sizeDenom - 1] <<16;
	if (sizeDenom >= 2) denom += shortDenom[sizeDenom - 2];

	//assignation de l'espace
	size_t diffS = (sizeThis - sizeDenom) + 1;
	unsigned short* cptr = new unsigned short[diffS];	//char* contenant la r�ponse

	//Pour chaqu'un des �l�ments de la division
	for (size_t i = diffS, j = 0; i > 0;)
	{
		//d�cr�mentation
		i--;
		//num�rateur raccourcie
		size_t sizeNum = cpy.nbrLong << 1;
		unsigned short* shortNum = (unsigned short*)cpy.valeur;
		unsigned long long num = 0;
		if (sizeThis - j - 1 < sizeNum)	num += (unsigned long long)shortNum[sizeThis - j - 1] <<16;
		if (sizeThis - j < sizeNum)		num += (unsigned long long)shortNum[sizeThis - j] <<32;
		if (sizeDenom >= 2 && sizeThis - j - 2 < sizeNum)	num += shortNum[sizeThis - j - 2];

		//estimation
		unsigned long long ps = (num / denom) - 1;

		//valeur r�elle
		if (ps >= 0)
		{
			//cr�ation du facteur de test
			unsigned short* facptr;
			size_t tailleFact = sizeDenom + i;
			facptr = new unsigned short[sizeDenom + i];
			for (size_t k = 0; k < i; k++) facptr[k] = 0;
			for (size_t k = 0; k < sizeDenom; k++)facptr[k + i] = shortDenom[k];
			
			inft fact(facptr, tailleFact);
			delete[] facptr;

			//test avec pr�diction
			cpy = cpy - (inft( (long)ps ) * fact);

			//test exterieur � prediction
			inft limit = cpy - fact;
			if (!limit.isNegatif())
			{
				cpy = limit;
				ps++;
			}
			cptr[i] = (unsigned short)ps;
		}
		else { cptr[i] = 0; }

		//incr�mentation
		j++;
	}

	inft out(cptr, diffS, isNegatif() ^ val.isNegatif());
	delete[] cptr;
	return out;
}
const inft inft::operator % (const inft& val) const
{
	if (abs() < val.abs()) return *this; //r�ponse triviale
		//assignation de l'espace
		inft cpy((*this).abs());

		//taille de la r�ponse
		size_t sizeThis = nbrLong << 1;
		if (((unsigned short*)valeur)[sizeThis - 1] == 0)sizeThis--;
		

		//d�nominateur raccourcie
		size_t sizeDenom = val.nbrLong << 1;
		unsigned short* shortDenom = (unsigned short*)val.valeur;
		if (shortDenom[sizeDenom - 1] == 0)sizeDenom--;
		long long  denom = (unsigned long long)shortDenom[sizeDenom - 1] << 16;
		if (sizeDenom >= 2) denom += shortDenom[sizeDenom - 2];

		//taille reponse
		size_t diffS = sizeThis - sizeDenom + 1;

		//Pour chaqu'un des �l�ments de la division
		for (size_t i = diffS, j = 0; i > 0;)
		{
			//d�cr�mentation
			i--;
			//num�rateur raccourcie
			size_t sizeNum = cpy.nbrLong << 1;
			unsigned short* shortNum = (unsigned short*)cpy.valeur ;
			unsigned long long num = 0;
			if (sizeThis - j - 1 < sizeNum)	num += (unsigned long long)shortNum[sizeThis - j - 1] << 16;
			if (sizeThis - j < sizeNum)		num += (unsigned long long)shortNum[sizeThis - j] << 32;
			if (sizeDenom >= 2 && sizeThis - j - 2 < sizeNum)	num += shortNum[sizeThis - j - 2];

			//estimation
			unsigned long long ps = (num / denom) - 1;

			//valeur r�elle
			if (ps >= 0)
			{
				//cr�ation du facteur de test
				unsigned short* facptr;
				size_t tailleFact = sizeDenom + i;
				facptr = new unsigned short[sizeDenom + i];
				for (size_t k = 0; k < i; k++) facptr[k] = 0;
				for (size_t k = 0; k < sizeDenom; k++)facptr[k + i] = shortDenom[k];

				inft fact(facptr, tailleFact);
				delete[] facptr;

				//test avec pr�diction
				cpy = cpy - (inft((long)ps) * fact);

				//test exterieur � prediction
				inft limit = cpy - fact;
				if (!limit.isNegatif())
				{
					cpy = limit;
				}
			}

			//incr�mentation
			j++;
		}
	
	cpy.negatif = negatif;
	return cpy;
}
const inft inft::half() const
{
	//assignation de l'espace
	unsigned long* cptr = new unsigned long[nbrLong];	//char* contenant la r�ponse

	//Pour chaqu'un des �l�ments de la r�ponse
	for (size_t i = 0 ; i < nbrLong- 1; i++) cptr[i] = valeur[i+1] << 31 | valeur[i] >> 1;
	cptr[nbrLong - 1] = valeur[nbrLong - 1] >> 1;

	inft out(cptr, nbrLong, isNegatif());
	delete[] cptr;
	return out;
}
const inft& inft::halfThis()
{
	for (size_t i = 0; i < nbrLong - 1; i++)valeur[i] = valeur[i + 1] << 31 | valeur[i] >> 1;
	valeur[nbrLong - 1] = valeur[nbrLong - 1] >> 1;
	cut();
	return *this;
}
const inft inft::dbl() const
{
	//assignation de l'espace
	unsigned long* cptr = new unsigned long[nbrLong+1];	//char* contenant la r�ponse

	//Pour chaqu'un des �l�ments de la r�ponse
	cptr[0] = valeur[0] << 1;
	for (size_t i = 1; i < nbrLong ; i++) cptr[i] = valeur[i] << 1 | valeur[i-1] >> 31;
	cptr[nbrLong] = valeur[nbrLong - 1] >> 31;

	inft out(cptr, nbrLong+1, isNegatif());
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
		}

		driver = driver * driver;
		val = val.half();
		
	}
	return out;
}
const inft inft::modPow(inft exposant, const inft& modulo) const {
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
	//pr�test
	if (*this == inft(2) || *this == inft(3))return true;
	if ((!isImpair()) || *this <= inft(1))return false;
	for (int i = 0; i < 256; i++)
	{
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

	//test assur�ment vrai
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
		//g�n�ration d'une valeur de test
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


	while (true)
	{
		//g�n�ration d'un nombre al�atoire
		unsigned long* cptrA = new unsigned long[nbrLongA];
		for (int j = 0; j < nbrLongA; j++)cptrA[j] = randomMachine();
		a = (inft(cptrA, nbrLongA) % (terrain)) + min;
		delete[] cptrA;
		if (a.isPrime(10)) break;
	}

	return a;
}

inft randinft(inft min, inft max)
{
	std::random_device randomMachine;
	inft terrain = (max + 1) - min;
	size_t nbrLongA = terrain.size() + 1;
	//g�n�ration d'un nombre al�atoire
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