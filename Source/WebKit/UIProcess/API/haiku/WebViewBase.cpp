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
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "WebViewBase.h"
#include "APIPageConfiguration.h"
#include "DrawingAreaProxyCoordinatedGraphics.h"
#include "WebProcessPool.h"
#include "WebPageGroup.h"
#include <WebCore/IntRect.h>

using namespace WebKit; 
using namespace WebCore;

WebKit::WebViewBase::WebViewBase(const char*name,BRect rect,BWindow* parentWindow,
const API::PageConfiguration& pageConfig)
:BView(rect,name,B_FOLLOW_ALL_SIDES,B_WILL_DRAW),
 fPageClient(std::make_unique<PageClientImpl>(*this))
{
	fprintf(stderr,"Init");
	auto config = pageConfig.copy();
	auto* preferences = config->preferences();
	SetLowColor(255,0,255,255);
	SetViewColor(255,0,255,255);
	if(!preferences && config->pageGroup())
	{
		preferences = &config->pageGroup()->preferences();
		config->setPreferences(preferences);
	}
	if(preferences)
	{
		preferences->setAcceleratedCompositingEnabled(false);
	}
	
	WebProcessPool* processPool = config->processPool();
	fPage = processPool->createWebPage(*fPageClient,WTFMove(config));
	fPage->initializeWebPage();
	
	if(fPage->drawingArea())
	{
		setSize = true;
		fPage->drawingArea()->setSize(IntSize(rect.right - rect.left,
		rect.bottom - rect.top));
	}
}

void WebViewBase::paint(const IntRect& dirtyRect)
{
	
}
void WebViewBase::FrameResized(float newWidth, float newHeight)
{
	auto drawingArea = static_cast<DrawingAreaProxyCoordinatedGraphics*>(page()->drawingArea());
	if(!drawingArea)
	return;
	drawingArea->setSize(IntSize(newWidth,
		newHeight));
}	
void WebViewBase::Draw(BRect update)
{
	auto drawingArea = static_cast<DrawingAreaProxyCoordinatedGraphics*>(page()->drawingArea());
	if(!drawingArea)
	return;
	if(!setSize)
	{
		setSize = true;
		BRect rect = Frame();
		drawingArea->setSize(IntSize(rect.right - rect.left,
		rect.bottom - rect.top));
	}
	IntRect updateArea(update);
	WebCore::Region unpainted;
	drawingArea->paint(this,updateArea,unpainted);
	
}
void WebViewBase::MouseMoved(BPoint where,uint32 code,const BMessage* dragMessage)
{
}
	
	
