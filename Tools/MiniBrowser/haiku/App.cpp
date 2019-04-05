/*
 * Copyright 2019, RAJAGOPALAN-GANGADHARAN <g.raju2000@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */


#include "App.h"
//#include "BrowserWindow.h"
#include <Window.h>
#include <View.h>

#include "WKPageConfigurationRef.h"
#include "WKPage.h"
#include "WKView.h"
#include "WKURL.h"
#include "WKString.h"
#include "WKContext.h"
#include "WKPreferencesRef.h"
//test import
//#include "WebView.h"

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
  BRect frame(100,100,500,400);
  myWindow = new BWindow(frame,"MiniBrowser"
                                 , B_TITLED_WINDOW
                                 , B_ASYNCHRONOUS_CONTROLS
                                   | B_QUIT_ON_WINDOW_CLOSE);
  //webkit stuff
  auto config = adoptWK(WKPageConfigurationCreate());
  auto prefs = WKPreferencesCreate();
  
  
  WKPreferencesSetDeveloperExtrasEnabled(prefs, true);
  WKPageConfigurationSetPreferences(config.get(),prefs);
  
  fContext = adoptWK(WKContextCreateWithConfiguration(nullptr));
  //fprintf(stderr,"here");
  WKPageConfigurationSetContext(config.get(),fContext.get());
  
  fViewPort=adoptWK(WKViewCreate("Webkit",frame,myWindow,config.get()));
  //
  myWindow->Show();
  this->testLoader();
}

void
App::MessageReceived(BMessage *msg)
{
  switch(msg->what)
  {
    case M_BUTTON_GO_URL:
    {
      testLoader();
      break;
    }
    default:
    {
      BApplication::MessageReceived(msg);
      break;
    }
  }
}

void 
App::testLoader()
{
	fprintf(stderr,"\nim loading");
	auto page = WKViewGetPage( fViewPort.get());
	WKRetainPtr<WKURLRef> uri;
	uri = adoptWK(WKURLCreateWithUTF8CString("about:blank"));
	WKRetainPtr<WKStringRef> str;
	str = adoptWK(WKStringCreateWithUTF8CString("<body>Hello world</body>"));
	//WKPageLoadURL(page,uri.get());
	WKPageLoadHTMLString(page,str.get(),uri.get()); 
}

int
main (void)
{
  App *app = new App();
  app->Run();
  delete app;
  return 0;
}

