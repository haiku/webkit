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

#include "config.h" 
#include "NetworkDataTaskHaiku.h"

#include <WebCore/AuthenticationChallenge.h>
#include <WebCore/CookieJar.h>
#include <WebCore/NetworkStorageSession.h>
#include <WebCore/ResourceError.h>
#include <WebCore/SameSiteInfo.h>
#include <WebCore/SharedBuffer.h>

#include <Url.h>
#include <UrlRequest.h>
#include <HttpRequest.h>
#include <assert.h>

namespace WebKit {

using namespace WebCore;
	
NetworkDataTaskHaiku::NetworkDataTaskHaiku(NetworkSession& session, NetworkDataTaskClient& client, const ResourceRequest& requestWithCredentials, StoredCredentialsPolicy storedCredentialsPolicy, ContentSniffingPolicy shouldContentSniff, ContentEncodingSniffingPolicy, bool shouldClearReferrerOnHTTPSToHTTPRedirect, bool dataTaskIsForMainFrameNavigation)
    : NetworkDataTask(session, client, requestWithCredentials, storedCredentialsPolicy, shouldClearReferrerOnHTTPSToHTTPRedirect, dataTaskIsForMainFrameNavigation)
    , m_postData(NULL)
{
    if (m_scheduledFailureType != NoFailure)
        return;

    auto request = requestWithCredentials;
    if (request.url().protocolIsInHTTPFamily()) {
        m_startTime = MonotonicTime::now();
        auto url = request.url();
        if (m_storedCredentialsPolicy == StoredCredentialsPolicy::Use) {
            m_user = url.user();
            m_password = url.pass();
            request.removeCredentials();
        }
    }
    createRequest(WTFMove(request));
}

NetworkDataTaskHaiku::~NetworkDataTaskHaiku()
{
}

void NetworkDataTaskHaiku::createRequest(ResourceRequest&& request)
{
	m_currentRequest = WTFMove(request);
	m_request = m_currentRequest.toNetworkRequest(nullptr);
	
    BString method = BString(m_currentRequest.httpMethod());

    m_postData = NULL;

    if (m_request == NULL)
        return;

    m_baseUrl = URL(m_request->Url());

    BHttpRequest* httpRequest = dynamic_cast<BHttpRequest*>(m_request);
    if(httpRequest) {

        if(method == B_HTTP_POST || method == B_HTTP_PUT) {
            FormData* form = m_currentRequest.httpBody();
            if(form) {
                m_postData = new BFormDataIO(form);
                httpRequest->AdoptInputData(m_postData, m_postData->Size());
            }
        }

        httpRequest->SetMethod(method.String());
    }

    if(this->SynchronousListener()) {
        m_request->SetListener(this->SynchronousListener());
    } else {
        m_request->SetListener(this);
    }

    if (m_request->Run() < B_OK) 
    {
    }
}
void NetworkDataTaskHaiku::cancel()
{
	if(m_state == State::Canceling || m_state == State::Completed)
	return;
	
	m_state = State::Canceling;

	if(m_request)
	m_request->Stop();
	
}

void NetworkDataTaskHaiku::resume()
{
	if(m_state == State::Completed || m_state == State::Canceling)
	return;
	
	m_state = State::Running;
	
}

void NetworkDataTaskHaiku::invalidateAndCancel()
{

}

NetworkDataTask::State NetworkDataTaskHaiku::state() const
{
	return m_state;	
}

void NetworkDataTaskHaiku::ConnectionOpened(BUrlRequest* caller)
{

}
void NetworkDataTaskHaiku::HeadersReceived(BUrlRequest* caller, const BUrlResult& result)
{
}
void NetworkDataTaskHaiku::DataReceived(BUrlRequest* caller, const char* data, off_t position,
        ssize_t size)
{
	if(size < 0 )
	{
		return;
	}
	m_client->didReceiveData(SharedBuffer::create(data,size));
}
void NetworkDataTaskHaiku::UploadProgress(BUrlRequest* caller, ssize_t bytesSent, ssize_t bytesTotal)
{
	
}
void NetworkDataTaskHaiku::RequestCompleted(BUrlRequest* caller, bool success)
{
	
}
bool NetworkDataTaskHaiku::CertificateVerificationFailed(BUrlRequest* caller, BCertificate& certificate, const char* message)
{
	
}

}
