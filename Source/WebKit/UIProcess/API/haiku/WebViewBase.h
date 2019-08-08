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
#include <View.h>
#include <Window.h>
#include <Rect.h>

#include "config.h"

#include "APIObject.h"
#include "APIPageConfiguration.h"
#include "WebPageProxy.h"
#include "PageClientImplHaiku.h"

using namespace WebKit;
namespace WebKit
{
    class WebViewBase:public API::ObjectImpl<API::Object::Type::View>,
    public BView
    {
        public:
        static RefPtr<WebViewBase> create(const char*name,BRect rect, 
        BWindow* parentWindow,const API::PageConfiguration& config)
        {
            auto fWebView=adoptRef(*new WebViewBase(name,rect,parentWindow,config));
            return fWebView;
        }
        WebPageProxy* page() const { return fPage.get(); }
        void initializeOnce();
        const char* currentURL() { return page()->pageLoadState().activeURL().utf8().data(); }
        //hook methods
        virtual void FrameResized(float,float);
        virtual void Draw (BRect);
        virtual void MouseMoved (BPoint,uint32,const BMessage*);
        private:
        WebViewBase(const char*,BRect,BWindow*,const API::PageConfiguration&);
        
        void paint(const WebCore::IntRect&);
        
        RefPtr<WebPageProxy> fPage;
        std::unique_ptr<PageClientImpl> fPageClient;
        
        bool setSize {false};
    };
}


