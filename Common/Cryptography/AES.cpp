#include "AES.h"

#if defined(AESMemoryMult)
#include "AES_mem.h"
#endif

const char nr = 14; //14 round
const char nb = 60; //10 round + initial avec 4 mots
const char nk = 8; //256bits

using namespace AES;

#pragma region GF8

inline const unsigned char GF8Mult(const unsigned char a_, const unsigned char b_)
{
	#if defined(AESMemoryMult)
	return TABLEMULT[a_][b_];
	#else
	unsigned char retour = 0;
	while (a_ && b_)
	{
		if (b_ & 1) retour ^= a_;
		if (a_ & 0x80) a_ = (a_ << 1) ^ 0x11b;
		else a_ <<= 1;
		b_ >>= 1;
	}
	return retour;
	#endif
}

#pragma endregion GF8

#pragma region clé
//génération d'une clé
void AES::generation(char* const cle_)
{
	std::random_device rand;

	for (int i = 0; i < nk * 4; i++)cle_[i] = rand();
}

//substitue 4bytes avec la S_BOXE
inline void subWord(char* const b4_)
{
	for (char i = 0; i < 4; i++) b4_[i] = S_BOX[(unsigned char)b4_[i]];
}

//effecue une rotation avec 4 bytes
inline void RotWord(char* const b4_)
{
	char b40 = b4_[0];
	b4_[0] = b4_[1];
	b4_[1] = b4_[2];
	b4_[2] = b4_[3];
	b4_[3] = b40;
}

//génère un mot de 4 bytes dans buffer_b4
void Rcon(const unsigned char i, char* const buffer_b4_)
{
	unsigned char x(0x01);
	for (unsigned char j = 1; j < i; j++) x = GF8Mult(x,0x02);
	buffer_b4_[0] = x;
	buffer_b4_[1] = 0;
	buffer_b4_[2] = 0;
	buffer_b4_[3] = 0;
}


//augmente la taille de la clé, 
//la taille prévue pour la clé doit être de 176, 208 ou 240 bytes
//et la taille initiale de 16, 24 ou 32 bytes;
void AES::expensionCle(char* const cle_)
{
	char temp[4];
	for (char i = nk; i < nb; i++)
	{
		for (char j = 0; j < 4; j++)temp[j] = cle_[((i - 1) * 4) + j];
		if (i % nk == 0)
		{
			char rconVal[4];
			Rcon(i / nk, rconVal);
			RotWord(temp);
			subWord(temp);
			for (char j = 0; j < 4; j++)temp[j] = temp[j] ^ rconVal[j];
		}
		else if (i % nk == 4)subWord(temp);//pour 256 bits seulement
		for (char j = 0; j < 4; j++)cle_[(i * 4) + j] = cle_[((i - nk) * 4) + j] ^ temp[j];
	}
}
#pragma endregion

#pragma region cryptage
//substitue 16bytes avec la S_BOXE
inline void subBytes(char* const b16_)
{
	for (char i = 0; i < 16; i++) b16_[i] = S_BOX[(unsigned char)b16_[i]];
}

//effectue le decalage de range avec 16bytes
inline void shiftRows(char* const b16_)
{
	char b1236[4] = { b16_[1], b16_[2], b16_[3],b16_[6] };
	//colonne 2
	b16_[1] = b16_[5];
	b16_[5] = b16_[9];
	b16_[9] = b16_[13];
	b16_[13] = b1236[0];
	//colonne 3
	b16_[2] = b16_[10];
	b16_[6] = b16_[14];
	b16_[10] = b1236[1];
	b16_[14] = b1236[3];
	//colonne 4
	b16_[3] = b16_[15];
	b16_[15] = b16_[11];
	b16_[11] = b16_[7];
	b16_[7] = b1236[2];
}

//effectue le brassage de colonne avec 16bytes
inline void mixColumns(char* const b16_)
{
	char nb16[16];
	for (unsigned char i = 0; i < 4; i++)
	{
		nb16[4 * i]       = GF8Mult(0x02, b16_[4 * i]) ^ GF8Mult(0x03, b16_[(4 * i) + 1]) ^ b16_[(4 * i) + 2] ^ b16_[(4 * i) + 3];
		nb16[(4 * i) + 1] = b16_[4 * i] ^ GF8Mult(0x02, b16_[(4 * i) + 1]) ^ GF8Mult(0x03, b16_[(4 * i) + 2]) ^ b16_[(4 * i) + 3];
		nb16[(4 * i) + 2] = b16_[4 * i] ^ b16_[(4 * i) + 1] ^ GF8Mult(0x02, b16_[(4 * i) + 2]) ^ GF8Mult(0x03, b16_[(4 * i) + 3]);
		nb16[(4 * i) + 3] = GF8Mult(0x03, b16_[4 * i]) ^ b16_[(4 * i) + 1] ^ b16_[(4 * i) + 2] ^ GF8Mult(0x02, b16_[(4 * i) + 3]);
	}
	//for (int i = 0; i < 16; i++)b16[i] = nb16[i];
	std::copy(nb16, nb16 + 16, b16_);
}

