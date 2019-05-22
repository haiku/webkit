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
				switch(message->what)
				{
					case 'ipcm':
					connection->prepareIncomingMessage(message);
					break;
					
					default:
					BHandler::MessageReceived(message);
					
				}
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
    void Connection::prepareIncomingMessage(BMessage* message)
    {
    	size_t size;
    	const uint8_t* Buffer;
    	type_code type;
    	int32 count;
    	status_t result;
    	message->GetInfo("bufferData",&type,&count);
    	fprintf(stderr,"\n******** %ld ---- ",getpid());
    	message->PrintToStream();
    	fprintf(stderr,"*********\n");
    	if(count == 1)
    	result = message->FindData("bufferData",B_ANY_TYPE,(const void**)&Buffer,(ssize_t*)&size);
    	else
    	{
    		result = message->FindData("bufferData",B_ANY_TYPE,1,(const void**)&Buffer,(ssize_t*)&size);
    		fprintf(stderr,"\n{%ld}\n",size);
    	}
    	
    	
    	if(result == B_OK)
    	{
    		Vector<Attachment> attachments(0);
    		auto decoder = std::make_unique<Decoder>(Buffer,size,nullptr,WTFMove(attachments));
    		processIncomingMessage(WTFMove(decoder));
    	}
    	else
    	return;
    }
    void Connection::runReadEventLoop()
    {
    	BLooper* looper = BLooper::LooperForThread(find_thread(NULL));
    	if(!looper)
    		return;
    	looper->Lock();
    	looper->AddHandler(m_readHandler);
    	looper->SetPreferredHandler(m_readHandler);
    	looper->Unlock();
    	BMessage init('init');
    	init.AddString("identifier",m_connectedProcess.key.String());
    	init.AddPointer("looper",(const void*)looper);
    	//
    	BMessenger hostProcess(NULL,getpid());
    	hostProcess.SendMessage(&init);
    }
    void Connection::runWriteEventLoop()
    {
    }
    bool Connection::open()
    {
    	status_t result = m_messenger.SetTo(NULL,m_connectedProcess.connectedProcess);
    	m_readHandler = new ReadLoop(this);
    	m_connectionQueue->dispatch([this,protectedThis = makeRef(*this)]{
    		this->runReadEventLoop();
    	});
    	//runReadEventLoop();
    	if(result == B_OK)
    	{
    		m_isConnected = true;
    		return true;
    	}
    	else
    	{
    		m_isConnected = false;
    		return false;
    	}
    }
    bool Connection::platformCanSendOutgoingMessages() const
    {
    	//write with encoder
    	return true;
    }
    bool Connection::sendOutgoingMessage(std::unique_ptr<Encoder> encoder)
    {
    	BMessage processMessage('ipcm');
    	processMessage.AddString("identifier",m_connectedProcess.key.String());
    	const uint8_t* Buffer= encoder->buffer();
    	status_t result = processMessage.AddData("bufferData",B_ANY_TYPE,(void*)Buffer,encoder->bufferSize());
    	result = m_messenger.SendMessage(&processMessage);

    	if(result == B_OK)
    	return true;
    	else
    	{
    		fprintf(stderr,"\n not success:%s\n",strerror(result));
    		return false;
    	}
    }
    void Connection::willSendSyncMessage(OptionSet<SendSyncOption> )
    {

    }
    void Connection::didReceiveSyncReply(OptionSet<SendSyncOption> )
    {

    }
}
