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

#include "WebView.h"

class App : public BApplication
{
public:
  App(void);
  void MessageReceived(BMessage *msg);
  void ReadyToRun();
  void testLoader();
private:
    int32 fCount;
    BWindow *myWindow;
    BWebView* webView;
    BRect frame;
};

#endif
