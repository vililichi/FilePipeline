#include "Communication/command.h"
#include "General/general.h"
#include "General/list.h"
#include "General/load.h"
#include "cmds/cmds.h"
#include "repl/repl.h"

#include <iostream>
#include <mutex>
#include <thread>

#pragma region thread
void traitementPacket(CryptoSocket* csocket_ptr, Packet& pq)
{
    uint8_t commande;
    pq >> commande;
    switch (commande)
    {
        case command::Comm::list:
        {
            Packet reponse;
            std::vector<fileInfo> liste = list();
            reponse << liste.size();
            for (const auto& file : liste)
                reponse << file.name << file.size;
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
            if (filename == ".." || split(filename, '\\', false).size() > 1
                || split(filename, '/', false).size() > 1)
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
            if (newName == ".." || split(newName, '\\', false).size() > 1
                || split(newName, '/', false).size() > 1)
            {
                rep << (char)2;
            }
            else
            {
                for (const auto& file : files)
                {
                    if (file.name == oldName)
                    {
                        std::string old_path = DOWN_PATH;
                        old_path += "/" + oldName;
                        std::string new_path = DOWN_PATH;
                        new_path += "/" + newName;
                        if (std::rename(old_path.c_str(), new_path.c_str()) == 0)
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
            for (const auto& file : files)
            {
                if (file.name == name)
                {
                    std::string path = DOWN_PATH;
                    path += "/" + name;
                    if (std::remove(path.c_str()) == 0)
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
    *tcp_ptr = nullptr;
}
void listeningFunction(sf::TcpListener* listener_ptr,
                       std::vector<CryptoSocket**>* socketptr_List, std::mutex* mutex)
{
    std::vector<std::thread> socketThread;
    while (true)
    {
        CryptoSocket** new_socket_ptr = new CryptoSocket*;
        *new_socket_ptr = new CryptoSocket;
        (*new_socket_ptr)->m_socket_ptr = new sf::TcpSocket;
        sf::Socket::Status stat =
            listener_ptr->accept(*((*new_socket_ptr)->m_socket_ptr));
        if (stat == sf::Socket::Status::Error)
        {
            delete *new_socket_ptr;
            delete new_socket_ptr;
            break;
        }
        (*new_socket_ptr)->m_socket_ptr->setBlocking(true);
        socketThread.emplace_back(&socketFunction, new_socket_ptr);

        {
            std::lock_guard lock(*mutex);
            socketptr_List->push_back(new_socket_ptr);
        }
    }

    std::lock_guard lock(*mutex);

    for (size_t i = 0; i < socketptr_List->size(); i++)
    {
        if (*(*socketptr_List)[i])
            (*(*socketptr_List)[i])->m_socket_ptr->disconnect();
        if (socketThread[i].joinable())
            socketThread[i].join();
        delete (*socketptr_List)[i];
    }
}
#pragma endregion

void createFolders()
{
    createFolder(UP_PATH);
    createFolder(TRUST_PATH);
    createFolder(UNTRUST_PATH);
}

#pragma region socket
class SocketListener
{
public:
    virtual ~SocketListener()
    {
        listener.setBlocking(false);
        listener.close();
        if (listeningThread && listeningThread->joinable())
        {
            std::cout << "Fermeture en cours..." << std::endl;
            listeningThread->join();
        }
    }
    [[nodiscard]] std::pair<std::vector<CryptoSocket**>*, std::mutex*> connectListener()
    {
        listener.listen(getPort());
        std::cout << "Connecte au port [" << listener.getLocalPort() << "]\n"
                  << std::endl;

        listeningThread = std::make_unique<std::thread>(
            &listeningFunction, &listener, &socketPtrList, &socketPtrListMutex);

        return {&socketPtrList, &socketPtrListMutex};
    }

protected:
    virtual uint16_t getPort() = 0;

private:
    sf::TcpListener listener;
    std::unique_ptr<std::thread> listeningThread;
    std::vector<CryptoSocket**> socketPtrList;
    std::mutex socketPtrListMutex;
};

class InteractiveSocketListener : public SocketListener
{
protected:
    uint16_t getPort() override
    {
        uint16_t port;
        while (true)
        {
            // Obtention du port
            std::cout << "Port du serveur: " << std::flush;
            std::cin >> port;
            if (std::cin.fail())
            {
                std::cout << "Valeur invalide\n";
                std::cin.clear();
                ignoreLine();
                continue;
            }
            ignoreLine();
            break;
        }
        return port;
    }
};
#pragma endregion socket

#pragma region repl
std::unique_ptr<repl::Repl> createRepl(std::vector<CryptoSocket**>* socketList,
                                       std::mutex* mutex)
{
    using repl::ParamVec;

    auto clientRepl = std::make_unique<repl::Repl>();

    clientRepl->addExitCommand();
    clientRepl->addHelpCommand();
    clientRepl->setUnknownCommandString("Commande inconnue :");

    clientRepl->addCommand(
        "list", [](const ParamVec& command) -> bool { return cmd::listCmd(command); },
        "Liste les fichiers telechargeables", {"ls"});

    clientRepl->addCommand("user",
                           [socketList, mutex](const ParamVec& command) -> bool
                           { return cmd::userCmd(*socketList, *mutex, command); },
                           "Liste les utilisateurs sous la forme \"ip | port | acces\"",
                           {"u"});

    clientRepl->addCommand(
        "key", [mutex](const ParamVec& command) -> bool { return cmd::keyCmd(command); },
        "Liste toutes les cles d'acces");

    clientRepl->addCommand(
        "trust", [](const ParamVec& command) -> bool { return cmd::trustCmd(command); },
        "Accorde l'acces au detenteur de la cle <cle>", {"t"}, "<cle>");

    clientRepl->addCommand(
        "untrust",
        [](const ParamVec& command) -> bool { return cmd::untrustCmd(command); },
        "Retire l'acces au detenteur de la cle <cle>", {"ut"}, "<cle>");

    clientRepl->addCommand(
        "kick",
        [socketList, mutex](const ParamVec& command) -> bool
        { return cmd::kickCmd(*socketList, *mutex, command); },
        "Deconnecte les utilisateurs pour une <ip> donnee. Si le [port] est specifie, ne "
        "deconnecte que l'utilisateur utilisant ce port",
        {}, "<ip> [port]");

    return clientRepl;
}
#pragma endregion repl

int main()
{
    // creation des répertoires utiles
    createFolders();

    std::unique_ptr<SocketListener> socketListener =
        std::make_unique<InteractiveSocketListener>();
    auto [sockets, mutex] = socketListener->connectListener();

    // repl
    auto replPtr = createRepl(sockets, mutex);

    std::string commande = "h";
    std::vector<std::string> p_commande = splitCommand(commande);

    bool shouldContinue = true;

    while (shouldContinue)
    {
        shouldContinue = replPtr->parse(p_commande);

        if (shouldContinue)
        {
            std::cout << std::endl;
            replPtr->printPrompt();
            std::getline(std::cin, commande);
            p_commande = splitCommand(commande);
            std::cout << std::endl;
        }
    }

    return 0;
}
