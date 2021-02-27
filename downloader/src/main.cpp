// AVCs_5.cpp : définit le point d'entrée de l'application.
//


#include <iostream>
#include <fstream>
#include <thread>
#include "command.h"
#include "general.h"
#include "list.h"
#include "load.h"

int main()
{
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

	while (p_commande[0] != "exit" && p_commande[0] != "e")
	{
		//help
		if (p_commande[0] == "h" || p_commande[0] == "help")
		{
			std::cout << "download [fichier] : telecharge le fichier" << std::endl;
			std::cout << "exit : ferme le programme" << std::endl;
			std::cout << "help : liste toutes les commandes" << std::endl;
			std::cout << "list : liste tout les fichiers telechargeables" << std::endl;
		}
		//liste
		else if (p_commande[0] == "ls" || p_commande[0] == "list")
		{

			std::cout << "fichiers disponibles:" << std::endl;
			std::vector<fileInfo> liste = getlist(&cSocket);
			for (size_t i = 0; i < liste.size(); i++) std::cout << liste[i].name<< '\t'<<liste[i].size << std::endl;
		}
		//download
		else if (p_commande[0] == "dwn" || p_commande[0] == "download")
		{
			if (p_commande.size() < 2) std::cout << "argument manquant" << std::endl;
			else
			{
				download(&cSocket,p_commande[1],DOWN_PATH);
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