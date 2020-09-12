/*
 * Copyright (C) 2019 Apple Inc. All rights reserved.
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

WI.URLBreakpoint = class URLBreakpoint extends WI.Breakpoint
{
    constructor(type, url, {disabled} = {})
    {
        console.assert(Object.values(WI.URLBreakpoint.Type).includes(type), type);
        console.assert(typeof url === "string", url);

        super({disabled});

        this._type = type;
        this._url = url;
    }

    // Static

    static fromJSON(json)
    {
        return new WI.URLBreakpoint(json.type, json.url, {
            disabled: json.disabled,
        });
    }

    // Public

    get type() { return this._type; }
    get url() { return this._url; }

    get special()
    {
        return this === WI.domDebuggerManager.allRequestsBreakpoint || super.special;
    }

    remove()
    {
        super.remove();

        WI.domDebuggerManager.removeURLBreakpoint(this);
    }

    saveIdentityToCookie(cookie)
    {
        cookie["url-breakpoint-type"] = this._type;
        cookie["url-breakpoint-url"] = this._url;
    }

    toJSON(key)
    {
        let json = super.toJSON(key);
        json.type = this._type;
        json.url = this._url;
        if (key === WI.ObjectStore.toJSONSymbol)
            json[WI.objectStores.urlBreakpoints.keyPath] = this._type + ":" + this._url;
        return json;
    }
};

WI.URLBreakpoint.Type = {
    Text: "text",
    RegularExpression: "regex",
};
