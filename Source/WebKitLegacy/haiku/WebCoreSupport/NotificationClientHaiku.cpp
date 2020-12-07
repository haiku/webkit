/*
 * Copyright 2017, Adrien Destugues, pulkomandy@pulkomandy.tk
 * Distributed under terms of the MIT license.
 */


#include "NotificationClientHaiku.h"

#include "WebPage.h"

#include <UrlRequest.h>

namespace WebCore {

BNotification
NotificationClientHaiku::fromDescriptor(Notification* descriptor)
{
    BNotification notification(B_INFORMATION_NOTIFICATION);
    notification.SetGroup("WebPositive");
    // Unfortunately, we don't get a website name or so…
    if (descriptor->body().length() > 0) {
        notification.SetTitle(descriptor->title());
        notification.SetContent(descriptor->body());
    } else {
        notification.SetContent(descriptor->title());
    }

    // TODO we should cache the data, in case the notification is re-sent
    // with some changes for an update.
    BUrl iconURL(descriptor->icon());
    BMallocIO buffer;
    BUrlSession session;
    if (session.InitCheck() == B_OK) {
        BUrlRequest* request = session.MakeRequest(iconURL, &buffer);
        if (request) {
            if (request->Run())
                request->WaitForCompletion();

            BBitmap* bitmap = BTranslationUtils::GetBitmap(&buffer);
            if (bitmap) {
                notification.SetIcon(bitmap);
                delete bitmap;
            }

            delete request;
        }
    }

    notification.SetMessageID(descriptor->tag());

    return notification;
}

};
