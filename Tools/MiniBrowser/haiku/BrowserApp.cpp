/*
 * Copyright 2019, RAJAGOPALAN-GANGADHARAN <g.raju2000@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */



#include <Window.h>
#include <View.h>

#include "BrowserApp.h"
#include "BrowserWindow.h"

#include "WebViewConstants.h"


BrowserApp::BrowserApp(void)
	:	BApplication("application/x-vnd.lh-MiniBrowser")
{
	fWindow = new BrowserWindow();
	webView = new BWebView(BRect(100,100,800,800),fWindow);
}

void BrowserApp::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case READY_TO_PAINT:
		webView->paintContent();
		break;
		case URL_LOAD_HANDLE:
		webView->loadURI(message);
		break;
		default:
		BApplication::MessageReceived(message);	
	}
}
void BrowserApp::ReadyToRun()
{
	webView->initializeOnce();
	webView->navigationCallbacks();
	fWindow->Construct(webView);	
	fWindow->Show();
}

int main (void)
{
	BrowserApp *app = new BrowserApp();
	app->Run();
	delete app;
	return 0;
}

