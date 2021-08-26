#include "inft.h"

#include "primeList.h"

#include <algorithm>
#include <cstdlib>
#include <random>


void inft::cut()
{
    size_t i = m_nbrLong - 1;
    while (i > 0)
    {
        if (m_valeur[i] == 0)
        {
            i--;
            m_nbrLong -= 1;
        }
        else
            break;
    }
}
// constructeur et destructeur
inft::inft() : m_nbrLong(1), m_negatif(false)
{
    m_valeur = new uint32_t[m_nbrLong];
    m_valeur[0] = 0;
}
inft::inft(uint32_t* const valeur_, size_t nbrLong_, bool negatif_)
    : m_nbrLong(nbrLong_), m_negatif(negatif_)
{
    m_valeur = new uint32_t[m_nbrLong];
    std::copy(valeur_, valeur_ + m_nbrLong, m_valeur);
    cut();
}
inft::inft(uint16_t* const valeur_, size_t nbrShort_, bool negatif_) : m_negatif(negatif_)
{
    if (nbrShort_ & 1)
    {
        m_nbrLong = (nbrShort_ + 1) / 2;
        m_valeur = new uint32_t[m_nbrLong];
        m_valeur[m_nbrLong - 1] = 0;
    }
    else
    {
        m_nbrLong = nbrShort_ / 2;
        m_valeur = new uint32_t[m_nbrLong];
    }
    uint16_t* valShort = (uint16_t*)m_valeur;
    std::copy(valeur_, valeur_ + nbrShort_, valShort);
}
inft::inft(uint8_t* const valeur_, size_t nbrOctet_, bool negatif_) : m_negatif(negatif_)
{
    if (nbrOctet_ % 4 != 0)
    {
        m_nbrLong = (nbrOctet_ / 2) + 1;
        m_valeur = new uint32_t[m_nbrLong];
        m_valeur[m_nbrLong - 1] = 0;
    }
    else
    {
        m_nbrLong = nbrOctet_ / 4;
        m_valeur = new uint32_t[m_nbrLong];
    }
    uint8_t* valChar = (uint8_t*)m_valeur;
    std::copy(valeur_, valeur_ + nbrOctet_, valChar);
}
inft::inft(const inft& val_) : m_nbrLong(val_.m_nbrLong), m_negatif(val_.m_negatif)
{
    m_valeur = new uint32_t[m_nbrLong];
    std::copy(val_.m_valeur, val_.m_valeur + m_nbrLong, m_valeur);
    cut();
}
inft::inft(inft&& val_) noexcept
    : m_nbrLong(val_.m_nbrLong), m_valeur(val_.m_valeur), m_negatif(val_.m_negatif)
{
    val_.m_valeur = nullptr;
    val_.m_nbrLong = 0;

    cut();
}
inft::inft(const uint32_t val_)
{
    m_nbrLong = 1;

    m_valeur = new uint32_t[m_nbrLong];

    m_valeur[0] = val_;

    m_negatif = false;
    cut();
}
inft::inft(const int32_t val_)
{
    int32_t mval = val_;
    if (val_ < 0)
    {
        m_negatif = true;
        mval *= -1;
    }
    else
        m_negatif = false;

    m_nbrLong = 1;
    m_valeur = new uint32_t[m_nbrLong];

    m_valeur[0] = mval;
    cut();
}

inft::~inft()
{
    delete[] m_valeur;
}

// opérateur d'assignation

inft& inft::operator=(const inft& val_)
{
    if (this == &val_)
        return *this;
    m_nbrLong = val_.m_nbrLong;

    delete[] m_valeur;
    m_valeur = new uint32_t[m_nbrLong];

    std::copy(val_.m_valeur, val_.m_valeur + m_nbrLong, m_valeur);

    m_negatif = val_.m_negatif;
    cut();

    return *this;
}

inft& inft::operator=(inft&& val_) noexcept
{
    m_nbrLong = val_.m_nbrLong;

    delete[] m_valeur;
    m_valeur = val_.m_valeur;
    val_.m_valeur = nullptr;
    val_.m_nbrLong = 0;

    m_negatif = val_.m_negatif;

    cut();
    return *this;
}

