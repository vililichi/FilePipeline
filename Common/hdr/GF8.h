#pragma once
class GF8
{
public:
	unsigned char value;

	//constructeur
	GF8();
	GF8(unsigned char);

	//opérateur d'égalité
	GF8& operator = (const unsigned char c) { value = c; return *this; }

	//opérateur arytmétique
	const GF8 operator + (const GF8 o) { return value ^ o.value; }
	GF8& operator += (const GF8 o) { value = value ^ o.value;  return *this; }

	const GF8 operator * (const GF8 o); //multiplaction
	const GF8 operator *= (const GF8 o) { value = (*this * o).value; return *this; } //multiplaction
	const GF8 operator ! ();	//obtention de l'inverse tel que B*B! = 1;

};

