/*
 * Copyright 2019, RAJAGOPALAN-GANGADHARAN <g.raju2000@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */



#include <Window.h>
#include <View.h>

#include "App.h"

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
};

App::App(void)
    :    BApplication("application/x-vnd.lh-MiniBrowser")
{
	//initialize counter
	fCount = 0;
	
	//create window
	frame.Set(100,100,800,800);
	myWindow = new BWindow(frame,"MiniBrowser"
	                               , B_TITLED_WINDOW
	                               , B_AUTO_UPDATE_SIZE_LIMITS | B_ASYNCHRONOUS_CONTROLS
	                                 | B_QUIT_ON_WINDOW_CLOSE);
	                                 
	myWindow->SetLayout(new BGroupLayout(B_HORIZONTAL));
	
	                                 
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
	
	
	m_BackButton = new BButton("Back",new BMessage(GO_BACK));
    m_ForwardButton = new BButton("Forward", new BMessage(GO_FORWARD));
    m_StopButton = new BButton("Stop", new BMessage(STOP));
	
	m_url = new BTextControl("url", "", "", NULL);
	
	m_goButton = new BButton("", "Go", new BMessage(GOTO_URL));
	m_goButton->SetTarget(this);
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
    
}

void App::LocalMessage(BMessage* message)
{
	const char* idTempStr;
	BLooper* looperTemp;
	message->FindString("identifier",&idTempStr);
	message->FindPointer("looper",(void**)&looperTemp);
	string id(idTempStr);
	message = DetachCurrentMessage();
	if(messengerMapping[id])
	{
		/* 
		We have recieved the other process's BMessenger data just send it to our workqueue
		*/
		looperTemp->PostMessage(messengerMapping[id],looperTemp->PreferredHandler());
	}
	else
	{
		/*
		Messenger is not yet known save it for later use
		*/
		looperMapping[id] = looperTemp;
	}
	
}
void App::GlobalMessage(BMessage* message)
{
	const char* idTempStr;
	message->FindString("identifier",&idTempStr);
	string id(idTempStr);
	message = DetachCurrentMessage();
	if(looperMapping[id])
	{
		/*
		We know about the looper so send the message directly then
		*/
		BLooper* temp = looperMapping[id];
		temp->PostMessage(message,temp->PreferredHandler());
	}
	else
	{
		/* 
		We dont know about the looper yet so put in the mapping of messengers
		*/
		messengerMapping[id] = message;
	}
}

void
App::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case 'inil':
		LocalMessage(message);
		break;
		case 'inig':
		GlobalMessage(message);
		break;
		
		case GOTO_URL:
		fprintf(stderr,"Gotoooooooo");
		webView->loadURI(m_url->Text());
		break;
		
		default:
		BApplication::MessageReceived(message);
		
	}
}
void App::ReadyToRun()
{
	webView = new BWebView(frame,myWindow);
	const float kInsetSpacing = 5;
    const float kElementSpacing = 7;
	myWindow->AddChild(BGroupLayoutBuilder(B_VERTICAL)
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
        .Add(webView->getRenderView())//this is the view be it mail or anywhere can be added
        .Add(BGroupLayoutBuilder(B_HORIZONTAL, kElementSpacing)
            .Add(m_statusText)
            .Add(m_loadingProgressBar, 0.2)
            .AddStrut(12 - kElementSpacing)
            .SetInsets(kInsetSpacing, 0, kInsetSpacing, 0)
        )
    );

    m_url->MakeFocus(true);
    myWindow->Show();
}
void 
App::testLoader()
{
	 webView->loadHTML();
}

int
main (void)
{
  App *app = new App();
  app->Run();
  delete app;
  return 0;
}

