#pragma once

#include "cocos2d.h"
#include "./include/cef_app.h"

class CEFManager
{
public:
	static CEFManager * getInstance();
	static void releaseInstance();
	
	bool initCEF(HINSTANCE instance, bool bMultiProcess);
	void closeCEF();
	void releaseCEF();

private:
	CEFManager();
	~CEFManager();
	void doMessageLoop();
	bool isMulThreadedMessageLoop();

private:
	CefRefPtr<CefApp>	cef_app_;
	bool				is_close_cef_;
	std::thread			thread_;
	static CEFManager*	instance_;
};
