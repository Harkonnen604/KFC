#ifndef notification_h
#define notification_h

// -------------
// Notification
// -------------
struct TNotification : public NMHDR
{
    TNotification(HWND hSWnd, int iID, UINT uiSCode)
    {
        hwndFrom    = hSWnd;
        idFrom      = iID;
        code        = uiSCode;
    }
};

#endif // notification_h
