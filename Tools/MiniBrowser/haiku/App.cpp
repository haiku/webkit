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

void App::ProcessMessage(BMessage* message)
{
	const char* tempStr;
	BLooper* tempLooper;
	
	message->FindString("identifier",&tempStr);
	message->FindPointer("looper",(void**)&tempLooper);
	string temp(tempStr);
	proxy[temp]=tempLooper;

	while(!stash.IsEmpty())
	{
		BMessage* msg = stash.NextMessage();
		tempLooper->PostMessage(msg,tempLooper->PreferredHandler());
	}
}
void App::AttachAndSend(BMessage* message)
{
	const char* tempStr;
	message->FindString("identifier",&tempStr);
	string temp(tempStr);
	BLooper *looper = proxy[temp];
	message = DetachCurrentMessage();
	if(!looper)
	{
		stash.AddMessage(message);
	}
	else
	{
		looper->PostMessage(message,looper->PreferredHandler());
	}
}

void
App::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case 'init':
		ProcessMessage(message);
		break;
		case 'ipcm':
		AttachAndSend(message);
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

