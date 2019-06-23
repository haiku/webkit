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
#include "config.h"

#include "PageClientImplHaiku.h"

#include "DrawingAreaProxyCoordinatedGraphics.h"
#include "WebProcessProxy.h"
#include "WebViewBase.h"

namespace WebKit
{
	using namespace WebCore;
	
	PageClientImpl::PageClientImpl(WebViewBase& view)
	:fWebView(view)
	{
		//fprintf(stderr,"page client called");	
	}
	std::unique_ptr<DrawingAreaProxy> PageClientImpl::createDrawingAreaProxy(
	WebProcessProxy& process)
	{
		return std::make_unique<DrawingAreaProxyCoordinatedGraphics>(*fWebView.page(),process);
	}
	void PageClientImpl::setViewNeedsDisplay(const WebCore::Region& region)
	{
		fprintf(stderr,"printing");
	    //fWebView.setViewNeedsDisplay(region);
	}
	
	void PageClientImpl::requestScroll(const WebCore::FloatPoint&, const WebCore::IntPoint&)
	{
	    notImplemented();
	}
	
	WebCore::FloatPoint PageClientImpl::viewScrollPosition()
	{
	    notImplemented();
	    return { };
	}
	
	WebCore::IntSize PageClientImpl::viewSize()
	{
		notImplemented();
	    /*if (fWebView.drawingArea())
	        return fWebView.drawingArea()->size();
		*/
	    return IntSize();
	}
	
	bool PageClientImpl::isViewWindowActive()
	{
	    //return fWebView.isWindowActive();
		return false;
	}
	
	bool PageClientImpl::isViewFocused()
	{
	    //return fWebView.isFocused();
		return false;
	}
	
	bool PageClientImpl::isViewVisible()
	{
	    //return fWebView.isVisible();
		return false;
	}
	
	bool PageClientImpl::isViewInWindow()
	{
	    //return fWebView.isInWindow();
		return false;
	}
	
	void PageClientImpl::PageClientImpl::processDidExit()
	{
	    notImplemented();
	}
	
	void PageClientImpl::didRelaunchProcess()
	{
	    notImplemented();
	}
	
	void PageClientImpl::toolTipChanged(const String&, const String& newToolTip)
	{
	    //fWebView.setToolTip(newToolTip);
	}
	
	void PageClientImpl::setCursor(const WebCore::Cursor& cursor)
	{
	    //fWebView.setCursor(cursor);
	}
	
	void PageClientImpl::setCursorHiddenUntilMouseMoves(bool /* hiddenUntilMouseMoves */)
	{
	    notImplemented();
	}
	
	void PageClientImpl::didChangeViewportProperties(const WebCore::ViewportAttributes&)
	{
	    notImplemented();
	}
	
	void PageClientImpl::registerEditCommand(Ref<WebEditCommandProxy>&& command, UndoOrRedo undoOrRedo)
	{
	    fUndoController.registerEditCommand(WTFMove(command), undoOrRedo);
	}
	
	void PageClientImpl::clearAllEditCommands()
	{
	    fUndoController.clearAllEditCommands();
	}
	
	bool PageClientImpl::canUndoRedo(UndoOrRedo undoOrRedo)
	{
	    return fUndoController.canUndoRedo(undoOrRedo);
	}
	
	void PageClientImpl::executeUndoRedo(UndoOrRedo undoOrRedo)
	{
	    fUndoController.executeUndoRedo(undoOrRedo);
	}
	
	FloatRect PageClientImpl::convertToDeviceSpace(const FloatRect& viewRect)
	{
	    notImplemented();
	    return viewRect;
	}
	
	FloatRect PageClientImpl::convertToUserSpace(const FloatRect& viewRect)
	{
	    notImplemented();
	    return viewRect;
	}
	
	IntPoint PageClientImpl::screenToRootView(const IntPoint& point)
	{
	    return IntPoint();
	}
	
	IntRect PageClientImpl::rootViewToScreen(const IntRect& rect)
	{
	    return IntRect();
	}
	
