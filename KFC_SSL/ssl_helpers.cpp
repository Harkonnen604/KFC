#include "kfc_ssl_pch.h"
#include "ssl_helpers.h"

// ----------------
// Global routines
// ----------------
BIGNUM* Load_RSA_BIGNUM(TStream& Stream)
{
	size_t szN;

	Stream >> szN;

	TArray<BYTE, true> Data(szN);

	Stream.StreamRead(Data.GetDataPtr(), Data.GetN());

	BIGNUM* const pBN = BN_bin2bn(Data.GetDataPtr(), Data.GetN(), 0);

	if(!pBN)
		INITIATE_DEFINED_FAILURE(TEXT("Error reading RSA big number."));

	return pBN;
}

void Save_RSA_BIGNUM(const BIGNUM* pBN, TStream& Stream)
{
	DEBUG_VERIFY(pBN);

	const size_t szN = BN_num_bytes(pBN);

	Stream << szN;

	TArray<BYTE, true> Data(szN);

	BN_bn2bin(pBN, Data.GetDataPtr());	

	Stream.StreamWrite(Data.GetDataPtr(), Data.GetN());	
}