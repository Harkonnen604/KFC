#ifndef synched_value_h
#define synched_value_h

#include "critical_section.h"

// --------------
// Synched value
// --------------
template <class t>
class TSynchedValue
{
private:
	t m_Data;

	mutable TCriticalSection m_CriticalSection;
	
public:
	TSynchedValue() {}

	TSynchedValue(const t& SData) : m_Data(SData) {}

	operator t () const
	{
		TCriticalSectionLocker Locker0(m_CriticalSection);

		return m_Data;
	}

	TSynchedValue<t>& operator = (const t& SData)
	{
		TCriticalSectionLocker Locker0(m_CriticalSection);

		m_Data = SData;

		return *this;
	}

	TSynchedValue<t>& operator -= (const t& SData)
	{
		TCriticalSectionLocker Locker0(m_CriticalSection);

		m_Data -= m_Data;

		return *this;
	}

	TSynchedValue<t>& operator += (const t& SData)
	{
		TCriticalSectionLocker Locker0(m_CriticalSection);

		m_Data += m_Data;

		return *this;
	}

	TSynchedValue<t>& operator *= (const t& SData)
	{
		TCriticalSectionLocker Locker0(m_CriticalSection);

		m_Data *= m_Data;

		return *this;
	}

	TSynchedValue<t>& operator /= (const t& SData)
	{
		TCriticalSectionLocker Locker0(m_CriticalSection);

		m_Data /= m_Data;

		return *this;
	}

	TSynchedValue<t>& operator -- ()
	{
		TCriticalSectionLocker Locker0(m_CriticalSection);

		--m_Data;

		return *this;
	}

	TSynchedValue<t>& operator ++ ()
	{
		TCriticalSectionLocker Locker1(m_CriticalSection);

		++m_Data;

		return *this;
	}

	t operator -- (int)
	{
		TCriticalSectionLocker Locker0(m_CriticalSection);

		return m_Data--;
	}

	t operator ++ (int)
	{
		TCriticalSectionLocker Locker0(m_CriticalSection);

		return m_Data++;
	}
};

#endif // synched_value_h
