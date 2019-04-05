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

#include "WKRetainPtr.h"

class App : public BApplication
{
public:
  App(void);
  void MessageReceived(BMessage *msg);
  void testLoader();
private:
    int32 fCount;
    BWindow *myWindow;
    WKRetainPtr<WKViewRef> fViewPort;
    WKRetainPtr<WKContextRef> fContext;
};

#endif