//ajout de la clé
inline void addRoundKey(char* const b16_, char* const key_)
{
	for (unsigned char i = 0; i < 16; i++)b16_[i] = b16_[i] ^ key_[i];
}

//cryptage de 16 bytes avec une clé étendue
void AES::cryptage(char* const b16_, char* const cle_)
{
	addRoundKey(b16_, cle_);

	for (char i = 1; i <= nr - 1; i++)
	{
		subBytes(b16_);
		shiftRows(b16_);
		mixColumns(b16_);
		addRoundKey(b16_, cle_ + 16 * i);
	}

	subBytes(b16_);
	shiftRows(b16_);
	addRoundKey(b16_, cle_ + 16 * nr);
}
#pragma endregion

#pragma region decryptage
//substitue 16bytes avec la INV_S_BOXE
void invSubBytes(char* const b16_)
{
	for (char i = 0; i < 16; i++) b16_[i] = INV_S_BOX[(unsigned char)b16_[i]];
}

//effectue l'inverse du decalage de range avec 16bytes
void invShiftRows(char* const b16_)
{
	char b1236[4] = { b16_[1], b16_[2], b16_[3],b16_[6] };
	//colonne 2
	b16_[1]  = b16_[13];
	b16_[13] = b16_[9];
	b16_[9]  = b16_[5];
	b16_[5]  = b1236[0];
	//colonne 3
	b16_[2]  = b16_[10];
	b16_[6]  = b16_[14];
	b16_[10] = b1236[1];
	b16_[14] = b1236[3];
	//colonne 4
	b16_[3]  = b16_[7];
	b16_[7]  = b16_[11];
	b16_[11] = b16_[15];
	b16_[15] = b1236[2];

}

//effectue l'inverse du brassage de colonne avec 16bytes
void invMixColumns(char* const b16_)
{
	char nb16[16];
	for (unsigned char i = 0; i < 4; i++)
	{
		const char nb1 = GF8Mult(0x0e, b16_[4 * i]) ^ GF8Mult(0x0b, b16_[(4 * i) + 1]) ^ GF8Mult(0x0d, b16_[(4 * i) + 2]) ^ GF8Mult(0x09, b16_[(4 * i) + 3]);
		const char nb2 = GF8Mult(0x09, b16_[4 * i]) ^ GF8Mult(0x0e, b16_[(4 * i) + 1]) ^ GF8Mult(0x0b, b16_[(4 * i) + 2]) ^ GF8Mult(0x0d, b16_[(4 * i) + 3]);
		const char nb3 = GF8Mult(0x0d, b16_[4 * i]) ^ GF8Mult(0x09, b16_[(4 * i) + 1]) ^ GF8Mult(0x0e, b16_[(4 * i) + 2]) ^ GF8Mult(0x0b, b16_[(4 * i) + 3]);
		b16_[(4 * i) + 3] = GF8Mult(0x0b, b16_[4 * i]) ^ GF8Mult(0x0d, b16_[(4 * i) + 1]) ^ GF8Mult(0x09, b16_[(4 * i) + 2]) ^ GF8Mult(0x0e, b16_[(4 * i) + 3]);

		b16_[4 * i] = nb1;
		b16_[(4 * i) + 1] = nb2;
		b16_[(4 * i) + 2] = nb3;
	}
}

//cryptage de 16 bytes avec une clé étendue
void AES::decryptage(char* const b16_, char* const cle_)
{
	addRoundKey(b16_, cle_ + 16 * nr);
	invShiftRows(b16_);
	invSubBytes(b16_);

	for (char i = nr - 1; i > 0; i--)
	{
		addRoundKey(b16_, cle_ + 16 * i);
		invMixColumns(b16_);
		invShiftRows(b16_);
		invSubBytes(b16_);
	}

	addRoundKey(b16_, cle_);
}


#pragma endregion
