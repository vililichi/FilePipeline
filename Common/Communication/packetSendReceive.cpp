#include "PacketSendReceive.h"

void packetSender::send( const Packet& pq_, sf::TcpSocket& socket_, std::optional<sf::Socket::Status>& stat_ )
{
	char* data = new char[pq_.size() + 2];
	short size = pq_.size();
	char* csize = (char*)&size;
	data[0] = csize[0];
	data[1] = csize[1];
	pq_.move(0);
	pq_.read(data + 2, pq_.size());
	if (stat_.has_value()) stat_.value() = socket_.send(data, pq_.size() + 2);
	else socket_.send(data, pq_.size() + 2);
	delete[] data;
}

Packet packetSender::receive(sf::TcpSocket& socket_, std::optional<sf::Socket::Status>& stat_ )
{
	size_t size = 0;
	Packet pq;
	//reception de la taille
	char cpSize[2];
	if (stat_.has_value())
	{
		while (size != 2) 
		{
			size_t addSize = 0;
			stat_.value() = socket_.receive(cpSize + size, 2 - size, addSize);
			size += addSize;
			if (stat_.value() == sf::Socket::Status::Disconnected || stat_.value() == sf::Socket::Status::Error) return pq;
		}
	}
	else
	{
		while (size != 2) 
		{
			size_t addSize = 0;
			sf::Socket::Status stat = socket_.receive( cpSize + size, 2 - size, addSize );
			size += addSize;
			if ( stat == sf::Socket::Status::Disconnected || stat == sf::Socket::Status::Error ) return pq;
		}
	}
	short pSize = *(short*)cpSize;

	//reception du packet
	size = 0;
	char* data = new char[pSize];
	if (stat_.has_value())
	{
		while (size != pSize) 
		{
			size_t addSize = 0;
			stat_.value() = socket_.receive(data + size, pSize - size, addSize);
			size += addSize;
			if (stat_.value() == sf::Socket::Status::Disconnected || stat_.value() == sf::Socket::Status::Error) return pq;
		}
	}
	else
	{
		while (size != pSize) 
		{
			size_t addSize = 0;
			sf::Socket::Status stat = socket_.receive(data + size, pSize - size, addSize);
			size += addSize;
			if (stat == sf::Socket::Status::Disconnected || stat == sf::Socket::Status::Error) return pq;
		}
	}
	pq.add(data, size);
	pq.move(0);
	delete[] data;
	return pq;
}