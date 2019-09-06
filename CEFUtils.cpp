#include "CEFUtils.h"
#include "CEFManager.h"

bool cocos2d::CEFUtils::initCEF(void* instance, bool bMultiProcess)
{
	return CEFManager::getInstance()->initCEF((HINSTANCE)instance, bMultiProcess);
}

void cocos2d::CEFUtils::closeCEF()
{
	CEFManager::getInstance()->closeCEF();
}

void cocos2d::CEFUtils::releaseCEF()
{
	CEFManager::releaseInstance();
}