#include "CEFClientHandler.h"
#include <sstream>
#include <string>
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

CEFClientHandler::CEFClientHandler(Delegate* delegate)
	: is_closing_(false) 
	, delegate_(delegate)
	, browser_count_(0)
{

}

CEFClientHandler::~CEFClientHandler()
{
	delegate_ = NULL;
}

void CEFClientHandler::DetachDelegate() {
	DCHECK(delegate_);
	delegate_ = NULL;
}

void CEFClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	browser_count_++;

	if (delegate_)
		delegate_->OnBrowserCreated(browser);
}

bool CEFClientHandler::DoClose(CefRefPtr<CefBrowser> browser) 
{
	CEF_REQUIRE_UI_THREAD();

	is_closing_ = true;

	if (delegate_)
		delegate_->OnBrowserClosing(browser);

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void CEFClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) 
{
	CEF_REQUIRE_UI_THREAD();

	is_closing_ = true;

	--browser_count_;
	
	if (delegate_)
		delegate_->OnBrowserClosed(browser);
}

void CEFClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
	bool isLoading,
	bool canGoBack,
	bool canGoForward)
{
	CEF_REQUIRE_UI_THREAD();

	if (delegate_)
		delegate_->OnSetLoadingState(isLoading, canGoBack, canGoForward);
}

void CEFClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	CEF_REQUIRE_UI_THREAD();

	if (delegate_)
	{
		delegate_->OnLoadingStart(frame->GetURL());
	}
}

void CEFClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	CEF_REQUIRE_UI_THREAD();

	if (delegate_)
	{
		delegate_->OnLoadingFinish(frame->GetURL());
	}
}

void CEFClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl)
{
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(failedUrl) <<
		" with error " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);

	if (delegate_)
	{
		delegate_->OnLoadingError(failedUrl);
	}
}

void CEFClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, 
	CefRefPtr<CefFrame> frame, 
	CefRefPtr<CefContextMenuParams> params, 
	CefRefPtr<CefMenuModel> model)
{
	CEF_REQUIRE_UI_THREAD();
	model->Clear();
}

bool CEFClientHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefRequest> request, bool is_redirect)
{
	if (delegate_)
	{
		return !delegate_->OnProcessRequest(request->GetURL());
	}

	return false;
}

bool CEFClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser, 
	CefRefPtr<CefFrame> frame, 
	const CefString& target_url, 
	const CefString& target_frame_name, 
	CefLifeSpanHandler::WindowOpenDisposition target_disposition,
	bool user_gesture, 
	const CefPopupFeatures& popupFeatures, 
	CefWindowInfo& windowInfo, 
	CefRefPtr<CefClient>& client, 
	CefBrowserSettings& settings, 
	bool* no_javascript_access)
{
	CEF_REQUIRE_UI_THREAD();

	switch (target_disposition)
	{
	case WOD_NEW_FOREGROUND_TAB:
	case WOD_NEW_BACKGROUND_TAB:
	case WOD_NEW_POPUP:
	case WOD_NEW_WINDOW:
		browser->GetMainFrame()->LoadURL(target_url);
		return true; //cancel create
	}

	return false;
}

void CEFClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
	CEF_REQUIRE_UI_THREAD();

	if (delegate_)
		delegate_->OnSetTitle(title);
}

void CEFClientHandler::OnFullscreenModeChange(CefRefPtr<CefBrowser> browser,
	bool fullscreen) 
{
	CEF_REQUIRE_UI_THREAD();

	if (delegate_)
		delegate_->OnSetFullscreen(fullscreen);
}

void CEFClientHandler::OnAddressChange(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& url) 
{
	CEF_REQUIRE_UI_THREAD();

	// Only update the address for the main (top-level) frame.
	if (frame->IsMain()) {
		if (delegate_)
			delegate_->OnSetAddress(url);
	}
}