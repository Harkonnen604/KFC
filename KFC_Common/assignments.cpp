#include "kfc_common_pch.h"
#include "assignments.h"

#ifdef _MSC_VER

// -----------------
// Assignments list
// -----------------
void TAssignmentsList::Release()
{
}

void TAssignmentsList::Allocate(HWND hWnd)
{
    TAssignmentsList::Release();

    try
    {
    }

    catch(...)
    {
        TAssignmentsList::Release();
        throw;
    }
}

void TAssignmentsList::Load(const KString& KeyName)
{
    KRegistryKey Key(HKEY_LOCAL_MACHINE, KeyName);

    Key.Create(KEY_READ, 0, true);

    LoadItems(Key);
}

void TAssignmentsList::Save(const KString& KeyName) const
{
    KRegistryKey Key(HKEY_LOCAL_MACHINE, KeyName);

    Key.Create(KEY_WRITE, 0, true);

    SaveItems(Key);
}

#endif // _MSC_VER
