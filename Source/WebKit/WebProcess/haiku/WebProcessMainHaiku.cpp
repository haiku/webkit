/*
 * Copyright 2014,2019 Haiku, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "WebProcessMainUnix.h"

#include "AuxiliaryProcessMain.h"
#include "WebProcess.h"

#include <Application.h>
#include<Message.h>

using namespace std;
namespace WebKit {
using namespace WebCore;
class WebProcessApp: public BApplication
{
	public:
	WebProcessApp(void):BApplication("application/x-vnd.haiku-webkit.webprocess")
	{
	}
	/*void MessageReceived(BMessage* msg)
	{
		switch(msg->what)
		{
			case 'loop':
			fprintf(stderr,"\n looper called");
		}
	}*/
	void ReadyToRun()
	{	
		fprintf(stderr,"\n Im app\n");
		RunLoop::initializeMainRunLoop();
		RunLoop::run();
	}
};
class WebProcessMain final : public AuxiliaryProcessMainBase {
public:
    bool platformInitialize() override
    {
    	
    	WebProcessApp* app= new WebProcessApp();
    	fprintf(stderr,"webporcess in an application\n");
    	app->Run();
    	return true;
    }
};

int WebProcessMainUnix(int argc, char** argv)
{
	fprintf(stderr,"Aux process call\n");
    return AuxiliaryProcessMain<WebProcess, WebProcessMain>(argc, argv);
}

} // namespace WebKit
