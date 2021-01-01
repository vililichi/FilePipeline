#include "AES.h"

const char nr = 14; //14 round
const char nb = 60; //10 round + initial avec 4 mots
const char nk = 8; //256bits

using namespace AES;

#pragma region clé
//génération d'une clé
void AES::generation(char* cle)
{
	std::random_device rand;

	for (int i = 0; i < nk * 4; i++)cle[i] = rand();
}

//substitue 4bytes avec la S_BOXE
void subWord(char* b4)
{
	for (char i = 0; i < 4; i++) b4[i] = S_BOX[(b4[i] >> 4) & 0x0f][b4[i] & 0x0f];
}

//effecue une rotation avec 4 bytes
void RotWord(char* b4)
{
	char b40 = b4[0];
	b4[0] = b4[1];
	b4[1] = b4[2];
	b4[2] = b4[3];
	b4[3] = b40;
}

//génère un mot de 4 bytes dans buffer_b4
void Rcon(unsigned char i, char* buffer_b4)
{
	GF8 x(0x01);
	for (unsigned char j = 1; j < i; j++) x *= GF8(0x02);
	buffer_b4[0] = x.value;
	buffer_b4[1] = 0;
	buffer_b4[2] = 0;
	buffer_b4[3] = 0;
}


//augmente la taille de la clé, 
//la taille prévue pour la clé doit être de 176, 208 ou 240 bytes
//et la taille initiale de 16, 24 ou 32 bytes;
void AES::expensionCle(char* cle)
{
	char temp[4];
	for (char i = nk; i < nb; i++)
	{
		for (char j = 0; j < 4; j++)temp[j] = cle[((i - 1) * 4) + j];
		if (i % nk == 0)
		{
			char rconVal[4];
			Rcon(i / nk, rconVal);
			RotWord(temp);
			subWord(temp);
			for (char j = 0; j < 4; j++)temp[j] = temp[j] ^ rconVal[j];
		}
		else if (i % nk == 4)subWord(temp);//pour 256 bits seulement
		for (char j = 0; j < 4; j++)cle[(i*4) + j] = cle[((i - nk )*4) + j ] ^ temp[j];
	}
}
#pragma endregion

#pragma region cryptage
//substitue 16bytes avec la S_BOXE
void subBytes(char* b16)
{
	for (char i = 0; i < 16; i++) b16[i] = S_BOX[(b16[i] >> 4) & 0x0f][b16[i] & 0x0f];
}

//effectue le decalage de range avec 16bytes
void shiftRows(char* b16)
{
	char nb16[16] =
	{
		b16[0],b16[5],b16[10],b16[15],
		b16[4],b16[9],b16[14],b16[3],
		b16[8],b16[13],b16[2],b16[7],
		b16[12],b16[1],b16[6],b16[11]
	};
	for(int i = 0 ; i < 16; i++)b16[i] = nb16[i];
}

//effectue le brassage de colonne avec 16bytes
void mixColumns(char* b16)
{
	char nb16[16];
	for (unsigned char i = 0; i < 4; i++)
	{
		nb16[ 4 * i		] = (GF8(0x02) * GF8(b16[4 * i])).value ^ (GF8(0x03) * GF8(b16[(4 * i) + 1])).value ^ b16[(4 * i) + 2] ^ b16[(4 * i) + 3];
		nb16[(4 * i) + 1] = b16[4 * i] ^ (GF8(0x02) * GF8(b16[(4 * i) + 1])).value ^ (GF8(0x03) * GF8(b16[(4 * i) + 2])).value ^ b16[(4 * i) + 3];
		nb16[(4 * i) + 2] = b16[4 * i] ^ b16[(4 * i) + 1] ^ (GF8(0x02) * GF8(b16[(4 * i) + 2])).value ^ (GF8(0x03) * GF8(b16[(4 * i) + 3])).value;
		nb16[(4 * i) + 3] = (GF8(0x03) * GF8(b16[4 * i])).value ^ b16[(4 * i) + 1] ^ b16[(4 * i) + 2] ^ (GF8(0x02) * GF8(b16[(4 * i) + 3])).value;
	}
	for (int i = 0; i < 16; i++)b16[i] = nb16[i];
}

