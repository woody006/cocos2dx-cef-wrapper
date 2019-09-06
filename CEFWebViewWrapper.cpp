#include "CEFWebViewWrapper.h"
#include "CEFManager.h"
#include "include/cef_parser.h"

CEFWebViewWrapper::WebViewList CEFWebViewWrapper::s_vec_webView_;
WNDPROC CEFWebViewWrapper::s_pCocosWndProc_ = nullptr;
bool CEFWebViewWrapper::s_bExitApp_ = false;
int CEFWebViewWrapper::s_iWrapperCount_ = 0;

CEFWebViewWrapper::CEFWebViewWrapper()
	: bIsCreated_(false)
	, bScalePageToFit_(false)
	, cef_browse_window_(nullptr)
{
	s_iWrapperCount_++;
}

CEFWebViewWrapper::~CEFWebViewWrapper()
{
	s_iWrapperCount_--;

	if (cef_browse_window_)
	{ 
		delete cef_browse_window_;
		cef_browse_window_ = nullptr;
	}
}

CEFWebViewWrapper * CEFWebViewWrapper::create(const std::string& url, const cocos2d::Rect& rect)
{
	CEFWebViewWrapper* webView = new(std::nothrow) CEFWebViewWrapper();
	if (webView && webView->init(url, rect))
	{
		webView->autorelease();
		return webView;
	}
	CC_SAFE_DELETE(webView);

	return nullptr;
}

CEFWebViewWrapper * CEFWebViewWrapper::create()
{
	return create("", cocos2d::Rect::ZERO);
}

bool CEFWebViewWrapper::init(const std::string& url, const cocos2d::Rect& rect)
{
	cef_browse_window_ = new(std::nothrow) CEFBrowseWindow(this);
	if (cef_browse_window_)
	{

		auto direct = cocos2d::Director::getInstance();
		CefRect cer_rect = { (int)rect.origin.x, (int)rect.origin.y, (int)rect.size.width, (int)rect.size.height };
		auto hWnd = direct->getOpenGLView()->getWin32Window();

		CefBrowserSettings browser_settings;
		cef_browse_window_->CreateBrowser(url, hWnd, cer_rect, browser_settings, NULL);

		return true;
	}

	return false;
}

void CEFWebViewWrapper::OnBrowserCreated(const CefRefPtr<CefBrowser>& browser)
{
	bIsCreated_ = true;
	hookWindowsProc();
	addWebView(this);

	if (!strUrl_.empty())
	{
		loadURL(strUrl_);
	}
}

void CEFWebViewWrapper::OnBrowserWindowDestroyed()
{
	bIsCreated_ = false;
	deleteWebView(this);
}

void CEFWebViewWrapper::OnSetAddress(const std::string & url)
{
}

void CEFWebViewWrapper::OnSetTitle(const std::string & title)
{
}

void CEFWebViewWrapper::OnSetFullscreen(bool fullscreen)
{
}

void CEFWebViewWrapper::OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward)
{
}

void CEFWebViewWrapper::OnLoadingStart(const std::string& url)
{
	
}

void CEFWebViewWrapper::OnLoadingFinish(const std::string& url)
{
	if (didFinishLoading)
	{
		didFinishLoading(url);
	}
}

void CEFWebViewWrapper::OnLoadingError(const std::string& url)
{
	if (didFailLoading)
	{
		didFailLoading(url);
	}
}

bool CEFWebViewWrapper::OnProcessRequest(const std::string& url)
{
	std::string scheme = url.substr(0, url.find_first_of(':'));
	if (scheme == strCustomScheme_)
	{
		if (onJsCallback)
		{
			onJsCallback(url);
		}
		return false;
	}

	if (shouldStartLoading != nullptr && url != "")
	{
		return shouldStartLoading(url);
	}

	return true;
}

void CEFWebViewWrapper::OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions)
{
}

void CEFWebViewWrapper::OnWindowDestroyed()
{

}

void CEFWebViewWrapper::loadData(const cocos2d::Data & data, const std::string & MIMEType, const std::string & encoding, const std::string & baseURL)
{
	
}

void CEFWebViewWrapper::loadHTMLString(const std::string &string, const std::string &baseURL /*= ""*/)
{
	if (bIsCreated_)
	{
		cef_browse_window_->GetBrowser()->GetMainFrame()->LoadString(string, baseURL);
	}
}

