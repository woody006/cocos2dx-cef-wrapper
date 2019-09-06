#pragma once

#include "cocos2d.h"
#include "CEFBrowseWindow.h"

class CEFWebViewWrapper : public cocos2d::Ref, public CEFBrowseWindow::Delegate
{
public:
	CEFWebViewWrapper();
	~CEFWebViewWrapper();

	/**
	 * Allocates and initializes a WebView.
	 */
	static CEFWebViewWrapper *create(const std::string& url, const cocos2d::Rect& rect);
	static CEFWebViewWrapper *create();

	/**
	 * Set javascript interface scheme.
	 *
	 * @see WebView::setOnJSCallback()
	 */
	void setJavascriptInterfaceScheme(const std::string &scheme) { strCustomScheme_ = scheme; };

	/**
	 * Sets the main page contents, MIME type, content encoding, and base URL.
	 *
	 * @param data The content for the main page.
	 * @param MIMEType The MIME type of the data.
	 * @param encoding The encoding of the data.
	 * @param baseURL The base URL for the content.
	 */
	void loadData(const cocos2d::Data &data,
		const std::string &MIMEType,
		const std::string &encoding,
		const std::string &baseURL);

	/**
	 * Sets the main page content and base URL.
	 *
	 * @param string The content for the main page.
	 * @param baseURL The base URL for the content.
	 */
	void loadHTMLString(const std::string &string, const std::string &baseURL = "");

	/**
	 * Loads the given URL. It doesn't clean cached data.
	 *
	 * @param url Content URL.
	 */
	void loadURL(const std::string &url);

	/**
	 * Loads the given URL with cleaning cached data or not.
	 * @param url Content URL.
	 * @cleanCachedData Whether to clean cached data.
	 */
	void loadURL(const std::string &url, bool cleanCachedData);

	/**
	 * Loads the given fileName.
	 *
	 * @param fileName Content fileName.
	 */
	void loadFile(const std::string &fileName);

	/**
	 * Stops the current load.
	 */
	void stopLoading();

	/**
	 * Reloads the current URL.
	 */
	void reload();

	/**
	 * Gets whether this WebView has a back history item.
	 *
	 * @return WebView has a back history item.
	 */
	bool canGoBack();

	/**
	 * Gets whether this WebView has a forward history item.
	 *
	 * @return WebView has a forward history item.
	 */
	bool canGoForward();

	/**
	 * Goes back in the history.
	 */
	void goBack();

	/**
	 * Goes forward in the history.
	 */
	void goForward();

	/**
	 * Evaluates JavaScript in the context of the currently displayed page.
	 */
	void evaluateJS(const std::string &js);

	
	/**
	 * Set whether the webview bounces at end of scroll of WebView.
	 */
	void setBounces(bool bounce) {};

	/**
	 * Set WebView should support zooming. The default value is false.
	 */
	void setScalesPageToFit(const bool scalesPageToFit);

	/**
	 * Toggle visibility of WebView.
	 */
	void setVisible(bool visible);
	/**
	 * SetOpacity of webview.
	 */
	void setOpacityWebView(float opacity);

	/**
	 * getOpacity of webview.
	 */
	float getOpacityWebView() const;

	/**
	 * set the background transparent
	 */
	void setBackgroundTransparent();

	/**
	 * close the browser
	 */
	bool closeBrowser();

	/**
	 * set the browser bounds
	 */
	void setBounds(int x, int y, size_t width, size_t height);

	/**
	 * on enter event
	 */
	void onEnter();

	/**
	 * on exit event
	 */
	void onExit();

public:
	static bool closeAll() {
		s_bExitApp_ = true;

		bool bClose = false;
		
		if (!s_vec_webView_.empty())
		{
			auto vTemp = s_vec_webView_;
			for each(auto iter in vTemp)
			{
				bClose |= iter->closeBrowser();
			}
		}
	
		return bClose;
	};

	static void addWebView(CEFWebViewWrapper* webView) {
		if (!s_vec_webView_.contains(webView))
		{
			s_vec_webView_.pushBack(webView);
		}
	}

	static void deleteWebView(CEFWebViewWrapper* webView) {
		s_vec_webView_.eraseObject(webView, true);

		if (s_bExitApp_ && s_vec_webView_.empty())
		{
			//quit message should run in main thread.
			cocos2d::AsyncTaskPool::getInstance()->enqueue(cocos2d::AsyncTaskPool::TaskType::TASK_OTHER, [](void*) {
				::PostQuitMessage(0);
			}, nullptr, []() {});
		}
	}

	static bool isEmpty() {
		return s_vec_webView_.empty();
	}

	static bool shouldCloseApp() {
		return s_bExitApp_ && s_vec_webView_.empty();
	}

	static int getWrapperCount() { return s_iWrapperCount_; }

	static float getDeviceScaleFactor();

private:
	void hookWindowsProc();
	static LRESULT CALLBACK hookGLFWWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	std::function<bool(std::string url)> shouldStartLoading = nullptr;
	std::function<void(std::string url)> didFinishLoading = nullptr;
	std::function<void(std::string url)> didFailLoading = nullptr;
	std::function<void(std::string url)> onJsCallback = nullptr;

protected:
	bool init(const std::string& url, const cocos2d::Rect& rect);

	// Called when the browser has been created.
	virtual void OnBrowserCreated(const CefRefPtr<CefBrowser>& browser) override;

	// Called when the BrowserWindow has been destroyed.
	virtual void OnBrowserWindowDestroyed() override;

	// Set the window URL address.
	virtual void OnSetAddress(const std::string& url) override;

	// Set the window title.
	virtual void OnSetTitle(const std::string& title) override;

	// Set fullscreen mode.
	virtual void OnSetFullscreen(bool fullscreen) override;

	// Set the loading state.
	virtual void OnSetLoadingState(bool isLoading, bool canGoBack, bool canGoForward) override;

	// Called the loading start.
	virtual void OnLoadingStart(const std::string& url) override;

	// Called the loading finish.
	virtual void OnLoadingFinish(const std::string& url) override;

	// Called the loading error.
	virtual void OnLoadingError(const std::string& url) override;

	// On process request event.
	virtual bool OnProcessRequest(const std::string& url) override;

	// Set the draggable regions.
	virtual void OnSetDraggableRegions(const std::vector<CefDraggableRegion>& regions) override;

	// On window destroyed event.
	virtual void OnWindowDestroyed() override;

private:
	bool bIsCreated_;
	bool bScalePageToFit_;
	std::string strUrl_;
	std::string strCustomScheme_;
	CEFBrowseWindow* cef_browse_window_;

	typedef cocos2d::Vector<CEFWebViewWrapper*> WebViewList;
	static WebViewList s_vec_webView_;
	static WNDPROC	s_pCocosWndProc_;
	static bool s_bExitApp_;
	static int s_iWrapperCount_;
};

