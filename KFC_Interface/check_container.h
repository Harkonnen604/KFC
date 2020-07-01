#ifndef check_container_h
#define check_container_h

// ----------------
// Check container
// ----------------
class TCheckContainer
{
public:
	virtual bool GetCheck(bool* pRSuccess = NULL) const = 0;

	virtual bool SetCheck(bool bCheck) = 0;
};

#endif // check_container_h