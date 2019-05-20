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

void
App::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case 'init':
		message->FindInt32("threadID",&workQueueLooperID);
		messageForward = BLooper::LooperForThread(workQueueLooperID);
		break;
		
		case 'ipcm':
		message = DetachCurrentMessage();
		result = messageForward->PostMessage(message,messageForward->PreferredHandler());
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

