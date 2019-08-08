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

void BrowserApp::LocalMessage(BMessage* message)
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
void BrowserApp::GlobalMessage(BMessage* message)
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

void BrowserApp::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case 'inil':
		LocalMessage(message);
		break;
		case 'inig':
		GlobalMessage(message);
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