inft& inft::operator=(const uint32_t val_)
{
    m_nbrLong = 1;

    delete[] m_valeur;
    m_valeur = new uint32_t[m_nbrLong];

    m_valeur[0] = val_;

    m_negatif = false;

    cut();
    return *this;
}
inft& inft::operator=(const int32_t val_)
{
    int32_t mval = val_;
    if (val_ < 0)
    {
        m_negatif = true;
        mval = (mval * -1);
    }
    else
        m_negatif = false;

    m_nbrLong = 1;

    delete[] m_valeur;
    m_valeur = new uint32_t[m_nbrLong];

    m_valeur[0] = mval;

    cut();
    return *this;
}


// pour débugage
std::ostream& operator<<(std::ostream& os_, const inft& val_)
{
    if (val_ == inft())
        os_ << "0";
    else
    {
        if (val_.isNegatif())
            os_ << '-';
        os_ << " ";
        inft cpy = val_.abs();
        int pow10 = 0;
        while (cpy > 0)
        {
            cpy = cpy / 10;
            pow10++;
        }

        cpy = val_;
        for (int i = pow10 - 1; i >= 0; i--)
        {
            inft aff = cpy / inft(10).pow(i);
            cpy = cpy % inft(10).pow(i);
            os_ << aff[0];
        }
    }
    return os_;
}

// operateur de comparaison
const bool inft::operator<(const inft& val_) const
{
    if (m_negatif && !val_.m_negatif)
        return true;
    else if (val_.m_negatif && !m_negatif)
        return false;
    else if (!m_negatif)
    {
        if (m_nbrLong < val_.m_nbrLong)
            return true;
        else if (m_nbrLong > val_.m_nbrLong)
            return false;
        else
            for (size_t i = m_nbrLong; i > 0;)
            {
                i--;
                if (m_valeur[i] < val_.m_valeur[i])
                    return true;
                else if (m_valeur[i] > val_.m_valeur[i])
                    return false;
            }
    }
    else
    {
        if (m_nbrLong > val_.m_nbrLong)
            return true;
        else if (m_nbrLong < val_.m_nbrLong)
            return false;
        else
            for (size_t i = m_nbrLong; i > 0;)
            {
                i--;
                if (m_valeur[i] > val_.m_valeur[i])
                    return true;
                else if (m_valeur[i] < val_.m_valeur[i])
                    return false;
            }
    }
    return false;
}
const bool inft::operator>(const inft& val_) const
{
    if (m_negatif && !val_.m_negatif)
        return false;
    else if (val_.m_negatif && !m_negatif)
        return true;
    else if (m_negatif)
    {
        if (m_nbrLong < val_.m_nbrLong)
            return true;
        else if (m_nbrLong > val_.m_nbrLong)
            return false;
        else
            for (size_t i = m_nbrLong; i > 0;)
            {
                i--;
                if (m_valeur[i] < val_.m_valeur[i])
                    return true;
                else if (m_valeur[i] > val_.m_valeur[i])
                    return false;
            }
    }
    else
    {
        if (m_nbrLong > val_.m_nbrLong)
            return true;
        else if (m_nbrLong < val_.m_nbrLong)
            return false;
        else
            for (size_t i = m_nbrLong; i > 0;)
            {
                i--;
                if (m_valeur[i] > val_.m_valeur[i])
                    return true;
                else if (m_valeur[i] < val_.m_valeur[i])
                    return false;
            }
    }
    return false;
}
const bool inft::operator==(const inft& val_) const
{
    if (m_negatif != val_.m_negatif)
        return false;
    if (m_nbrLong != val_.m_nbrLong)
        return false;
    for (size_t i = 0; i < m_nbrLong; i++)
        if (m_valeur[i] != val_.m_valeur[i])
            return false;
    return true;
}
const bool inft::operator<=(const inft& val_) const
{
    return !operator>(val_);
}
const bool inft::operator>=(const inft& val_) const
{
    return !operator<(val_);
}

