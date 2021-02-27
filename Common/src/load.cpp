#include "load.h"
#include "command.h"
#include "general.h"
#include "tachymeter.h"
#include "list.h"
#include <fstream>

void download(cryptoSocket* csocket_ptr, std::string filename, std::string folder)
{
	//demande d'Acces
	Packet pq;
	pq << command::DownToUp::download << filename;
	csocket_ptr->send(pq);

	//analyse de la reponse
	bool autorisation;
	Packet autoInfo = csocket_ptr->receive();
	autoInfo >> autorisation;

	if (!autorisation)
	{
		std::cout << "fichier indisponible" << std::endl;
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
	std::cout << 0 << "\t\t| " << tailleFichier;
	Tachymeter tachy;
	chronometer chronoTotal;
	chronoTotal.start();
	tachy.start();
	unsigned short trigger = 0;
	while (tailleActu < tailleFichier)
	{
		//reception
		Packet fchunk = csocket_ptr->receive();

		//ecriture
		file.write(fchunk.data() + fchunk.cursor(), fchunk.size());

		//suivie
		tailleActu += fchunk.size();
		tachy.addSample(fchunk.size());
		if (trigger == 0)
		{
			std::cout << '\r' << tailleActu << " octets\t\t| " << tailleFichier << " octets\t" << tachy.speed() << "Ko/s";
			trigger = 1024;
		}
		else trigger--;

	}
	std::cout << '\r' << tailleActu << " octets\t\t| " << tailleFichier << " octets\t" << tachy.speed() << "Ko/s";
	tachy.stop();
	chronoTotal.stop();
	std::cout << std::endl << "vitesse moyenne: " << tachy.avgSpeed() << "ko/s" << std::endl;
	std::cout << "temps Total: " << chronoTotal.get() << "ms" << std::endl;
	file.close();
}

void upload(cryptoSocket* csocket_ptr, std::string filename,std::string folder)
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
	prepq << true << liste[position].size;
	csocket_ptr->send(prepq);

	//reception d'une confirmation
	Packet depart;
	depart = csocket_ptr->receive();
	if (depart.size() == 0 || depart.data()[0] == false) return;


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
	}
	file.close();
}