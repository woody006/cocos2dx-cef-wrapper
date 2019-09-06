/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "platform/CCPlatformConfig.h"

// Webview not available on tvOS
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) 

#include "UIWebViewImpl-win32.h"
#include "renderer/CCRenderer.h"
#include "base/CCDirector.h"
#include "platform/CCGLView.h"
#include "platform/CCFileUtils.h"
#include "ui/UIWebView.h"
#include "UICEF/CEFWebViewWrapper.h"


namespace cocos2d {
namespace experimental {
    namespace ui{

WebViewImpl::WebViewImpl(WebView *webView)
        :_webView(webView) 
{
	_uiWebViewWrapper = CEFWebViewWrapper::create();
	_uiWebViewWrapper->retain();

    _uiWebViewWrapper->shouldStartLoading = [this](std::string url) {
        if (this->_webView->_onShouldStartLoading) {
            return this->_webView->_onShouldStartLoading(this->_webView, url);
        }
        return true;
    };
    _uiWebViewWrapper->didFinishLoading = [this](std::string url) {
        if (this->_webView->_onDidFinishLoading) {
            this->_webView->_onDidFinishLoading(this->_webView, url);
        }
    };
    _uiWebViewWrapper->didFailLoading = [this](std::string url) {
        if (this->_webView->_onDidFailLoading) {
            this->_webView->_onDidFailLoading(this->_webView, url);
        }
    };
    _uiWebViewWrapper->onJsCallback = [this](std::string url) {
        if (this->_webView->_onJSCallback) {
            this->_webView->_onJSCallback(this->_webView, url);
        }
    };
}

WebViewImpl::~WebViewImpl(){
	_uiWebViewWrapper->release();
    _uiWebViewWrapper = nullptr;
}

void WebViewImpl::setJavascriptInterfaceScheme(const std::string &scheme) {
	_uiWebViewWrapper->setJavascriptInterfaceScheme(scheme);
}

void WebViewImpl::loadData(const Data &data,
                           const std::string &MIMEType,
                           const std::string &encoding,
                           const std::string &baseURL) {
    
    std::string dataString(reinterpret_cast<char *>(data.getBytes()), static_cast<unsigned int>(data.getSize()));
	_uiWebViewWrapper->loadData(data, MIMEType, encoding, baseURL);
}

void WebViewImpl::loadHTMLString(const std::string &string, const std::string &baseURL) {
	_uiWebViewWrapper->loadHTMLString(string, baseURL);
}

void WebViewImpl::loadURL(const std::string &url) {
    this->loadURL(url, false);
}

void WebViewImpl::loadURL(const std::string &url, bool cleanCachedData) {
	_uiWebViewWrapper->loadURL(url, cleanCachedData);
}

void WebViewImpl::loadFile(const std::string &fileName) {
    auto fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(fileName);
	_uiWebViewWrapper->loadFile(fullPath);
}

void WebViewImpl::stopLoading() {
	_uiWebViewWrapper->stopLoading();
}

void WebViewImpl::reload() {
	_uiWebViewWrapper->reload();
}

bool WebViewImpl::canGoBack() {
    return _uiWebViewWrapper->canGoBack();
}

bool WebViewImpl::canGoForward() {
    return _uiWebViewWrapper->canGoForward();
}

void WebViewImpl::goBack() {
	_uiWebViewWrapper->goBack();
}

void WebViewImpl::goForward() {
	_uiWebViewWrapper->goForward();
}

void WebViewImpl::evaluateJS(const std::string &js) {
	_uiWebViewWrapper->evaluateJS(js);
}

void WebViewImpl::setBounces(bool bounces) {
	_uiWebViewWrapper->setBounces(bounces);
}

void WebViewImpl::setScalesPageToFit(const bool scalesPageToFit) {
	_uiWebViewWrapper->setScalesPageToFit(scalesPageToFit);
}

void WebViewImpl::draw(cocos2d::Renderer *renderer, cocos2d::Mat4 const &transform, uint32_t flags) {
    if (flags & cocos2d::Node::FLAGS_TRANSFORM_DIRTY) {
        
        auto director = cocos2d::Director::getInstance();
        auto glView = director->getOpenGLView();
        auto frameSize = glView->getFrameSize();
        
        auto scaleFactor = glView->getContentScaleFactor();

        auto winSize = director->getWinSize();

        auto leftBottom = this->_webView->convertToWorldSpace(cocos2d::Vec2::ZERO);
        auto rightTop = this->_webView->convertToWorldSpace(cocos2d::Vec2(this->_webView->getContentSize().width, this->_webView->getContentSize().height));

        auto x = (frameSize.width / 2 + (leftBottom.x - winSize.width / 2) * glView->getScaleX()) / scaleFactor;
        auto y = (frameSize.height / 2 - (rightTop.y - winSize.height / 2) * glView->getScaleY()) / scaleFactor;
        auto width = (rightTop.x - leftBottom.x) * glView->getScaleX() / scaleFactor;
        auto height = (rightTop.y - leftBottom.y) * glView->getScaleY() / scaleFactor;

		_uiWebViewWrapper->setBounds(x, y, width, height);
    }
}

void WebViewImpl::setVisible(bool visible){
	_uiWebViewWrapper->setVisible(visible);
}
        
void WebViewImpl::setOpacityWebView(float opacity){
	_uiWebViewWrapper->setOpacityWebView(opacity);
}
        
float WebViewImpl::getOpacityWebView() const{
	return _uiWebViewWrapper->getOpacityWebView();
}

void WebViewImpl::setBackgroundTransparent(){
	_uiWebViewWrapper->setBackgroundTransparent();
}

void WebViewImpl::onEnter()
{
	_uiWebViewWrapper->onEnter();
}

void WebViewImpl::onExit()
{
	_uiWebViewWrapper->onExit();
}

} // namespace ui
} // namespace experimental
} //namespace cocos2d

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
