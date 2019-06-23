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

#include <Window.h>
#include <View.h>
#include <Looper.h>


#include "WKPageConfigurationRef.h"
#include "WKPage.h"
#include "WKView.h"
#include "WKURL.h"
#include "WKString.h"
#include "WKContext.h"
#include "WKPreferencesRef.h"
#include "WKPageNavigationClient.h"
#include "WKPageLoaderClient.h"

#include "WebView.h"
#include "WebViewConstants.h"

BWebView::BWebView(BRect frame,BWindow* myWindow)
{
  initializeOnce();
	//webkit stuff
  auto config = adoptWK(WKPageConfigurationCreate());
  auto prefs = WKPreferencesCreate();
  
  
  WKPreferencesSetDeveloperExtrasEnabled(prefs, true);
  WKPageConfigurationSetPreferences(config.get(),prefs);
  
  fContext = adoptWK(WKContextCreateWithConfiguration(nullptr));
  WKPageConfigurationSetContext(config.get(),fContext.get());
  
  
  fViewPort=adoptWK(WKViewCreate("Webkit",frame,myWindow,config.get()));

}

void BWebView::navigationCallbacks(BLooper* app)
{
	auto page = WKViewGetPage( fViewPort.get());
	WKPageNavigationClientV0 navigationClient={};
	
	navigationClient.base.version = 0;
	navigationClient.base.clientInfo = app;
	
	navigationClient.didCommitNavigation = didCommitNavigation;
	navigationClient.didFinishDocumentLoad = didFinishDocumentLoad;
	navigationClient.didFailNavigation = didFailNavigation;
	navigationClient.didFinishNavigation = didFinishNavigation;
	navigationClient.didReceiveServerRedirectForProvisionalNavigation = didReceiveServerRedirectForProvisionalNavigation;
	WKPageSetPageNavigationClient(page,&navigationClient.base);
}
void BWebView::initializeOnce()
{
	WTF::RunLoop::initializeMainRunLoop();
	WTF::RunLoop::run();
}
void BWebView::loadHTML()
{
	auto page = WKViewGetPage( fViewPort.get());
	WKRetainPtr<WKURLRef> uri;
	uri = adoptWK(WKURLCreateWithUTF8CString("about:blank"));
	WKRetainPtr<WKStringRef> str;
	str = adoptWK(WKStringCreateWithUTF8CString("<body>Hello world</body>"));
	WKPageLoadHTMLString(page,str.get(),uri.get());
}

void BWebView::loadURI(const char* uri)
{
	auto page = WKViewGetPage( fViewPort.get());
	WKRetainPtr<WKURLRef> wuri;
	wuri = adoptWK(WKURLCreateWithUTF8CString(uri));
	WKPageLoadURL(page,wuri.get());
}
void BWebView::goForward()
{
	auto page = WKViewGetPage(fViewPort.get());
	WKPageGoForward(page);
	BMessage message(URL_CHANGE);
	message.AddString("url",BString(getCurrentURL()));
	be_app->PostMessage(&message);
}
void BWebView::goBackward()
{
	auto page = WKViewGetPage(fViewPort.get());
	WKPageGoBack(page);	
	BMessage message(URL_CHANGE);
	message.AddString("url",BString(getCurrentURL()));
	be_app->PostMessage(&message);
}
void BWebView::stop()
{
	auto page = WKViewGetPage(fViewPort.get());
	WKPageClose(page);
}
void BWebView::didCommitNavigation(WKPageRef page, WKNavigationRef navigation, WKTypeRef userData, const void* clientInfo)
{
	BMessage message(DID_COMMIT_NAVIGATION);
	be_app->PostMessage(&message);
}
void BWebView::didReceiveServerRedirectForProvisionalNavigation(WKPageRef page, WKNavigationRef navigation, WKTypeRef userData, const void* clientInfo)
{
	//this wont work for now as we need to make network process work
	//fprintf(stderr,"\n %s %s\n",__PRETTY_FUNCTION__,((BWebView*)clientInfo)->getCurrentURL());
}
void BWebView::didFinishDocumentLoad(WKPageRef page, WKNavigationRef navigation, WKTypeRef userData, const void* clientInfo)
{
	fprintf(stderr,"\n %s %s\n",__PRETTY_FUNCTION__);
}
void BWebView::didFinishNavigation(WKPageRef page, WKNavigationRef navigation, WKTypeRef userData,const void* clientInfo)
{
	BMessage message(DID_FINISH_NAVIGATION);
	be_app->PostMessage(&message);
}

void BWebView::didFailNavigation(WKPageRef page, WKNavigationRef navigation,WKErrorRef, WKTypeRef userData,const void* clientInfo)
{
	fprintf(stderr,"\n %s \n",__PRETTY_FUNCTION__);
}

void BWebView::didFinishProgress(WKPageRef page,const void* clientInfo)
{
	fprintf(stderr,"\n %s \n",__PRETTY_FUNCTION__);
}	
