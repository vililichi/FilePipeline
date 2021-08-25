// AVCs_5.cpp : définit le point d'entrée de l'application.
//

#include "Communication/command.h"
#include "General/General.h"
#include "General/List.h"
#include "General/Load.h"
#include "cmds/cmds.h"
#include "repl/repl.h"

#include <fstream>
#include <iostream>
#include <thread>

constexpr uint32_t MIN_INFT = 2147483648;
constexpr uint32_t MAX_INFT = std::numeric_limits<uint32_t>::max();

void createFolders()
{
    createFolder(DOWN_PATH);
    createFolder(UP_PATH);
}


#pragma region cle
std::pair<RSA::cle, RSA::cle> generateKeys()
{
    std::cout << "Creation de la cle en cours...\n";
    std::cout << "Cette operation peut prendre beaucoup de temps.\n";
    std::cout << "Elle peut aussi ne jamais finir." << std::endl;

    std::pair<RSA::cle, RSA::cle> cles = RSA::generation();

    std::cout << "Fin de la generation de la cle.\n" << std::endl;

    return cles;
}

void storeKeys(const std::string& publicKeyPath, const std::string& privateKeyPath,
               const RSA::cle& publicKey, const RSA::cle& privateKey)
{
    RSA::storeKeySet(privateKey, publicKey, privateKeyPath);
    RSA::storeKey(publicKey, publicKeyPath);
}

std::pair<RSA::cle, RSA::cle> makeAndStoreKey(const std::string& publicKeyPath,
                                              const std::string& privateKeyPath)
{
    auto [clePublique, clePrivee] = generateKeys();
    storeKeys(publicKeyPath, privateKeyPath, clePublique, clePrivee);

    return {clePublique, clePrivee};
}

std::pair<RSA::cle, RSA::cle> getKeys(const std::string& publicKeyPath,
                                      const std::string& privateKeyPath)
{
    RSA::cle clePrivee;
    RSA::cle clePublique;

    if (RSA::getKeySet(clePrivee, clePublique, privateKeyPath))
    {
        return {clePrivee, clePublique};
    }

    std::cout << "Aucune cle n'a ete trouvee..." << std::endl;

    return makeAndStoreKey(publicKeyPath, privateKeyPath);
}

bool testKey(const RSA::cle& publicKey, const RSA::cle& privateKey)
{
    inft valTest = randinft(MIN_INFT, MAX_INFT);
    inft cpy = valTest;
    RSA::cryptage(cpy, publicKey);
    RSA::decryptage(cpy, privateKey);

    bool isValidKey = (cpy == valTest);

    if (!isValidKey)
    {
        std::cout << "Cle non valide. Regeneration...\n";
    }

    return isValidKey;
}
#pragma endregion cle

#pragma region socket
class SocketInfo
{
public:
    virtual ~SocketInfo() = default;
    std::unique_ptr<CryptoSocket> connectSocket(const RSA::cle& clePublique,
                                                const RSA::cle& clePrivee)
    {
        std::unique_ptr<CryptoSocket> cSocket;
        bool connected = false;

        while (!connected)
        {
            connectTcp();
            cSocket = std::make_unique<CryptoSocket>();
            cSocket->m_socket_ptr = &m_tSocket;
            connected = cSocket->sendHandShake(clePublique, clePrivee);

            if (connected)
                std::cout << "Connection etablie\n" << std::endl;
            else
                std::cout << "Erreur lors de l'echange de cle.\n" << std::endl;
        }
        return cSocket;
    }

protected:
    sf::TcpSocket& getSocket() { return m_tSocket; };

    virtual void connectTcp() = 0;

private:
    sf::TcpSocket m_tSocket;
};

class InteractiveSocketInfo : public SocketInfo
{
protected:
    void connectTcp() override
    {
        bool connected = false;

        while (!connected)
        {
            std::string ip;
            short port;
            std::cout << "Adresse IP de l'hebergeur: " << std::flush;
            std::cin >> ip;
            std::cout << "Port du serveur: " << std::flush;
            std::cin >> port;
            std::cin.ignore();

            connected = (getSocket().connect(ip, port) == sf::Socket::Status::Done);
            if (!connected)
                std::cout << "Erreur de connection...\n" << std::endl;
        }
    }
};
#pragma endregion socket

