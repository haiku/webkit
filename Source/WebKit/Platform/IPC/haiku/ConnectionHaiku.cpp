/*
 * Copyright (C) 2019 Haiku, Inc. All rights reserved.
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
#include "Connection.h"

#include <unistd.h>
#include <Message.h>
#include <Looper.h>
namespace IPC{
	class ReadLoop: public BHandler
	{
		public:
			ReadLoop(IPC::Connection* con)
			:BHandler("Read Message Loop"),
			connection(con)
			{
			}
			void MessageReceived(BMessage* message)
			{
				
				fprintf(stderr,"%s %ld\n",__PRETTY_FUNCTION__,getpid());
				message->PrintToStream();
			}
		private:
			IPC::Connection* connection;
			
	};
    void Connection::platformInitialize(Identifier identifier)
    {
    	m_connectedProcess = identifier;
    }
    void Connection::platformInvalidate()
    {

    }
    void Connection::runReadEventLoop()
    {
    	BLooper* looper = BLooper::LooperForThread(find_thread(NULL));
    	looper->Lock();
    	looper->AddHandler(m_readHandler);
    	looper->SetPreferredHandler(m_readHandler);
    	looper->Unlock();
    	
    }
    void Connection::runWriteEventLoop()
    {
    }
    bool Connection::open()
    {
    	m_isConnected = true;
    	status_t result = m_messenger.SetTo(NULL,m_connectedProcess);
    	m_readHandler = new ReadLoop(this);
    	runReadEventLoop();
    	if(result == B_OK)
    	{
    		fprintf(stderr,"%s %ld %ld\n",__PRETTY_FUNCTION__,m_connectedProcess,getpid());
    		return true;
    	}
    	
    	return false;
    }
    bool Connection::platformCanSendOutgoingMessages() const
    {
    	//write with encoder
    	return true;
    }
    bool Connection::sendOutgoingMessage(std::unique_ptr<Encoder> encoder)
    {
    	BMessage processMessage('ipcm');
    	status_t result = processMessage.AddData("bufferData",B_ANY_TYPE,encoder->buffer(),encoder->bufferSize());
    	result = m_messenger.SendMessage(&processMessage);
    	//fprintf(stderr,"%s %s\n",__PRETTY_FUNCTION__,strerror(result));
    }
    void Connection::willSendSyncMessage(OptionSet<SendSyncOption> )
    {

    }
    void Connection::didReceiveSyncReply(OptionSet<SendSyncOption> )
    {

    }
}
