#include "general.h"

#include <filesystem>
#include <iostream>
#include <bit>


std::vector <std::string> split(const std::string& chaine, const char separateur, bool separateurMultiple)
{
    size_t i = 0;
    std::vector<std::string> out;

    while (chaine[i] != '\0')
    {
        size_t j = i;
        while (chaine[j] != separateur && chaine[j] != '\0')
        {
            j++;
        }
        if (!separateurMultiple || j != i)
        {
            const size_t taille = (j - i) + 1;
            char* mot = new char[taille];
            for (size_t k = 0; (k + i) < j; k++)
            {
                mot[k] = chaine[i + k];
            }
            mot[taille - 1] = '\0';
            out.emplace_back(mot);
            delete[] mot;
        }
        if (chaine[j] == '\0')
            break;
        i = j + 1;
    }
    return out;
}

std::vector<std::string> splitCommand(const std::string& commande) 
{
    size_t i = 0;
    std::vector<std::string> out;

    while (commande[i] != '\0')
    {
        size_t j = i;
        bool inSection = false;
        while ((commande[j] != ' ' || inSection) && commande[j] != '\0')
        {
            if (commande[j] == '"')
            {
                inSection = !inSection;
            }
            j++;
        }
        if ( j != i)
        {
            const size_t taille = (j - i) + 1;
            char* mot = new char[taille];
            for (size_t k = 0; (k + i) < j; k++)
            {
                mot[k] = commande[i + k];
            }
            mot[taille - 1] = '\0';
            out.push_back( split( std::string(mot), '"' ).back() );
            delete[] mot;
        }
        if (commande[j] == '\0')
            break;
        i = j + 1;
    }
    return out;
}

void createFolder(const std::string& path)
{
    if (!std::filesystem::exists(path))
        std::filesystem::create_directory(path);
}

void IgnoreLine()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void ManageEndian(uint8_t* const bytes_, const size_t nbrBytes_)
{
    if (std::endian::native == std::endian::big)
    {
        uint8_t* copyTable = new  uint8_t[nbrBytes_];
        std::copy(bytes_, bytes_ + nbrBytes_, copyTable);
        for (size_t i = 0; i < nbrBytes_ ;i++)
        {
            bytes_[i] = copyTable[nbrBytes_ - 1 - i];
        }
    }
}

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

void HideCursor(const bool hide_)
{
#ifdef _WIN32
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(consoleHandle, &info);
    if (hide_)
    {
        info.bVisible = FALSE;
    }
    else
    {
        info.bVisible = TRUE;
    }
    if (SetConsoleCursorInfo(consoleHandle, &info) == FALSE) throw;
#endif // _WIN32

    return;
}
