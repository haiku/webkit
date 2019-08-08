/*
 * Copyright 2019, RAJAGOPALAN-GANGADHARAN <g.raju2000@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef BrowserWindow_H
#define BrowserWindow_H

#include <Window.h>
#include <View.h>
#include <Button.h>
#include <String.h>
#include <TextControl.h>

class BButton;
class BCheckBox;
class BLayoutItem;
class BMenuBar;
class BStatusBar;
class BStringView;
class BTextControl;
class BWebView;

class BrowserWindow:public BWindow
{
public:
	BrowserWindow();
	~BrowserWindow();
	void MessageReceived(BMessage*);
	void Construct(BWebView*);
private:
	void SetStatus(const char*);
	void ChangeUrl(BMessage*);
	void LoadingProgress(double value);
	void ChangeTitle(const char* title);	
	
	BWebView* fWebView;
	
	BMenuBar* m_menuBar;
	BButton* m_BackButton;
	BButton* m_ForwardButton;
	BButton* m_StopButton;
	BButton* m_goButton;
	BTextControl* m_url;
	BStringView* m_statusText;
	BStatusBar* m_loadingProgressBar;
};


#endif // _H
