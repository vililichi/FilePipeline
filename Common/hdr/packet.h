#pragma once
#include <string>

namespace PacketConst
{
	const static size_t MAXSIZE = 1454;
}

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
	Packet(size_t beginCapacity = PacketConst::MAXSIZE); //constructeur avec capacit� de d�part
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

