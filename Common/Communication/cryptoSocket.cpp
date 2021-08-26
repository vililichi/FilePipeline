#include "cryptoSocket.h"

#include "General/MacroBank.h"
#include "General/list.h"

#include <filesystem>

/*
  Indique si la clé est de confiance
  [in]  cle_RSA_public_ : Clé à valider
  [in]  cle_folder_     : Répertoire où chercher la clé
  [out] acces_          : Nom du point d'accès correspondant à la clé
*/
static bool IsThrust(const RSA::cle& cle_RSA_public_, const std::string& cle_folder_,
                     std::string& acces_)
{
    // Recherche si la clé est de confiance
    std::filesystem::directory_iterator dirItt(cle_folder_);
    std::vector<fileInfo> trust = list(cle_folder_);
    for (const auto& cle : trust)
    {
        RSA::cle cle_test;
        RSA::getKey(cle_test, cle_folder_ + "/" + cle.name);
        if (cle_test.exposant == cle_RSA_public_.exposant
            && cle_test.modulus == cle_RSA_public_.modulus)
        {
            acces_ = cle.name;
            return true;
        }
    }

    // Recherche si la clé est enregistrée
    std::vector<fileInfo> untrust = list(UNTRUST_PATH);
    for (const auto& cle : untrust)
    {
        RSA::cle cle_test;
        std::string path = UNTRUST_PATH;
        path += "/" + cle.name;
        RSA::getKey(cle_test, path);
        if (cle_test.exposant == cle_RSA_public_.exposant
            && cle_test.modulus == cle_RSA_public_.modulus)
        {
            acces_ = cle.name;
            return false;
        }
    }

    // enregistrement de la clé
    for (size_t i = 0; i < SIZE_MAX; i++)
    {
        std::string file = "k" + std::to_string(i);
        bool present = false;
        for (const auto& cle : trust)
        {
            if (file == cle.name)
            {
                present = true;
                break;
            }
        }
        if (present)
            continue;
        for (const auto& cle : untrust)
        {
            if (file == cle.name)
            {
                present = true;
                break;
            }
        }
        if (present)
            continue;
        std::string path = UNTRUST_PATH;
        path += "/" + file;
        RSA::storeKey(cle_RSA_public_, path);
        acces_ = file;
        return false;
    }
    acces_ = "";
    return false;
}

bool CryptoSocket::sendHandShake(const RSA::cle& cle_RSA_public_,
                                 const RSA::cle& cle_RSA_privee_)
{
    sf::TcpSocket::Status status;

    // Envoie de la clé
    Packet pq;
    pq << cle_RSA_public_;
    packetSender::send(pq, *m_socket_ptr);

    // Validation de la confiance
    pq.clear();
    pq = packetSender::receive(*m_socket_ptr, status);
    RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, false, c_ClientCommErrMsg);

    bool confiance = false;
    pq >> confiance;
    pq >> m_acces;
    if (!confiance)
    {
        std::cout << "le serveur ne vous fait pas confiance" << std::endl;
        std::cout << "l'id de votre acces en attente est " << m_acces << std::endl;
        return false;
    }

    // Preuve d'authenticité
    pq.clear();
    pq = packetSender::receive(*m_socket_ptr, status);
    RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, false, c_ClientCommErrMsg);

    if (status != sf::Socket::Status::Done)
    {
        return false;
    }

    inft val;
    pq >> val;
    RSA::decryptage(val, cle_RSA_privee_);
    pq.clear();
    pq << val;
    packetSender::send(pq, *m_socket_ptr, status);
    RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, false, c_ClientCommErrMsg);

    pq.clear();
    pq = packetSender::receive(*m_socket_ptr, status);
    RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, false, c_ClientCommErrMsg);

    bool authentic = false;
    pq >> authentic;
    if (!authentic)
        return false;
    pq.clear();

    // réception de la clé symétrique
    pq = packetSender::receive(*m_socket_ptr, status);
    RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, false, c_ClientCommErrMsg);

    inft inft_cle;
    pq >> inft_cle;

    RSA::decryptage(inft_cle, cle_RSA_privee_);
    uint8_t* cle_reception = reinterpret_cast<uint8_t*>(inft_cle());
    for (uint8_t i = 0; i < 32; i++)
    {
        m_cle[i] = cle_reception[i];
    }

    // expension de la clé
    AES::expensionCle(m_cle);
    return true;
}

bool CryptoSocket::getHandShake()
{
    sf::TcpSocket::Status status;

    // reception de la cle
    Packet pq;
    pq = packetSender::receive(*m_socket_ptr, status);
    RETURN_IF(status != sf::TcpSocket::Status::Done, false)

    RSA::cle cle_RSA_public;
    pq >> cle_RSA_public;
    pq.clear();

    // test de présence
    if (!IsThrust(cle_RSA_public, TRUST_PATH, m_acces))
    {
        pq << false << m_acces;
        packetSender::send(pq, *m_socket_ptr);
        return false;
    }
    pq << true << m_acces;
    packetSender::send(pq, *m_socket_ptr, status);
    RETURN_IF(status != sf::TcpSocket::Status::Done, false)

    // test d'authenticité
    inft valTest = randinft(uint32_t(2147483648), uint32_t(4294967295));
    inft cpy = valTest;
    RSA::cryptage(cpy, cle_RSA_public);
    pq.clear();
    pq << cpy;
    packetSender::send(pq, *m_socket_ptr, status);
    RETURN_IF(status != sf::TcpSocket::Status::Done, false)

    pq.clear();
    pq = packetSender::receive(*m_socket_ptr, status);
    RETURN_IF(status != sf::TcpSocket::Status::Done, false)

    inft valDecrypt;
    pq >> valDecrypt;
    pq.clear();
    if (!(valDecrypt == valTest))
    {
        pq << false;
        packetSender::send(pq, *m_socket_ptr);
        return false;
    }
    pq << true;
    packetSender::send(pq, *m_socket_ptr, status);
    RETURN_IF(status != sf::TcpSocket::Status::Done, false)

    pq.clear();

    // génération de la clé symétrique
    AES::generation(m_cle);

    // envoie de la clé symétrique
    inft cle_inft(m_cle, 32);
    RSA::cryptage(cle_inft, cle_RSA_public);
    pq << cle_inft;
    packetSender::send(pq, *m_socket_ptr, status);
    RETURN_IF(status != sf::TcpSocket::Status::Done, false)

    // expension de la clé
    AES::expensionCle(m_cle);
    return true;
}

void CryptoSocket::send(Packet pq_, sf::Socket::Status& stat_)
{
    // reception
    try
    {
        AES::cryptage(pq_, m_cle);
    }
    catch (...)
    {
        std::cerr << "Erreur lors du cryptage d'un paquet" << std::endl;
        throw;
    }

    packetSender::send(pq_, *m_socket_ptr, stat_);
}

void CryptoSocket::send(Packet pq_)
{
    sf::Socket::Status _;
    return CryptoSocket::send(pq_, _);
}

Packet CryptoSocket::receive(sf::Socket::Status& stat_)
{
    Packet pq;
    pq = packetSender::receive(*m_socket_ptr, stat_);
    RETURN_IF(stat_ != sf::Socket::Status::Done, pq);

    try
    {
        AES::decryptage(pq, m_cle);
    }
    catch (...)
    {
        std::cerr << "Erreur lors du decryptage d'un paquet" << std::endl;
        throw;
    }

    return pq;
}

Packet CryptoSocket::receive()
{
    sf::Socket::Status _;
    return CryptoSocket::receive(_);
}