// arithmétique
inft additiveComp(const inft& A_, const inft& B_, const bool invB_)
{
    // positif + negatif ou negatif + positif
    if (A_.isNegatif() != B_.isNegatif())
    {
        // comparaison de taille
        const inft* big = NULL;
        const inft* small = NULL;

        const bool thisNeg = A_.m_negatif;
        const bool valNeg = B_.m_negatif;

        const_cast<inft*>(&A_)->m_negatif = false;
        const_cast<inft*>(&B_)->m_negatif = false;

        if (A_ == B_)
            return inft();
        else if (A_ > B_)
        {
            big = &A_;
            small = &B_;
        }
        else
        {
            big = &B_;
            small = &A_;
        }

        const_cast<inft*>(&A_)->m_negatif = thisNeg;
        const_cast<inft*>(&B_)->m_negatif = valNeg;

        // transformation int
        inft out(big->size(), big->isNegatif());

        // soustraction
        int64_t buffer = 0;
        for (size_t i = 0; i < big->size(); i++)
        {
            buffer += big->m_valeur[i];
            if (i < small->size())
                buffer -= small->m_valeur[i];
            if (buffer < 0)
            {
                buffer += 4294967296;
                out.m_valeur[i] = *(uint32_t*)&buffer;
                buffer = -1;
            }
            else
            {
                out.m_valeur[i] = *(uint32_t*)&buffer;
                buffer = 0;
            }
        }

        out.cut();
        if (invB_)
            const_cast<inft*>(&B_)->m_negatif = !const_cast<inft*>(&B_)->m_negatif;
        return out;
    }

    // positif + positif ou negatif + negatif
    else
    {
        // mesure de la taille des valeurs
        size_t max = B_.m_nbrLong;
        if (A_.m_nbrLong > B_.m_nbrLong)
            max = A_.m_nbrLong;

        // création de l'espace nécessaire
        inft out(max + 1, A_.m_negatif);

        uint64_t buffer = 0;
        for (size_t i = 0; i < max + 1; i++)
        {
            buffer = buffer >> 32;
            if (i < A_.m_nbrLong)
                buffer += A_.m_valeur[i];
            if (i < B_.m_nbrLong)
                buffer += B_.m_valeur[i];
            out.m_valeur[i] = *(uint32_t*)&buffer;
        }

        out.cut();
        if (invB_)
            const_cast<inft*>(&B_)->m_negatif = !const_cast<inft*>(&B_)->m_negatif;
        return out;
    }
}

void inft::operator--()
{
    if (*this == 0 || m_negatif)
    {
        *this = *this + inft(-1);
    }
    else
    {
        size_t i = 0;
        while (true)
        {
            if (m_valeur[i] == 0)
            {
                m_valeur[i] = 4294967295;
                i++;
            }
            else
            {
                m_valeur[i]--;
                break;
            }
        }
    }
}

inft inft::operator*(const inft& val_) const
{
    inft out(isNegatif() ^ val_.isNegatif(), m_nbrLong + val_.m_nbrLong);

    for (size_t i = 0; i < m_nbrLong; i++)
        for (size_t j = 0; j < val_.m_nbrLong; j++)
        {
            uint64_t produit = (uint64_t)m_valeur[i] * val_.m_valeur[j];

            size_t k = 0;
            while (produit != 0)
            {
                produit += out.m_valeur[i + j + k];
                out.m_valeur[i + j + k] = *(uint32_t*)&produit;
                produit = produit >> 32;
                k++;
            }
        }

    out.cut();
    return out;
}

