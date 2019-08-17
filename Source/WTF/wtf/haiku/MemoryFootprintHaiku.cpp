/*
 * Copyright 2018, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adrien Destugues <pulkomandy@pulkomandy.tk>
 */

#include "config.h"
#include "wtf/MemoryFootprint.h"

#include <image.h>


namespace WTF {

size_t memoryFootprint()
{
	image_info info;
	int32* cookie = NULL;

	size_t heapSize;

	while(get_next_image_info(B_CURRENT_TEAM, cookie, &info) == B_OK) {
		heapSize += info.data_size;
	}

	return heapSize;
}

}
