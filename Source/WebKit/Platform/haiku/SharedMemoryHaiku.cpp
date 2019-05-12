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
#include "SharedMemory.h"

#include "Decoder.h"
#include "Encoder.h"

namespace WebKit {

SharedMemory::Handle::Handle()
{
}

SharedMemory::Handle::~Handle()
{
}

SharedMemory::Handle::Handle(Handle&&) = default;
SharedMemory::Handle& SharedMemory::Handle::operator=(Handle&& other) = default;

void SharedMemory::Handle::clear()
{
}

bool SharedMemory::Handle::isNull() const
{
}

void SharedMemory::Handle::encode(IPC::Encoder& encoder) const
{
}

bool SharedMemory::Handle::decode(IPC::Decoder& decoder, Handle& handle)
{
}

static int createSharedMemory()
{
}

RefPtr<SharedMemory> SharedMemory::allocate(size_t size)
{
}

RefPtr<SharedMemory> SharedMemory::map(const Handle& handle, Protection protection)
{

}

SharedMemory::~SharedMemory()
{
}

bool SharedMemory::createHandle(Handle& handle, Protection)
{
}

unsigned SharedMemory::systemPageSize()
{
}

} // namespace WebKit