inft inft::operator/(const inft& val_) const
{
    if (abs() < val_.abs())
        return inft(); // réponse triviale

    // assignation de l'espace
    inft cpy((*this).abs());


    // taille de la réponse
    size_t sizeThis = m_nbrLong << 1;
    if (((uint16_t*)m_valeur)[sizeThis - 1] == 0)
        sizeThis--;


    // dénominateur raccourcie
    size_t sizeDenom = val_.m_nbrLong << 1;
    uint16_t* shortDenom = (uint16_t*)val_.m_valeur;
    if (shortDenom[sizeDenom - 1] == 0)
        sizeDenom--;
    int64_t denom = (uint64_t)shortDenom[sizeDenom - 1] << 16;
    if (sizeDenom >= 2)
        denom += shortDenom[sizeDenom - 2];

    // assignation de l'espace
    size_t diffS = (sizeThis - sizeDenom) + 1;
    uint16_t* cptr = new uint16_t[diffS]; // char* contenant la réponse

    // Pour chaqu'un des éléments de la division
    for (size_t i = diffS, j = 0; i > 0;)
    {
        // décrémentation
        i--;
        // numérateur raccourcie
        size_t sizeNum = cpy.m_nbrLong << 1;
        uint16_t* shortNum = (uint16_t*)cpy.m_valeur;
        uint64_t num = 0;
        if (sizeThis - j - 1 < sizeNum)
            num += (uint64_t)shortNum[sizeThis - j - 1] << 16;
        if (sizeThis - j < sizeNum)
            num += (uint64_t)shortNum[sizeThis - j] << 32;
        if (sizeDenom >= 2 && sizeThis - j - 2 < sizeNum)
            num += shortNum[sizeThis - j - 2];

        // estimation
        int64_t ps = (num / denom) - 1;

        // valeur réelle
        if (ps >= 0)
        {
            // création du facteur de test
            uint16_t* facptr;
            size_t tailleFact = sizeDenom + i;
            facptr = new uint16_t[sizeDenom + i];
            for (size_t k = 0; k < i; k++)
                facptr[k] = 0;
            for (size_t k = 0; k < sizeDenom; k++)
                facptr[k + i] = shortDenom[k];

            inft fact(facptr, tailleFact);
            delete[] facptr;

            // test avec prédiction
            cpy = cpy - (inft((int32_t)ps) * fact);

            // test exterieur à prediction
            inft limit = cpy - fact;
            if (!limit.isNegatif())
            {
                cpy.swap(limit);
                ps++;
            }
            cptr[i] = (uint16_t)ps;
        }
        else
        {
            cptr[i] = 0;
        }

        // incrémentation
        j++;
    }

    inft out(cptr, diffS, isNegatif() ^ val_.isNegatif());
    delete[] cptr;
    return out;
}
inft inft::operator%(const inft& val_) const
{
    if (abs() < val_.abs())
        return *this; // réponse triviale
    // assignation de l'espace
    inft cpy((*this).abs());

    // taille de la réponse
    size_t sizeThis = m_nbrLong << 1;
    if (((uint16_t*)m_valeur)[sizeThis - 1] == 0)
        sizeThis--;


    // dénominateur raccourcie
    size_t sizeDenom = val_.m_nbrLong << 1;
    uint16_t* shortDenom = (uint16_t*)val_.m_valeur;
    if (shortDenom[sizeDenom - 1] == 0)
        sizeDenom--;
    int64_t denom = (uint64_t)shortDenom[sizeDenom - 1] << 16;
    if (sizeDenom >= 2)
        denom += shortDenom[sizeDenom - 2];

    // taille reponse
    size_t diffS = sizeThis - sizeDenom + 1;

    // Pour chaqu'un des éléments de la division
    for (size_t i = diffS, j = 0; i > 0;)
    {
        // décrémentation
        i--;
        // numérateur raccourcie
        size_t sizeNum = cpy.m_nbrLong << 1;
        uint16_t* shortNum = (uint16_t*)cpy.m_valeur;
        uint64_t num = 0;
        if (sizeThis - j - 1 < sizeNum)
            num += (uint64_t)shortNum[sizeThis - j - 1] << 16;
        if (sizeThis - j < sizeNum)
            num += (uint64_t)shortNum[sizeThis - j] << 32;
        if (sizeDenom >= 2 && sizeThis - j - 2 < sizeNum)
            num += shortNum[sizeThis - j - 2];

        // estimation
        int64_t ps = (num / denom) - 1;

        // valeur réelle
        if (ps >= 0)
        {
            // création du facteur de test
            uint16_t* facptr;
            size_t tailleFact = sizeDenom + i;
            facptr = new uint16_t[sizeDenom + i];
            for (size_t k = 0; k < i; k++)
                facptr[k] = 0;
            for (size_t k = 0; k < sizeDenom; k++)
                facptr[k + i] = shortDenom[k];

            inft fact(facptr, tailleFact);
            delete[] facptr;

            // test avec prédiction
            cpy = cpy - (inft((int32_t)ps) * fact);

            // test exterieur à prediction
            inft limit = cpy - fact;
            if (!limit.isNegatif())
            {
                cpy.swap(limit);
            }
        }

        // incrémentation
        j++;
    }

    cpy.m_negatif = m_negatif;
    return cpy;
}
inft inft::half() const
{
    // assignation de l'espace
    inft out(m_nbrLong, false);

    // Pour chaqu'un des éléments de la réponse
    for (size_t i = 0; i < m_nbrLong - 1; i++)
        out.m_valeur[i] = m_valeur[i + 1] << 31 | m_valeur[i] >> 1;
    out.m_valeur[m_nbrLong - 1] = m_valeur[m_nbrLong - 1] >> 1;

    out.cut();
    return out;
}
inft& inft::halfThis()
{
    for (size_t i = 0; i < m_nbrLong - 1; i++)
        m_valeur[i] = m_valeur[i + 1] << 31 | m_valeur[i] >> 1;
    m_valeur[m_nbrLong - 1] = m_valeur[m_nbrLong - 1] >> 1;
    cut();
    return *this;
}
inft inft::dbl() const
{
    // assignation de l'espace
    inft out(m_nbrLong + 1, false);

    // Pour chaqu'un des éléments de la réponse
    out.m_valeur[0] = m_valeur[0] << 1;
    for (size_t i = 1; i < m_nbrLong; i++)
        out.m_valeur[i] = m_valeur[i] << 1 | m_valeur[i - 1] >> 31;
    out.m_valeur[m_nbrLong] = m_valeur[m_nbrLong - 1] >> 31;

    out.cut();
    return out;
}

