


#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <SFML/Network.hpp>
#include "command.h"
#include "packet.h"
#include "list.h"
#include "cryptoSocket.h"

#pragma region fonction de communication
void upload(cryptoSocket* csocket_ptr, std::string filename)
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

	std::string path = UP_PATH;
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
	depart =  csocket_ptr->receive();
	if (depart.size() == 0 || depart.data()[0] == false) return;


	while (!file.eof())
	{
		const size_t packetSize = 16 * 90;
		Packet fchunk(packetSize);
		const size_t maxSize = packetSize -1;
		char data[maxSize];
		size_t size = 0;

		file.read(data, maxSize);
		size = file.gcount();

		fchunk.add(data, size);
		csocket_ptr->send(fchunk);
	}
	file.close();
}
#pragma endregion

#pragma region thread
void traitementPacket(cryptoSocket* csocket_ptr, Packet& pq)
{
	char commande;
	pq >> commande;
	switch (commande)
	{
	case command::DownToUp::list:
		{
		Packet reponse;
		std::vector<fileInfo> liste = list();
		reponse << liste.size();
		for (size_t i = 0; i < liste.size(); i++)reponse << liste[i].name<<liste[i].size;
		csocket_ptr->send(reponse);
		break;
		}
	case command::DownToUp::download:
		{
		std::string filename;
		pq >> filename;
		upload(csocket_ptr, filename);
		break;
		}
	default:
		break;
	}
}
void socketFunction(sf::TcpSocket** tcp_ptr)
{
	cryptoSocket cSocket;
	cSocket.socket_ptr = *tcp_ptr;
	if (!cSocket.getHandShake())
		(*tcp_ptr)->disconnect();

	while (true)
	{
		char data[INIT_PACKET_SIZE];
		size_t size;
		
		sf::Socket::Status stat;
		Packet pq = cSocket.receive(&stat);
		if (stat == sf::Socket::Status::Error || stat == sf::Socket::Status::Disconnected)
		{
			break;
		}
		traitementPacket(&cSocket, pq);
	}
	delete *tcp_ptr;
	*tcp_ptr = NULL;
}
void listeningFunction(sf::TcpListener* listener_ptr)
{
	std::vector<std::thread> socketThread;
	std::vector<sf::TcpSocket**> socketptr_List;
	while (true)
	{
		sf::TcpSocket** new_socket_ptr = new sf::TcpSocket*;
		*new_socket_ptr = new sf::TcpSocket;
		sf::Socket::Status stat = listener_ptr->accept(**new_socket_ptr);
		if (stat == sf::Socket::Status::Error)
		{
			delete *new_socket_ptr;
			delete new_socket_ptr;
			break;
		}
		(*new_socket_ptr)->setBlocking(true);
		socketThread.emplace(socketThread.end(),&socketFunction, new_socket_ptr);
		socketptr_List.push_back(new_socket_ptr);
	}
	for (size_t i = 0; i < socketptr_List.size(); i++)
	{
		if (*socketptr_List[i])(*socketptr_List[i])->disconnect();
		if (socketThread[i].joinable())socketThread[i].join();
		delete socketptr_List[i];
	}
}
#pragma endregion

int main()
{
	//instanciation
	sf::TcpListener listener;

	//demande du port
	std::cout << "port: " << std::endl;
	short port;
	std::cin >> port;

	//ouverture du listener
	listener.listen(port);
	std::thread listeningThread(&listeningFunction, &listener);

	std::string commande = "h";
	std::vector<std::string> p_commande = split(commande, ' ');
	std::cin.ignore();
	while (p_commande[0] != "exit" && p_commande[0] != "e")
	{
		//help
		if (p_commande[0] == "h" || p_commande[0] == "help")
		{
			std::cout << "exit : ferme le programme" << std::endl;
			std::cout << "help : liste toutes les commandes" << std::endl;
			std::cout << "list : liste tout les fichiers telechargeables" << std::endl;
		}
		//liste
		else if (p_commande[0] == "ls" || p_commande[0] == "list")
		{

			std::cout << "fichiers disponibles:" << std::endl;
			std::vector<fileInfo> liste = list();
			for (size_t i = 0; i < liste.size(); i++) std::cout << liste[i].name <<'\t'<<liste[i].size<< std::endl;
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
