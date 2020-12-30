// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include "command.h"
#include "list.h"


#pragma region fonction commande
void download(sf::TcpSocket* socket_ptr,std::string filename)
{
	//demande d'Acces
	Packet pq;
	pq << command::DownToUp::download << filename;
	socket_ptr->send(pq.data(), pq.size());

	//analyse de la reponse
	bool autorisation;
	Packet autoInfo = receive(socket_ptr);
	autoInfo >> autorisation;

	if(!autorisation)
	{
		std::cout << "fichier indisponible" << std::endl;
		return;
	}

	uint32_t tailleFichier;
	uint32_t tailleActu = 0;
	autoInfo >> tailleFichier;

	std::string path = DOWN_PATH;
	path += "/" + filename;

	char message[1] = { true };
	socket_ptr->send(message, sizeof(char));

	std::fstream file(path, std::fstream::out | std::fstream::binary);
	std::cout << 0 << "\t\t| " << tailleFichier;
	while (true)
	{
		Packet fchunk = receive(socket_ptr);
		char command;
		fchunk >> command;
		if (command == command::UpToDown::file)
		{
			file.write(fchunk.data() + fchunk.cursor(), fchunk.size() - fchunk.cursor());
			tailleActu += fchunk.size() - fchunk.cursor();


			char reponse[1] = { true };
			socket_ptr->send(reponse, sizeof(char));

			std::cout <<'\r'<< tailleActu << "\t\t| " << tailleFichier;
		}
		else break;
	}
	std::cout << std::endl;
	file.close();
}
#pragma endregion

int main()
{

	//demande des info de connection
	std::string ip;
	short port;
	std::cout << "ip de l'hebergeur" << std::endl;
	std::cin >> ip;
	std::cout << "port du serveur" << std::endl;
	std::cin >> port;

	//création du socket
	sf::TcpSocket socket;
	socket.connect(ip, port);

	//commande
	std::string commande = "h";
	std::vector<std::string> p_commande = split(commande, ' ');
	std::cin.ignore();
	while (p_commande[0] != "exit" && p_commande[0] != "e")
	{
		//help
		if (p_commande[0] == "h" || p_commande[0] == "help")
		{
			std::cout << "download [fichier] : télécharge le fichier" << std::endl;
			std::cout << "exit : ferme le programme" << std::endl;
			std::cout << "help : liste toutes les commandes" << std::endl;
			std::cout << "list : liste tout les fichiers téléchargeables" << std::endl;
		}
		//liste
		else if (p_commande[0] == "ls" || p_commande[0] == "list")
		{

			std::cout << "fichiers disponibles:" << std::endl;
			std::vector<fileInfo> liste = list(&socket);
			for (size_t i = 0; i < liste.size(); i++) std::cout << liste[i].name<< '\t'<<liste[i].size << std::endl;
		}
		//download
		else if (p_commande[0] == "dwn" || p_commande[0] == "download")
		{
			if (p_commande.size() < 2) std::cout << "argument manquant" << std::endl;
			else
			{
				download(&socket,p_commande[1]);
			}
		}

		//prise de commande
		std::cout << std::endl;
		std::cout << "inserer une commande" << std::endl;
		std::getline(std::cin, commande);
		p_commande = split(commande, ' ', true);
		std::cout << std::endl;
	}
	return 0;
}