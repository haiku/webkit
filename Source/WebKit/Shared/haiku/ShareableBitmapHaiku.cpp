/*
 * Copyright (C) 2014 Haiku, Inc.
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
#include "ShareableBitmap.h"

#include <WebCore/BitmapImage.h>
#include <WebCore/NotImplemented.h>
#include <WebCore/GraphicsContext.h>
#include <WebCore/IntRect.h>

#include <Bitmap.h>
#include <View.h>

using namespace WebCore;

namespace WebKit {

static inline RefPtr<StillImage> createSurfaceFromBitmap(BitmapRef* bitmap)
{
    RefPtr<StillImage> image = StillImage::create(bitmap);
    return image;
}

std::unique_ptr<GraphicsContext> ShareableBitmap::createGraphicsContext()
{
    RefPtr<StillImage> image = createBitmapSurface();
    BView* surface = new BView(image->nativeImageForCurrentFrame(nullptr)->Bounds(),
        "Shareable", 0, 0);
    image->nativeImageForCurrentFrame(nullptr)->AddChild(surface);
    surface->LockLooper();
    
    return std::make_unique<GraphicsContext>(surface);
}

void ShareableBitmap::paint(GraphicsContext& context, const IntPoint& dstPoint, const IntRect& srcRect)
{
	RefPtr<StillImage> bitmapImage = createBitmapSurface();
	BView* viewSurface = context.platformContext();
	viewSurface->LockLooper();
	viewSurface->SetHighColor(255,0,0,255);
	viewSurface->FillRect(BRect(10,10,40,40));
	//viewSurface->DrawBitmap(bitmapImage->nativeImageForCurrentFrame(nullptr).get(),BPoint(dstPoint));
	viewSurface->UnlockLooper();	
}

void ShareableBitmap::paint(GraphicsContext& context, float scaleFactor, const IntPoint& dstPoint, const IntRect& srcRect)
{
    FloatRect destRect(dstPoint, srcRect.size());
    FloatRect srcRectScaled(srcRect);
    srcRectScaled.scale(scaleFactor);
    notImplemented();
}

RefPtr<StillImage> ShareableBitmap::createBitmapSurface()
{
    RefPtr<StillImage> image = createSurfaceFromBitmap(m_sharedMemory->bitmap());

    ref(); // Balanced by deref in releaseSurfaceData.
    return image;
}

RefPtr<Image> ShareableBitmap::createImage()
{
    RefPtr<StillImage> surface = createBitmapSurface();
    if (!surface)
        return 0;

    return BitmapImage::create(surface->nativeImageForCurrentFrame(nullptr));
}

Checked<unsigned, RecordOverflow> ShareableBitmap::calculateBytesPerRow(WebCore::IntSize size, const ShareableBitmap::Configuration& config)
{
	unsigned bytes = calculateBytesPerPixel(config)*size.width();
	
	return bytes;
}

unsigned ShareableBitmap::calculateBytesPerPixel(const Configuration&)
{
	return 4;	
}
}