#pragma region repl
std::unique_ptr<repl::Repl> createRepl(CryptoSocket* cSocket)
{
    using repl::ParamVec;

    auto clientRepl = std::make_unique<repl::Repl>();

    clientRepl->addExitCommand();
    clientRepl->addHelpCommand();
    clientRepl->setUnknownCommandString("Commande inconnue :");

    clientRepl->addCommand("list",
                           [cSocket](const ParamVec& command) -> bool
                           { return cmd::listCmd(cSocket, command); },
                           "Liste les fichiers telechargeables et televersables", {"ls"});

    clientRepl->addCommand(
        "download",
        [cSocket](const ParamVec& command) -> bool
        { return cmd::downloadCmd(cSocket, command); },
        "Telecharge le fichier <fichier>", {"dwn"}, "<fichier>");

    clientRepl->addCommand(
        "upload",
        [cSocket](const ParamVec& command) -> bool
        { return cmd::uploadCmd(cSocket, command); },
        "Televerse le fichier <fichier>", {"up"}, "<fichier>");

    clientRepl->addCommand(
        "rename",
        [cSocket](const ParamVec& command) -> bool
        { return cmd::renameCmd(cSocket, command); },
        "Renomme le fichier <fichier> avec <nom>", {"rn"},
        "[local {lc}|server {sv}] <fichier> <nom>");

    clientRepl->addCommand(
        "remove",
        [cSocket](const ParamVec& command) -> bool
        { return cmd::removeCmd(cSocket, command); },
        "Supprime le fichier <fichier>", {"rm"}, "[local {lc}|server {sv}] <fichier>");

    return clientRepl;
}
#pragma endregion repl

