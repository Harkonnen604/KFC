#ifndef suspendable_h
#define suspendable_h

#include "basic_types.h"

// ------------
// Suspendable
// ------------
class TSuspendable
{
private:
	size_t m_szSuspendCount;

protected:
	virtual bool OnSuspend	();
	virtual bool OnResume	();

public:
	TSuspendable();

	virtual ~TSuspendable() {}

	void ResetSuspendCount(size_t szSSuspendCount = 0);

	bool SetSuspendCount(size_t szSSuspendCount);

	bool Suspend();
	bool Resume	();

	// ---------------- TRIVIALS ----------------
	size_t GetSuspendCount() const { return m_szSuspendCount; }

	bool IsSuspended() const { return m_szSuspendCount ? true : false; }
};

// ----------
// Suspender
// ----------
class TSuspender
{
private:
	TSuspendable* m_pSuspendable;

public:
	TSuspender(TSuspendable& SSuspendable);

	~TSuspender();

	// ---------------- TRIVIALS ----------------
	TSuspendable* GetSuspendable() const { return m_pSuspendable; }

	bool HasSucceeded() const { return m_pSuspendable != NULL; }
};

#endif // suspendable_h
