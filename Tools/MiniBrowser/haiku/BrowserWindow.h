/*
 * Copyright 2019, RAJAGOPALAN-GANGADHARAN <g.raju2000@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef BrowserWindow_H
#define BrowserWindow_H

#include <Window.h>
#include <View.h>
#include <WebKit/WebView.h>

class BrowserWindow:public BWindow{
public:
	BWebView* webView;
	
};


#endif // _H
