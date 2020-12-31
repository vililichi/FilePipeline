#pragma once
#include <string>

namespace PacketConst
{
	const static size_t MAXSIZE = 1454;
}

class Packet
{
protected:

	// gestion de la taille et de la capacité
	size_t _capacity;
	size_t _size;
	size_t _cursor;

	// données
	char* _data;

public:

	//constructeur et destructeur
	Packet(size_t beginCapacity = PacketConst::MAXSIZE); //constructeur avec capacité de départ
	Packet(const Packet& base);
	~Packet();

	//modification de la taille des données
	void setCapacity(size_t newCapacity); //change la capacité du packet

	//lecture des attributs
	const size_t size() { return _size; }
	const size_t capacity() { return _capacity; }
	const size_t cursor() { return _cursor; }
	char* data() { return _data; }

	//navigation dans les données
	const bool end() { return _cursor == _size; } //retourne vrai si le curseur est à la fin des données
	void move(size_t);//déplace le curseur

	//modification
	void add(char* newData, size_t dataSize); //ajoute les données à partir de la position du curseur et déplace le curseur à la fin des données


	//operateur pour écriture

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

