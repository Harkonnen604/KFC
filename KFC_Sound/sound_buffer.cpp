#include "kfc_sound_pch.h"
#include "sound_buffer.h"

#include "sound_device_globals.h"

// -------------
// Sound buffer
// -------------
TSoundBuffer::TSoundBuffer()
{
	m_pBuffer = NULL;
}

void TSoundBuffer::Release()
{
	if(m_pBuffer)
		m_pBuffer->Release(), m_pBuffer = NULL;
}

void TSoundBuffer::Allocate(const WAVEFORMATEX&	WaveFormat,
							size_t				szLengthSamples)
{
	Release();

	try
	{
		WAVEFORMATEX TempWaveFormat = WaveFormat;

		DSBUFFERDESC Desc;
		memset(&Desc, 0, sizeof(Desc)), Desc.dwSize = sizeof(Desc);

		Desc.dwFlags		= DSBCAPS_LOCSOFTWARE;
		Desc.dwBufferBytes	= (WaveFormat.wBitsPerSample * szLengthSamples) >> 3;
		Desc.lpwfxFormat	= &TempWaveFormat;

		if(g_SoundDeviceGlobals.m_pDSound->CreateSoundBuffer(&Desc, &m_pBuffer, NULL))
		{
			m_pBuffer = NULL;
			INITIATE_DEFINED_FAILURE(TEXT("Error creating DirectSound buffer."));
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TSoundBuffer::AllocatePrimary()
{
	Release();

	try
	{
		DSBUFFERDESC Desc;
		memset(&Desc, 0, sizeof(Desc)), Desc.dwSize = sizeof(Desc);

		Desc.dwFlags = DSBCAPS_LOCSOFTWARE | DSBCAPS_PRIMARYBUFFER;

		if(g_SoundDeviceGlobals.m_pDSound->CreateSoundBuffer(&Desc, &m_pBuffer, NULL))
		{
			m_pBuffer = NULL;
			INITIATE_DEFINED_FAILURE(TEXT("Error creating primary DirectSound buffer."));
		}
	}

	catch(...)
	{
		Release();
		throw;
	}
}

void TSoundBuffer::GetFormat(TWaveFormat& RFormat) const
{
	DEBUG_VERIFY_ALLOCATION;

	HRESULT r;

	DWORD dwSize = 0;

	if(r = m_pBuffer->GetFormat(&RFormat, RFormat.cbSize, &dwSize))
		INITIATE_DEFINED_CODE_FAILURE(TEXT("Error getting sound buffer format"), r);

	RFormat.cbSize = (WORD)dwSize;
}

void TSoundBuffer::SetFormat(const TWaveFormat& Format)
{
	DEBUG_VERIFY_ALLOCATION;

	HRESULT r;

	if(r = m_pBuffer->SetFormat(&Format))
		INITIATE_DEFINED_CODE_FAILURE(TEXT("Error setting (primary) sound buffer format"), r);
}

LPDIRECTSOUNDBUFFER TSoundBuffer::GetBuffer() const
{
	DEBUG_VERIFY_ALLOCATION;

	return m_pBuffer;
}
