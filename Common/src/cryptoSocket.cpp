#include "cryptoSocket.h";
#include <filesystem>

bool isThrust(RSA::cle cle_RSA_public, std::string cle_folder)
{
	std::filesystem::directory_iterator dirItt(cle_folder);
	for (auto& entry : dirItt)
	{
		RSA::cle cle_test;
		RSA::getKey(cle_test, entry.path().string());
		if (cle_test.exposant == cle_RSA_public.exposant && cle_test.modulus == cle_RSA_public.modulus) return true;
	}
	return false;
}

bool cryptoSocket::sendHandShake(RSA::cle cle_RSA_public, RSA::cle cle_RSA_privee)
{
	//envoie de la cle
	Packet pq;
	pq << cle_RSA_public;
	packetSender::send(pq, socket_ptr);

	//confiance
	pq.clear();
	pq = packetSender::receive(socket_ptr);
	bool confiance = false;
	pq >> confiance;
	if (!confiance)
	{
		std::cout << "le serveur ne vous fait pas confiance" << std::endl;
		return false;
	}

	//preuve d'authenticité
	pq.clear();
	pq = packetSender::receive(socket_ptr);
	inft val;
	pq >> val;
	RSA::decryptage(val, cle_RSA_privee);
	pq.clear();
	pq << val;
	packetSender::send(pq, socket_ptr);
	pq.clear();
	pq = packetSender::receive(socket_ptr);
	bool authentic = false;
	pq >> authentic;
	if (!authentic) return false;
	pq.clear();

	//réception de la clé symétrique
	pq = packetSender::receive(socket_ptr);
	inft inft_cle;
	pq >> inft_cle;

	RSA::decryptage(inft_cle, cle_RSA_privee);
	char* cle_reception = (char*)inft_cle();
	for (int i = 0; i < 32; i++)
	{
		cle[i] = cle_reception[i];
	}

	//expension de la clé
	AES::expensionCle(cle);
	return true;
}
bool cryptoSocket::getHandShake()
{
	//reception de la cle
	Packet pq;
	pq = packetSender::receive(socket_ptr);
	RSA::cle cle_RSA_public;
	pq >> cle_RSA_public;
	pq.clear();

	//test de présence
	if (!isThrust(cle_RSA_public, THRUST_PATH))
	{
		pq << false;
		packetSender::send(pq, socket_ptr);
		return false;
	}
	pq << true;
	packetSender::send(pq, socket_ptr);

	//test d'authenticité
	inft valTest = randinft(unsigned int(2147483648), unsigned int(4294967295));
	inft cpy = valTest;
	RSA::cryptage(cpy, cle_RSA_public);
	pq.clear();
	pq << cpy;
	packetSender::send(pq, socket_ptr);
	pq.clear();
	pq = packetSender::receive(socket_ptr);
	inft valDecrypt;
	pq >> valDecrypt;
	pq.clear();
	if (!(valDecrypt == valTest))
	{
		pq << false;
		packetSender::send(pq, socket_ptr);
		return false;
	}
	pq << true;
	packetSender::send(pq, socket_ptr);
	pq.clear();

	//génération de la clé symétrique
	AES::generation(cle);

	//envoie de la clé symétrique
	inft cle_inft((unsigned char*)cle, 32);
	RSA::cryptage(cle_inft, cle_RSA_public);
	pq << cle_inft;
	packetSender::send(pq, socket_ptr);

	//expension de la clé
	AES::expensionCle(cle);
	return true;
}

void cryptoSocket::send(Packet pq, sf::Socket::Status* stat)
{
	//reception
	AES::cryptage(pq, cle);
	packetSender::send(pq, socket_ptr, stat);
}

Packet cryptoSocket::receive(sf::Socket::Status* stat)
{
	Packet pq;
	if (stat)
	{
		pq = packetSender::receive(socket_ptr, stat);
		if (*stat == sf::Socket::Status::Error || *stat == sf::Socket::Status::Disconnected)
			return pq;
	}
	else
	{
		sf::Socket::Status stat;
		pq = packetSender::receive(socket_ptr, &stat);
		if (stat == sf::Socket::Status::Error || stat == sf::Socket::Status::Disconnected)
			return pq;
	}

	AES::decryptage(pq, cle);
	return pq;
}