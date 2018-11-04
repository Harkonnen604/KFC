#ifndef vmatrix_h
#define vmatrix_h

#include "basic_types.h"
#include "basic_bitypes.h"

// -------------
// Value matrix
// -------------
template <class ObjectType, bool bPOD_Type = false>
class TValueMatrix
{
private:
	ObjectType*	m_pData;
	SZSIZE		m_Size;

public:
	TValueMatrix();

	TValueMatrix(const SZSIZE& SSize);

	TValueMatrix(const TValueMatrix& Matrix);

	~TValueMatrix()
		{ Release(); }

	bool IsAllocated() const
		{ return m_pData; }

	void Release(bool bFromAllocatorException = false);

	void Allocate(const SZSIZE& SSize);

	TValueMatrix& operator = (const TValueMatrix& Matrix);
	
	ObjectType& GetDataRef(const SZPOINT& Coords)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(Coords.x < m_Size.cx && Coords.y < m_Size.cy);

		return m_pData[Coords.y * m_Size.cx + Coords.x];
	}

	const ObjectType& GetDataRef(const SZPOINT& Coords) const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(Coords.x < m_Size.cx && Coords.y < m_Size.cy);

		return m_pData[Coords.y * m_Size.cx + Coords.x];
	}

	ObjectType* GetDataPtr(const SZPOINT& Coords)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(Coords.x < m_Size.cx && Coords.y < m_Size.cy);

		return m_pData + (Coords.y * m_Size.cx + Coords.x);
	}

	const ObjectType* GetDataPtr(const SZPOINT& Coords) const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(Coords.x < m_Size.cx && Coords.y < m_Size.cy);

		return m_pData + (Coords.y * m_Size.cx + Coords.x);
	}

	ObjectType* GetDataPtr()
	{
		DEBUG_VERIFY_ALLOCATION;

		return m_pData;
	}

	const ObjectType* GetDataPtr() const
	{
		DEBUG_VERIFY_ALLOCATION;

		return m_pData;
	}

	ObjectType& operator () (size_t y, size_t x)
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(y < m_Size.cy && x < m_Size.cx);

		return m_pData[y * m_Size.cx + x];
	}

	const ObjectType& operator () (size_t y, size_t x) const
	{
		DEBUG_VERIFY_ALLOCATION;

		DEBUG_VERIFY(y < m_Size.cy && x < m_Size.cx);

		return m_pData[y * m_Size.cx + x];
	}

	void Transpose();

	void FlipHorizontal();

	void FlipVertical();

	const SZSIZE& GetSize() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Size; }

	size_t GetWidth() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Size.cx; }

	size_t GetHeight() const
		{ DEBUG_VERIFY_ALLOCATION; return m_Size.cy; }
};

template <class ObjectType, bool bPOD_Type>
TValueMatrix<ObjectType, bPOD_Type>::TValueMatrix()
{
	m_pData = NULL;
}

template <class ObjectType, bool bPOD_Type>
TValueMatrix<ObjectType, bPOD_Type>::TValueMatrix(const SZSIZE& SSize)
{
	m_pData = NULL;

	Allocate(SSize);
}

template <class ObjectType, bool bPOD_Type>
TValueMatrix<ObjectType, bPOD_Type>::TValueMatrix(const TValueMatrix& Matrix)
{
	m_pData = NULL;

	*this = Matrix;
}

template <class ObjectType, bool bPOD_Type>
void TValueMatrix<ObjectType, bPOD_Type>::Release(bool bFromAllocatorException)
{
	delete[] m_pData, m_pData = NULL;
}

template <class ObjectType, bool bPOD_Type>
void TValueMatrix<ObjectType, bPOD_Type>::Allocate(const SZSIZE& SSize)
{
	Release();

	DEBUG_VERIFY(SSize.IsPositive());

	m_Size = SSize;

	m_pData = new ObjectType[m_Size.GetArea()];
}

template <class ObjectType, bool bPOD_Type>
TValueMatrix<ObjectType, bPOD_Type>& TValueMatrix<ObjectType, bPOD_Type>::operator = (const TValueMatrix& Matrix)
{
	if(&Matrix == this)
		return *this;

	Release();

	if(!Matrix.IsAllocated())
		return *this;

	Allocate(Matrix.GetSize());

	const size_t szLength = m_Size.GetSquare();

	m_pData = new ObjectType[szLength];

	if(bPOD_Type)
	{
		memcpy(m_pData, Matrix.m_pData, szLength * sizeof(ObjectType));
	}
	else
	{
		size_t i;

		for(i = 0 ; i < szLength ; i++)
			m_pData[i] = Matrix.m_pData[i];
	}

	return *this;
}

template <class ObjectType, bool bPOD_Type>
void TValueMatrix<ObjectType, bPOD_Type>::Transpose()
{
	DEBUG_VERIFY_ALLOCATION;

	size_t y, x;
	ObjectType tmp;

	for(y = 0 ; y < m_Size.cy ; y++)
	{
		for(x = 0 ; x < y ; x++)
		{
			ObjectType& v1 = GetDataRef(SZPOINT(x, y));
			ObjectType& v2 = GetDataRef(SZPOINT(y, x));

			tmp = v1, v1 = v2, v2 = tmp;
		}
	}
}

template <class ObjectType, bool bPOD_Type>
void TValueMatrix<ObjectType, bPOD_Type>::FlipHorizontal()
{
	DEBUG_VERIFY_ALLOCATION;

	size_t y, x;
	ObjectType tmp;

	for(y = 0 ; y < m_Size.cy / 2 ; y++)
	{
		for(x = 0 ; x < m_Size.cx ; x++)
		{
			ObjectType& v1 = GetDataRef(SZPOINT(x, y));
			ObjectType& v2 = GetDataRef(SZPOINT(x, m_Size.cy - y - 1));

			tmp = v1, v1 = v2, v2 = tmp;
		}
	}
}

template <class ObjectType, bool bPOD_Type>
void TValueMatrix<ObjectType, bPOD_Type>::FlipVertical()
{
	DEBUG_VERIFY_ALLOCATION;

	size_t x, y;
	ObjectType tmp;

	for(x = 0 ; x < m_Size.cx / 2 ; x++)
	{
		for(y = 0 ; y < m_Size.cy ; y++)
		{
			ObjectType& v1 = GetDataRef(SZPOINT(x, y));
			ObjectType& v2 = GetDataRef(SZPOINT(m_Size.cx - x - 1, y));

			tmp = v1, v1 = v2, v2 = tmp;
		}
	}
}

#endif // vmatrix_h
