#ifndef flag_setter_h
#define flag_setter_h

// ------------
// Flag setter
// ------------
class TFlagSetter
{
private:
	bool* m_pFlag;

public:
	TFlagSetter(bool& bSFlag);

	~TFlagSetter();
};

#endif // flag_setter_h
