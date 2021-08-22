#include "CryptoSocket.h";
#include <filesystem>
#include "General/list.h"
#include "General/MacroBank.h"

/*
  Indique si la cl� est de confiance
  [in]  cle_RSA_public_ : Cl� � valider
  [in]  cle_folder_     : R�pertoire o� chercher la cl�
  [out] acces_          : Nom du point d'acc�s correspondant � la cl�
*/
static bool IsThrust(RSA::cle cle_RSA_public_, std::string cle_folder_, std::string& acces_)
{
	// Recherche si la cl� est de confiance
	std::filesystem::directory_iterator dirItt(cle_folder_);
	std::vector<fileInfo> thrust = list(cle_folder_);
	for (int i = 0 ; i < thrust.size(); i++)
	{
		RSA::cle cle_test;
		RSA::getKey(cle_test, cle_folder_ + "/" + thrust[i].name);
		if (cle_test.exposant == cle_RSA_public_.exposant && cle_test.modulus == cle_RSA_public_.modulus)
		{
			acces_ = thrust[i].name;
			return true;
		}
	}

	// Recherche si la cl� est enregistr�e
	std::vector<fileInfo> uthrust = list(UNTRUST_PATH);
	for (int i = 0; i < uthrust.size(); i++)
	{
		RSA::cle cle_test;
		std::string path = UNTRUST_PATH;
		path += "/" + uthrust[i].name;
		RSA::getKey(cle_test, path);
		if (cle_test.exposant == cle_RSA_public_.exposant && cle_test.modulus == cle_RSA_public_.modulus)
		{
			acces_ = uthrust[i].name;
			return false;
		}
	}

	//enregistrement de la cl�
	for (size_t i = 0; i < SIZE_MAX; i++)
	{
		std::string file = "k" + std::to_string(i);
		bool present = false;
		for (int i = 0; i < thrust.size(); i++)
		{
			if (file == thrust[i].name)
			{
				present = true;
				break;
			}
		}
		if (present) continue;
		for (int i = 0; i < uthrust.size(); i++)
		{
			if (file == uthrust[i].name)
			{
				present = true;
				break;
			}
		}
		if (present) continue;
		std::string path = UNTRUST_PATH;
		path += "/" + file;
		RSA::storeKey(cle_RSA_public_, path);
		acces_ = file;
		return false;
	}
	acces_ = "";
	return false;
}

bool CryptoSocket::sendHandShake(RSA::cle cle_RSA_public_, RSA::cle cle_RSA_privee_)
{
	sf::TcpSocket::Status status;

	// Envoie de la cl�
	Packet pq;
	pq << cle_RSA_public_;
	packetSender::send(pq, *m_socket_ptr);

	// Validation de la confiance
	pq.clear();
	pq = packetSender::receive(*m_socket_ptr, status);
	RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, false, "Erreur lors de la communication avec le serveur");

	bool confiance = false;
	pq >> confiance;
	pq >> m_acces;
	if (!confiance)
	{
		std::cout << "le serveur ne vous fait pas confiance" << std::endl;
		std::cout << "l'id de votre acces en attente est " << m_acces << std::endl;
		return false;
	}

	// Preuve d'authenticit�
	pq.clear();
	pq = packetSender::receive(*m_socket_ptr, status);
	RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, false, "Erreur lors de la communication avec le serveur");

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
	RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, false, "Erreur lors de la communication avec le serveur");

	pq.clear();
	pq = packetSender::receive(*m_socket_ptr, status);
	RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, false, "Erreur lors de la communication avec le serveur");

	bool authentic = false;
	pq >> authentic;
	if (!authentic) return false;
	pq.clear();

	//r�ception de la cl� sym�trique
	pq = packetSender::receive(*m_socket_ptr, status);
	RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, false, "Erreur lors de la communication avec le serveur");

	inft inft_cle;
	pq >> inft_cle;

	RSA::decryptage(inft_cle, cle_RSA_privee_);
	char* cle_reception = (char*)inft_cle();
	for (int i = 0; i < 32; i++)
	{
		m_cle[i] = cle_reception[i];
	}

	//expension de la cl�
	AES::expensionCle(m_cle);
	return true;
}

bool CryptoSocket::getHandShake()
{
	sf::TcpSocket::Status status;

	//reception de la cle
	Packet pq;
	pq = packetSender::receive(*m_socket_ptr, status);
	RETURN_IF(status != sf::TcpSocket::Status::Done, false)

	RSA::cle cle_RSA_public;
	pq >> cle_RSA_public;
	pq.clear();

	//test de pr�sence
	if (!IsThrust(cle_RSA_public, THRUST_PATH, m_acces))
	{
		pq << false << m_acces;
		packetSender::send(pq, *m_socket_ptr);
		return false;
	}
	pq << true << m_acces;
	packetSender::send(pq, *m_socket_ptr, status);
	RETURN_IF(status != sf::TcpSocket::Status::Done, false)

	//test d'authenticit�
	inft valTest = randinft(unsigned int(2147483648), unsigned int(4294967295));
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

	//g�n�ration de la cl� sym�trique
	AES::generation(m_cle);

	//envoie de la cl� sym�trique
	inft cle_inft((unsigned char*)m_cle, 32);
	RSA::cryptage(cle_inft, cle_RSA_public);
	pq << cle_inft;
	packetSender::send(pq, *m_socket_ptr, status);
	RETURN_IF(status != sf::TcpSocket::Status::Done, false)

	//expension de la cl�
	AES::expensionCle(m_cle);
	return true;
}

void CryptoSocket::send( Packet pq_, std::optional<sf::Socket::Status> stat_)
{
	//reception
	try 
	{
		AES::cryptage(pq_, m_cle);
	}
	catch (...)
	{
		std::cerr << "Erreur lors du cryptage d'un paquet" << std::endl;
		throw std::current_exception();
	}

	packetSender::send(pq_, *m_socket_ptr, stat_);
}

Packet CryptoSocket::receive( std::optional<sf::Socket::Status> stat_)
{
	Packet pq;
	if (stat_)
	{
		pq = packetSender::receive(*m_socket_ptr, stat_);
		RETURN_IF( stat_.value() != sf::Socket::Status::Done, pq);
	}
	else
	{
		sf::Socket::Status stat;
		pq = packetSender::receive( *m_socket_ptr, stat );
		RETURN_IF( stat != sf::Socket::Status::Done, pq);
	}

	try
	{
		AES::decryptage(pq, m_cle);
	}
	catch (...)
	{
		std::cerr << "Erreur lors du decryptage d'un paquet" << std::endl;
		throw std::current_exception();
	}

	return pq;
}