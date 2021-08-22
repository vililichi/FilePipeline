#include "load.h"
#include "Communication/command.h"
#include "general.h"
#include "tachymeter.h"
#include "list.h"
#include <fstream>

void download(cryptoSocket* csocket_ptr, std::string filename, std::string folder, bool ui)
{
	//demande d'Acces
	Packet pq;
	pq << command::Comm::download << filename;
	csocket_ptr->send(pq);

	//analyse de la reponse
	bool autorisation;
	Packet autoInfo = csocket_ptr->receive();
	autoInfo >> autorisation;

	if (!autorisation)
	{
		if (ui) std::cout << "fichier indisponible" << std::endl;
		return;
	}

	uint32_t tailleFichier;
	uint32_t tailleActu = 0;
	autoInfo >> tailleFichier;

	std::string path = folder;
	path += "/" + filename;

	Packet message;
	message << true;
	csocket_ptr->send(message);

	createFolder(folder);
	std::fstream file(path, std::fstream::out | std::fstream::binary); 

	Tachymeter tachy;
	chronometer chronoTotal;
	unsigned short trigger = 0;
	if (ui)
	{
		std::cout << 0 << "\t\t| " << tailleFichier;
		chronoTotal.start();
		tachy.start();
	}

	while (tailleActu < tailleFichier)
	{
		//reception
		Packet fchunk = csocket_ptr->receive();

		//ecriture
		file.write(fchunk.data() + fchunk.cursor(), fchunk.size());

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

void upload(cryptoSocket* csocket_ptr, std::string filename,std::string folder, bool ui)
{
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
		csocket_ptr->send(message);
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
		csocket_ptr->send(message);
		return;
	}


	//envoie de message positif
	Packet prepq;
	uint32_t tailleFichier = liste[position].size;
	prepq << true << tailleFichier;
	csocket_ptr->send(prepq);

	//reception d'une confirmation
	Packet depart;
	depart = csocket_ptr->receive();
	if (depart.size() == 0 || depart.data()[0] == false) return;

	//téléversement
	Tachymeter tachy;
	chronometer chronoTotal;
	unsigned short trigger = 0;
	uint32_t tailleActu = 0;
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

		file.read(data, maxSize);
		size = file.gcount();

		fchunk.add(data, size);
		csocket_ptr->send(fchunk);

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

void uploadDemand(cryptoSocket* csocket_ptr, std::string filename, std::string folder)
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
	Packet pq;
	pq << command::Comm::upload << filename;
	csocket_ptr->send(pq);

	//attente d'une reponse
	Packet rep = csocket_ptr->receive();
	bool fichierOk;
	rep >> fichierOk;
	if (!fichierOk)
	{
		std::cout << "destination refusee" << std::endl;
	}
	rep.move(0);
	rep = csocket_ptr->receive();
	char commande;
	rep >> commande;
	if (commande != command::Comm::download)
	{
		std::cout << "reponse invalide" << std::endl;
		pq.move(0);
		pq << false;
		csocket_ptr->send(pq);
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
		csocket_ptr->send(pq);
		return;
	}

	//téléversement
	upload(csocket_ptr, filename, UP_PATH, true);
}