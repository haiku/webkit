/*
 * Copyright (C) 2020 Apple Inc. All rights reserved.
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

#if ENABLE(GPU_PROCESS) && ENABLE(MEDIA_SOURCE)

#include "MessageReceiver.h"
#include "RemoteMediaSourceIdentifier.h"
#include "RemoteSourceBufferProxy.h"
#include <WebCore/MediaSourcePrivate.h>
#include <WebCore/MediaSourcePrivateClient.h>
#include <wtf/MediaTime.h>
#include <wtf/Optional.h>

namespace IPC {
class Connection;
class Decoder;
class DataReference;
}

namespace WebCore {
class ContentType;
class MediaSourcePrivate;
class PlatformTimeRanges;
}

namespace WebKit {

class GPUConnectionToWebProcess;

class RemoteMediaSourceProxy final
    : public WebCore::MediaSourcePrivateClient
    , private IPC::MessageReceiver {
    WTF_MAKE_FAST_ALLOCATED;
public:
    RemoteMediaSourceProxy(RemoteMediaSourceIdentifier, GPUConnectionToWebProcess&);
    virtual ~RemoteMediaSourceProxy();

    // MediaSourcePrivateClient overrides
    void setPrivateAndOpen(Ref<WebCore::MediaSourcePrivate>&&) final;
    MediaTime duration() const final;
    std::unique_ptr<WebCore::PlatformTimeRanges> buffered() const final;
    void seekToTime(const MediaTime&) final;
    void monitorSourceBuffers() final;

#if !RELEASE_LOG_DISABLED
    void setLogIdentifier(const void*) final;
#endif

    void failedToCreateRenderer(RendererType) final;

private:
    // IPC::MessageReceiver
    // void didReceiveMessage(IPC::Connection&, IPC::Decoder&) final;
    void didReceiveSyncMessage(IPC::Connection&, IPC::Decoder&, std::unique_ptr<IPC::Encoder>&) final;

    using AddSourceBufferCallback = CompletionHandler<void(WebCore::MediaSourcePrivate::AddStatus, Optional<RemoteSourceBufferIdentifier> remoteSourceBufferIdentifier)>;
    void addSourceBuffer(const WebCore::ContentType&, AddSourceBufferCallback&&);

    RemoteMediaSourceIdentifier m_identifier;
    GPUConnectionToWebProcess& m_connectionToWebProcess;
    RefPtr<WebCore::MediaSourcePrivate> m_private;
    Vector<RefPtr<RemoteSourceBufferProxy>> m_sourceBuffers;
};

} // namespace WebKit

#endif // ENABLE(GPU_PROCESS) && ENABLE(MEDIA_SOURCE)
