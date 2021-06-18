#include "packet.h"

#pragma region méthode

Packet::Packet(size_t beginCapacity) :_capacity(beginCapacity), _size(0), _cursor(0)
{
	_data = new char[_capacity]();
}

Packet::Packet(const Packet& base) :_capacity(base._capacity), _size(base._size), _cursor(base._cursor)
{
	_data = new char[_capacity];
	std::copy(base._data, base._data + _capacity, _data);
}


Packet::~Packet()
{
	delete[] _data;
}

void Packet::setCapacity(size_t newCapacity)
{
	char* new_data = nullptr;
	if ((new_data = new char[newCapacity]) == nullptr)throw "echec de l'assignation";
	size_t limit = _size;
	if (newCapacity < limit) limit = newCapacity;
	std::copy(_data, _data + limit, new_data);

	delete[] _data;
	_data = new_data;
	new_data = nullptr;
	_capacity = newCapacity;
}

void Packet::move(size_t position)
{
	if (position > _size) throw "deplacement à l'exterieur des donnees";
	_cursor = position;
}

void Packet::add(char* newData, size_t dataSize)
{
	//modification de la taille et de la capacité
	size_t newSize = _cursor + dataSize;
	if (newSize > _capacity) setCapacity(newSize);
	if (newSize > _size) _size = newSize;
	//ajout des données
	std::copy(newData, newData + dataSize, _data + _cursor);
	_cursor += dataSize;
}

Packet& Packet::operator = (const Packet& _b) {
	delete[] _data;
	_data = new char[_capacity];

	_capacity = _b._capacity;
	_size = _b._size;
	_cursor = _b._cursor;

	std::copy(_b._data, _b._data + _capacity, _data);
	return *this;
}

void Packet::popBack(size_t nbr)
{
	if (((long)_size - (long)nbr) < 0)throw "nbr insufisant de data dans le packet";
	_size -= nbr;
	if (_cursor > _size) _cursor = _size;
}

void Packet::clear()
{
	_size = 0;
	_cursor = 0;
}

void Packet::read(char* data, size_t readSize)
{
	if ((_cursor + readSize) > _size)throw "depassement lors de la lecture";

	std::copy(_data + _cursor, _data + _cursor + readSize, data);
	_cursor += readSize;

}

#pragma endregion



