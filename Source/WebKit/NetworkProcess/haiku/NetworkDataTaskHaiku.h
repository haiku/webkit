/*
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

#include "config.h"
#include "NetworkDataTask.h"

#include <WebCore/ResourceResponse.h>
#include <WebCore/ResourceRequest.h>
#include <WebCore/NetworkLoadMetrics.h>

#include <UrlProtocolAsynchronousListener.h>
#include <Referenceable.h>
#include <Path.h>

namespace WebKit {
using namespace WebCore;

class NetworkDataTaskHaiku final : public NetworkDataTask
	,public BUrlProtocolAsynchronousListener
{
public:
    static Ref<NetworkDataTask> create(NetworkSession& session, NetworkDataTaskClient& client, const WebCore::ResourceRequest& request, WebCore::StoredCredentialsPolicy storedCredentialsPolicy, WebCore::ContentSniffingPolicy shouldContentSniff, WebCore::ContentEncodingSniffingPolicy shouldContentEncodingSniff, 
    bool shouldClearReferrerOnHTTPSToHTTPRedirect, bool dataTaskIsForMainFrameNavigation)
    {
        return adoptRef(*new NetworkDataTaskHaiku(session, client, request, storedCredentialsPolicy, shouldContentSniff, shouldContentEncodingSniff, shouldClearReferrerOnHTTPSToHTTPRedirect, dataTaskIsForMainFrameNavigation));
    }

    ~NetworkDataTaskHaiku();
private:
	NetworkDataTaskHaiku(NetworkSession&, NetworkDataTaskClient&, const WebCore::ResourceRequest&, WebCore::StoredCredentialsPolicy, WebCore::ContentSniffingPolicy, WebCore::ContentEncodingSniffingPolicy, bool shouldClearReferrerOnHTTPSToHTTPRedirect, bool dataTaskIsForMainFrameNavigation);
	void createRequest(WebCore::ResourceRequest&&);
	State m_state{State::Suspended};
	void cancel() override;
    void resume() override;
    void invalidateAndCancel() override;
    NetworkDataTask::State state() const override;
    
    void runOnMainThread(Function<void()>&&);
    
    void ConnectionOpened(BUrlRequest* caller) override;
    void HeadersReceived(BUrlRequest* caller, const BUrlResult& result) override;
    void DataReceived(BUrlRequest* caller, const char* data, off_t position,
        ssize_t size) override;
    void UploadProgress(BUrlRequest* caller, ssize_t bytesSent, ssize_t bytesTotal) override;
    void RequestCompleted(BUrlRequest* caller, bool success) override;
    bool CertificateVerificationFailed(BUrlRequest* caller, BCertificate& certificate, const char* message) override;
    void DebugMessage(BUrlRequest* caller,BUrlProtocolDebugMessage type,const char* text) override;
	
	WebCore::ResourceResponse m_response;
	WebCore::ResourceRequest m_currentRequest;
	WebCore::NetworkLoadMetrics m_networkLoadMetrics;
    unsigned m_redirectCount { 0 };
    unsigned m_authFailureCount { 0 };
    MonotonicTime m_startTime;
    BUrlRequest* m_request;
    BFormDataIO* m_postData;
    URL m_baseUrl;
    bool m_responseDataSent;
    bool m_redirected;
   	off_t m_position;
    
    int m_redirectionTries;
};
}
