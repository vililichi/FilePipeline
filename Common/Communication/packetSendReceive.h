#pragma once
#include "Packet.h"

#include <SFML/Network.hpp>
#include <optional>

namespace packetSender
{
    /*
      Envoie un paquet dans le socket
      [in]  pq_     : Paquet � envoyer
      [in]  socket_ : Socket o� s'effectue l'envoie
      [out] stat_   : Status de l'op?ration
    */
    void send(const Packet& pq_, sf::TcpSocket& socket_, sf::Socket::Status& stat_);
    void send(const Packet& pq_, sf::TcpSocket& socket_);

    /*
      Receptionne un paquet d'un socket
      [in]  socket_ : Socket o� s'effectue la reception
      [out] stat_   : Status de l'op�ration
    */
    Packet receive(sf::TcpSocket& socket_, sf::Socket::Status& stat_);
    Packet receive(sf::TcpSocket& socket_);
}
