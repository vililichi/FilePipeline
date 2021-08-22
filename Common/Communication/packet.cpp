#include "packet.h"

#pragma region méthode

Packet::Packet(const size_t beginCapacity) :m_capacity(beginCapacity), m_size(0), m_cursor(0)
{
	m_data = new char[m_capacity]();
}

Packet::Packet(const Packet& base) :m_capacity(base.m_capacity), m_size(base.m_size), m_cursor(base.m_cursor)
{
	m_data = new char[m_capacity];
	std::copy(base.m_data, base.m_data + m_capacity, m_data);
}


Packet::~Packet()
{
	delete[] m_data;
}

void Packet::setCapacity(const size_t newCapacity_)
{
	char* new_data = nullptr;
	if ((new_data = new char[newCapacity_]) == nullptr)throw "echec de l'assignation";
	size_t limit = m_size;
	if (newCapacity_ < limit) limit = newCapacity_;
	std::copy(m_data, m_data + limit, new_data);

	delete[] m_data;
	m_data = new_data;
	new_data = nullptr;
	m_capacity = newCapacity_;
}

void Packet::move(size_t position_) const
{
	if (position_ > m_size) throw "deplacement à l'exterieur des donnees";
	m_cursor = position_;
}

void Packet::add(const char* const newData_, const size_t dataSize_)
{
	//modification de la taille et de la capacité
	size_t newSize = m_cursor + dataSize_;
	if (newSize > m_capacity) setCapacity(newSize);
	if (newSize > m_size) m_size = newSize;
	//ajout des données
	std::copy(newData_, newData_ + dataSize_, m_data + m_cursor);
	m_cursor += dataSize_;
}

Packet& Packet::operator = (const Packet& b_) {
	delete[] m_data;
	m_data = new char[m_capacity];

	m_capacity = b_.m_capacity;
	m_size = b_.m_size;
	m_cursor = b_.m_cursor;

	std::copy(b_.m_data, b_.m_data + m_capacity, m_data);
	return *this;
}

void Packet::popBack(const size_t nbr_)
{
	if (((long)m_size - (long)nbr_) < 0)throw "nbr insufisant de data dans le packet";
	m_size -= nbr_;
	if (m_cursor > m_size) m_cursor = m_size;
}

void Packet::clear()
{
	m_size = 0;
	m_cursor = 0;
}

void Packet::read(char* const data_, const size_t readSize_) const
{
	if ((m_cursor + readSize_) > m_size)throw "depassement lors de la lecture";

	std::copy(m_data + m_cursor, m_data + m_cursor + readSize_, data_);
	m_cursor += readSize_;
}

#pragma endregion



