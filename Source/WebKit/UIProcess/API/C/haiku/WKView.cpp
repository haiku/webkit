/*
 * Copyright (C) 2012 Samsung Electronics
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
 * Copyright (C) 2019 Haiku, Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "WKView.h"

#include "APIPageConfiguration.h"
#include "WKAPICast.h"
#include "WebViewBase.h"


using namespace WebCore;
using namespace WebKit;


WKViewRef WKViewCreate(const char* name,BRect rect,BWindow* parentWindow,
    WKPageConfigurationRef pageRef)
{
    return toAPI(WebViewBase::create(name,rect,parentWindow,*toImpl(pageRef)).leakRef());
}


WKPageRef WKViewGetPage(WKViewRef viewRef)
{
    return toAPI(toImpl(viewRef)->page());
}
