#ifndef text_container_h
#define text_container_h

class KString;

// ---------------
// Text container
// ---------------
class TTextContainer
{
public:
    virtual ~TTextContainer() {}

    virtual KString GetText() const = 0;

    virtual void SetText(const KString& Text) = 0;
};

#endif // text_container_h
