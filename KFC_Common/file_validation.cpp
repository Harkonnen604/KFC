#include "kfc_common_pch.h"
#include "file_validation.h"

#include "file.h"

// ----------------
// Global routines
// ----------------
bool IsPEImageFile(const KString& FileName)
{
	TEST_BLOCK_BEGIN
	{
		TFile File(FileName, FOF_BINARYREAD);

		File.Seek(0x3C);

		size_t szOffset;
		File >> szOffset;
		
		File.Seek(szOffset);

		char Signature[4];
		File.Read(Signature, sizeof(Signature));

		if(memcmp(Signature, "PE\0\0", 4))
			return false;

		return true;
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		return false;
	}
	TEST_BLOCK_END
}

bool IsJPEGFile(const KString& FileName)
{
	TEST_BLOCK_BEGIN
	{
		BYTE bValue;
		
		TFile File(FileName, FOF_BINARYREAD);
		
		File >> bValue;

		if(bValue != 0xFF)
			return false;

		return true;
	}
	TEST_BLOCK_KFC_EXCEPTION_HANDLER
	{
		return false;
	}
	TEST_BLOCK_END
}
