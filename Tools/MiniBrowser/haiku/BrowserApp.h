/*
 * Copyright 2019, RAJAGOPALAN-GANGADHARAN <g.raju2000@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef APP_H
#define APP_H

#include <Application.h>
#include <map>
#include <MessageQueue.h>

#include "WebView.h"
using namespace std;

class BrowserWindow;

class BrowserApp : public BApplication
{
public:
	BrowserApp(void);
	void MessageReceived(BMessage *message);
	void ReadyToRun();
private:
	void LocalMessage(BMessage*);
	void GlobalMessage(BMessage*);
	BWebView* webView;
	status_t result;
	map<string,BLooper*> looperMapping;
	map<string,BMessage*> messengerMapping;
	BrowserWindow* fWindow;
};

#endif
