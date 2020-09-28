#include "kfc_ssl_pch.h"
#include "ssl_atoms.h"

#include <KFC_KTL\pow2.h>
#include <KFC_Common\file.h>
#include "ssl_helpers.h"

// --------
// RSA key
// --------
T_RSA_Key::T_RSA_Key()
{
    m_pRSA = NULL;
}

T_RSA_Key::T_RSA_Key(LPCTSTR pFileName)
{
    m_pRSA = NULL;

    LoadFrom_PEM_File(pFileName);
}

void T_RSA_Key::Release()
{
    if(m_pRSA)
        RSA_free(m_pRSA), m_pRSA = NULL;
}

// Generation
void T_RSA_Key::Generate(size_t szLength)
{
    Release();

    try
    {
        DEBUG_VERIFY(IsPow2(szLength));

        m_pRSA = RSA_generate_key(szLength, 65537, NULL, NULL);

        if(m_pRSA == NULL)
            INITIATE_DEFINED_FAILURE(TEXT("Error generating RSA key pair."));

        m_bHasPrivate = true;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

// Storage
void T_RSA_Key::LoadFrom_PEM_File(LPCTSTR pFileName)
{
    Release();

    try
    {
        if(!(m_pRSA = RSA_new()))
            INITIATE_DEFINED_FAILURE(TEXT("Error creating empty RSA key."));

        if(!PEM_read_RSAPrivateKey( TFile(pFileName, FOF_TEXTREAD).GetStream(),
                                    &m_pRSA,
                                    NULL,
                                    NULL))
        {
            INITIATE_DEFINED_FAILURE((KString)TEXT("Error reading RSA key from PEM file \"") + pFileName + TEXT("\"."));
        }
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void T_RSA_Key::LoadAll(TStream& Stream)
{
    Release();

    try
    {
        if(!(m_pRSA = RSA_new()))
            INITIATE_DEFINED_FAILURE(TEXT("Error creating RSA key pair placeholder."));

        m_pRSA->n = Load_RSA_BIGNUM(Stream);
        m_pRSA->e = Load_RSA_BIGNUM(Stream);
        m_pRSA->d = Load_RSA_BIGNUM(Stream);
        m_pRSA->p = Load_RSA_BIGNUM(Stream);
        m_pRSA->q = Load_RSA_BIGNUM(Stream);

        m_bHasPrivate = true;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void T_RSA_Key::LoadPublic(TStream& Stream)
{
    Release();

    try
    {
        m_pRSA = RSA_new();

        if(m_pRSA == NULL)
            INITIATE_DEFINED_FAILURE(TEXT("Error creating RSA public key placeholder."));

        m_pRSA->n = Load_RSA_BIGNUM(Stream);
        m_pRSA->e = Load_RSA_BIGNUM(Stream);

        m_bHasPrivate = false;
    }

    catch(...)
    {
        Release();
        throw;
    }
}

void T_RSA_Key::SaveAll(TStream& Stream) const
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(m_bHasPrivate);

    Save_RSA_BIGNUM(m_pRSA->n, Stream);
    Save_RSA_BIGNUM(m_pRSA->e, Stream);
    Save_RSA_BIGNUM(m_pRSA->d, Stream);
    Save_RSA_BIGNUM(m_pRSA->p, Stream);
    Save_RSA_BIGNUM(m_pRSA->q, Stream);
}

void T_RSA_Key::SavePublic(TStream& Stream) const
{
    DEBUG_VERIFY_ALLOCATION;

    Save_RSA_BIGNUM(m_pRSA->n, Stream);
    Save_RSA_BIGNUM(m_pRSA->e, Stream);
}

// Cypher
void T_RSA_Key::EncryptPublic(const TArray<BYTE, true>& Data, TStream& Stream) const
{
    DEBUG_VERIFY_ALLOCATION;

    const size_t szRSASize = RSA_size(*this);

    DEBUG_VERIFY(szRSASize > 12);

    TArray<BYTE, true> DstBuffer(szRSASize);

    size_t szOffset;

    for(szOffset = 0 ; szOffset < Data.GetN() ; )
    {
        const size_t szCurDataChunkSize =
            Min(szRSASize - 12, Data.GetN() - szOffset);

        const int iCurCypherChunkSize =
            RSA_public_encrypt( szCurDataChunkSize,
                                Data.GetDataPtr() + szOffset,
                                DstBuffer.GetDataPtr(),
                                *this,
                                RSA_PKCS1_PADDING);

        if(iCurCypherChunkSize < 0)
            INITIATE_DEFINED_FAILURE(TEXT("RSA public key encryption failed."));

        Stream << iCurCypherChunkSize;

        Stream.StreamWrite(DstBuffer.GetDataPtr(), iCurCypherChunkSize);

        szOffset += szCurDataChunkSize;
    }

    Stream << UINT_MAX;
}

void T_RSA_Key::EncryptPrivate(const TArray<BYTE, true>& Data, TStream& Stream) const
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(m_bHasPrivate);

    const size_t szRSASize = RSA_size(*this);

    DEBUG_VERIFY(szRSASize > 12);

    TArray<BYTE, true> DstBuffer(szRSASize);

    size_t szOffset;

    for(szOffset = 0 ; szOffset < Data.GetN() ; )
    {
        const size_t szCurDataChunkSize =
            Min(szRSASize - 12, Data.GetN() - szOffset);

        const int iCurCypherChunkSize =
            RSA_private_encrypt(szCurDataChunkSize,
                                Data.GetDataPtr() + szOffset,
                                DstBuffer.GetDataPtr(),
                                *this,
                                RSA_PKCS1_PADDING);

        if(iCurCypherChunkSize < 0)
            INITIATE_DEFINED_FAILURE(TEXT("RSA private key encryption failed."));

        Stream << iCurCypherChunkSize;

        Stream.StreamWrite(DstBuffer.GetDataPtr(), iCurCypherChunkSize);

        szOffset += szCurDataChunkSize;
    }

    Stream << UINT_MAX;
}

void T_RSA_Key::DecryptPublic(TStream& Stream, TArray<BYTE, true>& RData) const
{
    DEBUG_VERIFY_ALLOCATION;

    const size_t szRSASize = RSA_size(*this);

    RData.Clear();

    TArray<BYTE, true> SrcBuffer(szRSASize);
    TArray<BYTE, true> DstBuffer(szRSASize);

    for(;;)
    {
        size_t szCurCypherChunkSize;

        Stream >> szCurCypherChunkSize;

        if(szCurCypherChunkSize == UINT_MAX)
            break;

        if(szCurCypherChunkSize > SrcBuffer.GetN())
            INITIATE_DEFINED_FAILURE(TEXT("Malformed private cyphertext received."));

        Stream.StreamRead(SrcBuffer.GetDataPtr(), szCurCypherChunkSize);

        const int iCurDataChunkSize =
            RSA_public_decrypt( szCurCypherChunkSize,
                                SrcBuffer.GetDataPtr(),
                                DstBuffer.GetDataPtr(),
                                *this,
                                RSA_PKCS1_PADDING);

        if(iCurDataChunkSize < 0)
            INITIATE_DEFINED_FAILURE(TEXT("RSA public key decryption failed."));

        memcpy(&RData.Add(iCurDataChunkSize), DstBuffer.GetDataPtr(), iCurDataChunkSize);
    }
}

void T_RSA_Key::DecryptPrivate(TStream& Stream, TArray<BYTE, true>& RData) const
{
    DEBUG_VERIFY_ALLOCATION;

    DEBUG_VERIFY(m_bHasPrivate);

    const size_t szRSASize = RSA_size(*this);

    RData.Clear();

    TArray<BYTE, true> SrcBuffer(szRSASize);
    TArray<BYTE, true> DstBuffer(szRSASize);

    for(;;)
    {
        size_t szCurCypherChunkSize;

        Stream >> szCurCypherChunkSize;

        if(szCurCypherChunkSize == UINT_MAX)
            break;

        if(szCurCypherChunkSize > SrcBuffer.GetN())
            INITIATE_DEFINED_FAILURE(TEXT("Malformed public cyphertext received."));

        Stream.StreamRead(SrcBuffer.GetDataPtr(), szCurCypherChunkSize);

        const int iCurDataChunkSize =
            RSA_private_decrypt(szCurCypherChunkSize,
                                SrcBuffer.GetDataPtr(),
                                DstBuffer.GetDataPtr(),
                                *this,
                                RSA_PKCS1_PADDING);

        if(iCurDataChunkSize < 0)
            INITIATE_DEFINED_FAILURE(TEXT("RSA private key decryption failed."));

        memcpy(&RData.Add(iCurDataChunkSize), DstBuffer.GetDataPtr(), iCurDataChunkSize);
    }
}

// -----------------
// X509 certificate
// -----------------
T_X509_Certificate::T_X509_Certificate()
{
    m_pX509 = NULL;
}

T_X509_Certificate::T_X509_Certificate(LPCTSTR pFileName)
{
    m_pX509 = NULL;

    LoadFrom_PEM_File(pFileName);
}

void T_X509_Certificate::Release()
{
    if(m_pX509)
        X509_free(m_pX509), m_pX509 = NULL;
}

void T_X509_Certificate::LoadFrom_PEM_File(LPCTSTR pFileName)
{
    Release();

    try
    {
        if(!(m_pX509 = X509_new()))
            INITIATE_DEFINED_FAILURE(TEXT("Error creating empty X509 certificate."));

        if(!PEM_read_X509(TFile(pFileName, FOF_TEXTREAD).GetStream(), &m_pX509, NULL, NULL))
            INITIATE_DEFINED_FAILURE((KString)TEXT("Error reading X509 certificate from PEM file \"") + pFileName + TEXT("\"."));

    }

    catch(...)
    {
        Release();
        throw;
    }
}
