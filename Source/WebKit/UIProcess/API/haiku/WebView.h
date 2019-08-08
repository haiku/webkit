/*
 * Copyright (C) 2019 Haiku, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "config.h"
#include "WKPage.h"
#include "WKView.h"
#include "WKContext.h"
#include "WKRetainPtr.h"
#include "WKAPICast.h"
#include "WebViewBase.h"
#include "PageLoadStateObserver.h"

using namespace WebKit;

class BWebView
{
    public:
    BWebView(BRect,BWindow*);
    void initializeOnce();
    void loadHTML();
    void loadURIRequest(const char*);//use this in app to load a url
    void loadURI(BMessage*);
    void goForward();
    void goBackward();
    void stop();
    WebViewBase* getRenderView() { return toImpl(fViewPort.get()); }
    const char* getCurrentURL() { return getRenderView()->currentURL(); }
    BLooper* getAppLooper() { return fAppLooper; }
    
    void navigationCallbacks();
    double didChangeProgress();
    const char* title();
    
    private:
    WKRetainPtr<WKViewRef> fViewPort;
    WKRetainPtr<WKContextRef> fContext;
    static void didCommitNavigation(WKPageRef page, WKNavigationRef navigation, WKTypeRef userData, const void* clientInfo);
    static void didReceiveServerRedirectForProvisionalNavigation(WKPageRef page, WKNavigationRef navigation, WKTypeRef userData, const void* clientInfo);
    static void didFinishDocumentLoad(WKPageRef page, WKNavigationRef navigation, WKTypeRef userData, const void* clientInfo);
    static void didFinishProgress(WKPageRef,const void*);
    static void didFinishNavigation(WKPageRef page, WKNavigationRef navigation, WKTypeRef userData,const void* clientInfo);
    static void didFailNavigation(WKPageRef page, WKNavigationRef navigation, WKErrorRef,WKTypeRef userData,const void* clientInfo);
    PageLoadStateObserver* fObserver;
    BLooper* fAppLooper;
};

