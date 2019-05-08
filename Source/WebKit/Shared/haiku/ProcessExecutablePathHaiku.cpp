/*
 * Copyright (C) 2012 Samsung Electronics
 * Copyright (C) 2014 Haiku, inc.
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
#include "ProcessExecutablePath.h"

#include "NotImplemented.h"
#include <libgen.h>
#include <unistd.h>
#include <wtf/FileSystem.h>
#include <wtf/NeverDestroyed.h>
#include <wtf/StdLibExtras.h>
#include <wtf/text/CString.h>

namespace WebKit {

// On Haiku, processes are actually launched by MIME type, no need to worry about the path.

String executablePathOfWebProcess()
{
    static NeverDestroyed<const String> webKitWebProcessName("application/x-vnd.haiku-webkit.webprocess");
    return webKitWebProcessName;
}

String executablePathOfPluginProcess()
{
    static NeverDestroyed<const String> webKitPluginProcessName("application/x-vnd.haiku-webkit.pluginprocess");
    return webKitPluginProcessName;
}

#if ENABLE(NETWORK_PROCESS)
String executablePathOfNetworkProcess()
{
    static NeverDestroyed<const String> webKitNetworkProcessName("application/x-vnd.haiku-webkit.networkprocess");
    return webKitNetworkProcessName;
}
#endif

} // namespace WebKit

