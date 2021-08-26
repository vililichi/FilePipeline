#include "List.h"
#include "MacroBank.h"
#include "Communication/Command.h"
#include <filesystem>
#include "General.h"

std::vector<fileInfo> getlist(CryptoSocket* csocket_ptr)
{
    sf::Socket::Status status;

	//envoie de la demande
	Packet demande;
	demande << command::Comm::list;
	csocket_ptr->send(demande, status);
    RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, std::vector<fileInfo> {},
                      CryptoSocket::c_ClientCommErrMsg);

	//reception de la reponse
	Packet pq = csocket_ptr->receive(status);
    RETURN_IF_MESSAGE(status != sf::TcpSocket::Status::Done, std::vector<fileInfo> {},
                      CryptoSocket::c_ClientCommErrMsg);

	size_t size;
	pq >> size;
	std::vector<fileInfo> retour;
	for (size_t i = 0; i < size; i++)
	{
		fileInfo fi;
		pq >> fi.name;
		pq >> fi.size;
		retour.push_back(fi);
	}
	return retour;
}

std::vector<fileInfo> list(std::string path)
{
	std::vector<fileInfo> retour;
	std::filesystem::directory_iterator dirItt(path);
	for (auto& entry : dirItt)
	{
		fileInfo fi;
		fi.size = entry.file_size();
		fi.name = split(entry.path().string(), '\\', true).back();
		retour.push_back(fi);
	}
	return retour;
}