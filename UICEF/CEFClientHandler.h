#pragma once

#include "include/base/cef_lock.h"
#include "include/cef_client.h"

class CEFClientHandler : public CefClient,
						 public CefDisplayHandler,
						 public CefLifeSpanHandler,
						 public CefLoadHandler,
						 public CefContextMenuHandler,
						 public CefRequestHandler
{
public:
	// Implement this interface to receive notification of ClientHandler
	// events. The methods of this class will be called on the main thread.
	class Delegate {
	public:
		// Called when the browser is created.
		virtual void OnBrowserCreated(const CefRefPtr<CefBrowser>& browser) = 0;

		// Called when the browser is closing.
		virtual void OnBrowserClosing(const CefRefPtr<CefBrowser>& browser) = 0;

		// Called when the browser has been closed.
		virtual void OnBrowserClosed(const CefRefPtr<CefBrowser>& browser) = 0;

		// Set the window URL address.
		virtual void OnSetAddress(const std::string& url) = 0;

		// Set the window title.
		virtual void OnSetTitle(const std::string& title) = 0;

		// Set fullscreen mode.
		virtual void OnSetFullscreen(bool fullscreen) = 0;

		// Set the loading state.
		virtual void OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward) = 0;

		// Called the loading start.
		virtual void OnLoadingStart(const std::string& url) = 0;

		// Called the loading finish.
		virtual void OnLoadingFinish(const std::string& url) = 0;

		// Called the loading error.
		virtual void OnLoadingError(const std::string& url) = 0;

		// On process request event.
		virtual bool OnProcessRequest(const std::string& url) = 0;

		// Set the draggable regions.
		virtual void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions) = 0;

	protected:
		virtual ~Delegate() {}
	};

public:
	CEFClientHandler(Delegate* delegate);
	virtual ~CEFClientHandler();
	// This object may outlive the Delegate object so it's necessary for the
	// Delegate to detach itself before destruction.
	void DetachDelegate();

	// CefClient methods:
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE {
		return this;
	}

	// CefDisplayHandler methods:
	virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& url) OVERRIDE;
	virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
		const CefString& title) OVERRIDE;
	virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser,
		bool fullscreen) OVERRIDE;

	// CefLifeSpanHandler methods:
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		const CefString& target_url,
		const CefString& target_frame_name,
		CefLifeSpanHandler::WindowOpenDisposition target_disposition,
		bool user_gesture,
		const CefPopupFeatures& popupFeatures,
		CefWindowInfo& windowInfo,
		CefRefPtr<CefClient>& client,
		CefBrowserSettings& settings,
		bool* no_javascript_access) OVERRIDE;

	// CefLoadHandler methods:
	virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
		bool isLoading,
		bool canGoBack,
		bool canGoForward) OVERRIDE;
	virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame) OVERRIDE;
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode) OVERRIDE;
	virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) OVERRIDE;

	// CefContextMenuHandler methods:
	virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) OVERRIDE;

	// CefRequestHandler methods:
	bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool is_redirect) OVERRIDE;

	// Class member methods:
	bool IsClosing() const { return is_closing_; }
	int GetBrowserCount() const { return browser_count_; }

private:
	// MAIN THREAD MEMBERS
	// The following members will only be accessed on the main thread. This will
	// be the same as the CEF UI thread except when using multi-threaded message
	// loop mode on Windows.
	Delegate* delegate_;
	int browser_count_;
	bool is_closing_;

	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(CEFClientHandler);
	DISALLOW_COPY_AND_ASSIGN(CEFClientHandler);
};

