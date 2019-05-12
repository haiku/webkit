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
	/* one time relying we could resuse this if connection is last*/
	map<string,BLooper*> looperMapping;
	map<string,BMessage*> messengerMapping;
	public:
	
	ProcessApp(char* signature):BApplication(signature)
	{
	}
	void LocalMessage(BMessage* message)
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
	void GlobalMessage(BMessage* message)
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
	void MessageReceived(BMessage* message)
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
