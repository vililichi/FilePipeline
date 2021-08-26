#pragma once

#include "Cryptography/RSA.h"
#include "PacketCryptation.h"
#include "PacketSendReceive.h"

#include <optional>

#define THRUST_PATH  "thrust"
#define UNTRUST_PATH "unthrust"

class CryptoSocket
{
public:
    sf::TcpSocket* m_socket_ptr;

    /*
      [client] Envoie une demande de poignée de main
      retourne true si l'accès est validé, false sinon
    */
    bool sendHandShake(RSA::cle cle_RSA_public_, RSA::cle cle_RSA_privee_);

    /*
      [serveur] Receptionne une demande de poignée de main et valide l'utilisateur
      retourne true si l'utilisateur à un accès valide, false sinon
    */
    bool getHandShake();

    /*
      Envoie un paquet
      [in] pq     : Paquet à envoyer
      [out] stat_ : Status de l'opération
    */
    void send(Packet pq_, sf::Socket::Status& stat_);
    void send(Packet pq_);

    /*
      Attend qu'un paquet soit soit réceptionné et retourne ce packet
      [out] stat_ : Status de l'opération
    */
    Packet receive(sf::Socket::Status& stat_);
    Packet receive();

    // Retourne le nom du point d'acces du CryptoSocket
    std::string getAcces() { return m_acces; }

    // Message pour les erreurs de communication
    static constexpr char c_ClientCommErrMsg[] =
        "Erreur lors de la communication avec le serveur";

protected:
    uint8_t m_cle[240];
    std::string m_acces;
};
