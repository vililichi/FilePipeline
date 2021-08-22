#pragma once
#include <string>

#define INIT_PACKET_SIZE 1440

class Packet
{
protected:

	// gestion de la taille et de la capacit�
	size_t m_capacity;
	size_t m_size;
	mutable size_t m_cursor;

	// donn�es
	char* m_data;

public:

	/*
	  Cr�e un paquet
	  [in] beginCapacity_ : capacit�e de d�pard du Packet
	*/
	Packet(const size_t beginCapacity_ = INIT_PACKET_SIZE); //constructeur avec capacit� de d�part
	// Contructeur de copie d'un paquet
	Packet(const Packet& base_);
	Packet& operator = (const Packet& b_);
	~Packet();

	// Modification de la taille des donn�es
	void setCapacity(const size_t newCapacity_); //change la capacit� du packet

	// Retourne la taille des donn�es du paquet
	size_t size() const { return m_size; }
	// Retourne la capacit� du paquet
	size_t capacity() const { return m_capacity; }
	// Retourne la position du curseur de lecture/�criture du paquet
	size_t cursor() const { return m_cursor; }
	// Retourne les donn�es brutes du paquet
	char* data() const { return m_data; }

	// Indique si la fin du paquet est atteinte
	bool end() const { return m_cursor == m_size; }

	// D�place le curseur de lecture/�criture su paquet
	void move(size_t) const;

	// Ajoute les donn�es � partir de la position du curseur et d�place le curseur � la fin des donn�es
	// [in] newData_  : Pointeur vers le d�but des donn�es � ajouter
	// [in] dataSize_ : Taille des donn�es � ajouter
	void add(const char* const newData_, const size_t dataSize_);

	// Retire les derniers octets du packet
	// [in] nbr_ : Nombre d'octets � retirer
	void popBack(const size_t nbr_);

	// Lit un nombre d�terminer de donn� et d�place le curseur
	// [in] newData_  : Pointeur vers le d�but des donn�es � �crire
	// [in] readSize_ : Taille des donn�es � lire
	void read(char* const data_, const size_t readSize_) const; 

	// Vide la paquet
	void clear();


	//operateur pour �criture

	template <typename T>
	Packet& operator << (const T data);
	template <>
	Packet& operator << (const std::string& data_);

	//operateur pour lecture

	template <typename T>
	const Packet& operator >> (T& data_) const;
	template <>
	const Packet& operator >> (std::string& data_) const;

};

#pragma region << operator
template <typename T>
inline Packet& Packet::operator << (const T data_)
{
	const size_t bytesNbr = sizeof(T);

	char* cdata = (char*)&data_;
	add(cdata, bytesNbr);
	return *this;
}

template <>
inline Packet& Packet::operator << (const std::string& data_)
{
	const size_t bytesNbr = data_.size() + 1;

	char* cdata = (char*)data_.c_str();

	add(cdata, bytesNbr);
	return *this;
}

#pragma endregion

#pragma region >> operator

template <typename T>
inline const Packet& Packet::operator >> (T& data_) const
{
	const size_t bytesNbr = sizeof(T);
	if ((m_cursor + bytesNbr) > m_size)throw "depassement lors de la lecture";

	data_ = *(T*)(m_data + m_cursor);
	m_cursor += bytesNbr;
	return *this;
}

template <>
inline const Packet& Packet::operator >> (std::string& data_) const
{
	size_t bytesNbr = 0;
	const size_t virtSize = m_size - 1;
	while (*(m_data + m_cursor + bytesNbr) != 0)
	{
		bytesNbr++;
		if ((m_cursor + bytesNbr) > virtSize)throw "depassement lors de la lecture";
	}

	data_ = std::string(m_data + m_cursor);
	m_cursor += bytesNbr + 1;
	return *this;
}

#pragma endregion
