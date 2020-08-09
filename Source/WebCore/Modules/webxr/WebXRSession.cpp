/*
 * Copyright (C) 2020 Igalia S.L. All rights reserved.
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
#include "WebXRSession.h"

#if ENABLE(WEBXR)

#include "WebXRSystem.h"
#include <wtf/IsoMallocInlines.h>

namespace WebCore {

WTF_MAKE_ISO_ALLOCATED_IMPL(WebXRSession);

Ref<WebXRSession> WebXRSession::create(Document& document, WebXRSystem& system, XRSessionMode mode, PlatformXR::Device& device)
{
    return adoptRef(*new WebXRSession(document, system, mode, device));
}

WebXRSession::WebXRSession(Document& document, WebXRSystem& system, XRSessionMode mode, PlatformXR::Device& device)
    : ActiveDOMObject(&document)
    , m_xrSystem(system)
    , m_mode(mode)
    , m_device(makeWeakPtr(device))
    , m_activeRenderState(WebXRRenderState::create(mode))
{
    // TODO: If no other features of the user agent have done so already,
    // perform the necessary platform-specific steps to initialize the device’s
    // tracking and rendering capabilities, including showing any necessary
    // instructions to the user.
}

WebXRSession::~WebXRSession() = default;

XREnvironmentBlendMode WebXRSession::environmentBlendMode() const
{
    return m_environmentBlendMode;
}

XRVisibilityState WebXRSession::visibilityState() const
{
    return m_visibilityState;
}

const WebXRRenderState& WebXRSession::renderState() const
{
    return *m_activeRenderState;
}

const WebXRInputSourceArray& WebXRSession::inputSources() const
{
    return *m_inputSources;
}

void WebXRSession::updateRenderState(const XRRenderStateInit&)
{
}

void WebXRSession::requestReferenceSpace(const XRReferenceSpaceType&, RequestReferenceSpacePromise&&)
{
}

int WebXRSession::requestAnimationFrame(Ref<XRFrameRequestCallback>&&)
{
    return 0;
}

void WebXRSession::cancelAnimationFrame(int)
{
}

// https://immersive-web.github.io/webxr/#shut-down-the-session
void WebXRSession::shutdown()
{
    // 1. Let session be the target XRSession object.
    // 2. Set session's ended value to true.
    m_ended = true;

    // 3. If the active immersive session is equal to session, set the active immersive session to null.
    // 4. Remove session from the list of inline sessions.
    m_xrSystem.sessionEnded(*this);

    // TODO: complete the implementation
    // 5. Reject any outstanding promises returned by session with an InvalidStateError, except for any promises returned by end().
    // 6. If no other features of the user agent are actively using them, perform the necessary platform-specific steps to shut down the device's tracking and rendering capabilities. This MUST include:
    //  6.1. Releasing exclusive access to the XR device if session is an immersive session.
    //  6.2. Deallocating any graphics resources acquired by session for presentation to the XR device.
    //  6.3. Putting the XR device in a state such that a different source may be able to initiate a session with the same device if session is an immersive session.
    // 7. Queue a task that fires an XRSessionEvent named end on session.
}

// https://immersive-web.github.io/webxr/#dom-xrsession-end
void WebXRSession::end(EndPromise&& promise)
{
    // The shutdown() call bellow might remove the sole reference to session
    // that could exist (the XRSystem owns the sessions) so let's protect this.
    Ref<WebXRSession> protectedThis(*this);
    // 1. Let promise be a new Promise.
    // 2. Shut down the target XRSession object.
    shutdown();

    // 3. Queue a task to perform the following steps:
    queueTaskKeepingObjectAlive(*this, TaskSource::WebXR, [promise = WTFMove(promise)] () mutable {
        // 3.1 Wait until any platform-specific steps related to shutting down the session have completed.
        // 3.2 Resolve promise.
        promise.resolve();
    });

    // 4. Return promise.
}

const char* WebXRSession::activeDOMObjectName() const
{
    return "XRSession";
}

void WebXRSession::stop()
{
}

} // namespace WebCore

#endif // ENABLE(WEBXR)
