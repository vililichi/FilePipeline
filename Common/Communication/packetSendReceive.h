#pragma once
#include <SFML/Network.hpp>
#include <optional>
#include "packet.h"

namespace packetSender
{
	/*
	  Envoie un paquet dans le socket
	  [in]  pq_     : Paquet à envoyer
	  [in]  socket_ : Socket où s'éffectue l'envoie
	  [out] stat_   : Status de l'opération
	*/
	void send(Packet& pq_, sf::TcpSocket& socket_, std::optional<sf::Socket::Status> stat_ = std::nullopt );

	/*
	  Receptionne un paquet d'un socket
	  [in]  socket_ : Socket où s'éffectue la reception
	  [out] stat_   : Status de l'opération
	*/
	Packet receive(sf::TcpSocket& socket_, std::optional<sf::Socket::Status> stat_ = std::nullopt );
}