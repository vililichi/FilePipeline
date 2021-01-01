#pragma once
#include <string>

#define INIT_PACKET_SIZE 512 

class Packet
{
protected:

	// gestion de la taille et de la capacit�
	size_t _capacity;
	size_t _size;
	size_t _cursor;

	// donn�es
	char* _data;

public:

	//constructeur et destructeur
	Packet(size_t beginCapacity = INIT_PACKET_SIZE); //constructeur avec capacit� de d�part
	Packet(const Packet& base);
	~Packet();

	//modification de la taille des donn�es
	void setCapacity(size_t newCapacity); //change la capacit� du packet

	//lecture des attributs
	const size_t size() { return _size; }
	const size_t capacity() { return _capacity; }
	const size_t cursor() { return _cursor; }
	char* data() { return _data; }

	//navigation dans les donn�es
	const bool end() { return _cursor == _size; } //retourne vrai si le curseur est � la fin des donn�es
	void move(size_t);//d�place le curseur

	//modification
	void add(char* newData, size_t dataSize); //ajoute les donn�es � partir de la position du curseur et d�place le curseur � la fin des donn�es
	void popBack(size_t nbr);//retire les nbr dernier data du packet
	void read(char* data, size_t readSize); // lit un nombre d�terminer de donn� et d�place le curseur


	//operateur pour �criture

	Packet& operator = (const Packet& _b);

	template <typename T>
	Packet& operator << (T data);
	template <>
	Packet& operator << (std::string data);


	//Packet& operator << (float data);

	//operateur pour lecture

	template <typename T>
	Packet& operator >> (T& data);
	template <>
	Packet& operator >> (std::string& data);

};

#pragma region << operator
template <typename T>
inline Packet& Packet::operator << (T data)
{
	const size_t bytesNbr = sizeof(T);

	char* cdata = (char*)&data;
	add(cdata, bytesNbr);
	return *this;
}

template <>
inline Packet& Packet::operator << (std::string data)
{
	const size_t bytesNbr = data.size() + 1;

	char* cdata = (char*)data.c_str();

	add(cdata, bytesNbr);
	return *this;
}

#pragma endregion

#pragma region >> operator

template <typename T>
inline Packet& Packet::operator >> (T& data)
{
	const size_t bytesNbr = sizeof(T);
	if ((_cursor + bytesNbr) > _size)throw "depassement lors de la lecture";

	data = *(T*)(_data + _cursor);
	_cursor += bytesNbr;
	return *this;
}

template <>
inline Packet& Packet::operator >> (std::string& data)
{
	size_t bytesNbr = 0;
	const size_t virtSize = _size - 1;
	while (*(_data + _cursor + bytesNbr) != 0)
	{
		bytesNbr++;
		if ((_cursor + bytesNbr) > virtSize)throw "depassement lors de la lecture";
	}

	data = std::string(_data + _cursor);
	_cursor += bytesNbr + 1;
	return *this;
}

#pragma endregion
