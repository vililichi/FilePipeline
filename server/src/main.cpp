#include <iostream>
#include <thread>
#include "Communication/command.h"
#include "General/list.h"
#include "General/general.h"
#include "General/load.h"

#pragma region thread
void traitementPacket(CryptoSocket* csocket_ptr, Packet& pq)
{
	char commande;
	pq >> commande;
	switch (commande)
	{
	case command::Comm::list:
		{
		Packet reponse;
		std::vector<fileInfo> liste = list();
		reponse << liste.size();
		for (size_t i = 0; i < liste.size(); i++)reponse << liste[i].name<<liste[i].size;
		csocket_ptr->send(reponse);
		break;
		}
	case command::Comm::download:
		{
		std::string filename;
		pq >> filename;
		upload(csocket_ptr, filename, UP_PATH);
		break;
		}
	case command::Comm::upload:
		{
		std::string filename;
		pq >> filename;
		pq.clear();
		if (filename == ".." || split(filename, '\\', false).size() > 1 || split(filename, '/', false).size() > 1)
		{
			pq << false;
			csocket_ptr->send(pq);
			return;
		}
		pq << true;
		csocket_ptr->send(pq);
		download(csocket_ptr, filename, DOWN_PATH);
		break;
		}
	case command::Comm::rename:
		{
		std::string oldName;
		std::string newName;
		pq >> oldName;
		pq >> newName;

		std::vector<fileInfo> files = list(DOWN_PATH);
		bool find = false;
		Packet rep;
		if (newName == ".." || split(newName, '\\', false).size() > 1 || split(newName, '/', false).size() > 1)
		{
			rep << (char)2;
		}
		else
		{
			for (size_t i = 0; i < files.size(); i++)
			{
				if (files[i].name == oldName)
				{
					std::string old_path = DOWN_PATH;
					old_path += "/" + oldName;
					std::string new_path = DOWN_PATH;
					new_path += "/" + newName;
					if (std::rename(old_path.c_str(), new_path.c_str()) == 0);
					{
						std::remove(old_path.c_str());
						find = true;
					}
					break;
				}
			}
			if (find)
			{
				rep << (char)0;
			}
			else
			{
				rep << (char)1;
			}
		}
		csocket_ptr->send(rep);
		break;
		}
	case command::Comm::remove:
	{
		std::string name;
		pq >> name;
		std::vector<fileInfo> files = list(DOWN_PATH);
		bool find = false;
		for (size_t i = 0; i < files.size(); i++)
		{
			if (files[i].name == name)
			{
				std::string path = DOWN_PATH;
				path += "/" + name;
				if (std::remove(path.c_str()) == 0);
				{
					find = true;
				}
				break;
			}
		}
		Packet rep;
		if (find)
		{
			rep << true;
		}
		else
		{
			rep << false;
		}
		csocket_ptr->send(rep);
		break;
	}
	default:
		break;
	}
}
void socketFunction(CryptoSocket** tcp_ptr)
{
	CryptoSocket* cSocket;
	cSocket = *tcp_ptr;
	if (!cSocket->getHandShake())
		cSocket->m_socket_ptr->disconnect();

	while (true)
	{
		char data[INIT_PACKET_SIZE];
		size_t size;
		
		sf::Socket::Status stat;
		Packet pq = cSocket->receive(stat);
		if (stat == sf::Socket::Status::Error || stat == sf::Socket::Status::Disconnected)
		{
			break;
		}
		traitementPacket(cSocket, pq);
	}
	delete *tcp_ptr;
	*tcp_ptr = NULL;
}
void listeningFunction(sf::TcpListener* listener_ptr, std::vector<CryptoSocket**>* socketptr_List)
{
	std::vector<std::thread> socketThread;
	while (true)
	{
		CryptoSocket** new_socket_ptr = new CryptoSocket*;
		*new_socket_ptr = new CryptoSocket;
		(*new_socket_ptr)->m_socket_ptr = new sf::TcpSocket;
		sf::Socket::Status stat = listener_ptr->accept(*((*new_socket_ptr)->m_socket_ptr));
		if (stat == sf::Socket::Status::Error)
		{
			delete *new_socket_ptr;
			delete new_socket_ptr;
			break;
		}
		(*new_socket_ptr)->m_socket_ptr->setBlocking(true);
		socketThread.emplace(socketThread.end(),&socketFunction, new_socket_ptr);
		socketptr_List->push_back(new_socket_ptr);
	}
	for (size_t i = 0; i < socketptr_List->size(); i++)
	{
		if (*(*socketptr_List)[i])(*(*socketptr_List)[i])->m_socket_ptr->disconnect();
		if (socketThread[i].joinable())socketThread[i].join();
		delete (*socketptr_List)[i];
	}
}
#pragma endregion

