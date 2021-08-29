// AVCs_5.cpp : définit le point d'entrée de l'application.
//

#include "Communication/command.h"
#include "General/general.h"
#include "General/list.h"
#include "cmds/cmds.h"
#include "repl/repl.h"

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
            sf::IpAddress ip;
            while (true)
            {
                // Obtention de l'ip sous forme de chaine
                std::string strIp;
                std::cout << "Adresse IP de l'hebergeur: " << std::flush;
                std::cin >> strIp;
                if (std::cin.fail())
                {
                    std::cout << "Valeur invalide\n";
                    std::cin.clear();
                    ignoreLine();
                    continue;
                }
                ignoreLine();

                // Convertion de la chaine en ip
                ip = sf::IpAddress(strIp);
                if (ip.toInteger() == 0)
                {
                    std::cout << "Valeur invalide\n";
                    continue;
                }

                break;
            }

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

            // Connection
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
        "<local{lc}|server{sv}> <fichier> <nom>");

    clientRepl->addCommand(
        "remove",
        [cSocket](const ParamVec& command) -> bool
        { return cmd::removeCmd(cSocket, command); },
        "Supprime le fichier <fichier>", {"rm"}, "<local{lc}|server{sv}> <fichier>");

    clientRepl->addCommand(
        "index",
        [cSocket](const ParamVec& command) -> bool
        { return cmd::indexCmd(cSocket, command); },
        "Telecharge ou upload un fichier selon son [index], si l'[index] n'est pas specifie, la liste sera presentee à l'utilisateur et l'index sera ensuite demande.", { "id" }, "<upload{up}|download{dwn}> [index]");

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
