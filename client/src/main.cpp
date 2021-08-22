// AVCs_5.cpp : définit le point d'entrée de l'application.
//


#include <iostream>
#include <fstream>
#include <thread>
#include "Communication/command.h"
#include "General/general.h"
#include "General/list.h"
#include "General/load.h"

int main()
{
	//création des répertoires
	createFolder(DOWN_PATH);
	createFolder(UP_PATH);

	//Gestion des clé
	std::string clePath = "private";
	std::string clePathPublic = "public";
	RSA::cle clePrive;
	RSA::cle clePublic;

	//creation de clé abscente
	if (!RSA::getKeySet(clePrive, clePublic, clePath))
	{
		std::cout << "aucune cle n'a ete trouvee" << std::endl;
		creationCle:;
		std::cout << "creation de la cle en cours\ncette operation peut prendre beaucoup de temps" << std::endl;
		std::cout << "elle peut aussi ne jamais finir" << std::endl;
		RSA::generation(clePrive, clePublic);
		std::cout << "fin de la generation de la cle" << std::endl << std::endl;
		RSA::storeKeySet(clePrive, clePublic, clePath);
		RSA::storeKey(clePublic, clePathPublic);
	}

	//test de la cle
	{
		inft valTest = randinft(unsigned int(2147483648), unsigned int(4294967295));
		inft cpy = valTest;
		RSA::cryptage(cpy, clePublic);
		RSA::decryptage(cpy, clePrive);
		if (!(cpy == valTest))
		{
			std::cout << "cle non valide" << std::endl;
			goto creationCle;
		}
	}

	//création du socket
	bool disconnected = true;
	sf::TcpSocket tsocket;
	cryptoSocket cSocket;

	while (disconnected)
	{
		//demande des info de connection
		std::string ip;
		short port;
		std::cout << "ip de l'hebergeur" << std::endl;
		std::cin >> ip;
		std::cout << "port du serveur" << std::endl;
		std::cin >> port;
		std::cin.ignore();

		//tentative de connection
		disconnected = (tsocket.connect(ip, port) != sf::Socket::Status::Done);
		if (disconnected) std::cout << "erreur de connection" << std::endl <<std::endl;
		else
		{
			//connection sécurisée
			cSocket.socket_ptr = &tsocket;
			if (!cSocket.sendHandShake(clePublic, clePrive))
			{
				disconnected = true;
				std::cout << "erreur lors de l'echange de cle" << std::endl << std::endl;
			}
			else std::cout << "connection etablie" << std::endl << std::endl;
		}
	}
	

	//commande
	std::string commande = "h";
	std::vector<std::string> p_commande = split(commande, ' ');

	while (!(p_commande.size() > 0 && p_commande[0] == "exit") && !(p_commande.size() > 0 && p_commande[0] == "e"))
	{
		if (p_commande.size() == 0);
		//help
		else if (p_commande[0] == "h" || p_commande[0] == "help")
		{
			std::cout << "download [fichier] : telecharge le fichier" << std::endl;
			std::cout << "exit : ferme le programme" << std::endl;
			std::cout << "help : liste toutes les commandes" << std::endl;
			std::cout << "list : liste tout les fichiers telechargeables et televersables" << std::endl;
			std::cout << "remove [local|server] [fichier] : supprime le fichier" << std::endl;
			std::cout << "rename [local|server] [fichier] [nom] : renomme le fichier avec nom" << std::endl;
			std::cout << "upload [fichier] : televerse le fichier" << std::endl;
		}
		//liste
		else if (p_commande[0] == "ls" || p_commande[0] == "list")
		{
			std::cout << "fichiers locaux :" << std::endl;
			std::vector<fileInfo> listeLoc = list();
			for (size_t i = 0; i < listeLoc.size(); i++) std::cout << listeLoc[i].name << '\t' << listeLoc[i].size << std::endl;
			std::cout << std::endl;
			std::cout << "fichiers serveurs :" << std::endl;
			std::vector<fileInfo> listeServ = getlist(&cSocket);
			for (size_t i = 0; i < listeServ.size(); i++) std::cout << listeServ[i].name<< '\t'<< listeServ[i].size << std::endl;
		}
		//download
		else if (p_commande[0] == "dwn" || p_commande[0] == "download")
		{
			if (p_commande.size() < 2) std::cout << "argument manquant" << std::endl;
			else
			{
				download(&cSocket,p_commande[1],DOWN_PATH,true);
			}
		}
		//upload
		else if (p_commande[0] == "up" || p_commande[0] == "upload")
		{
			if (p_commande.size() < 2) std::cout << "argument manquant" << std::endl;
			else
			{
				uploadDemand(&cSocket, p_commande[1], UP_PATH);
			}
		}
		//rename
		else if (p_commande[0] == "rn" || p_commande[0] == "rename")
		{
			if (p_commande.size() >= 4)
			{
				if (p_commande[1] == "lc" || p_commande[1] == "local")
				{
					std::vector<fileInfo> files = list(UP_PATH);
					bool find = false;
					if (p_commande[3] == ".." ||split(p_commande[3], '\\', false).size() > 1 || split(p_commande[3], '/', false).size() > 1)
					{
						std::cout << "nom invalide" << std::endl;
					}
					else
					{
						for (size_t i = 0; i < files.size(); i++)
						{
							if (files[i].name == p_commande[2])
							{
								std::string old_path = UP_PATH;
								old_path += "/" + p_commande[2];
								std::string new_path = UP_PATH;
								new_path += "/" + p_commande[3];
								if (std::rename(old_path.c_str(), new_path.c_str()) == 0);
								{
									std::remove(old_path.c_str());
									find = true;
								}
								break;
							}
						}
						if (!find)
						{
							std::cout << "fichier introuvable" << std::endl;
						}
						else
						{
							std::cout << "succes" << std::endl;
						}
					}
				}
				else if (p_commande[1] == "sv" || p_commande[1] == "server")
				{
					Packet packet;
					packet << command::Comm::rename<<p_commande[2]<<p_commande[3];
					cSocket.send(packet);

					packet.move(0);
					packet = cSocket.receive();
					char reussite = 0;
					packet >> reussite;

					if (reussite == 0)
						std::cout << "succes" << std::endl;
					else if (reussite == 1)
						std::cout << "fichier introuvable" << std::endl;
					else if (reussite == 2)
						std::cout << "nom invalide" << std::endl;
					else
						std::cout << "erreur" << std::endl;
				}
				else
				{
					std::cout << "argument 1 invalide" << std::endl;
				}
			}
			else
			{
				std::cout << "arguments invalides" << std::endl;
			}
		}
		//remove
		else if (p_commande[0] == "rm" || p_commande[0] == "remove")
		{
			if (p_commande.size() >= 3)
			{
				if (p_commande[1] == "lc" || p_commande[1] == "local")
				{
					std::vector<fileInfo> files = list(UP_PATH);
					bool find = false;
					for (size_t i = 0; i < files.size(); i++)
					{
						if (files[i].name == p_commande[2])
						{
							std::string path = UP_PATH;
							path += "/" + p_commande[2];
							if (std::remove(path.c_str()) == 0);
							{
								find = true;
							}
							break;
						}
					}
					if (!find)
					{
						std::cout << "fichier introuvable" << std::endl;
					}
					else
					{
						std::cout << "succes" << std::endl;
					}
				}
				else if (p_commande[1] == "sv" || p_commande[1] == "server")
				{
					Packet packet;
					packet << command::Comm::remove << p_commande[2];
					cSocket.send(packet);

					packet.move(0);
					packet = cSocket.receive();
					bool reussite = 0;
					packet >> reussite;

					if (reussite)
						std::cout << "succes" << std::endl;
					else
						std::cout << "fichier introuvable" << std::endl;
				}
				else
				{
					std::cout << "argument 1 invalide" << std::endl;
				}
			}
			else
			{
				std::cout << "arguments invalides" << std::endl;
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