void CEFWebViewWrapper::loadURL(const std::string &url)
{
	strUrl_ = url;
	if (bIsCreated_)
	{
		cef_browse_window_->GetBrowser()->GetMainFrame()->LoadURL(url);
	}
}

void CEFWebViewWrapper::loadURL(const std::string & url, bool cleanCachedData)
{
	loadURL(url);
}

void CEFWebViewWrapper::loadFile(const std::string & fileName)
{
	loadURL(fileName);
}

void CEFWebViewWrapper::stopLoading()
{
	if (bIsCreated_)
	{
		cef_browse_window_->GetBrowser()->StopLoad();
	}
}

void CEFWebViewWrapper::reload()
{
	if (bIsCreated_)
	{
		cef_browse_window_->GetBrowser()->ReloadIgnoreCache();
	}
}

bool CEFWebViewWrapper::canGoBack()
{
	if (bIsCreated_)
	{
		cef_browse_window_->GetBrowser()->CanGoBack();
	}

	return false;
}

bool CEFWebViewWrapper::canGoForward()
{
	if (bIsCreated_)
	{
		cef_browse_window_->GetBrowser()->CanGoForward();
	}

	return false;
}

void CEFWebViewWrapper::goBack()
{
	if (bIsCreated_)
	{
		cef_browse_window_->GetBrowser()->GoBack();
	}
}

void CEFWebViewWrapper::goForward()
{
	if (bIsCreated_)
	{
		cef_browse_window_->GetBrowser()->GoForward();
	}
}

void CEFWebViewWrapper::evaluateJS(const std::string & js)
{
}

void CEFWebViewWrapper::setScalesPageToFit(const bool scalesPageToFit)
{
	bScalePageToFit_ = scalesPageToFit;
}

void CEFWebViewWrapper::setVisible(bool visible)
{
	if (cef_browse_window_)
	{
		if (visible)
		{
			cef_browse_window_->Show();
		}
		else
		{
			cef_browse_window_->Hide();
		}
	}
}

void CEFWebViewWrapper::setOpacityWebView(float opacity)
{
}

float CEFWebViewWrapper::getOpacityWebView() const
{
	return 0.0f;
}

void CEFWebViewWrapper::setBackgroundTransparent()
{

}

bool CEFWebViewWrapper::closeBrowser()
{
	if (cef_browse_window_)
	{
		return cef_browse_window_->Close(false);
	}

	return false;
}

void CEFWebViewWrapper::setBounds(int x, int y, size_t width, size_t height)
{
	if (cef_browse_window_)
	{
		cef_browse_window_->SetBounds(x, y, width, height);
	}
}

void CEFWebViewWrapper::onEnter()
{
	if (cef_browse_window_)
	{
		if (bScalePageToFit_)
		{
			//cef_browse_window_->SetZoomLevel(CEFWebViewWrapper::getDeviceScaleFactor());
		}
		
		cef_browse_window_->OnEnter();
	}
}

void CEFWebViewWrapper::onExit()
{
	if (cef_browse_window_)
	{
		cef_browse_window_->OnExit();
	}
}

float CEFWebViewWrapper::getDeviceScaleFactor()
{
	static float scale_factor = 1.0;
	static bool initialized = false;

	if (!initialized) {
		// This value is safe to cache for the life time of the app since the user
		// must logout to change the DPI setting. This value also applies to all
		// screens.
		HDC screen_dc = ::GetDC(NULL);
		int dpi_x = GetDeviceCaps(screen_dc, LOGPIXELSX);
		scale_factor = static_cast<float>(dpi_x) / 96.0;
		::ReleaseDC(NULL, screen_dc);
		initialized = true;
	}

	return scale_factor;
}

void CEFWebViewWrapper::hookWindowsProc()
{
	if (!s_pCocosWndProc_)
	{
		auto hwndCocos = cocos2d::Director::getInstance()->getOpenGLView()->getWin32Window();
		s_pCocosWndProc_ = (WNDPROC)SetWindowLongPtr(hwndCocos, GWL_WNDPROC, (LONG_PTR)hookGLFWWindowProc);
	}
}

LRESULT CEFWebViewWrapper::hookGLFWWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		if (CEFWebViewWrapper::closeAll())
		{
			return 0;
		}
	}break;
	case WM_QUIT:
	{
		//CefQuitMessageLoop();
	}break;
	default:
		break;
	}

	return ::CallWindowProc(s_pCocosWndProc_, hwnd, uMsg, wParam, lParam);
}
