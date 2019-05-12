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

#include "AuxiliaryProcessMainHaiku.h"
#include "WebProcess.h"

#include <Application.h>
#include <Message.h>
#include <Messenger.h>

using namespace std;
using namespace WebCore;

namespace WebKit {

class WebProcessApp: public BApplication
{
	private:
	int argc;
	char** argv;
	public:
	WebProcessApp(int argc,char** argv):BApplication("application/x-vnd.haiku-webkit.webprocess")
	{
		this->argc = argc;
		this->argv = argv;
	}

	void MessageReceived(BMessage* msg)
	{
		//msg->PrintToStream();
		switch(msg->what)
		{
			default:
			BApplication::MessageReceived(msg);
		}
	}

	void ReadyToRun()
	{	
		AuxiliaryProcessMain<WebProcess>(argc,argv);
	}
};

int WebProcessMainUnix(int argc, char** argv)
{
	fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
	WebProcessApp* app = new WebProcessApp(argc,argv);
    return app->Run();
}

} // namespace WebKit
