#include "Load.h"
#include "Communication/Command.h"
#include "General.h"
#include "Tachymeter.h"
#include "List.h"
#include "MacroBank.h"
#include <fstream>

void download(CryptoSocket* csocket_ptr, std::string filename, std::string folder, bool ui)
{
    sf::TcpSocket::Status status;

	//demande d'Acces
	Packet pq;
	pq << command::Comm::download << filename;
	csocket_ptr->send(pq, status);
    if (ui)
    {
        RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, ,
                          CryptoSocket::c_ClientCommErrMsg);
    }
    else
    {
        RETURN_IF(status != sf::TcpSocket::Status::Done, )
    }


	//analyse de la reponse
	bool autorisation;
	Packet autoInfo = csocket_ptr->receive(status);
    if (ui)
    {
        RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, ,
                          CryptoSocket::c_ClientCommErrMsg);
    }
    else
    {
        RETURN_IF(status != sf::TcpSocket::Status::Done, )
    }

	autoInfo >> autorisation;

	if (!autorisation)
	{
		if (ui) std::cout << "fichier indisponible" << std::endl;
		return;
	}

	uint64_t tailleFichier;
	size_t tailleActu = 0;
	autoInfo >> tailleFichier;

	std::string path = folder;
	path += "/" + filename;

	Packet message;
	message << true;
	csocket_ptr->send(message, status);
    if (ui)
    {
        RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, ,
                          CryptoSocket::c_ClientCommErrMsg);
    }
    else
    {
        RETURN_IF(status != sf::TcpSocket::Status::Done, )
    }

	createFolder(folder);
	std::fstream file(path, std::fstream::out | std::fstream::binary); 

	Tachymeter tachy;
	chronometer chronoTotal;
	uint16_t trigger = 0;
	if (ui)
	{
		std::cout << 0 << "\t\t| " << tailleFichier;
		chronoTotal.start();
		tachy.start();
	}

	while (tailleActu < tailleFichier)
	{
		//reception
		Packet fchunk = csocket_ptr->receive(status);
        if (status != sf::TcpSocket::Status::Done)
        {
            if (ui)
            {
                std::cout << CryptoSocket::c_ClientCommErrMsg << std::endl;
            }
            file.close();
            return;
        }

		//ecriture
		file.write( reinterpret_cast<char *>( fchunk.data() + fchunk.cursor() ), fchunk.size() );

		//suivie
		tailleActu += fchunk.size();
		if (ui)
		{
			tachy.addSample(fchunk.size());
		
			if (trigger == 0)
			{
				std::cout << '\r' << tailleActu << " octets\t\t| " << tailleFichier << " octets\t" << tachy.speed() << "Ko/s";
				trigger = 1024;
			}
			else trigger--;
		}

	}
	if (ui) 
	{
		std::cout << '\r' << tailleActu << " octets\t\t| " << tailleFichier << " octets\t" << tachy.speed() << "Ko/s";
		tachy.stop();
		chronoTotal.stop();
		std::cout << std::endl << "vitesse moyenne: " << tachy.avgSpeed() << "ko/s" << std::endl;
		std::cout << "temps Total: " << chronoTotal.get() << "ms" << std::endl;
	}
	file.close();
}