int main()
{
	//creation des répertoires utiles
	createFolder(UP_PATH);
	createFolder(THRUST_PATH);
	createFolder(UNTRUST_PATH);


	//instanciation
	sf::TcpListener listener;

	//demande du port
	std::cout << "port: " << std::endl;
	short port;
	std::cin >> port;

	//ouverture du listener
	listener.listen(port);
	std::vector<CryptoSocket**> socketptr_List;
	std::thread listeningThread(&listeningFunction, &listener, &socketptr_List);

	std::string commande = "h";
	std::vector<std::string> p_commande = split(commande, ' ');
	std::cin.ignore();
	while (!(p_commande.size() > 0 && p_commande[0] == "exit") && !(p_commande.size() > 0 && p_commande[0] == "e") )
	{
		if (p_commande.size() == 0);
		//help
		else if (p_commande[0] == "h" || p_commande[0] == "help")
		{
			std::cout << "exit : ferme le programme" << std::endl;
			std::cout << "help : liste toutes les commandes" << std::endl;
			std::cout << "key  : leste toutes les cle d'acces" << std::endl;
			std::cout << "kick [ip] [port] : deconnecte un utilisateur, si le port est omis, tout les utilisateurs de l'ip sont deconnectes" << std::endl;
			std::cout << "list : liste tout les fichiers telechargeables" << std::endl;
			std::cout << "thrust [cle] : accorde l'acces a la cle" << std::endl;
			std::cout << "unthrust [cle] : retire l'acces a la cle" << std::endl;
			std::cout << "user : liste les utilisateurs ip | port | acces" << std::endl;
		}
		//liste
		else if (p_commande[0] == "ls" || p_commande[0] == "list")
		{

			std::cout << "fichiers disponibles :" << std::endl;
			std::vector<fileInfo> liste = list();
			for (size_t i = 0; i < liste.size(); i++) std::cout << liste[i].name <<'\t'<<liste[i].size<< std::endl;
		}
		//user
		else if (p_commande[0] == "u" || p_commande[0] == "user")
		{
			std::cout << "utilisateurs :" << std::endl;
			for (size_t i = 0; i < socketptr_List.size(); i++)
			{
				CryptoSocket * socketptr = *socketptr_List[i];
				if (socketptr)
					std::cout << socketptr->m_socket_ptr->getRemoteAddress().toString() << " | " << socketptr->m_socket_ptr->getRemotePort() << " | " << socketptr ->getAcces() << std::endl;
			}

		}
		//kick
		else if (p_commande[0] == "kck" || p_commande[0] == "kick")
		{
			if (p_commande.size() >= 2)
			{
				std::cout << "les utilisateurs suivant ont ete retires :" << std::endl;
				std::string ip = p_commande[1];
				if (p_commande.size() >= 3)
				{
					unsigned short port = std::stoi(p_commande[2]);
					for (size_t i = 0; i < socketptr_List.size(); i++)
					{
						CryptoSocket* socketptr = *socketptr_List[i];
						if (socketptr && socketptr->m_socket_ptr->getRemoteAddress().toString() == ip && socketptr->m_socket_ptr->getRemotePort() == port)
						{
							std::cout << socketptr->m_socket_ptr->getRemoteAddress().toString() << " | " << socketptr->m_socket_ptr->getRemotePort() << std::endl;
							socketptr->m_socket_ptr->disconnect();
						}
					}

				}
				else
				{
					for (size_t i = 0; i < socketptr_List.size(); i++)
					{
						CryptoSocket* socketptr = *socketptr_List[i];
						if (socketptr && socketptr->m_socket_ptr->getRemoteAddress().toString() == ip)
						{
							std::cout << socketptr->m_socket_ptr->getRemoteAddress().toString() << " | " << socketptr->m_socket_ptr->getRemotePort() << std::endl;
							socketptr->m_socket_ptr->disconnect();
						}
					}
				}
			}
			else
			{
				std::cout << "arguments manquants :" << std::endl;
			}
		}
		//key
		else if (p_commande[0] == "ky" || p_commande[0] == "key")
		{
			std::vector<fileInfo> thrust = list(THRUST_PATH);
			std::vector<fileInfo> uthrust = list(UNTRUST_PATH);
			std::cout << "acces :" << std::endl;
			for (int i = 0; i < thrust.size(); i++)
			{
				std::cout << thrust[i].name << std::endl;
			}
			std::cout << std::endl;
			std::cout << "acces en attente :" << std::endl;
			for (int i = 0; i < uthrust.size(); i++)
			{
				std::cout << uthrust[i].name << std::endl;
			}
		}
		//thrust
		else if (p_commande[0] == "t" || p_commande[0] == "thrust")
		{
			if (p_commande.size() >= 2)
			{
				std::vector<fileInfo> uthrust = list(UNTRUST_PATH);
				bool find = false;
				for (size_t i = 0; i < uthrust.size(); i++)
				{
					if (uthrust[i].name == p_commande[1])
					{
						std::string old_path = UNTRUST_PATH;
						old_path += "/" + p_commande[1];
						std::string new_path = THRUST_PATH;
						new_path += "/" + p_commande[1];
						std::rename(old_path.c_str(), new_path.c_str());
						std::remove(old_path.c_str());
						find = true;
						break;
					}
				}
				if(!find)
				{
					std::cout << "cle introuvable" << std::endl;
				}
				else
				{
					std::cout << "accord de l'acces de la cle" << std::endl;
				}
			}
		}
		//unthrust
		else if (p_commande[0] == "ut" || p_commande[0] == "unthrust")
		{
			if (p_commande.size() >= 2)
			{
				std::vector<fileInfo> uthrust = list(THRUST_PATH);
				bool find = false;
				for (size_t i = 0; i < uthrust.size(); i++)
				{
					if (uthrust[i].name == p_commande[1])
					{
						std::string old_path = THRUST_PATH;
						old_path += "/" + p_commande[1];
						std::string new_path = UNTRUST_PATH;
						new_path += "/" + p_commande[1];
						std::rename(old_path.c_str(), new_path.c_str());
						std::remove(old_path.c_str());
						find = true;
						break;
					}
				}
				if (!find)
				{
					std::cout << "cle introuvable" << std::endl;
				}
				else
				{
					std::cout << "retrait de l'acces de la cle" << std::endl;
				}
			}
		}

		//prise de commande
		std::cout << std::endl;
		std::cout << "inserer une commande" << std::endl;
		std::getline( std::cin,commande);
		p_commande = split(commande, ' ',true);
		std::cout << std::endl;
	}


	//destruction
	listener.close();
	if (listeningThread.joinable())listeningThread.join();

	return 0;
}
