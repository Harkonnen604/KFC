#ifndef assignments_h
#define assignments_h

#include "registry.h"

#ifdef _MSC_VER

// -----------------
// Assignments list
// -----------------
class TAssignmentsList
{
protected:
	virtual void LoadItems(KRegistryKey& Key) = 0;
	virtual void SaveItems(KRegistryKey& Key) const = 0;

public:
	virtual ~TAssignmentsList() { TAssignmentsList::Release(); }

	virtual void Release();

	virtual void Allocate(HWND hWnd);

	void Load(const KString& KeyName);
	void Save(const KString& KeyName) const;
};

#endif // _MSC_VER

#endif // assignments_h
