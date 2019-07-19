/*
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
#include "BackingStore.h"
#include "ShareableBitmap.h"

#include <WebCore/IntRect.h>
#include <WebCore/GraphicsContext.h>
#include "UpdateInfo.h"

#include <Bitmap.h>
#include <View.h>
using namespace WebCore;

namespace WebKit {

void BackingStore::incorporateUpdate(ShareableBitmap* bitmap, const UpdateInfo& updateInfo)
{
	if(!m_bitmap)
	{
		m_bitmap = new BitmapRef(BRect(BPoint(0,0),BSize(m_size)),B_RGBA32,true);
		BView* m_surface = new BView(m_bitmap->Bounds(),"view surface",0,0);
		m_bitmap->AddChild(m_surface);
	}
		
	IntPoint updateRectLocation = updateInfo.updateRectBounds.location();
	
	GraphicsContext graphicsContext(m_surface);
	
	for(auto const& updateRect : updateInfo.updateRects)
	{
		IntRect srcRect = updateRect;
		srcRect.move(-updateRectLocation.x(), -updateRectLocation.y());
		bitmap->paint(graphicsContext,updateRect.location(),srcRect);
	}
}

void BackingStore::paint(BView* context,const IntRect& rect)
{
	context->DrawBitmap(m_bitmap);
}

}