	void PageClientImpl::doneWithKeyEvent(const NativeWebKeyboardEvent& event, bool wasEventHandled)
	{
	    notImplemented();
	}
	
	RefPtr<WebPopupMenuProxy> PageClientImpl::createPopupMenuProxy(WebPageProxy& page)
	{
		fprintf(stderr,"context menu");
		notImplemented();
	    //return WebPopupMenuProxyWin::create(&fWebView, page);
	}
	
	#if ENABLE(CONTEXT_MENUS)
	Ref<WebContextMenuProxy> PageClientImpl::createContextMenuProxy(WebPageProxy& page, ContextMenuContextData&& context, const UserData& userData)
	{
		notImplemented();
	    //return WebContextMenuProxyWin::create(page, WTFMove(context), userData);
	}
	#endif
	
	//#if ENABLE(INPUT_TYPE_COLOR)
	RefPtr<WebColorPicker> PageClientImpl::createColorPicker(WebPageProxy*, const WebCore::Color& intialColor, 
	const WebCore::IntRect&, Vector<WebCore::Color>&&)
	{
	    return nullptr;
	}
	//#endif
	
	void PageClientImpl::enterAcceleratedCompositingMode(const LayerTreeContext& layerTreeContext)
	{
	    notImplemented();
	}
	
	void PageClientImpl::exitAcceleratedCompositingMode()
	{
	    notImplemented();
	}
	
	void PageClientImpl::updateAcceleratedCompositingMode(const LayerTreeContext& layerTreeContext)
	{
	    notImplemented();
	}
	
	void PageClientImpl::pageClosed()
	{
	    notImplemented();
	}
	
	void PageClientImpl::preferencesDidChange()
	{
	    notImplemented();
	}
	
	void PageClientImpl::didChangeContentSize(const IntSize& size)
	{
	    notImplemented();
	}
	
	void PageClientImpl::handleDownloadRequest(DownloadProxy& download)
	{
	    notImplemented();
	}
	
	void PageClientImpl::didCommitLoadForMainFrame(const String& /* mimeType */, bool /* useCustomContentProvider */ )
	{
	    notImplemented();
	}
	void PageClientImpl::wheelEventWasNotHandledByWebCore(const NativeWebWheelEvent& event)
	{
	    notImplemented();
	}
	
	void PageClientImpl::didFinishLoadingDataForCustomContentProvider(const String&, const IPC::DataReference&)
	{
	    notImplemented();
	}
	
	void PageClientImpl::navigationGestureDidBegin()
	{
	    notImplemented();
	}
	
	void PageClientImpl::navigationGestureWillEnd(bool, WebBackForwardListItem&)
	{
	    notImplemented();
	}
	
	void PageClientImpl::navigationGestureDidEnd(bool, WebBackForwardListItem&)
	{
	    notImplemented();
	}
	
	void PageClientImpl::navigationGestureDidEnd()
	{
	    notImplemented();
	}
	
	void PageClientImpl::willRecordNavigationSnapshot(WebBackForwardListItem&)
	{
	    notImplemented();
	}
	
	void PageClientImpl::didRemoveNavigationGestureSnapshot()
	{
	    notImplemented();
	}
	
	void PageClientImpl::didFirstVisuallyNonEmptyLayoutForMainFrame()
	{
	    notImplemented();
	}
	
	void PageClientImpl::didFinishLoadForMainFrame()
	{
	    notImplemented();
	}
	
	void PageClientImpl::didSameDocumentNavigationForMainFrame(SameDocumentNavigationType)
	{
	    notImplemented();
	}
	
	void PageClientImpl::didChangeBackgroundColor()
	{
	    notImplemented();
	}
	
	void PageClientImpl::isPlayingAudioWillChange()
	{
	    notImplemented();
	}
	
	void PageClientImpl::isPlayingAudioDidChange()
	{
	    notImplemented();
	}
	
	void PageClientImpl::refView()
	{
	    notImplemented();
	}
	
	void PageClientImpl::derefView()
	{
	    notImplemented();
	}
	
	BView* PageClientImpl::viewWidget()
	{
	    return fWebView.getView();
	}

}
	