// puissance
inft inft::pow(inft val_) const
{
    inft out(1);
    inft driver = *this;
    while (val_ > 0)
    {
        if (val_.isImpair())
        {
            out = out * driver;
        }

        driver = driver * driver;
        val_ = val_.half();
    }
    return out;
}
inft inft::modPow(inft exposant_, const inft& modulo_) const
{
    inft out(1);
    inft driver = *this % modulo_;
    while (exposant_ > inft())
    {
        if (exposant_.isImpair())
        {
            out = (out * driver) % modulo_;
        }

        driver = (driver * driver) % modulo_;
        exposant_.halfThis();
    }
    return out;
}

bool inft::isPrime(uint32_t precision_ = 0) const
{
    std::random_device randomMachine;
    // prétest
    if (*this == inft(2) || *this == inft(3))
        return true;
    bool pair = !isImpair();
    bool neg = *this <= inft(1);
    if ((!isImpair()) || *this <= inft(1))
        return false;
    for (int i = 0; i < 256; i++)
    {
        if (*this == fistPrime[i])
            return true;
        if (*this % fistPrime[i] == inft())
            return false;
    }
    // recherche de d dans 2^s * d + i = *this
    inft d = *this - 1;
    inft s = inft(0);
    while (!d.isImpair())
    {
        d = d.half();
        s = s + 1;
    }

    // calcul this-1
    inft nM1 = *this - inft(1);

    // test assurément vrai
    if (precision_ == 0)
    {
        for (uint32_t a = 2; a < m_nbrLong * m_nbrLong * 8; a++)
        {
            // test de miller

            // a^d%n
            inft x = inft(a).modPow(d, *this);

            // test
            if (x == inft(1) || x == nM1)
                goto endTest;
            for (inft k(1); k < s; k = k + 1)
            {
                x = (x * x) % *this;
                d = d.dbl();

                if (x == inft(1))
                    return false;
                if (x == nM1)
                    goto endTest;
            }
            return false;
        endTest:;
        }

        return true;
    }


    // test probabiliste
    for (uint32_t i = 0; i < precision_; i++)
    {
        // test de miller
        // génération d'une valeur de test
        size_t nbrLongA = m_nbrLong + 1;
        uint32_t* cptrA = new uint32_t[nbrLongA];
        for (size_t j = 0; j < nbrLongA; j++)
            cptrA[j] = randomMachine();
        inft a = (inft(2) + inft(cptrA, nbrLongA)) % (*this - inft(4));
        delete[] cptrA;

        // a^d%n
        inft x = a.modPow(d, *this);

        // test
        if (x == inft(1) || x == nM1)
            goto endTestP;
        for (inft k(1); k < s; k = k + 1)
        {
            x = (x * x) % *this;
            d = d.dbl();

            if (x == inft(1))
                return false;
            if (x == nM1)
                goto endTestP;
        }
        return false;
    endTestP:;
    }

    return true;
}


