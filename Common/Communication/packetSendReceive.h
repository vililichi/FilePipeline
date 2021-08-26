#pragma once
#include "packet.h"

#include <SFML/Network.hpp>

namespace packetSender
{
    /*
      Envoie un paquet dans le socket
      [in]  pq_     : Paquet à envoyer
      [in]  socket_ : Socket où s'effectue l'envoie
      [out] stat_   : Status de l'opération
    */
    void send(const Packet& pq_, sf::TcpSocket& socket_, sf::Socket::Status& stat_);
    void send(const Packet& pq_, sf::TcpSocket& socket_);

    /*
      Receptionne un paquet d'un socket
      [in]  socket_ : Socket où s'effectue la reception
      [out] stat_   : Status de l'opération
    */
    Packet receive(sf::TcpSocket& socket_, sf::Socket::Status& stat_);
    Packet receive(sf::TcpSocket& socket_);
}
