#ifndef APPLICATION_CBAPPLICATION_WIN_H
#define APPLICATION_CBAPPLICATION_WIN_H

private:

	HWND m_mainWindow;
	ATOM m_mainWindowClass;
	HINSTANCE m_thisInstanceOfTheApplication;

	unsigned int m_resolutionWidth, m_resolutionHeight;

private:

	virtual const char* GetMainWindowName() const = 0;
	virtual const char* GetMainWindowClassName() const = 0;
	virtual const WORD* GetLargeIconId() const { return NULL; }
	virtual const WORD* GetSmallIconId() const { return NULL; }

protected:

	static cbApplication* GetApplicationFromWindow( const HWND i_window );

private:

	static LRESULT CALLBACK OnMessageReceivedFromWindows( HWND i_window, UINT i_message, WPARAM i_wParam, LPARAM i_lParam );

#endif
