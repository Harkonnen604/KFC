#include "kfc_formats_pch.h"
#include "digest.h"

// --------
// Globals
// --------
static const BYTE R[64] =
{	7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22, 
	5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
	4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
	6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21};

static DWORD K[64];

// ----------------
// MD5 initializer
// ----------------
struct T_MD5_Initializer
{
public:
	T_MD5_Initializer()
	{
		double pw = pow(2.0, 32.0);

		for(size_t i = 0 ; i < 64 ; i++)
			K[i] = (DWORD)(fabs(sin(i + 1.0)) * pw);
	}
};

static T_MD5_Initializer gs_MD5_Initializer;

// ----------------
// Helper routines
// ----------------
inline DWORD LeftRotate(DWORD x, BYTE c)
{
	return (x << c) | (x >> (32 - c));
}

// ----------------
// Global routines
// ----------------
KString MD5(const BYTE* pData, size_t szN)
{
	QWORD qwBitLength = szN * 8;

	size_t szVirtualN = (szN + 72) / 64 * 64;

	DWORD h[4] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476};

	for(size_t i = 0 ; i < szVirtualN ; i += 64)
	{
		DWORD Chunk[16];

		BYTE* pChunk = (BYTE*)Chunk;

		for(size_t j = 0 ; j < 64 ; j++)
		{
			if(i + j < szN)
				pChunk[j] = pData[i + j];
			else if(i + j == szN)
				pChunk[j] = 0x80;
			else if(i + j >= szVirtualN - 8)
				pChunk[j] = ((const BYTE*)&qwBitLength)[i + j - (szVirtualN - 8)];
			else
				pChunk[j] = 0x00;
		}

		DWORD a = h[0];
		DWORD b = h[1];
		DWORD c = h[2];
		DWORD d = h[3];

		DWORD f, g, t;

		for(DWORD j = 0 ; j < 64 ; j++)
		{
			if(j < 16)
			{
				f = (b & c) | (~b & d);
				g = j;
			}
			else if(j < 32)
			{
				f = (d & b) | (~d & c);
				g = (j*5 + 1) & 0xF;
			}
			else if(j < 48)
			{
				f = b ^ c ^ d;
				g = (j*3 + 5) & 0xF;
			}
			else
			{
				f = c ^ (b | ~d);
				g = (j*7) & 0xF;
			}

			t = d;
			d = c;
			c = b;
			b += LeftRotate(a + f + K[j] + Chunk[g], R[j]);
			a = t;
		}

		h[0] += a;
		h[1] += b;
		h[2] += c;
		h[3] += d;
	}

	KString Result;
	Result.Allocate(32);

	const BYTE* ph = (const BYTE*)h;

	for(size_t i = 0 ; i < 16 ; i++)
	{
		Result.SetChar(i*2 + 0, tolower(HexToChar(ph[i] >> 4 )));
		Result.SetChar(i*2 + 1, tolower(HexToChar(ph[i] & 0xF)));
	}

	return Result;
}
