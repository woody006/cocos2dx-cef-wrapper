#include "CEFManager.h"
#include "CEFClientHandler.h"
#include "CEFWebViewWrapper.h"
#include "./include/cef_app.h"

#define USE_CEF_MULTI_THREADED_MESSAGE_LOOP false

CEFManager* CEFManager::instance_ = nullptr;

CEFManager * CEFManager::getInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new CEFManager();
	}

	return instance_;
}

void CEFManager::releaseInstance()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

CEFManager::CEFManager()
	: is_close_cef_(false)
{
}

CEFManager::~CEFManager()
{
	if (thread_.joinable())
	{
		thread_.join();
	}

	releaseCEF();
}

bool CEFManager::initCEF(HINSTANCE instance, bool bMultiProcess)
{
	CefMainArgs mainargs(instance);

	int exit_code = CefExecuteProcess(mainargs, NULL, NULL);
	if (exit_code >= 0)
	{
		// The sub-process has completed so return here.
		return false;
	}

	CefEnableHighDPISupport();
	CefSettings settings;
	settings.multi_threaded_message_loop = isMulThreadedMessageLoop();
	settings.no_sandbox = true;
	settings.single_process = !bMultiProcess;
	auto ret = CefInitialize(mainargs, settings, cef_app_, nullptr);
	if (ret)
	{
		thread_ = std::thread(
			[this]
		{
			for (;;)
			{
				if (this->is_close_cef_ && CEFWebViewWrapper::isEmpty())
				{
					return;
				}
				else if (CEFWebViewWrapper::getWrapperCount() > 0)
				{
					cocos2d::Director::getInstance()->getScheduler()->performFunctionInCocosThread(std::bind(&CEFManager::doMessageLoop, this));
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}
		}
		);

	}

	return ret;
}

void CEFManager::closeCEF()
{
	is_close_cef_ = true;
}

void CEFManager::releaseCEF()
{
	CefShutdown();
}

bool CEFManager::isMulThreadedMessageLoop()
{
	return USE_CEF_MULTI_THREADED_MESSAGE_LOOP;
}

void CEFManager::doMessageLoop()
{
	CefDoMessageLoopWork();
}