int main()
{
    createFolders();

    // cle
    const std::string publicKeyPath {"public"};
    const std::string privateKeyPath {"private"};

    auto [clePublique, clePrivee] = getKeys(publicKeyPath, privateKeyPath);

    while (!testKey(clePublique, clePrivee))
    {
        std::tie(clePublique, clePrivee) = makeAndStoreKey(publicKeyPath, privateKeyPath);
    }

    // socket
    std::unique_ptr<SocketInfo> socketInfo = std::make_unique<InteractiveSocketInfo>();
    auto cSocket = socketInfo->connectSocket(clePublique, clePrivee);

    // repl
    auto replPtr = createRepl(cSocket.get());

    std::string commande = "h";
    std::vector<std::string> p_commande = split(commande, ' ');

    bool shouldContinue = true;

    while (shouldContinue)
    {
        shouldContinue = replPtr->parse(p_commande);

        if (shouldContinue)
        {
            std::cout << std::endl;
            replPtr->printPrompt();
            std::getline(std::cin, commande);
            p_commande = split(commande, ' ', true);
            std::cout << std::endl;
        }
    }

    // while (!(p_commande.size() > 0 && p_commande[0] == "exit")
    //        && !(p_commande.size() > 0 && p_commande[0] == "e"))
    // {
    //     if (p_commande.size() == 0)
    //         ;
    //     // help
    //     else if (p_commande[0] == "h" || p_commande[0] == "help")
    //     {
    //         std::cout << "download [fichier] : telecharge le fichier" << std::endl;
    //         std::cout << "exit : ferme le programme" << std::endl;
    //         std::cout << "help : liste toutes les commandes" << std::endl;
    //         std::cout << "list : liste tout les fichiers telechargeables et
    //         televersables"
    //                   << std::endl;
    //         std::cout << "remove [local|server] [fichier] : supprime le fichier"
    //                   << std::endl;
    //         std::cout
    //             << "rename [local|server] [fichier] [nom] : renomme le fichier avec
    //             nom"
    //             << std::endl;
    //         std::cout << "upload [fichier] : televerse le fichier" << std::endl;
    //     }
    //     // liste
    //     else if (p_commande[0] == "ls" || p_commande[0] == "list")
    //     {
    //         std::cout << "fichiers locaux :" << std::endl;
    //         std::vector<fileInfo> listeLoc = list();
    //         for (size_t i = 0; i < listeLoc.size(); i++)
    //             std::cout << listeLoc[i].name << '\t' << listeLoc[i].size << std::endl;
    //         std::cout << std::endl;
    //         std::cout << "fichiers serveurs :" << std::endl;
    //         std::vector<fileInfo> listeServ = getlist(&cSocket);
    //         for (size_t i = 0; i < listeServ.size(); i++)
    //             std::cout << listeServ[i].name << '\t' << listeServ[i].size <<
    //             std::endl;
    //     }
    //     // download
    //     else if (p_commande[0] == "dwn" || p_commande[0] == "download")
    //     {
    //         if (p_commande.size() < 2)
    //             std::cout << "argument manquant" << std::endl;
    //         else
    //         {
    //             download(&cSocket, p_commande[1], DOWN_PATH, true);
    //         }
    //     }
    //     // upload
    //     else if (p_commande[0] == "up" || p_commande[0] == "upload")
    //     {
    //         if (p_commande.size() < 2)
    //             std::cout << "argument manquant" << std::endl;
    //         else
    //         {
    //             uploadDemand(&cSocket, p_commande[1], UP_PATH);
    //         }
    //     }
    //     // rename
    //     else if (p_commande[0] == "rn" || p_commande[0] == "rename")
    //     {
    //         if (p_commande.size() >= 4)
    //         {
    //             if (p_commande[1] == "lc" || p_commande[1] == "local")
    //             {
    //                 std::vector<fileInfo> files = list(UP_PATH);
    //                 bool find = false;
    //                 if (p_commande[3] == ".."
    //                     || split(p_commande[3], '\\', false).size() > 1
    //                     || split(p_commande[3], '/', false).size() > 1)
    //                 {
    //                     std::cout << "nom invalide" << std::endl;
    //                 }
    //                 else
    //                 {
    //                     for (size_t i = 0; i < files.size(); i++)
    //                     {
    //                         if (files[i].name == p_commande[2])
    //                         {
    //                             std::string old_path = UP_PATH;
    //                             old_path += "/" + p_commande[2];
    //                             std::string new_path = UP_PATH;
    //                             new_path += "/" + p_commande[3];
    //                             if (std::rename(old_path.c_str(), new_path.c_str()) ==
    //                             0)
    //                             {
    //                                 std::remove(old_path.c_str());
    //                                 find = true;
    //                             }
    //                             break;
    //                         }
    //                     }
    //                     if (!find)
    //                     {
    //                         std::cout << "fichier introuvable" << std::endl;
    //                     }
    //                     else
    //                     {
    //                         std::cout << "succes" << std::endl;
    //                     }
    //                 }
    //             }
    //             else if (p_commande[1] == "sv" || p_commande[1] == "server")
    //             {
    //                 Packet packet;
    //                 packet << command::Comm::rename << p_commande[2] << p_commande[3];
    //                 cSocket.send(packet);

    //                 packet.move(0);
    //                 packet = cSocket.receive();
    //                 char reussite = 0;
    //                 packet >> reussite;

    //                 if (reussite == 0)
    //                     std::cout << "succes" << std::endl;
    //                 else if (reussite == 1)
    //                     std::cout << "fichier introuvable" << std::endl;
    //                 else if (reussite == 2)
    //                     std::cout << "nom invalide" << std::endl;
    //                 else
    //                     std::cout << "erreur" << std::endl;
    //             }
    //             else
    //             {
    //                 std::cout << "argument 1 invalide" << std::endl;
    //             }
    //         }
    //         else
    //         {
    //             std::cout << "arguments invalides" << std::endl;
    //         }
    //     }
    //     // remove
    //     else if (p_commande[0] == "rm" || p_commande[0] == "remove")
    //     {
    //         if (p_commande.size() >= 3)
    //         {
    //             if (p_commande[1] == "lc" || p_commande[1] == "local")
    //             {
    //                 std::vector<fileInfo> files = list(UP_PATH);
    //                 bool find = false;
    //                 for (size_t i = 0; i < files.size(); i++)
    //                 {
    //                     if (files[i].name == p_commande[2])
    //                     {
    //                         std::string path = UP_PATH;
    //                         path += "/" + p_commande[2];
    //                         if (std::remove(path.c_str()) == 0)
    //                         {
    //                             find = true;
    //                         }
    //                         break;
    //                     }
    //                 }
    //                 if (!find)
    //                 {
    //                     std::cout << "fichier introuvable" << std::endl;
    //                 }
    //                 else
    //                 {
    //                     std::cout << "succes" << std::endl;
    //                 }
    //             }
    //             else if (p_commande[1] == "sv" || p_commande[1] == "server")
    //             {
    //                 Packet packet;
    //                 packet << command::Comm::remove << p_commande[2];
    //                 cSocket.send(packet);

    //                 packet.move(0);
    //                 packet = cSocket.receive();
    //                 bool reussite = 0;
    //                 packet >> reussite;

    //                 if (reussite)
    //                     std::cout << "succes" << std::endl;
    //                 else
    //                     std::cout << "fichier introuvable" << std::endl;
    //             }
    //             else
    //             {
    //                 std::cout << "argument 1 invalide" << std::endl;
    //             }
    //         }
    //         else
    //         {
    //             std::cout << "arguments invalides" << std::endl;
    //         }
    //     }

    //     // prise de commande
    //     std::cout << std::endl;
    //     std::cout << "inserer une commande" << std::endl;
    //     std::getline(std::cin, commande);
    //     p_commande = split(commande, ' ', true);
    //     std::cout << std::endl;
    // }
    return 0;
}
