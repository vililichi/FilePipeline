#include "cmds.h"

#include "Communication/command.h"
#include "General/general.h"
#include "General/list.h"
#include "General/load.h"

#include <iostream>
#include <sstream>
#include <vector>

using namespace cmd;

bool cmd::listCmd(const ParamVec&)
{
    std::cout << "Fichiers disponibles :\n";
    std::vector<fileInfo> liste = list();
    for (const auto& fichier : liste)
    {
        std::cout << fichier.name << '\t' << fichier.size << "\n";
    }
    std::cout.flush();

    return true;
}

bool cmd::userCmd(std::vector<CryptoSocket**>& sockets, std::mutex& mutex,
                  const ParamVec& command)
{
    std::lock_guard lock(mutex);

    std::cout << "Utilisateurs :\n";

    for (const auto& socket : sockets)
    {
        CryptoSocket* socketptr = *socket;
        if (socketptr)
        {
            std::cout << socketptr->m_socket_ptr->getRemoteAddress().toString() << " | "
                      << socketptr->m_socket_ptr->getRemotePort() << " | "
                      << socketptr->getAcces() << "\n";
        }
    }
    std::cout.flush();

    return true;
}

bool cmd::keyCmd(const ParamVec& command)
{
    std::vector<fileInfo> trust = list(TRUST_PATH);
    std::vector<fileInfo> untrust = list(UNTRUST_PATH);

    std::cout << "Acces accorde :\n";
    for (const auto& fichier : trust)
    {
        std::cout << fichier.name << "\n";
    }
    std::cout << "\n";

    std::cout << "Acces en attente :\n";
    for (const auto& fichier : untrust)
    {
        std::cout << fichier.name << "\n";
    }
    std::cout.flush();

    return true;
}

bool cmd::trustCmd(const ParamVec& command)
{
    if (command.size() < 2)
    {
        std::cout << "Argument invalide: attendu [1] -> recu [" << command.size() - 1
                  << "]" << std::endl;
        return true;
    }

    std::vector<fileInfo> untrust = list(UNTRUST_PATH);
    bool found = false;

    for (const auto& fichier : untrust)
    {
        if (fichier.name == command[1])
        {
            std::string old_path = UNTRUST_PATH;
            old_path += "/" + command[1];

            std::string new_path = TRUST_PATH;
            new_path += "/" + command[1];

            std::rename(old_path.c_str(), new_path.c_str());
            std::remove(old_path.c_str());

            found = true;
            break;
        }
    }
    if (!found)
    {
        std::cout << "La cle [" << command[1] << "] est introuvable" << std::endl;
    }
    else
    {
        std::cout << "Acces accorde pour la cle [" << command[1] << "]" << std::endl;
    }

    return true;
}

bool cmd::untrustCmd(const ParamVec& command)
{
    if (command.size() < 2)
    {
        std::cout << "Argument invalide: attendu [1] -> recu [" << command.size() - 1
                  << "]" << std::endl;
        return true;
    }


    std::vector<fileInfo> trust = list(TRUST_PATH);
    bool found = false;

    for (const auto& fichier : trust)
    {
        if (fichier.name == command[1])
        {
            std::string old_path = TRUST_PATH;
            old_path += "/" + command[1];

            std::string new_path = UNTRUST_PATH;
            new_path += "/" + command[1];

            std::rename(old_path.c_str(), new_path.c_str());
            std::remove(old_path.c_str());

            found = true;
            break;
        }
    }

    if (!found)
    {
        std::cout << "La cle [" << command[1] << "] est introuvable" << std::endl;
    }
    else
    {
        std::cout << "Acces retire pour la cle [" << command[1] << "]" << std::endl;
    }

    return true;
}

bool cmd::kickCmd(std::vector<CryptoSocket**>& sockets, std::mutex& mutex,
                  const ParamVec& command)
{
    if (command.size() < 2)
    {
        std::cout << "Argument invalide: attendu [1] -> recu [" << command.size() - 1
                  << "]" << std::endl;
        return true;
    }

    std::lock_guard lock(mutex);

    std::string ip = command[1];
    bool allForIp = (command.size() < 3);
    auto port = allForIp ? 0 : static_cast<uint16_t>(std::stoul(command[2]));

    std::ostringstream oss;
    auto printUser = [&oss](CryptoSocket* sock)
    {
        oss << "  @ " << sock->m_socket_ptr->getRemoteAddress().toString() << " : "
            << sock->m_socket_ptr->getRemotePort() << "\n";
    };

    for (const auto& socket : sockets)
    {
        CryptoSocket* socketptr = *socket;
        if (socketptr && socketptr->m_socket_ptr->getRemoteAddress().toString() == ip
            && (allForIp || socketptr->m_socket_ptr->getRemotePort() == port))
        {
            printUser(socketptr);
            socketptr->m_socket_ptr->disconnect();
        }
    }

    if (!oss.str().empty())
    {
        std::cout << "Les utilisateurs suivants ont ete retires:\n";
        std::cout << oss.str();
    }

    return true;
}
