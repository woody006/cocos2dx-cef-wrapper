#pragma once

#include "platform/CCPlatformMacros.h"

namespace cocos2d {

class CC_DLL CEFUtils
{
public:
	static bool initCEF(void* instance, bool bMultiProcess);
	static void closeCEF();
	static void releaseCEF();
};

};
