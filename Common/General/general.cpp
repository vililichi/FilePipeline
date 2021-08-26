#include "general.h"

#include <filesystem>
#include <iostream>

std::vector<std::string> split(std::string chaine, const char separateur,
                               bool separateurMultiple)
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

void createFolder(const std::string& path)
{
    if (!std::filesystem::exists(path))
        std::filesystem::create_directory(path);
}

void IgnoreLine()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
