/*
 *  Copyright (C) 2019 Haiku, Inc.
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
#include "ProcessLauncher.h"

#define __STDC_FORMAT_MACROS
#include <unistd.h>
#include <string>
#include <inttypes.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/resource.h>

#include <Looper.h>
#include <Application.h>
#include <Message.h>

using namespace WebCore;

namespace WebKit {

static const char* processName(ProcessLauncher::ProcessType type)
{
	switch(type)
	{
		case ProcessLauncher::ProcessType::Web:
		//debug later will be taken system based absolute path
		return "/WebKit/webkit/WebKitBuild/Release/bin/WebProcess";
		case ProcessLauncher::ProcessType::Network:
		return "/WebKit/webkit/WebKitBuild/Release/bin/NetworkProcess";
	}
}

void ProcessLauncher::launchProcess()
{
	const char* name=processName(m_launchOptions.processType);
	char* procName;
	switch(m_launchOptions.processType)
	{
		case ProcessLauncher::ProcessType::Web:
		procName="WebProcess";
		break;
		case ProcessLauncher::ProcessType::Network:
		procName="NetworkProcess";
		break;
	}
	//process identifier
	uint64_t prID = m_launchOptions.processIdentifier.toUInt64();
	fprintf(stderr,"\nlaunch%d\n",prID);
	char buff[21];
	snprintf(buff,sizeof(buff),"%"PRIu64,prID);
	//
	
	//socket
	int sockets[2];
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets)==-1)
	{
		fprintf(stderr,":(%s\n",strerror(errno));
	}
	//1-> client 0-> server is taken convention
	
	std::string sock = std::to_string(sockets[1]);
	char* sockBuff = new char[sock.length()+1];
	strcpy(sockBuff,sock.c_str());
	//
	
	pid_t pid=fork();
	char* m_args[]={procName,buff,sockBuff,NULL};
	if(pid==0)
	{
		
		execvp(name,m_args);
	}
	else
	{
		fprintf(stderr,"\nChild:%d\n",pid);
	}

	fprintf(stderr,"\ngoing to send\n");
	RefPtr<ProcessLauncher> protectedLauncher(this);
	
	RunLoop::main().dispatch([protectedLauncher,pid,sockets]{
			fprintf(stderr,"Messaged function executing now\n");
			protectedLauncher->didFinishLaunchingProcess(pid,sockets[0]);
		});
}

void ProcessLauncher::terminateProcess()
{
}

void ProcessLauncher::platformInvalidate()
{
}

} // namespace WebKit