inft randPrime(inft min_, inft max_)
{
    std::random_device randomMachine;
    inft terrain = (max_ + 1) - min_;
    size_t nbrLongA = terrain.size() + 1;
    inft a;

    uint32_t* cptrA = new uint32_t[nbrLongA];
    while (true)
    {
        // génération d'un nombre aléatoire
        for (size_t j = 0; j < nbrLongA; j++)
            cptrA[j] = randomMachine();
        a = (inft(cptrA, nbrLongA) % (terrain)) + min_;
        if (!a.isImpair())
        {
            --a;
            if (a < min_)
            {
                a = a + 2;
                if (a > max_)
                    throw;
            }
        }
        if (a.isPrime(10))
            break;
    }
    delete[] cptrA;
    return a;
}

inft randinft(inft min_, inft max_)
{
    std::random_device randomMachine;
    inft terrain = (max_ + 1) - min_;
    size_t nbrLongA = terrain.size() + 1;
    // génération d'un nombre aléatoire
    uint32_t* cptrA = new uint32_t[nbrLongA];
    for (size_t j = 0; j < nbrLongA; j++)
        cptrA[j] = randomMachine();
    inft a = (inft(cptrA, nbrLongA) % (terrain)) + min_;
    delete[] cptrA;
    return a;
}

inft gdc(inft a_, inft b_)
{
    // algorythme Euclidienne
    while (!(b_ == inft()))
    {
        // gdc(a,b) = gdc(b, a%b);
        inft buffer = b_;
        b_ = a_ % b_;
        a_ = buffer;
    }
    return a_; // gdc(a,0) = a
}

inft extendedGdc(inft a_, inft b_, inft& x_, inft& y_)
{
    inft s = 0, old_s = 1, t = 1, old_t = 0, r = b_, old_r = a_;
    while (!(r == inft()))
    {
        inft q = old_r / r;

        inft buffer = r;
        r = old_r - q * r;
        old_r = buffer;

        buffer = s;
        s = old_s - q * s;
        old_s = buffer;

        buffer = t;
        t = old_t - q * t;
        old_t = buffer;
    }

    x_ = old_s;
    y_ = old_t;
    if (!((x_ * a_ + y_ * b_) == old_r))
        throw "algorythme euclide non fonctionnel";
    return old_r;
}

inft invMod(inft a_, inft m_)
{
    inft x;
    inft y;
    if (!(extendedGdc(a_, m_, x, y) == inft(1)))
        throw "a et m non premier entre eux";
    if (x < 0)
        x = x + m_;
    return x;
}

// util
inline void inft::swap(inft& val_)
{
    std::swap(m_negatif, val_.m_negatif);
    std::swap(m_nbrLong, val_.m_nbrLong);
    std::swap(m_valeur, val_.m_valeur);
}
