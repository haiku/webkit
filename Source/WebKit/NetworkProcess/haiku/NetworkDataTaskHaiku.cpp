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
#include <WebCore/ResourceResponse.h>
#include <WebCore/SameSiteInfo.h>
#include <WebCore/SharedBuffer.h>
#include <WebCore/HTTPParsers.h>
#include <wtf/MainThread.h>
#include <wtf/text/CString.h>

#include <Url.h>
#include <UrlRequest.h>
#include <HttpRequest.h>
#include <assert.h>
#include "BeDC.h"

static const int gMaxRecursionLimit = 10;

namespace WebKit {

using namespace WebCore;

NetworkDataTaskHaiku::NetworkDataTaskHaiku(NetworkSession& session, NetworkDataTaskClient& client, const ResourceRequest& requestWithCredentials, 
   StoredCredentialsPolicy storedCredentialsPolicy, ContentSniffingPolicy shouldContentSniff, ContentEncodingSniffingPolicy, 
      bool shouldClearReferrerOnHTTPSToHTTPRedirect, bool dataTaskIsForMainFrameNavigation)
    : NetworkDataTask(session, client, requestWithCredentials, storedCredentialsPolicy, shouldClearReferrerOnHTTPSToHTTPRedirect, dataTaskIsForMainFrameNavigation)
    , m_postData(NULL)
    , m_responseDataSent(false)
    , m_redirected(false)
    , m_redirectionTries(gMaxRecursionLimit)
    , m_position(0)
    
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
	cancel();
    if (m_request)
        m_request->SetListener(NULL);
    delete m_request;
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
        if (!m_client)
            return;

        ResourceError error("BUrlProtocol", 42,
            m_baseUrl,
            "The service kit failed to start the request.");
            
        m_networkLoadMetrics.responseEnd = MonotonicTime::now() - m_startTime;
        m_networkLoadMetrics.markComplete();
        m_client->didCompleteWithError(error,m_networkLoadMetrics);
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
	
	if(m_request)
	m_request->Resume();
}

void NetworkDataTaskHaiku::invalidateAndCancel()
{

}

NetworkDataTask::State NetworkDataTaskHaiku::state() const
{
	return m_state;	
}

void NetworkDataTaskHaiku::runOnMainThread(Function<void()>&& task)
{
	if(isMainThread())
	task();
	else
	callOnMainThreadAndWait(WTFMove(task));
}


void NetworkDataTaskHaiku::ConnectionOpened(BUrlRequest* caller)
{
	m_responseDataSent = false;
}
void NetworkDataTaskHaiku::HeadersReceived(BUrlRequest* caller, const BUrlResult& result)
{
	if (m_currentRequest.isNull())
        return;

    const BHttpResult* httpResult = dynamic_cast<const BHttpResult*>(&result);

    WTF::String contentType = result.ContentType();
    int contentLength = result.Length();
    URL url;

    WTF::String encoding = extractCharsetFromMediaType(contentType);
    WTF::String mimeType = extractMIMETypeFromMediaType(contentType);

    if (httpResult) {
        url = URL(httpResult->Url());

        BString location = httpResult->Headers()["Location"];
        if (location.Length() > 0) {
            m_redirected = true;
            url = URL(url, location);
        } else {
            m_redirected = false;
        }
    } else {
        url = m_baseUrl;
    }

    ResourceResponse response(url, mimeType, contentLength, encoding);

    if (httpResult) {
        int statusCode = httpResult->StatusCode();

        String suggestedFilename = filenameFromHTTPContentDisposition(
            httpResult->Headers()["Content-Disposition"]);

        if (!suggestedFilename.isEmpty())
            response.setSuggestedFilename(suggestedFilename);

        response.setHTTPStatusCode(statusCode);
        response.setHTTPStatusText(httpResult->StatusText());

        // Add remaining headers.
        const BHttpHeaders& resultHeaders = httpResult->Headers();
        for (int i = 0; i < resultHeaders.CountHeaders(); i++) {
            BHttpHeader& headerPair = resultHeaders.HeaderAt(i);
            response.setHTTPHeaderField(headerPair.Name(), headerPair.Value());
        }

        if (statusCode == 401) {
        	
        	//TODO
        	
            //AuthenticationNeeded((BHttpRequest*)m_request, response);
            // AuthenticationNeeded may have aborted the request
            // so we need to make sure we can continue.
            
            if (m_currentRequest.isNull())
                return;
        }
    }

    if (!m_client)
        return;

    if (m_redirected) {
        m_redirectionTries--;

        if (m_redirectionTries == 0) {
            ResourceError error(url.host().utf8().data(), 400, url,
                "Redirection limit reached");
            
            m_networkLoadMetrics.responseEnd = MonotonicTime::now() - m_startTime;
            m_networkLoadMetrics.markComplete();
            m_client->didCompleteWithError(error,m_networkLoadMetrics);
            return;
        }

        // Notify the client that we are redirecting.
        ResourceRequest request = m_currentRequest;
        ResourceResponse responseCopy = response;
        request.setURL(url);
    	fprintf(stderr,"((((((( redirecting to %s)))))))\n",request.url().string().utf8().data());
    	m_client->willPerformHTTPRedirection(WTFMove(responseCopy),WTFMove(request), [this](const ResourceRequest& newRequest){
    		
	    	if(newRequest.isNull() || m_state == State::Canceling)
	    	return;
	    	
	    	m_startTime = MonotonicTime::now();//network metrics
	    	
	    	if( m_state != State::Suspended ){
	    		m_state = State::Suspended;
	    		resume();
	    	}
	    });
    	
    } else {
        ResourceResponse responseCopy = response;
        m_client->didReceiveResponse(WTFMove(responseCopy),[this](WebCore::PolicyAction policyAction){
        	if(m_state == State::Canceling || m_state == State::Completed){
        		return;
        	}
        	});
    }
}
void NetworkDataTaskHaiku::DataReceived(BUrlRequest* caller, const char* data, off_t position,
        ssize_t size)
{
	if (m_currentRequest.isNull())
        return;

    if (!m_client)
        return;

    // don't emit the "Document has moved here" type of HTML
    if (m_redirected)
        return;

    if (position != m_position)
    {
        debugger("bad redirect");
        return;
    }

    if (size > 0) {
        m_responseDataSent = true;
        
        
        runOnMainThread([this,data = data, size = size]{
			m_client->didReceiveData(SharedBuffer::create(data,size));
		});
    }

    m_position += size;
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
void NetworkDataTaskHaiku::DebugMessage(BUrlRequest* caller,BUrlProtocolDebugMessage type,const char* text)
{
}

}
