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
 * SUBSTITUTE GOODS OR SERVICES{} LOSS OF USE, DATA, OR PROFITS{} OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "config.h"
#include "PageLoadState.h"
#include "WebViewConstants.h"


#include <Application.h>
#include <Message.h>

namespace WebKit{
class PageLoadStateObserver final :public PageLoadState::Observer
{
	public:
	PageLoadStateObserver(BLooper* looper)
	:m_looper(looper)
	{
	}
	void willChangeIsLoading() override{}
	void didChangeIsLoading() override{}
	
	void willChangeTitle() override
	{
	}
	void didChangeTitle() override
	{
		BMessage message(DID_CHANGE_TITLE);
		m_looper->PostMessage(&message);
	}
	
	void willChangeActiveURL() override{}
	void didChangeActiveURL() override{}
	
	void willChangeHasOnlySecureContent() override{}
	void didChangeHasOnlySecureContent() override{}
	
	void willChangeEstimatedProgress() override
	{
	}
	void didChangeEstimatedProgress() override
	{
		BMessage message(DID_CHANGE_PROGRESS);
		m_looper->PostMessage(&message);
	}
	
	void willChangeCanGoBack() override{}
	void didChangeCanGoBack() override{}
	
	void willChangeCanGoForward() override{}
	void didChangeCanGoForward() override{}
	
	void willChangeNetworkRequestsInProgress() override{}
	void didChangeNetworkRequestsInProgress() override{}
	
	void willChangeCertificateInfo() override{}
	void didChangeCertificateInfo() override{}
	
	void willChangeWebProcessIsResponsive() override{}
	void didChangeWebProcessIsResponsive() override{}
	
	void didSwapWebProcesses() override{}
	private:
	BLooper* m_looper;
};
}


