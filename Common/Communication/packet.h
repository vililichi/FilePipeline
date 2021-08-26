#pragma once
#include <string>

#define INIT_PACKET_SIZE 1440

class Packet
{
protected:
    // gestion de la taille et de la capacité
    size_t m_capacity;
    size_t m_size;
    mutable size_t m_cursor;

    // données
    uint8_t* m_data;

public:
    /*
      Crée un paquet
      [in] beginCapacity_ : capacitée de dépard du Packet
    */
    Packet(const size_t beginCapacity_ =
               INIT_PACKET_SIZE); // constructeur avec capacité de départ
    // Contructeur de copie d'un paquet
    Packet(const Packet& base_);
    Packet& operator=(const Packet& b_);
    ~Packet();

    // Modification de la taille des données
    void setCapacity(const size_t newCapacity_); // change la capacité du packet

    // Retourne la taille des données du paquet
    size_t size() const { return m_size; }
    // Retourne la capacité du paquet
    size_t capacity() const { return m_capacity; }
    // Retourne la position du curseur de lecture/écriture du paquet
    size_t cursor() const { return m_cursor; }
    // Retourne les données brutes du paquet
    uint8_t* data() const { return m_data; }

    // Indique si la fin du paquet est atteinte
    bool end() const { return m_cursor == m_size; }

    // Déplace le curseur de lecture/écriture su paquet
    void move(size_t) const;

    // Ajoute les données à partir de la position du curseur et déplace le curseur à la
    // fin des données [in] newData_  : Pointeur vers le début des données à ajouter [in]
    // dataSize_ : Taille des données à ajouter
    void add(const uint8_t* const newData_, const size_t dataSize_);

    // Retire les derniers octets du packet
    // [in] nbr_ : Nombre d'octets à retirer
    void popBack(const size_t nbr_);

    // Lit un nombre déterminer de donné et déplace le curseur
    // [in] newData_  : Pointeur vers le début des données à écrire
    // [in] readSize_ : Taille des données à lire
    void read(uint8_t* const data_, const size_t readSize_) const;

    // Vide la paquet
    void clear();


    // operateur pour écriture

    template <typename T>
    Packet& operator<<(const T data);
    template <typename T>
    inline Packet& operator<<(const std::basic_string<T>& data_);

    // operateur pour lecture

    template <typename T>
    const Packet& operator>>(T& data_) const;
    template <typename T>
    inline const Packet& operator>>(std::basic_string<T>& data_) const;
};

#pragma region << operator
template <typename T>
inline Packet& Packet::operator<<(const T data_)
{
    constexpr size_t bytesNbr = sizeof(T) / sizeof(uint8_t);

    const uint8_t* cdata = reinterpret_cast<const uint8_t*>(&data_);
    add(cdata, bytesNbr);
    return *this;
}

template <typename T>
inline Packet& Packet::operator<<(const std::basic_string<T>& data_)
{
    constexpr size_t sizeFactor = sizeof(T) / sizeof(uint8_t);
    const size_t bytesNbr = (data_.size() + 1)*sizeFactor;

    const uint8_t* cdata = reinterpret_cast<const uint8_t*>(data_.c_str());

    add(cdata, bytesNbr);
    return *this;
}

#pragma endregion

#pragma region>> operator

template <typename T>
inline const Packet& Packet::operator>>(T& data_) const
{
    constexpr size_t bytesNbr = sizeof(T)/sizeof(uint8_t);
    if ((m_cursor + bytesNbr) > m_size)
        throw "depassement lors de la lecture";

    data_ = *reinterpret_cast<T*>(m_data + m_cursor);
    m_cursor += bytesNbr;
    return *this;
}

template <typename T>
inline const Packet& Packet::operator>>(std::basic_string<T>& data_) const
{
    constexpr size_t sizeFactor = sizeof(T) / sizeof(uint8_t);

    size_t bytesNbr = 0;
    T* charData = reinterpret_cast<T*>(m_data + m_cursor);


    while (*(charData + bytesNbr) != '\0')
    {
        bytesNbr++;
        if ((m_cursor + bytesNbr * sizeFactor) >= m_size)
            throw "depassement lors de la lecture";
    }

    data_ = std::string( charData );
    m_cursor += bytesNbr * sizeFactor + 1;
    return *this;
}

#pragma endregion
