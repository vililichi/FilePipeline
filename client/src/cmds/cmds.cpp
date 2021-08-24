#include "cmds.h"

#include "Communication/command.h"
#include "General/general.h"
#include "General/list.h"
#include "General/load.h"

#include <iostream>
#include <vector>

using namespace cmd;

bool cmd::listCmd(CryptoSocket* cSocket, const ParamVec& command)
{
    std::cout << "Fichiers locaux :\n";
    std::vector<fileInfo> listeLoc = list();
    for (const auto& fichier : listeLoc)
    {
        std::cout << fichier.name << '\t' << fichier.size << "\n";
    }
    std::cout << "\n";

    std::cout << "Fichiers distants :\n";
    std::vector<fileInfo> listeServ = getlist(cSocket);
    for (const auto& fichier : listeServ)
    {
        std::cout << fichier.name << '\t' << fichier.size << "\n";
    }
    std::cout.flush();

    return true;
}

bool cmd::downloadCmd(CryptoSocket* cSocket, const ParamVec& command)
{
    if (command.size() < 2)
    {
        std::cout << "Argument manquant" << std::endl;
    }
    else
    {
        download(cSocket, command[1], DOWN_PATH, true);
    }

    return true;
}

bool cmd::uploadCmd(CryptoSocket* cSocket, const ParamVec& command)
{
    if (command.size() < 2)
    {
        std::cout << "Argument manquant" << std::endl;
    }
    else
    {
        uploadDemand(cSocket, command[1], UP_PATH);
    }

    return true;
}

static bool renameLocalCmd(CryptoSocket* cSocket, const ParamVec& command)
{
    std::vector<fileInfo> files = list(UP_PATH);
    bool found = false;

    if (command[3] == ".." || split(command[3], '\\', false).size() > 1
        || split(command[3], '/', false).size() > 1)
    {
        std::cout << "<nom> n'est pas un nom de fichier valide" << std::endl;
        return true;
    }

    for (const auto& fichier : files)
    {
        if (fichier.name == command[2])
        {
            std::string old_path = UP_PATH;
            old_path += "/" + command[2];
            std::string new_path = UP_PATH;
            new_path += "/" + command[3];
            if (std::rename(old_path.c_str(), new_path.c_str()) == 0)
            {
                std::remove(old_path.c_str());
                found = true;
            }
            break;
        }
    }

    if (!found)
    {
        std::cout << "Fichier [" << command[2] << "] introuvable" << std::endl;
    }
    else
    {
        std::cout << "Fichier [" << command[2] << "] renomme avec succes en ["
                  << command[3] << "]" << std::endl;
    }

    return true;
}
static bool renameServerCmd(CryptoSocket* cSocket, const ParamVec& command)
{
    Packet packet;
    packet << command::Comm::rename << command[2] << command[3];
    cSocket->send(packet);

    packet.move(0);
    packet = cSocket->receive();
    char reussite = 0;
    packet >> reussite;

    if (reussite == 0)
    {
        std::cout << "Fichier [" << command[2] << "] renomme avec succes en ["
                  << command[3] << "]" << std::endl;
    }
    else if (reussite == 1)
    {
        std::cout << "Fichier [" << command[2] << "] introuvable" << std::endl;
    }
    else if (reussite == 2)
    {
        std::cout << "<nom> n'est pas un nom de fichier valide" << std::endl;
    }
    else
    {
        std::cout << "Erreur inconnue" << std::endl;
    }

    return true;
}

bool cmd::renameCmd(CryptoSocket* cSocket, const ParamVec& command)
{
    if (command.size() < 4)
    {
        std::cout << "Argument invalide: attendu [3] -> recu [" << command.size() - 1
                  << "]" << std::endl;
    }
    else if (command[1] == "lc" || command[1] == "local")
    {
        return renameLocalCmd(cSocket, command);
    }
    else if (command[1] == "sv" || command[1] == "server")
    {
        return renameServerCmd(cSocket, command);
    }
    else
    {
        std::cout << "Argument 1 invalide: doit être parmi [local, lc, server, sv]"
                  << std::endl;
    }

    return true;
}

static bool removeLocalCmd(CryptoSocket* cSocket, const ParamVec& command)
{
    std::vector<fileInfo> files = list(UP_PATH);
    bool found = false;

    for (const auto& fichier : files)
    {
        if (fichier.name == command[2])
        {
            std::string path = UP_PATH;
            path += "/" + command[2];
            if (std::remove(path.c_str()) == 0)
            {
                found = true;
            }
            break;
        }
    }

    if (!found)
    {
        std::cout << "Fichier [" << command[2] << "] introuvable" << std::endl;
    }
    else
    {
        std::cout << "Fichier [" << command[2] << "] supprime avec succes" << std::endl;
    }

    return true;
}

static bool removeServerCmd(CryptoSocket* cSocket, const ParamVec& command)
{
    Packet packet;
    packet << command::Comm::remove << command[2];
    cSocket->send(packet);

    packet.move(0);
    packet = cSocket->receive();
    bool reussite = 0;
    packet >> reussite;

    if (reussite)
    {
        std::cout << "Fichier [" << command[2] << "] supprime avec succes" << std::endl;
    }
    else
    {
        std::cout << "Fichier [" << command[2] << "] introuvable" << std::endl;
    }

    return true;
}

bool cmd::removeCmd(CryptoSocket* cSocket, const ParamVec& command)
{
    if (command.size() < 3)
    {
        std::cout << "Argument invalide: attendu [2] -> recu [" << command.size() - 1
                  << "]" << std::endl;
    }
    else if (command[1] == "lc" || command[1] == "local")
    {
        return removeLocalCmd(cSocket, command);
    }
    else if (command[1] == "sv" || command[1] == "server")
    {
        return removeServerCmd(cSocket, command);
    }
    else
    {
        std::cout << "Argument 1 invalide: doit être parmi [local, lc, server, sv]"
                  << std::endl;
    }

    return true;
}
