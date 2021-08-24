#include "PacketSendReceive.h"

void packetSender::send(const Packet& pq_, sf::TcpSocket& socket_,
                        sf::Socket::Status& stat_)
{
    char* data = new char[pq_.size() + 2];
    short size = pq_.size();
    char* csize = (char*)&size;
    data[0] = csize[0];
    data[1] = csize[1];
    pq_.move(0);
    pq_.read(data + 2, pq_.size());
    stat_ = socket_.send(data, pq_.size() + 2);
    delete[] data;
}

void packetSender::send(const Packet& pq_, sf::TcpSocket& socket_,
                        sf::Socket::Status& stat_)
{
    sf::Socket::Status _;
    return packetSender::send(pq_, socket_, _);
}

Packet packetSender::receive(sf::TcpSocket& socket_, sf::Socket::Status& stat_)
{
    size_t size = 0;
    Packet pq;
    // reception de la taille
    char cpSize[2];
    while (size != 2)
    {
        size_t addSize = 0;
        stat_ = socket_.receive(cpSize + size, 2 - size, addSize);
        size += addSize;
        if (stat_ == sf::Socket::Status::Disconnected
            || stat_ == sf::Socket::Status::Error)
            return pq;
    }

    short pSize = *(short*)cpSize;

    // reception du packet
    size = 0;
    char* data = new char[pSize];
    while (size != pSize)
    {
        size_t addSize = 0;
        stat_ = socket_.receive(data + size, pSize - size, addSize);
        size += addSize;
        if (stat_ == sf::Socket::Status::Disconnected
            || stat_ == sf::Socket::Status::Error)
            return pq;
    }

    pq.add(data, size);
    pq.move(0);
    delete[] data;
    return pq;
}

Packet packetSender::receive(sf::TcpSocket& socket_)
{
    sf::Socket::Status _;
    return packetSender::receive(socket_, _);
}
