#pragma once

#include "include/base/cef_scoped_ptr.h"
#include "include/cef_browser.h"
#include "CEFClientHandler.h"

class CEFBrowseWindow : public CEFClientHandler::Delegate
{
public:
	// This interface is implemented by the owner of the BrowserWindow. The
	// methods of this class will be called on the main thread.
	class Delegate {
	public:
		// Called when the browser has been created.
		virtual void OnBrowserCreated(const CefRefPtr<CefBrowser>& browser) = 0;

		// Called when the BrowserWindow has been destroyed.
		virtual void OnBrowserWindowDestroyed() = 0;

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
		
		// On window destroyed event.
		virtual void OnWindowDestroyed() = 0;

	protected:
		virtual ~Delegate() {}
	};

	explicit CEFBrowseWindow(Delegate* delegate);
	virtual ~CEFBrowseWindow();

	// Create a new browser and native window.
	void CreateBrowser(const std::string& url, 
		CefWindowHandle parent_handle,
		const CefRect& rect,
		const CefBrowserSettings& settings,
		CefRefPtr<CefRequestContext> request_context);

	// Show the window.
	void Show();

	// Hide the window.
	void Hide();

	// Close the browser
	bool Close(bool force_close);

	// Set the window bounds in parent coordinates.
	void SetBounds(int x, int y, size_t width, size_t height);

	// Set focus to the window.
	void SetFocus(bool focus);

	// Set zoom level
	void SetZoomLevel(double fZoom);

	// Get zoom level
	double GetZoomLevel();

	// Returns the browser owned by the window.
	CefRefPtr<CefBrowser> GetBrowser() const;

	// Returns the window handle.
	CefWindowHandle GetWindowHandle() const;

	// Returns true if the browser is closing.
	bool IsClosing() const;

	// Window enter event.
	void OnEnter();

	// Window close event.
	void OnExit();

	// Window paint event.
	void OnPaint();

	// Window erase event.
	bool OnEraseBkgnd();

	// Window destroy event.
	void OnDestroy();
	
	// Window size event.
	void OnResize();
protected:
	// ClientHandler::Delegate methods.
	void OnBrowserCreated(const CefRefPtr<CefBrowser>& browser) OVERRIDE;
	void OnBrowserClosing(const CefRefPtr<CefBrowser>& browser) OVERRIDE;
	void OnBrowserClosed(const CefRefPtr<CefBrowser>& browser) OVERRIDE;
	void OnSetAddress(const std::string& url) OVERRIDE;
	void OnSetTitle(const std::string& title) OVERRIDE;
	void OnSetFullscreen(bool fullscreen) OVERRIDE;
	void OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward) OVERRIDE;
	void OnLoadingStart(const std::string& url) OVERRIDE;
	void OnLoadingFinish(const std::string& url) OVERRIDE;
	void OnLoadingError(const std::string& url) OVERRIDE;
	bool OnProcessRequest(const std::string& url) OVERRIDE;
	void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions) OVERRIDE;

private:
	Delegate* delegate_;
	CefRefPtr<CefBrowser> browser_;
	CefRefPtr<CEFClientHandler> client_handler_;
	bool is_closing_;
	int  iWindowdId_;
	bool is_sizeDirty_;
	CefRect browserSize_;
	HWND hWnd_;
	DISALLOW_COPY_AND_ASSIGN(CEFBrowseWindow);
};

