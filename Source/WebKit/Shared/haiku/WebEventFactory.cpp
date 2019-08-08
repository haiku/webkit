/*
 * Copyright (C) 2019 Haiku Inc. All rights reserved.
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

#include "WebEventFactory.h"
#include <WebCore/IntPoint.h>

enum {
    BUTTON_PRESS = 'btps',
    BUTTON_RELEASE = 'btrl',
    MOUSE_MOVEMENT = 'mmmv',
}
namespace WebKit
{
    static inline int clickCount(WebEvent::Type type, WebMouseEvent::Button button, const POINT& position, int64_t timeStampSeconds)
    {
        static int gLastClickCount;
        static int64_t gLastClickTime;
        static POINT lastClickPosition;
        static WebMouseEvent::Button lastClickButton = WebMouseEvent::LeftButton;
    
        bool cancelPreviousClick = 0;
    
        if (type == WebEvent::MouseDown) {
            if (!cancelPreviousClick && (button == lastClickButton))
                ++gLastClickCount;
            else {
                gLastClickCount = 1;
                lastClickPosition = position;
            }
            gLastClickTime = timeStampSeconds;
            lastClickButton = button;
        } else if (type == WebEvent::MouseMove) {
            if (cancelPreviousClick) {
                gLastClickCount = 0;
                lastClickPosition.x = 0;
                lastClickPosition.y = 0;
                gLastClickTime = 0;
            }
        }
    
        return gLastClickCount;
    }
    static inline WebMouseEvent::Button buttonForEvent(const BMessage* message)
    {
        int32_t buttonType;
        unsigned button = 0;
        message->FindInt32("button",&buttonType);
        switch(buttonType)
        {
            case B_PRIMARY_MOUSE_BUTTON:
            button = WebMouseEvent::LeftButton;
            break;
            case B_SECONDARY_MOUSE_BUTTON:
            button = WebMouseEvent::RightButton;
            break;
            case B_TERTIARY_MOUSE_BUTTON:
            button = WebMouseEvent::MiddleButton;
            break;
        }
        
        return static_cast<WebMouseEvent::Button>(button);
    }
    static WebMouseEvent createWebMouseEvent(const BMessage* message)
    {
        WebEvent::Type type = static_cast<WebEvent::Type>(0);
        int32_t mouseEventType;
        message->FindInt32("type",&mouseEventType);
        switch(mouseEventType)
        {
            case BUTTON_PRESS:
            type = WebEvent::MouseDown;
            break;
            case BUTTON_RELEASE:
            type = WebEvent::MouseUp;
            break;
            case MOUSE_MOVEMENT:
            type = WebEvent::MouseMove;
            break;
            default:
            ASSERT_NOT_REACHED();
        }
        
        BPoint where;
        message->FindPoint("where",&where);
        
        int64_t when;
        message->FindInt64("when",&when);
        
        /*return WebMouseEvent(
        type,buttonForEvent(message),0,IntPoint(where),IntPoint(where),
        0,0,0,
        )*/
        //it says some deltax delta y should it be added from be_deltax?
        
    }
    
}
