/*
 * Copyright 2019, RAJAGOPALAN-GANGADHARAN <g.raju2000@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
 
#include "BrowserWindow.h"

#include <interface/StringView.h>
#include <Button.h>
#include <Entry.h>
#include <GridLayoutBuilder.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <SeparatorView.h>
#include <SpaceLayoutItem.h>
#include <StatusBar.h>
#include <StringView.h>
#include <TextControl.h>

#include "WebViewConstants.h"
#include "WebView.h"

enum {
	OPEN_LOCATION = 'open',
	OPEN_INSPECTOR = 'insp',
	SAVE_PAGE = 'save',
	GO_BACK = 'goba',
	GO_FORWARD = 'gofo',
	STOP = 'stop',
	GOTO_URL = 'goul',
	RELOAD = 'reld',

	TEXT_SIZE_INCREASE = 'tsin',
	TEXT_SIZE_DECREASE = 'tsdc',
	TEXT_SIZE_RESET = 'tsrs',
	
	NEW_WINDOW = 'nwnd',
    WINDOW_OPENED = 'wndo',
    WINDOW_CLOSED = 'wndc',
};

BrowserWindow::BrowserWindow()
:
BWindow(BRect(100,100,800,800),"MiniBrowser",
			B_DOCUMENT_WINDOW_LOOK,B_NORMAL_WINDOW_FEEL,
			B_AUTO_UPDATE_SIZE_LIMITS | B_ASYNCHRONOUS_CONTROLS
			| B_QUIT_ON_WINDOW_CLOSE ),
fWebView(NULL)
{
	SetLayout(new BGroupLayout(B_HORIZONTAL));
	
	m_menuBar = new BMenuBar("Main menu");
	BMenu* menu = new BMenu("Window");
	BMessage* newWindowMessage = new BMessage(NEW_WINDOW);
	newWindowMessage->AddString("url", "");
	BMenuItem* newItem = new BMenuItem("New", newWindowMessage, 'N');
	menu->AddItem(newItem);
	newItem->SetTarget(be_app);
	menu->AddItem(new BMenuItem("Open location", new BMessage(OPEN_LOCATION), 'L'));
	menu->AddItem(new BMenuItem("Inspect page", new BMessage(OPEN_INSPECTOR), 'I'));
	menu->AddItem(new BMenuItem("Save page", new BMessage(SAVE_PAGE), 'S'));
	menu->AddSeparatorItem();
	menu->AddItem(new BMenuItem("Close", new BMessage(B_QUIT_REQUESTED), 'W', B_SHIFT_KEY));
	BMenuItem* quitItem = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q');
	menu->AddItem(quitItem);
	quitItem->SetTarget(be_app);
	m_menuBar->AddItem(menu);
	
	menu = new BMenu("Text");
	menu->AddItem(new BMenuItem("Increase size", new BMessage(TEXT_SIZE_INCREASE), '+'));
	menu->AddItem(new BMenuItem("Decrease size", new BMessage(TEXT_SIZE_DECREASE), '-'));
	menu->AddItem(new BMenuItem("Reset size", new BMessage(TEXT_SIZE_RESET), '0'));
	m_menuBar->AddItem(menu);
	
	
	m_BackButton = new BButton("","Back",new BMessage(GO_BACK));
	m_ForwardButton = new BButton("","Forward", new BMessage(GO_FORWARD));
	m_StopButton = new BButton("","Stop", new BMessage(STOP));
	
	m_url = new BTextControl("url", "", "", NULL);
	
	m_goButton = new BButton("", "Go", new BMessage(GOTO_URL));
	
	m_goButton->SetTarget(this);
	m_BackButton->SetTarget(this);
	m_ForwardButton->SetTarget(this);
	m_StopButton->SetTarget(this);
	
	m_statusText = new BStringView("status", "");
	m_statusText->SetAlignment(B_ALIGN_LEFT);
	m_statusText->SetExplicitMaxSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
	m_statusText->SetExplicitMinSize(BSize(150, 12));
	
	BFont font(be_plain_font);
	font.SetSize(ceilf(font.Size() * 0.8));
	m_statusText->SetFont(&font, B_FONT_SIZE);
	
	m_loadingProgressBar = new BStatusBar("progress");
	m_loadingProgressBar->SetMaxValue(100);
	m_loadingProgressBar->Hide();
	m_loadingProgressBar->SetBarHeight(12);
	
	
	
	m_url->MakeFocus(true);	
}

BrowserWindow::~BrowserWindow()
{
}
void BrowserWindow::Construct(BWebView* webView)
{
	fWebView = webView;
	
	const float kInsetSpacing = 5;
	const float kElementSpacing = 7;
	AddChild(BGroupLayoutBuilder(B_VERTICAL)
		.Add(m_menuBar)
		.Add(BGridLayoutBuilder(kElementSpacing, kElementSpacing)
			.Add(m_BackButton, 0, 0)
			.Add(m_ForwardButton, 1, 0)
			.Add(m_StopButton, 2, 0)
			.Add(m_url, 3, 0)
			.Add(m_goButton, 4, 0)
			.SetInsets(kInsetSpacing, kInsetSpacing, kInsetSpacing, kInsetSpacing)
		)
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(new BSeparatorView(B_HORIZONTAL, B_PLAIN_BORDER))
		.Add(fWebView->getRenderView())//this is the view be it mail or anywhere can be added
		.Add(BGroupLayoutBuilder(B_HORIZONTAL, kElementSpacing)
			.Add(m_statusText)
			.Add(m_loadingProgressBar, 0.2)
			.AddStrut(12 - kElementSpacing)
			.SetInsets(kInsetSpacing, 0, kInsetSpacing, 0)
		)
	);
}
void BrowserWindow::MessageReceived(BMessage* message)
{
	switch(message->what)
	{	
		case GOTO_URL:
		SetStatus(m_url->Text());
		fWebView->loadURIRequest(m_url->Text());
		break;
		
		case GO_BACK:
		//TODO - write through main loop
		fWebView->goBackward();
		break;
		
		case GO_FORWARD:
		//TODO - write through main loop
		fWebView->goForward();
		break;
		
		case DID_COMMIT_NAVIGATION:
		SetStatus("loading");
		break;
		
		case DID_FINISH_NAVIGATION:
		SetStatus("finished");
		LoadingProgress(1);
		break;
		
		case URL_CHANGE:
		ChangeUrl(message);
		break;
		
		case DID_CHANGE_PROGRESS:
		LoadingProgress(fWebView->didChangeProgress());
		break;
		
		case DID_CHANGE_TITLE:
		ChangeTitle(fWebView->title());
		break;
		
		case STOP:
		SetStatus("cancelling");
		fWebView->stop();
		break; 
		
		default:
		BWindow::MessageReceived(message);
		
	}
}

void BrowserWindow::SetStatus(const char* str)
{
	m_statusText->SetText(str);
}

void BrowserWindow::ChangeUrl(BMessage* message)
{
	BString str;
	message->FindString("url",&str);
	m_url->SetText(str.String());
}

void BrowserWindow::LoadingProgress(double value)
{	
	if(value*100<100 && m_loadingProgressBar->IsHidden())
	m_loadingProgressBar->Show();

	m_loadingProgressBar->SetTo(value*100);
}

void BrowserWindow::ChangeTitle(const char* title)
{
	SetTitle(title);
}
