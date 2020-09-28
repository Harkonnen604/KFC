#ifndef visual_binder_h
#define visual_binder_h

#include <KFC_Physics\physical_object.h>
#include <KFC_Graphics\visual_model.h>

// --------------
// Visual binder
// --------------
class TVisualBinder
{
public:
    virtual ~TVisualBinder() {}

    virtual void Bind() const = 0;
};

// -----------------------------------
// Soft physical object visual binder
// -----------------------------------
class TSoftPhysicalObjectVisualBinder : public TVisualBinder
{
private:
    const TSoftPhysicalObject* m_pObject;

    TVisualModelBase* m_pModel;

public:
    TSoftPhysicalObjectVisualBinder();

    ~TSoftPhysicalObjectVisualBinder()
        { Release(); }

    bool IsAllocated() const
        { return m_pObject && m_pModel; }

    void Release();

    void Allocate(  const TSoftPhysicalObject&  SObject,
                    TVisualModelBase&           SModel);

    void Bind() const;
};

#endif // visual_binder_h
