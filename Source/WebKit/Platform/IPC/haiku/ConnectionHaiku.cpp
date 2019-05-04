/*
 * Copyright (C) 2019 Haiku,Inc. All rights reserved.
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
#include <Messenger.h>
#include <unistd.h>

namespace IPC
{
	bool Connection::open()
	{
		fprintf(stderr,"\connection belongs to:%d",getpid());
		status_t* result;
		BMessenger sender("application/x-vnd.haiku-webkit.webprocess",-1,result);
		if(*result == B_OK)
		fprintf(stderr,"\n yes\n"	);
		return true;	
	}
	void Connection::platformInitialize(Identifier identifier)
	{
		fprintf(stderr,"\n init\n");
	}
	void Connection::platformInvalidate()
	{
	}
	bool Connection::platformCanSendOutgoingMessages() const
	{
		fprintf(stderr,"\n con::pcsom\n");	
	}
	bool Connection::sendOutgoingMessage(std::unique_ptr<Encoder> encoder)
	{
		fprintf(stderr,"\n con::som\n");
	}
	void Connection::willSendSyncMessage(OptionSet<SendSyncOption>)
	{
	}
	
	void Connection::didReceiveSyncReply(OptionSet<SendSyncOption>)
	{
	}
}