//ajout de la clé
void addRoundKey(char* b16,char* key)
{
	for (unsigned char i = 0; i < 16; i++)b16[i] = b16[i] ^ key[i];
}

//cryptage de 16 bytes avec une clé étendue
void AES::cryptage(char* b16, char* cle)
{
	addRoundKey(b16, cle);

	for (char i = 1; i <= nr - 1; i++)
	{
		subBytes(b16);
		shiftRows(b16);
		mixColumns(b16);
		addRoundKey(b16, cle + 16 * i);
	}

	subBytes(b16);
	shiftRows(b16);
	addRoundKey(b16, cle + 16 * nr);
}
#pragma endregion

#pragma region decryptage
//substitue 16bytes avec la INV_S_BOXE
void invSubBytes(char* b16)
{
	for (char i = 0; i < 16; i++) b16[i] = INV_S_BOX[(b16[i] >> 4) & 0x0f][b16[i] & 0x0f];
}

//effectue l'inverse du decalage de range avec 16bytes
void invShiftRows(char* b16)
{
	char nb16[16] =
	{
		b16[0],b16[13],b16[10],b16[7],
		b16[4],b16[1],b16[14],b16[11],
		b16[8],b16[5],b16[2],b16[15],
		b16[12],b16[9],b16[6],b16[3]
	};
	for (int i = 0; i < 16; i++)b16[i] = nb16[i];
}

//effectue l'inverse du brassage de colonne avec 16bytes
void invMixColumns(char* b16)
{
	char nb16[16];
	for (unsigned char i = 0; i < 4; i++)
	{
		nb16[ 4 * i		] = (GF8(0x0e) * GF8(b16[4 * i])).value ^ (GF8(0x0b) * GF8(b16[(4 * i) + 1])).value ^ (GF8(0x0d) * GF8(b16[(4 * i) + 2])).value ^ (GF8(0x09) * GF8(b16[(4 * i) + 3])).value;
		nb16[(4 * i) +1 ] = (GF8(0x09) * GF8(b16[4 * i])).value ^ (GF8(0x0e) * GF8(b16[(4 * i) + 1])).value ^ (GF8(0x0b) * GF8(b16[(4 * i) + 2])).value ^ (GF8(0x0d) * GF8(b16[(4 * i) + 3])).value;
		nb16[(4 * i) +2 ] = (GF8(0x0d) * GF8(b16[4 * i])).value ^ (GF8(0x09) * GF8(b16[(4 * i) + 1])).value ^ (GF8(0x0e) * GF8(b16[(4 * i) + 2])).value ^ (GF8(0x0b) * GF8(b16[(4 * i) + 3])).value;
		nb16[(4 * i) +3 ] = (GF8(0x0b) * GF8(b16[4 * i])).value ^ (GF8(0x0d) * GF8(b16[(4 * i) + 1])).value ^ (GF8(0x09) * GF8(b16[(4 * i) + 2])).value ^ (GF8(0x0e) * GF8(b16[(4 * i) + 3])).value;
	}
	for (int i = 0; i < 16; i++)b16[i] = nb16[i];
}

//cryptage de 16 bytes avec une clé étendue
void AES::decryptage(char* b16, char* cle)
{
	addRoundKey(b16, cle+ 16*nr);
	invShiftRows(b16);
	invSubBytes(b16);

	for (char i = nr - 1; i > 0; i--)
	{
		addRoundKey(b16, cle + 16 * i);
		invMixColumns(b16);
		invShiftRows(b16);
		invSubBytes(b16);
	}

	addRoundKey(b16, cle);
}


#pragma endregion
