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

#include "ProcessExecutablePath.h"

#include <Roster.h>
#include <StackOrHeapArray.h>
#include <String.h>

using namespace WebCore;

namespace WebKit {

status_t processRef(BString path, entry_ref* pathRef)
{
	BEntry pathEntry(path);
	if(!pathEntry.Exists())
	return B_BAD_VALUE;
	
	status_t result = pathEntry.GetRef(pathRef);
	if(result != B_OK)
	return result;
	
	return B_OK;
}

void ProcessLauncher::launchProcess()
{
    BString executablePath;

    switch (m_launchOptions.processType) {
    case ProcessLauncher::ProcessType::Web:
        executablePath = executablePathOfWebProcess();
        break;
#if ENABLE(NETWORK_PROCESS)
    case ProcessLauncher::ProcessType::Network:
        executablePath = executablePathOfNetworkProcess();
        break;
#endif
    default:
        ASSERT_NOT_REACHED();
        return;
    }

	BString processIdentifier;
	processIdentifier.SetToFormat("%" PRIu64, m_launchOptions.processIdentifier.toUInt64());
    unsigned nargs = 5; // size of the argv array for g_spawn_async()

#if ENABLE(DEVELOPER_MODE)
    Vector<CString> prefixArgs;
    if (!m_launchOptions.processCmdPrefix.isNull()) {
        for (auto& arg : m_launchOptions.processCmdPrefix.split(' '))
            prefixArgs.append(arg.utf8());
        nargs += prefixArgs.size();
    }
#endif

	entry_ref executableRef;
	if(processRef(executablePath,&executableRef)!=B_OK)
	{
		return;
	}
    BStackOrHeapArray<const char*, 10> argv(nargs);
    unsigned i = 0;
#if ENABLE(DEVELOPER_MODE)
    // If there's a prefix command, put it before the rest of the args.
	// FIXME this won't work with lauching using BRoster...
    for (auto& arg : prefixArgs)
        argv[i++] = const_cast<char*>(arg.data());
#endif
    argv[i++] = executablePath.String();
    argv[i++] = processIdentifier.String();
	// TODO pass our team_id so the web process can message us?
    argv[i++] = nullptr;

	assert(i <= nargs);

	team_id child_id; // TODO do we need to store this somewhere?
	status_t result = be_roster->Launch(&executableRef, i-1, argv, &child_id);

	fprintf(stderr, "%s: %s\n", __PRETTY_FUNCTION__, strerror(result));

    // We've finished launching the process, message back to the main run loop.
    RunLoop::main().dispatch([protectedThis = makeRef(*this), this, child_id] {
        didFinishLaunchingProcess(m_processIdentifier, child_id);
    });
}

void ProcessLauncher::terminateProcess()
{
    if (m_isLaunching) {
        invalidate();
        return;
    }

    if (!m_processIdentifier)
        return;

    kill(m_processIdentifier, SIGKILL);
    m_processIdentifier = 0;
}

void ProcessLauncher::platformInvalidate()
{
}

} // namespace WebKit

