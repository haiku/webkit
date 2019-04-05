/*
 * Copyright 2019, RAJAGOPALAN-GANGADHARAN <g.raju2000@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef APP_H
#define APP_H

#include <Application.h>
#include <Button.h>
#include <String.h>
#include <TextControl.h>
<<<<<<< HEAD
#include <map>
#include <MessageQueue.h>

#include "WebView.h"
using namespace std;
=======

#include "WKRetainPtr.h"
>>>>>>> f4cd9c312ea... Initial implementation to load blank url and now trying to load html string Note:destructors not handled yet

class App : public BApplication
{
public:
<<<<<<< HEAD
	App(void);
	void MessageReceived(BMessage *message);
	void ReadyToRun();
	void testLoader();
	void LocalMessage(BMessage*);
	void GlobalMessage(BMessage*);
private:
    int32 fCount;
    BWindow *myWindow;
    BWebView* webView;
    BRect frame;
    status_t result;
    map<string,BLooper*> looperMapping;
	map<string,BMessage*> messengerMapping;
=======
  App(void);
  void MessageReceived(BMessage *msg);
  void testLoader();
private:
    int32 fCount;
    BWindow *myWindow;
    WKRetainPtr<WKViewRef> fViewPort;
    WKRetainPtr<WKContextRef> fContext;
>>>>>>> f4cd9c312ea... Initial implementation to load blank url and now trying to load html string Note:destructors not handled yet
};

#endif