void upload(CryptoSocket* csocket_ptr, std::string filename,std::string folder, bool ui)
{
    sf::TcpSocket::Status status;

	//test de la validité du fichier
	std::vector<fileInfo> liste = list();
	bool presence = false;
	size_t position;
	for (size_t i = 0; i < liste.size(); i++) if (liste[i].name == filename) { presence = true; position = i; break; }
	if (!presence)
	{
		//envoie de message negatif
		Packet message;
		message << false;
		csocket_ptr->send(message, status);
        if (ui)
        {
            RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, ,
                              CryptoSocket::c_ClientCommErrMsg);
        }

		return;
	}

	std::string path = folder;
	path += "/" + filename;
	std::fstream file(path, std::fstream::in | std::fstream::binary);

	if (!file)
	{
		//envoie de message negatif
		Packet message;
		message << false;
		csocket_ptr->send(message, status);
        if (ui)
        {
            RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, ,
                              CryptoSocket::c_ClientCommErrMsg);
        }

		return;
	}


	//envoie de message positif
	Packet prepq;
	uint64_t tailleFichier = liste[position].size;
	prepq << true << tailleFichier;
	csocket_ptr->send(prepq, status);
    if (ui)
    {
        RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, ,
                          CryptoSocket::c_ClientCommErrMsg);
    }
    else
    {
        RETURN_IF(status != sf::TcpSocket::Status::Done, )
    }

	//reception d'une confirmation
	Packet depart;
	depart = csocket_ptr->receive(status);
    if (ui)
    {
        RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, ,
                          CryptoSocket::c_ClientCommErrMsg);
    }
    else
    {
        RETURN_IF(status != sf::TcpSocket::Status::Done, )
    }

	if (depart.size() == 0 || depart.data()[0] == false) return;

	//téléversement
	Tachymeter tachy;
	chronometer chronoTotal;
	uint16_t trigger = 0;
	size_t tailleActu = 0;
	if (ui)
	{
		std::cout << 0 << "\t\t| " << tailleFichier;
		chronoTotal.start();
		tachy.start();
	}

	while (!file.eof())
	{
		const size_t packetSize = 16 * 90;
		Packet fchunk(packetSize);
		const size_t maxSize = packetSize - 1;
		char data[maxSize];
		size_t size = 0;

		file.read( data, maxSize);
		size = file.gcount();

		fchunk.add(reinterpret_cast<uint8_t*>(data), size);

		csocket_ptr->send(fchunk, status);
        if ( status != sf::TcpSocket::Status::Done )
        {
            if (ui)
            {
                std::cout << CryptoSocket::c_ClientCommErrMsg << std::endl;
            }
            file.close();
			return;
        }

		//suivie
		if (ui)
		{

			tailleActu += fchunk.size();
			tachy.addSample(fchunk.size());

			if (trigger == 0)
			{
				std::cout << '\r' << tailleActu << " octets\t\t| " << tailleFichier << " octets\t" << tachy.speed() << "Ko/s";
				trigger = 1024;
			}
			else trigger--;
		}
	}
	if (ui)
	{
		std::cout << '\r' << tailleActu << " octets\t\t| " << tailleFichier << " octets\t" << tachy.speed() << "Ko/s";
		tachy.stop();
		chronoTotal.stop();
		std::cout << std::endl << "vitesse moyenne: " << tachy.avgSpeed() << "ko/s" << std::endl;
		std::cout << "temps Total: " << chronoTotal.get() << "ms" << std::endl;
	}
	file.close();
}

void uploadDemand(CryptoSocket* csocket_ptr, std::string filename, std::string folder)
{
	//vérification de la demande de fichier
	std::vector<fileInfo> liste = list();
	bool presence = false;
	size_t position;
	for (size_t i = 0; i < liste.size(); i++) if (liste[i].name == filename) { presence = true; position = i; break; }
	if (!presence)
	{
		std::cout << "Fichier non valide";
		return;
	}

	//Envoie d'une demande d'upload
    sf::TcpSocket::Status status;

	Packet pq;
	pq << command::Comm::upload << filename;
	csocket_ptr->send(pq, status);
    RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, ,
                      CryptoSocket::c_ClientCommErrMsg);

	//attente d'une reponse
	Packet rep = csocket_ptr->receive(status);
    RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, ,
                      CryptoSocket::c_ClientCommErrMsg);
	bool fichierOk;
	rep >> fichierOk;
	if (!fichierOk)
	{
		std::cout << "destination refusee" << std::endl;
	}
	rep.move(0);
	rep = csocket_ptr->receive(status);
    RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, ,
                      CryptoSocket::c_ClientCommErrMsg);

	char commande;
	rep >> commande;
    if (commande != command::Comm::download)
    {
        std::cout << "reponse invalide" << std::endl;
        pq.move(0);
        pq << false;
        csocket_ptr->send(pq, status);
        if (status != sf::TcpSocket::Status::Done)
        {
            std::cout << CryptoSocket::c_ClientCommErrMsg << std::endl;
        }

		return;
	}

	//validation du fichier
	std::string filename_demande;
	rep >> filename_demande;
	if(filename != filename_demande)
	{
		std::cout << "fichier non conforme" << std::endl;
		pq.move(0);
		pq << false;
		csocket_ptr->send(pq, status);
        if (status != sf::TcpSocket::Status::Done)
        {
            std::cout << CryptoSocket::c_ClientCommErrMsg << std::endl;
        }

		return;
	}

	//téléversement
	upload(csocket_ptr, filename, UP_PATH, true);
}