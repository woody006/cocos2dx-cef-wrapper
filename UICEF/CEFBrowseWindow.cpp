#include "CEFBrowseWindow.h"

static const wchar_t s_kWndClassName[] = L"CEFBrowseWindowWndClass";
static int s_WindowID_ = 100;

CEFBrowseWindow::CEFBrowseWindow(Delegate* delegate)
	: delegate_(delegate),
	is_closing_(false),
	is_sizeDirty_(false),
	hWnd_(NULL),
	iWindowdId_(++s_WindowID_)
{
	client_handler_ = new CEFClientHandler(this);
}

CEFBrowseWindow::~CEFBrowseWindow()
{
	delegate_ = NULL;
}

LRESULT WINAPI _browseWindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	auto pThis = (CEFBrowseWindow *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (pThis != nullptr)
	{
		switch (Msg)
		{
		case WM_PAINT:
			pThis->OnPaint();
			return 0;

		case WM_ERASEBKGND:
			if (pThis->OnEraseBkgnd())
				break;
			return 0;

		case WM_NCDESTROY:
			SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
			pThis->OnDestroy();
			return 0;

		default:
			break;
		}
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

bool registerWindowClass()
{
	static bool class_registered = false;
	if (class_registered)
	{
		return true;
	}
	class_registered = true;

	const cef_color_t background_color = CefColorSetARGB(255, 255, 255, 255);
	const HBRUSH background_brush = CreateSolidBrush(
		RGB(CefColorGetR(background_color),
			CefColorGetG(background_color),
			CefColorGetB(background_color)));

	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = _browseWindowProc;             // WndProc Handles Messages
	wc.cbClsExtra = 0;                              // No Extra Window Data
	wc.cbWndExtra = 0;                              // No Extra Window Data
	wc.hInstance = GetModuleHandle(nullptr);        // Set The Instance
	wc.hIcon = 0;                                   // Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);       // Load The Arrow Pointer
	wc.hbrBackground = background_brush;            // Background Required For GL
	wc.lpszMenuName = NULL;                         // We Don't Want A Menu
	wc.lpszClassName = s_kWndClassName;             // Set The Class Name

	if (!RegisterClass(&wc) && ERROR_CLASS_ALREADY_EXISTS != GetLastError())
	{
		return false;
	}

	return true;
}

void CEFBrowseWindow::CreateBrowser(const std::string& url, 
	CefWindowHandle parent_handle,
	const CefRect& rect,
	const CefBrowserSettings& settings, 
	CefRefPtr<CefRequestContext> request_context)
{
	registerWindowClass();

	hWnd_ = ::CreateWindowEx(
		WS_EX_CLIENTEDGE,
		s_kWndClassName,   // predefined class 
		NULL,         // no window title 
		WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_EX_TRANSPARENT,
		rect.x,
		rect.y,
		rect.width,
		rect.height,   // set size in WM_SIZE message 
		parent_handle,         // parent window 
		(HMENU)s_WindowID_,   // edit control ID 
		GetModuleHandle(NULL),
		NULL);        // pointer not needed 

	if (hWnd_)
	{
		SetWindowLongPtr(hWnd_, GWLP_USERDATA, (LONG_PTR)this);

		CefWindowInfo window_info;
		browserSize_ = rect;
		RECT wnd_rect = { rect.x, rect.y, rect.width, rect.height };
		window_info.SetAsChild(hWnd_, wnd_rect);

		CefBrowserHost::CreateBrowser(window_info, client_handler_, url, settings, request_context);
	}
}

void CEFBrowseWindow::Show()
{
	HWND hwnd = GetWindowHandle();
	if (hwnd && !::IsWindowVisible(hwnd))
		ShowWindow(hwnd, SW_SHOW);
}

void CEFBrowseWindow::Hide()
{
	HWND hwnd = GetWindowHandle();
	if (hwnd) {
		// When the frame window is minimized set the browser window size to 0x0 to
		// reduce resource usage.
		SetWindowPos(hwnd, NULL,
			0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	}
}

bool CEFBrowseWindow::Close(bool force_close)
{
	if (hWnd_ && !IsClosing()) {
		DestroyWindow(hWnd_);
		//browser_->GetHost()->CloseBrowser(force_close);

		return true;
	}
		
	return false;
}

void CEFBrowseWindow::SetBounds(int x, int y, size_t width, size_t height)
{
	is_sizeDirty_ = true;
	browserSize_.Set(x, y, width, height);

	OnResize();
}

void CEFBrowseWindow::SetFocus(bool focus)
{
	if (browser_)
		browser_->GetHost()->SetFocus(focus);
}

void CEFBrowseWindow::SetZoomLevel(double fZoom)
{
	if (browser_)
		browser_->GetHost()->SetZoomLevel(fZoom);
}

double CEFBrowseWindow::GetZoomLevel()
{
	if (browser_)
		return browser_->GetHost()->GetZoomLevel();

	return 1.0;
}

CefRefPtr<CefBrowser> CEFBrowseWindow::GetBrowser() const 
{
	return browser_;
}

CefWindowHandle CEFBrowseWindow::GetWindowHandle() const
{
	return hWnd_;
}

bool CEFBrowseWindow::IsClosing() const 
{
	return is_closing_;
}

void CEFBrowseWindow::OnEnter()
{
	
}

void CEFBrowseWindow::OnExit()
{
	Close(false);
}

void CEFBrowseWindow::OnBrowserCreated(const CefRefPtr<CefBrowser>& browser)
{
	browser_ = browser;
	
	delegate_->OnBrowserCreated(browser);

	if (is_sizeDirty_)
	{
		OnResize();
	}
}

void CEFBrowseWindow::OnBrowserClosing(const CefRefPtr<CefBrowser>& browser)
{
	is_closing_ = true;
}

void CEFBrowseWindow::OnBrowserClosed(const CefRefPtr<CefBrowser>& browser)
{
	is_closing_ = true;

	if (browser_.get()) {
		browser_ = NULL;
	}

	client_handler_->DetachDelegate();
	client_handler_ = NULL;

	// |this| may be deleted.
	delegate_->OnBrowserWindowDestroyed();
}

void CEFBrowseWindow::OnSetAddress(const std::string& url)
{
	delegate_->OnSetAddress(url);
}

void CEFBrowseWindow::OnSetTitle(const std::string& title) 
{
	delegate_->OnSetTitle(title);
}

void CEFBrowseWindow::OnSetFullscreen(bool fullscreen) 
{
	delegate_->OnSetFullscreen(fullscreen);
}

void CEFBrowseWindow::OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward) 
{
	delegate_->OnSetLoadingState(isLoading, canGoBack, canGoForward);
}

void CEFBrowseWindow::OnLoadingStart(const std::string& url)
{
	delegate_->OnLoadingStart(url);
}

void CEFBrowseWindow::OnLoadingFinish(const std::string& url)
{
	delegate_->OnLoadingFinish(url);
}

void CEFBrowseWindow::OnLoadingError(const std::string& url)
{
	delegate_->OnLoadingError(url);
}

bool CEFBrowseWindow::OnProcessRequest(const std::string& url)
{
	return delegate_->OnProcessRequest(url);
}

void CEFBrowseWindow::OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions) 
{
	delegate_->OnSetDraggableRegions(regions);
}

void CEFBrowseWindow::OnResize()
{
	HWND hwnd = GetWindowHandle();
	if (hwnd) {
		// Set the browser window bounds.
		SetWindowPos(hwnd, NULL, browserSize_.x, browserSize_.y,
			static_cast<int>(browserSize_.width), static_cast<int>(browserSize_.height),
			SWP_NOZORDER);
	}

	if (browser_)
	{
		HWND hBrowseWnd = browser_->GetHost()->GetWindowHandle();
		if (hBrowseWnd)
		{
			is_sizeDirty_ = false;
			SetWindowPos(hBrowseWnd, NULL, 0, 0,
				static_cast<int>(browserSize_.width), static_cast<int>(browserSize_.height),
				SWP_NOZORDER);
		}
	}
}

void CEFBrowseWindow::OnPaint()
{
	PAINTSTRUCT ps;
	BeginPaint(hWnd_, &ps);
	EndPaint(hWnd_, &ps);
}

bool CEFBrowseWindow::OnEraseBkgnd()
{
	// Erase the background when the browser does not exist.
	return (GetBrowser() == NULL);
}

void CEFBrowseWindow::OnDestroy()
{
	hWnd_ = NULL;
}
