#pragma once
#include <SFML/Network.hpp>
#include <optional>
#include "packet.h"

namespace packetSender
{
	/*
	  Envoie un paquet dans le socket
	  [in]  pq_     : Paquet � envoyer
	  [in]  socket_ : Socket o� s'�ffectue l'envoie
	  [out] stat_   : Status de l'op�ration
	*/
	void send(Packet& pq_, sf::TcpSocket& socket_, std::optional<sf::Socket::Status> stat_ = std::nullopt );

	/*
	  Receptionne un paquet d'un socket
	  [in]  socket_ : Socket o� s'�ffectue la reception
	  [out] stat_   : Status de l'op�ration
	*/
	Packet receive(sf::TcpSocket& socket_, std::optional<sf::Socket::Status> stat_ = std::nullopt );
}