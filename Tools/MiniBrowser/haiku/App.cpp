/*
 * Copyright 2019, RAJAGOPALAN-GANGADHARAN <g.raju2000@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */



#include <Window.h>
#include <View.h>

#include "App.h"



enum
{
  M_BUTTON_GO_URL = 'urlg'
};

App::App(void)
    :    BApplication("application/x-vnd.lh-MiniBrowser")
{
  //initialize counter
  fCount = 0;

  //create window
  frame.Set(100,100,500,400);
  myWindow = new BWindow(frame,"MiniBrowser"
                                 , B_TITLED_WINDOW
                                 , B_ASYNCHRONOUS_CONTROLS
                                   | B_QUIT_ON_WINDOW_CLOSE);
  myWindow->Show();
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
		
		default:
		BApplication::MessageReceived(message);
		
	}
}
void App::ReadyToRun()
{
	webView = new BWebView(frame,myWindow);
	this->testLoader();
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

