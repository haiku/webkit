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
#include <map>
#include <MessageQueue.h>

#include "WebView.h"
using namespace std;

class App : public BApplication
{
public:
  App(void);
  void MessageReceived(BMessage *message);
  void ReadyToRun();
  void testLoader();
  void AttachAndSend(BMessage*);
  void ProcessMessage(BMessage*);
private:
    int32 fCount;
    BWindow *myWindow;
    BWebView* webView;
    BRect frame;
    map<string,BLooper*> proxy;
    status_t result;
    BMessageQueue stash;
};

#endif
