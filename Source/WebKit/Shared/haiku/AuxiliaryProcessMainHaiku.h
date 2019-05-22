/*
 * Copyright (C) 2014 Igalia S.L.
 * Copyright (C) 2019 Haiku, Inc. 
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

#pragma once

#include "AuxiliaryProcess.h"
#include "WebKit2Initialize.h"
#include <wtf/RunLoop.h>
#include <Application.h>
#include <Looper.h>
#include <MessageQueue.h>
#include <String.h>
#include <map>

using namespace std;

namespace WebKit {

class AuxiliaryProcessMainBase {
public:
    virtual bool platformInitialize(char* sig) { return true; }
    virtual bool parseCommandLine(int argc, char** argv);
    virtual void platformFinalize() { }
	
    AuxiliaryProcessInitializationParameters&& takeInitializationParameters() { return WTFMove(m_parameters); }

protected:
    AuxiliaryProcessInitializationParameters m_parameters;
};

class ProcessApp : public BApplication
{
	public:
	BLooper* messageForward ;
	status_t result;
	map<string,BLooper*> proxy;
	BMessageQueue stash;
	ProcessApp(char* signature):BApplication(signature)
	{
		messageForward = nullptr;
	}
	void ProcessMessage(BMessage* message)
	{
		const char* tempStr;
		BLooper* tempLooper;
		message->FindString("identifier",&tempStr);
		message->FindPointer("looper",(void**)&tempLooper);
		string temp(tempStr);
		proxy[temp] = tempLooper;
		while(!stash.IsEmpty())
		{
			tempLooper->PostMessage(stash.NextMessage(),tempLooper->PreferredHandler());
		}
	}
	void AttachAndSend(BMessage* message)
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
	void MessageReceived(BMessage* message)
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
	void ReadyToRun()
	{
		RunLoop::run();
	}	
};

template<typename AuxiliaryProcessType>
void initializeAuxiliaryProcess(AuxiliaryProcessInitializationParameters&& parameters)
{
    AuxiliaryProcessType::singleton().initialize(WTFMove(parameters));
}

template<typename AuxiliaryProcessType, typename AuxiliaryProcessMainType>
int AuxiliaryProcessMain(int argc, char** argv)
{
    AuxiliaryProcessMainType auxiliaryMain;

    if (!auxiliaryMain.platformInitialize(argv[1]))
        return EXIT_FAILURE;
        
    InitializeWebKit2();

    if (!auxiliaryMain.parseCommandLine(argc, argv))
        return EXIT_FAILURE;

    initializeAuxiliaryProcess<AuxiliaryProcessType>(auxiliaryMain.takeInitializationParameters());

    auxiliaryMain.runApp();

    auxiliaryMain.platformFinalize();
	
    return EXIT_SUCCESS;
}

} // namespace WebKit
