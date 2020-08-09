/*
 * Copyright (C) 2020 Apple Inc. All rights reserved.
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
#include <wtf/Bitmap.h>

namespace TestWebKitAPI {

constexpr size_t size = 128;
constexpr size_t smallSize = 9;
constexpr size_t zeroSize = 0;

static constexpr bool expectedBits1[size] = {
    false, false, true,  false,  false, false, false, false,
    false, false, false, false,  false, false, false, true,
    false, true,  false, false,  false, false, true,  false,
    false, false, true,  false,  false, false, true,  true,
    true,  false, false, false,  false, true,  false, false,
    true,  false, false, false,  false, true,  false, true,
    false, false, false, false,  false, true,  true,  false,
    false, true,  false, false,  false, true,  true,  true,
    false, false, true,  false,  true,  false, false, false,
    false, false, true,  false,  true,  false, false, true,
    true,  false, false, false,  true,  false, true,  false,
    false, false, false, false,  true,  false, true,  true,
    false, false, false, false,  true,  true,  false, false,
    false, true,  false, false,  true,  true,  false, true,
    false, true,  false, false,  true,  true,  true,  false,
    false, false, true,  false,  true,  true,  true,  true,
};

static constexpr bool expectedBits2[size] = {
    false, true,  false, true,   false, false, false, false,
    false, false, false, true,   false, false, false, true,
    false, false, false, true,   false, false, true,  false,
    false, false, false, true,   false, false, true,  true,
    true,  false, false, true,   false, true,  false, false,
    false, false, false, true,   false, true,  false, true,
    false, false, false, true,   false, true,  true,  false,
    false, false, true,  true,   false, true,  true,  true,
    false, true,  false, true,   true,  false, false, false,
    false, true,  false, true,   true,  false, false, true,
    false, false, false, true,   true,  false, true,  false,
    false, false, true,  true,   true,  false, true,  true,
    true,  false, false, true,   true,  true,  false, false,
    false, false, true,  true,   true,  true,  false, true,
    true,  false, false, true,   true,  true,  true,  false,
    false, true,  false, true,   true,  true,  true,  true,
};

constexpr size_t countBits(const bool boolArray[], size_t size)
{
    size_t result = 0;
    for (size_t i = 0; i < size; ++i) {
        if (boolArray[i])
            result++;
    }
    return result;
}

constexpr size_t expectedNumberOfSetBits1 = countBits(expectedBits1, size);
constexpr size_t expectedNumberOfSetBits2 = countBits(expectedBits2, size);

#define DECLARE_BITMAPS_FOR_TEST() \
    Bitmap<size, WordType> bitmap0; /* All bits will remain cleared. */ \
    Bitmap<size, WordType> bitmap1; /* Will hold values specified in expectedBits1. */ \
    Bitmap<size, WordType> bitmap2; /* Will hold values specified in expectedBits2. */ \
    Bitmap<size, WordType> bitmap3; /* Same as bitmap2. */ \
    Bitmap<size, WordType> bitmapFilled; \
    Bitmap<smallSize, WordType> bitmapSmallZeroed; \
    Bitmap<smallSize, WordType> bitmapSmallFilled;

#define DECLARE_AND_INIT_BITMAPS_FOR_TEST() \
    DECLARE_BITMAPS_FOR_TEST() \
    for (size_t i = 0; i < size; ++i) { \
        bitmap1.set(i, expectedBits1[i]); \
        bitmap2.set(i, expectedBits2[i]); \
        bitmap3.set(i, expectedBits2[i]); \
        bitmapFilled.set(i); \
    } \
    for (size_t i = 0; i < smallSize; ++i) \
        bitmapSmallFilled.set(i);

template<typename WordType>
void testBitmapSize()
{
    DECLARE_BITMAPS_FOR_TEST();

    auto verifySize = [=] (auto& bitmap, size_t expectedSize, size_t notExpectedSize) {
        EXPECT_EQ(bitmap.size(), expectedSize);
        EXPECT_NE(bitmap.size(), notExpectedSize);
    };

    verifySize(bitmap0, size, smallSize);
    verifySize(bitmap1, size, smallSize);
    verifySize(bitmap2, size, smallSize);
    verifySize(bitmap3, size, smallSize);
    verifySize(bitmapFilled, size, smallSize);
    verifySize(bitmapSmallZeroed, smallSize, size);
    verifySize(bitmapSmallFilled, smallSize, size);
}

template<typename WordType>
void testBitmapConstructedEmpty()
{
    DECLARE_BITMAPS_FOR_TEST();

    // Verify that the default constructor initializes all bits to 0.
    auto verifyIsEmpty = [=] (const auto& bitmap) {
        EXPECT_TRUE(bitmap.isEmpty());
        EXPECT_EQ(bitmap.count(), zeroSize);
        for (size_t i = 0; i < bitmap.size(); ++i)
            EXPECT_FALSE(bitmap.get(i));
    };

    verifyIsEmpty(bitmap0);
    verifyIsEmpty(bitmap1);
    verifyIsEmpty(bitmap2);
    verifyIsEmpty(bitmap3);
    verifyIsEmpty(bitmapFilled);
    verifyIsEmpty(bitmapSmallZeroed);
    verifyIsEmpty(bitmapSmallFilled);
}

template<typename WordType>
void testBitmapSetGet()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    auto verifyIsEmpty = [=] (const auto& bitmap) {
        EXPECT_TRUE(bitmap.isEmpty());
        EXPECT_EQ(bitmap.count(), zeroSize);
        for (size_t i = 0; i < bitmap.size(); ++i)
            EXPECT_FALSE(bitmap.get(i));
    };

    for (size_t i = 0; i < size; ++i)
        EXPECT_EQ(bitmap1.get(i), expectedBits1[i]);

    for (size_t i = 0; i < size; ++i) {
        EXPECT_EQ(bitmap2.get(i), expectedBits2[i]);
        EXPECT_EQ(bitmap3.get(i), expectedBits2[i]);
    }
    for (size_t i = 0; i < size; ++i)
        EXPECT_EQ(bitmap2.get(i), bitmap3.get(i));

    for (size_t i = 0; i < size; ++i)
        EXPECT_TRUE(bitmapFilled.get(i));

    for (size_t i = 0; i < smallSize; ++i)
        EXPECT_TRUE(bitmapSmallFilled.get(i));

    // Verify that there is no out of bounds write from the above set operations.
    verifyIsEmpty(bitmap0);
    verifyIsEmpty(bitmapSmallZeroed);
}

template<typename WordType>
void testBitmapTestAndSet()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    ASSERT_FALSE(bitmap1.isFull());
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.get(i), expectedBits1[i]);
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.testAndSet(i), expectedBits1[i]);
    ASSERT_TRUE(bitmap1.isFull());

    ASSERT_FALSE(bitmapSmallZeroed.isFull());
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    for (size_t i = 0; i < smallSize; ++i)
        ASSERT_FALSE(bitmapSmallZeroed.testAndSet(i));
    ASSERT_TRUE(bitmapSmallZeroed.isFull());
}

template<typename WordType>
void testBitmapTestAndClear()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    ASSERT_FALSE(bitmap1.isEmpty());
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.get(i), expectedBits1[i]);
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.testAndClear(i), expectedBits1[i]);
    ASSERT_TRUE(bitmap1.isEmpty());

    ASSERT_FALSE(bitmapSmallFilled.isEmpty());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    for (size_t i = 0; i < smallSize; ++i)
        ASSERT_TRUE(bitmapSmallFilled.testAndClear(i));
    ASSERT_TRUE(bitmapSmallFilled.isEmpty());
}

template<typename WordType>
void testBitmapConcurrentTestAndSet()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    // FIXME: the following does not test the concurrent part. It only ensures that
    // the TestAndSet part of the operation is working.
    ASSERT_FALSE(bitmap1.isFull());
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.get(i), expectedBits1[i]);
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.testAndSet(i), expectedBits1[i]);
    ASSERT_TRUE(bitmap1.isFull());

    ASSERT_FALSE(bitmapSmallZeroed.isFull());
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    for (size_t i = 0; i < smallSize; ++i)
        ASSERT_FALSE(bitmapSmallZeroed.testAndSet(i));
    ASSERT_TRUE(bitmapSmallZeroed.isFull());
}

template<typename WordType>
void testBitmapConcurrentTestAndClear()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    // FIXME: the following does not test the concurrent part. It only ensures that
    // the TestAndClear part of the operation is working.
    ASSERT_FALSE(bitmap1.isEmpty());
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.get(i), expectedBits1[i]);
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.testAndClear(i), expectedBits1[i]);
    ASSERT_TRUE(bitmap1.isEmpty());

    ASSERT_FALSE(bitmapSmallFilled.isEmpty());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    for (size_t i = 0; i < smallSize; ++i)
        ASSERT_TRUE(bitmapSmallFilled.testAndClear(i));
    ASSERT_TRUE(bitmapSmallFilled.isEmpty());
}

template<typename WordType>
void testBitmapClear()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    ASSERT_TRUE(bitmapFilled.isFull());
    for (size_t i = 0; i < size; ++i) {
        if (!expectedBits1[i])
            bitmapFilled.clear(i);
    }
    ASSERT_EQ(bitmapFilled, bitmap1);
}

template<typename WordType>
void testBitmapClearAll()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    auto verifyIsEmpty = [=] (const auto& bitmap) {
        EXPECT_TRUE(bitmap.isEmpty());
        EXPECT_EQ(bitmap.count(), zeroSize);
        for (size_t i = 0; i < bitmap.size(); ++i)
            EXPECT_FALSE(bitmap.get(i));
    };

    auto verifyIsFull = [=] (const auto& bitmap) {
        EXPECT_TRUE(bitmap.isFull());
        for (size_t i = 0; i < bitmap.size(); ++i)
            EXPECT_TRUE(bitmap.get(i));
    };

    EXPECT_FALSE(bitmap1.isEmpty());
    bitmap1.clearAll();
    verifyIsEmpty(bitmap1);
    verifyIsFull(bitmapFilled);
    verifyIsFull(bitmapSmallFilled);

    EXPECT_FALSE(bitmap2.isEmpty());
    bitmap2.clearAll();
    verifyIsEmpty(bitmap1);
    verifyIsEmpty(bitmap2);
    verifyIsFull(bitmapFilled);
    verifyIsFull(bitmapSmallFilled);

    EXPECT_FALSE(bitmap3.isEmpty());
    bitmap3.clearAll();
    verifyIsEmpty(bitmap1);
    verifyIsEmpty(bitmap2);
    verifyIsEmpty(bitmap3);
    verifyIsFull(bitmapFilled);
    verifyIsFull(bitmapSmallFilled);
}

template<typename WordType>
void testBitmapInvert()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    auto verifyInvert = [=] (auto& bitmap) {
        ASSERT_TRUE(bitmap.isFull());
        ASSERT_FALSE(bitmap.isEmpty());
        bitmap.invert();
        ASSERT_FALSE(bitmap.isFull());
        ASSERT_TRUE(bitmap.isEmpty());
        bitmap.invert();
        ASSERT_TRUE(bitmap.isFull());
        ASSERT_FALSE(bitmap.isEmpty());
    };

    verifyInvert(bitmapFilled);
    verifyInvert(bitmapSmallFilled);

    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.get(i), expectedBits1[i]);
    bitmap1.invert();
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.get(i), !expectedBits1[i]);
    bitmap1.invert();
    for (size_t i = 0; i < size; ++i)
        ASSERT_EQ(bitmap1.get(i), expectedBits1[i]);
}

template<typename WordType>
void testBitmapFindRunOfZeros()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    const int64_t bitmap1RunsOfZeros[15] = {
        0, 0, 0, 3, 3,
        3, 3, 3, 3, 3,
        3, 3, 3, -1, -1
    };

    const int64_t bitmap2RunsOfZeros[15] = {
        0, 0, 4, 4, 4,
        4, 4, 4, -1, -1,
        -1, -1, -1, -1, -1,
    };

    Bitmap<smallSize, WordType> smallTemp;
    smallTemp.set(1, true); // bits low to high are: 0100 0000 0

    const int64_t smallTempRunsOfZeros[15] = {
        0, 0, 2, 2, 2,
        2, 2, 2, -1, -1,
        -1, -1, -1, -1, -1
    };

    for (size_t runLength = 0; runLength < 15; ++runLength) {
        ASSERT_EQ(bitmap0.findRunOfZeros(runLength), 0ll);
        ASSERT_EQ(bitmap1.findRunOfZeros(runLength), bitmap1RunsOfZeros[runLength]);
        ASSERT_EQ(bitmap2.findRunOfZeros(runLength), bitmap2RunsOfZeros[runLength]);
        ASSERT_EQ(bitmapFilled.findRunOfZeros(runLength), -1ll);

        if (runLength <= smallSize)
            ASSERT_EQ(bitmapSmallZeroed.findRunOfZeros(runLength), 0ll);
        else
            ASSERT_EQ(bitmapSmallZeroed.findRunOfZeros(runLength), -1ll);

        ASSERT_EQ(bitmapSmallFilled.findRunOfZeros(runLength), -1ll);

        ASSERT_EQ(smallTemp.findRunOfZeros(runLength), smallTempRunsOfZeros[runLength]);
    }
}

template<typename WordType>
void testBitmapCount()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    EXPECT_EQ(bitmap0.count(), zeroSize);
    EXPECT_EQ(bitmap1.count(), expectedNumberOfSetBits1);
    EXPECT_EQ(bitmap2.count(), expectedNumberOfSetBits2);
    EXPECT_EQ(bitmap3.count(), expectedNumberOfSetBits2);
    EXPECT_EQ(bitmapFilled.count(), size);
    EXPECT_EQ(bitmapSmallZeroed.count(), zeroSize);
    EXPECT_EQ(bitmapSmallFilled.count(), smallSize);
}

template<typename WordType>
void testBitmapIsEmpty()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    EXPECT_TRUE(bitmap0.isEmpty());
    EXPECT_FALSE(bitmap1.isEmpty());
    EXPECT_FALSE(bitmap2.isEmpty());
    EXPECT_FALSE(bitmap3.isEmpty());
    EXPECT_FALSE(bitmapFilled.isEmpty());
    EXPECT_TRUE(bitmapSmallZeroed.isEmpty());
    EXPECT_FALSE(bitmapSmallFilled.isEmpty());

    auto verifyIsEmpty = [=] (const auto& bitmap) {
        EXPECT_TRUE(bitmap.isEmpty());
        EXPECT_EQ(bitmap.count(), zeroSize);
        for (size_t i = 0; i < bitmap.size(); ++i)
            EXPECT_FALSE(bitmap.get(i));
    };

    verifyIsEmpty(bitmap0);
    verifyIsEmpty(bitmapSmallZeroed);
}

template<typename WordType>
void testBitmapIsFull()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    EXPECT_FALSE(bitmap0.isFull());
    EXPECT_FALSE(bitmap1.isFull());
    EXPECT_FALSE(bitmap2.isFull());
    EXPECT_FALSE(bitmap3.isFull());
    EXPECT_TRUE(bitmapFilled.isFull());
    EXPECT_FALSE(bitmapSmallZeroed.isFull());
    EXPECT_TRUE(bitmapSmallFilled.isFull());

    auto verifyIsFull = [=] (auto& bitmap) {
        EXPECT_TRUE(bitmap.isFull());
        for (size_t i = 0; i < bitmap.size(); ++i)
            EXPECT_TRUE(bitmap.get(i));
    };

    verifyIsFull(bitmapFilled);
    verifyIsFull(bitmapSmallFilled);
}

template<typename WordType>
void testBitmapMerge()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    ASSERT_TRUE(bitmap0.isEmpty());
    ASSERT_EQ(bitmap2, bitmap3);
    bitmap2.merge(bitmap0);
    ASSERT_EQ(bitmap2, bitmap3);
    
    ASSERT_NE(bitmap1, bitmap2);
    ASSERT_NE(bitmap1, bitmap3);
    ASSERT_EQ(bitmap2, bitmap3);
    bitmap3.merge(bitmap1);
    ASSERT_NE(bitmap2, bitmap3);
    for (size_t i = 0; i < size; ++i) {
        bool expectedBit = expectedBits2[i] | expectedBits1[i];
        ASSERT_EQ(bitmap3.get(i), expectedBit);
    }

    ASSERT_TRUE(bitmapFilled.isFull());
    ASSERT_TRUE(bitmap0.isEmpty());
    bitmapFilled.merge(bitmap0);
    ASSERT_TRUE(bitmapFilled.isFull());
    ASSERT_TRUE(bitmap0.isEmpty());

    bitmap0.merge(bitmap1);
    ASSERT_EQ(bitmap0, bitmap1);

    bitmap1.merge(bitmapFilled);
    ASSERT_EQ(bitmap1, bitmapFilled);

    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    bitmapSmallZeroed.merge(bitmapSmallFilled);
    ASSERT_FALSE(bitmapSmallZeroed.isEmpty());
    ASSERT_TRUE(bitmapSmallZeroed.isFull());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    
    bitmapSmallZeroed.clearAll();
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    bitmapSmallFilled.merge(bitmapSmallZeroed);
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
}

template<typename WordType>
void testBitmapFilter()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    ASSERT_TRUE(bitmapFilled.isFull());
    ASSERT_EQ(bitmap2, bitmap3);
    bitmap2.filter(bitmapFilled);
    ASSERT_EQ(bitmap2, bitmap3);
    
    ASSERT_NE(bitmap1, bitmap2);
    ASSERT_NE(bitmap1, bitmap3);
    ASSERT_EQ(bitmap2, bitmap3);
    bitmap3.filter(bitmap1);
    ASSERT_NE(bitmap2, bitmap3);
    for (size_t i = 0; i < size; ++i) {
        bool expectedBit = expectedBits2[i] & expectedBits1[i];
        ASSERT_EQ(bitmap3.get(i), expectedBit);
    }

    ASSERT_TRUE(bitmap0.isEmpty());
    bitmap2.filter(bitmap0);
    ASSERT_EQ(bitmap2, bitmap0);

    ASSERT_TRUE(bitmap0.isEmpty());
    bitmap0.filter(bitmap1);
    ASSERT_TRUE(bitmap0.isEmpty());

    ASSERT_TRUE(bitmapFilled.isFull());
    bitmapFilled.filter(bitmap1);
    ASSERT_EQ(bitmapFilled, bitmap1);

    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    bitmapSmallZeroed.filter(bitmapSmallFilled);
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    ASSERT_FALSE(bitmapSmallZeroed.isFull());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    
    bitmapSmallFilled.filter(bitmapSmallZeroed);
    ASSERT_FALSE(bitmapSmallFilled.isFull());
    ASSERT_TRUE(bitmapSmallFilled.isEmpty());
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
}

template<typename WordType>
void testBitmapExclude()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    ASSERT_TRUE(bitmap0.isEmpty());
    ASSERT_EQ(bitmap2, bitmap3);
    bitmap2.exclude(bitmap0);
    ASSERT_EQ(bitmap2, bitmap3);
    
    ASSERT_NE(bitmap1, bitmap2);
    ASSERT_NE(bitmap1, bitmap3);
    ASSERT_EQ(bitmap2, bitmap3);
    bitmap3.exclude(bitmap1);
    ASSERT_NE(bitmap2, bitmap3);
    for (size_t i = 0; i < size; ++i) {
        bool expectedBit = expectedBits2[i] & !expectedBits1[i];
        ASSERT_EQ(bitmap3.get(i), expectedBit);
    }

    ASSERT_TRUE(bitmap0.isEmpty());
    ASSERT_TRUE(bitmapFilled.isFull());
    bitmap2.exclude(bitmapFilled);
    ASSERT_EQ(bitmap2, bitmap0);

    ASSERT_TRUE(bitmapFilled.isFull());
    bitmapFilled.exclude(bitmap1);
    bitmap1.invert();
    ASSERT_EQ(bitmapFilled, bitmap1);

    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    bitmapSmallZeroed.exclude(bitmapSmallFilled);
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    ASSERT_FALSE(bitmapSmallZeroed.isFull());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    
    bitmapSmallFilled.exclude(bitmapSmallZeroed);
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());

    bitmapSmallFilled.exclude(bitmapSmallFilled);
    ASSERT_FALSE(bitmapSmallFilled.isFull());
    ASSERT_TRUE(bitmapSmallFilled.isEmpty());
}

template<typename WordType>
void testBitmapConcurrentFilter()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    // FIXME: the following does not test the concurrent part. It only ensures that
    // the Filter part of the operation is working.
    ASSERT_TRUE(bitmapFilled.isFull());
    ASSERT_EQ(bitmap2, bitmap3);
    bitmap2.concurrentFilter(bitmapFilled);
    ASSERT_EQ(bitmap2, bitmap3);
    
    ASSERT_NE(bitmap1, bitmap2);
    ASSERT_NE(bitmap1, bitmap3);
    ASSERT_EQ(bitmap2, bitmap3);
    bitmap3.concurrentFilter(bitmap1);
    ASSERT_NE(bitmap2, bitmap3);
    for (size_t i = 0; i < size; ++i) {
        bool expectedBit = expectedBits2[i] & expectedBits1[i];
        ASSERT_EQ(bitmap3.get(i), expectedBit);
    }

    ASSERT_TRUE(bitmap0.isEmpty());
    bitmap2.concurrentFilter(bitmap0);
    ASSERT_EQ(bitmap2, bitmap0);

    ASSERT_TRUE(bitmap0.isEmpty());
    bitmap0.concurrentFilter(bitmap1);
    ASSERT_TRUE(bitmap0.isEmpty());

    ASSERT_TRUE(bitmapFilled.isFull());
    bitmapFilled.concurrentFilter(bitmap1);
    ASSERT_EQ(bitmapFilled, bitmap1);

    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    bitmapSmallZeroed.concurrentFilter(bitmapSmallFilled);
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    ASSERT_FALSE(bitmapSmallZeroed.isFull());
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    
    bitmapSmallFilled.concurrentFilter(bitmapSmallZeroed);
    ASSERT_FALSE(bitmapSmallFilled.isFull());
    ASSERT_TRUE(bitmapSmallFilled.isEmpty());
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
}

template<typename WordType>
void testBitmapSubsumes()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    ASSERT_TRUE(bitmap0.isEmpty());
    ASSERT_TRUE(bitmapFilled.isFull());
    ASSERT_EQ(bitmap2, bitmap3);

    ASSERT_TRUE(bitmap0.subsumes(bitmap0));
    ASSERT_FALSE(bitmap0.subsumes(bitmap1));
    ASSERT_FALSE(bitmap0.subsumes(bitmap2));
    ASSERT_FALSE(bitmap0.subsumes(bitmap3));
    ASSERT_FALSE(bitmap0.subsumes(bitmapFilled));

    ASSERT_TRUE(bitmap1.subsumes(bitmap0));
    ASSERT_TRUE(bitmap1.subsumes(bitmap1));
    ASSERT_FALSE(bitmap1.subsumes(bitmap2));
    ASSERT_FALSE(bitmap1.subsumes(bitmap3));
    ASSERT_FALSE(bitmap1.subsumes(bitmapFilled));

    ASSERT_TRUE(bitmap2.subsumes(bitmap0));
    ASSERT_FALSE(bitmap2.subsumes(bitmap1));
    ASSERT_TRUE(bitmap2.subsumes(bitmap2));
    ASSERT_TRUE(bitmap2.subsumes(bitmap3));
    ASSERT_FALSE(bitmap2.subsumes(bitmapFilled));

    ASSERT_TRUE(bitmap3.subsumes(bitmap0));
    ASSERT_FALSE(bitmap3.subsumes(bitmap1));
    ASSERT_TRUE(bitmap3.subsumes(bitmap2));
    ASSERT_TRUE(bitmap3.subsumes(bitmap3));
    ASSERT_FALSE(bitmap3.subsumes(bitmapFilled));

    ASSERT_TRUE(bitmapFilled.subsumes(bitmap0));
    ASSERT_TRUE(bitmapFilled.subsumes(bitmap1));
    ASSERT_TRUE(bitmapFilled.subsumes(bitmap2));
    ASSERT_TRUE(bitmapFilled.subsumes(bitmap3));
    ASSERT_TRUE(bitmapFilled.subsumes(bitmapFilled));

    ASSERT_TRUE(bitmapSmallFilled.subsumes(bitmapSmallFilled));
    ASSERT_TRUE(bitmapSmallFilled.subsumes(bitmapSmallZeroed));

    ASSERT_FALSE(bitmapSmallZeroed.subsumes(bitmapSmallFilled));
    ASSERT_TRUE(bitmapSmallZeroed.subsumes(bitmapSmallZeroed));
}

template<typename WordType>
void testBitmapForEachSetBit()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    size_t count = 0;
    ASSERT_TRUE(bitmap0.isEmpty());
    bitmap0.forEachSetBit([&] (size_t i) {
        constexpr bool notReached = false;
        ASSERT_TRUE(notReached);
        count++;
    });
    ASSERT_EQ(count, zeroSize);

    count = 0;
    bitmap1.forEachSetBit([&] (size_t i) {
        ASSERT_TRUE(bitmap1.get(i));
        ASSERT_EQ(bitmap1.get(i), expectedBits1[i]);
        count++;
    });
    ASSERT_EQ(count, expectedNumberOfSetBits1);

    count = 0;
    ASSERT_TRUE(bitmapFilled.isFull());
    bitmapFilled.forEachSetBit([&] (size_t i) {
        ASSERT_TRUE(bitmapFilled.get(i));
        count++;
    });
    ASSERT_EQ(count, size);

    count = 0;
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    bitmapSmallZeroed.forEachSetBit([&] (size_t i) {
        constexpr bool notReached = false;
        ASSERT_TRUE(notReached);
        count++;
    });
    ASSERT_EQ(count, zeroSize);

    count = 0;
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    bitmapSmallFilled.forEachSetBit([&] (size_t i) {
        ASSERT_TRUE(bitmapSmallFilled.get(i));
        count++;
    });
    ASSERT_EQ(count, smallSize);
}

template<typename WordType>
void testBitmapFindBit()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    auto findExpectedBit = [=] (auto expectedBits, size_t startIndex, bool value) -> size_t {
        for (auto index = startIndex; index < size; ++index) {
            if (expectedBits[index] == value)
                return index;
        }
        return startIndex;
    };
    
    for (size_t i = 1, lastIndex = 1; i < size; lastIndex = i, i += lastIndex) {
        ASSERT_EQ(bitmap1.findBit(i, true), findExpectedBit(expectedBits1, i, true));
        ASSERT_EQ(bitmap1.findBit(i, false), findExpectedBit(expectedBits1, i, false));
        ASSERT_EQ(bitmap2.findBit(i, true), findExpectedBit(expectedBits2, i, true));
        ASSERT_EQ(bitmap2.findBit(i, false), findExpectedBit(expectedBits2, i, false));

        ASSERT_EQ(bitmap2.findBit(i, true), bitmap3.findBit(i, true));
        ASSERT_EQ(bitmap2.findBit(i, false), bitmap3.findBit(i, false));
    }

    ASSERT_EQ(bitmap0.findBit(0, false), zeroSize);
    ASSERT_EQ(bitmap0.findBit(10, false), static_cast<size_t>(10));
    ASSERT_EQ(bitmap0.findBit(size - 1, false), size-1);
    ASSERT_EQ(bitmap0.findBit(size, false), size);
    ASSERT_EQ(bitmap0.findBit(size + 1, false), size);

    ASSERT_EQ(bitmap0.findBit(0, true), size);
    ASSERT_EQ(bitmap0.findBit(10, true), size);
    ASSERT_EQ(bitmap0.findBit(size - 1, true), size);
    ASSERT_EQ(bitmap0.findBit(size, true), size);
    ASSERT_EQ(bitmap0.findBit(size + 1, true), size);

    ASSERT_EQ(bitmapFilled.findBit(0, false), size);
    ASSERT_EQ(bitmapFilled.findBit(10, false), size);
    ASSERT_EQ(bitmapFilled.findBit(size - 1, false), size);
    ASSERT_EQ(bitmapFilled.findBit(size, false), size);
    ASSERT_EQ(bitmapFilled.findBit(size + 1, false), size);

    ASSERT_EQ(bitmapFilled.findBit(0, true), zeroSize);
    ASSERT_EQ(bitmapFilled.findBit(10, true), static_cast<size_t>(10));
    ASSERT_EQ(bitmapFilled.findBit(size - 1, true), size-1);
    ASSERT_EQ(bitmapFilled.findBit(size, true), size);
    ASSERT_EQ(bitmapFilled.findBit(size + 1, true), size);
}

template<typename WordType>
void testBitmapIteration()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    auto computeSetBitsCount = [=] (auto& bitmap) -> size_t {
        size_t count = 0;
        for (auto bitIndex : bitmap) {
            UNUSED_PARAM(bitIndex);
            count++;
        }
        return count;
    };

    EXPECT_EQ(computeSetBitsCount(bitmap0), zeroSize);
    EXPECT_EQ(computeSetBitsCount(bitmap1), expectedNumberOfSetBits1);
    EXPECT_EQ(computeSetBitsCount(bitmap2), expectedNumberOfSetBits2);
    EXPECT_EQ(computeSetBitsCount(bitmap3), expectedNumberOfSetBits2);
    EXPECT_EQ(computeSetBitsCount(bitmapFilled), size);
    EXPECT_EQ(computeSetBitsCount(bitmapSmallZeroed), zeroSize);
    EXPECT_EQ(computeSetBitsCount(bitmapSmallFilled), smallSize);

    auto verifySetBits = [=] (auto& bitmap, auto& expectedBits) {
        for (auto bitIndex : bitmap) {
            EXPECT_TRUE(bitmap.get(bitIndex));
            EXPECT_EQ(bitmap.get(bitIndex), expectedBits[bitIndex]);
        }
    };

    verifySetBits(bitmap1, expectedBits1);
    verifySetBits(bitmap2, expectedBits2);
    verifySetBits(bitmap3, expectedBits2);

    auto verifyBitsAllSet = [=] (auto& bitmap) {
        size_t lastBitIndex = 0;
        for (auto bitIndex : bitmap) {
            EXPECT_TRUE(bitmap.get(bitIndex));
            if (bitIndex)
                EXPECT_EQ(bitIndex, lastBitIndex + 1);
            lastBitIndex = bitIndex;
        }
    };

    verifyBitsAllSet(bitmapFilled);
    verifyBitsAllSet(bitmapSmallFilled);
}

template<typename WordType>
void testBitmapMergeAndClear()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    Bitmap<size, WordType> temp;
    Bitmap<size, WordType> savedBitmap1;

    ASSERT_FALSE(bitmap2.isEmpty());

    savedBitmap1.merge(bitmap1);
    ASSERT_EQ(savedBitmap1, bitmap1);
    
    ASSERT_NE(bitmap1, bitmap2);
    ASSERT_NE(bitmap1, bitmap3);
    ASSERT_EQ(bitmap3, bitmap2);
    bitmap1.mergeAndClear(bitmap3);
    ASSERT_NE(bitmap1, bitmap2);
    ASSERT_NE(bitmap1, savedBitmap1);
    ASSERT_TRUE(bitmap3.isEmpty());
    for (size_t i = 0; i < size; ++i) {
        bool expectedBit = expectedBits1[i] | expectedBits2[i];
        ASSERT_EQ(bitmap1.get(i), expectedBit);
    }

    bitmap3.merge(bitmap2); // restore bitmap3
    ASSERT_EQ(bitmap3, bitmap2);

    ASSERT_TRUE(temp.isEmpty());
    temp.mergeAndClear(bitmap3);
    ASSERT_FALSE(temp.isEmpty());
    ASSERT_EQ(temp, bitmap2);
    ASSERT_TRUE(bitmap3.isEmpty());

    Bitmap<size, WordType> savedBitmapFilled;
    savedBitmapFilled.merge(bitmapFilled);

    temp.clearAll();
    ASSERT_TRUE(temp.isEmpty());
    ASSERT_FALSE(temp.isFull());
    ASSERT_FALSE(bitmapFilled.isEmpty());
    ASSERT_TRUE(bitmapFilled.isFull());
    temp.mergeAndClear(bitmapFilled);
    ASSERT_FALSE(temp.isEmpty());
    ASSERT_TRUE(temp.isFull());
    ASSERT_TRUE(bitmapFilled.isEmpty());
    ASSERT_FALSE(bitmapFilled.isFull());

    bitmap3.merge(bitmap2); // restore bitmap3
    ASSERT_EQ(bitmap3, bitmap2);
    bitmapFilled.merge(savedBitmapFilled); // restore bitmapFilled
    ASSERT_TRUE(bitmapFilled.isFull());

    ASSERT_TRUE(temp.isFull());
    temp.mergeAndClear(bitmap3);
    ASSERT_TRUE(temp.isFull());
    ASSERT_EQ(temp, bitmapFilled);
    ASSERT_TRUE(bitmap3.isEmpty());

    Bitmap<smallSize, WordType> smallTemp;

    smallTemp.merge(bitmapSmallFilled);
    ASSERT_TRUE(smallTemp.isFull());
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    smallTemp.mergeAndClear(bitmapSmallZeroed);
    ASSERT_TRUE(smallTemp.isFull());
    ASSERT_FALSE(smallTemp.isEmpty());
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());

    smallTemp.clearAll();
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    ASSERT_TRUE(smallTemp.isEmpty());
    smallTemp.mergeAndClear(bitmapSmallFilled);
    ASSERT_TRUE(smallTemp.isFull());
    ASSERT_TRUE(bitmapSmallFilled.isEmpty());
}

template<typename WordType>
void testBitmapSetAndClear()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    Bitmap<size, WordType> temp;
    Bitmap<size, WordType> savedBitmap1;

    ASSERT_FALSE(bitmap2.isEmpty());

    savedBitmap1.merge(bitmap1);
    ASSERT_EQ(savedBitmap1, bitmap1);
    
    ASSERT_NE(bitmap1, bitmap2);
    ASSERT_NE(bitmap1, bitmap3);
    ASSERT_EQ(bitmap3, bitmap2);
    bitmap1.setAndClear(bitmap3);
    ASSERT_EQ(bitmap1, bitmap2);
    ASSERT_NE(bitmap1, savedBitmap1);
    ASSERT_TRUE(bitmap3.isEmpty());

    bitmap3.merge(bitmap2); // restore bitmap3
    ASSERT_EQ(bitmap3, bitmap2);

    ASSERT_TRUE(temp.isEmpty());
    temp.setAndClear(bitmap3);
    ASSERT_FALSE(temp.isEmpty());
    ASSERT_EQ(temp, bitmap2);
    ASSERT_TRUE(bitmap3.isEmpty());

    temp.clearAll();
    ASSERT_TRUE(temp.isEmpty());
    ASSERT_FALSE(temp.isFull());
    ASSERT_FALSE(bitmapFilled.isEmpty());
    ASSERT_TRUE(bitmapFilled.isFull());
    temp.setAndClear(bitmapFilled);
    ASSERT_FALSE(temp.isEmpty());
    ASSERT_TRUE(temp.isFull());
    ASSERT_TRUE(bitmapFilled.isEmpty());
    ASSERT_FALSE(bitmapFilled.isFull());

    bitmap3.merge(bitmap2); // restore bitmap3
    ASSERT_EQ(bitmap3, bitmap2);

    ASSERT_TRUE(temp.isFull());
    temp.setAndClear(bitmap3);
    ASSERT_FALSE(temp.isFull());
    ASSERT_EQ(temp, bitmap2);
    ASSERT_TRUE(bitmap3.isEmpty());

    Bitmap<smallSize, WordType> smallTemp;

    smallTemp.merge(bitmapSmallFilled);
    ASSERT_TRUE(smallTemp.isFull());
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    smallTemp.setAndClear(bitmapSmallZeroed);
    ASSERT_TRUE(smallTemp.isEmpty());
    ASSERT_TRUE(bitmapSmallZeroed.isEmpty());
    
    ASSERT_TRUE(bitmapSmallFilled.isFull());
    ASSERT_TRUE(smallTemp.isEmpty());
    smallTemp.setAndClear(bitmapSmallFilled);
    ASSERT_TRUE(smallTemp.isFull());
    ASSERT_TRUE(bitmapSmallFilled.isEmpty());
}

template<typename WordType>
void testBitmapOperatorEqual()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    EXPECT_TRUE(bitmap0 == bitmap0);
    EXPECT_FALSE(bitmap0 == bitmap1);
    EXPECT_TRUE(bitmap1 == bitmap1);
    EXPECT_FALSE(bitmap1 == bitmap2);
    EXPECT_FALSE(bitmap1 == bitmap3);
    EXPECT_TRUE(bitmap2 == bitmap3);
    EXPECT_FALSE(bitmapFilled == bitmap1);
    EXPECT_FALSE(bitmapSmallZeroed == bitmapSmallFilled);
}

template<typename WordType>
void testBitmapOperatorNotEqual()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    EXPECT_FALSE(bitmap0 != bitmap0);
    EXPECT_TRUE(bitmap0 != bitmap1);
    EXPECT_FALSE(bitmap1 != bitmap1);
    EXPECT_TRUE(bitmap1 != bitmap2);
    EXPECT_TRUE(bitmap1 != bitmap3);
    EXPECT_FALSE(bitmap2 != bitmap3);
    EXPECT_TRUE(bitmapFilled != bitmap1);
    EXPECT_TRUE(bitmapSmallZeroed != bitmapSmallFilled);
}

template<typename WordType>
void testBitmapHash()
{
    DECLARE_AND_INIT_BITMAPS_FOR_TEST();

    constexpr unsigned wordSize = sizeof(WordType) * 8;
    constexpr size_t words = (size + wordSize - 1) / wordSize;

    auto expectedBitmap0Hash = [=] () -> unsigned {
        unsigned result = 0;
        WordType zeroWord = 0;
        for (size_t i = 0; i < words; ++i)
            result ^= IntHash<WordType>::hash(zeroWord);
        return result;
    };

    auto expectedBitmapFilledHash = [=] () -> unsigned {
        unsigned result = 0;
        WordType filledWord = static_cast<WordType>(-1);
        for (size_t i = 0; i < words; ++i)
            result ^= IntHash<WordType>::hash(filledWord);
        return result;
    };

    ASSERT_EQ(bitmap0.hash(), expectedBitmap0Hash());
    ASSERT_EQ(bitmapFilled.hash(), expectedBitmapFilledHash());

    auto expectedHash = [=] (auto expectedBits, size_t size) {
        unsigned result = 0;
        for (size_t i = 0; i < size;) {
            WordType word = 0;
            for (size_t j = 0; j < wordSize && i < size; ++i, ++j)
                word |= static_cast<WordType>(!!expectedBits[i]) << j;
            result ^= IntHash<WordType>::hash(word);
        }
        return result;
    };

    ASSERT_EQ(bitmap1.hash(), expectedHash(expectedBits1, size));
    ASSERT_EQ(bitmap2.hash(), expectedHash(expectedBits2, size));

    static constexpr bool expectedSmallBits[smallSize] = {
        true,  false, false, true,  false,  false, false, true,
        true,
    };
    Bitmap<smallSize, WordType> temp1;
    Bitmap<smallSize, WordType> temp2;

    auto initTemp = [&] (auto& bitmap) {
        for (size_t i = 0; i < smallSize; ++i)
            bitmap.set(i, expectedSmallBits[i]);
    };

    initTemp(temp1);
    ASSERT_EQ(temp1.hash(), expectedHash(expectedSmallBits, smallSize));

    temp2.invert();
    initTemp(temp2);
    ASSERT_EQ(temp2.hash(), expectedHash(expectedSmallBits, smallSize));
    ASSERT_EQ(temp2.hash(), temp1.hash());
}

TEST(WTF_Bitmap, Size_uint32_t) { testBitmapSize<uint32_t>(); }
TEST(WTF_Bitmap, ConstructedEmpty_uint32_t) { testBitmapConstructedEmpty<uint32_t>(); }
TEST(WTF_Bitmap, SetGet_uint32_t) { testBitmapSetGet<uint32_t>(); }
TEST(WTF_Bitmap, TestAndSet_uint32_t) { testBitmapTestAndSet<uint32_t>(); }
TEST(WTF_Bitmap, TestAndClear_uint32_t) { testBitmapTestAndClear<uint32_t>(); }
TEST(WTF_Bitmap, ConcurrentTestAndSet_uint32_t) { testBitmapConcurrentTestAndSet<uint32_t>(); }
TEST(WTF_Bitmap, ConcurrentTestAndClear_uint32_t) { testBitmapConcurrentTestAndClear<uint32_t>(); }
TEST(WTF_Bitmap, Clear_uint32_t) { testBitmapClear<uint32_t>(); }
TEST(WTF_Bitmap, ClearAll_uint32_t) { testBitmapClearAll<uint32_t>(); }
TEST(WTF_Bitmap, Invert_uint32_t) { testBitmapInvert<uint32_t>(); }
TEST(WTF_Bitmap, FindRunOfZeros_uint32_t) { testBitmapFindRunOfZeros<uint32_t>(); }
TEST(WTF_Bitmap, Count_uint32_t) { testBitmapCount<uint32_t>(); }
TEST(WTF_Bitmap, IsEmpty_uint32_t) { testBitmapIsEmpty<uint32_t>(); }
TEST(WTF_Bitmap, IsFull_uint32_t) { testBitmapIsFull<uint32_t>(); }
TEST(WTF_Bitmap, Merge_uint32_t) { testBitmapMerge<uint32_t>(); }
TEST(WTF_Bitmap, Filter_uint32_t) { testBitmapFilter<uint32_t>(); }
TEST(WTF_Bitmap, Exclude_uint32_t) { testBitmapExclude<uint32_t>(); }
TEST(WTF_Bitmap, ConcurrentFilter_uint32_t) { testBitmapConcurrentFilter<uint32_t>(); }
TEST(WTF_Bitmap, Subsumes_uint32_t) { testBitmapSubsumes<uint32_t>(); }
TEST(WTF_Bitmap, ForEachSetBit_uint32_t) { testBitmapForEachSetBit<uint32_t>(); }
TEST(WTF_Bitmap, FindBit_uint32_t) { testBitmapFindBit<uint32_t>(); }
TEST(WTF_Bitmap, Iteration_uint32_t) { testBitmapIteration<uint32_t>(); }
TEST(WTF_Bitmap, MergeAndClear_uint32_t) { testBitmapMergeAndClear<uint32_t>(); }
TEST(WTF_Bitmap, SetAndClear_uint32_t) { testBitmapSetAndClear<uint32_t>(); }
TEST(WTF_Bitmap, OperatorEqualAccess_uint32_t) { testBitmapOperatorEqual<uint32_t>(); }
TEST(WTF_Bitmap, OperatorNotEqualAccess_uint32_t) { testBitmapOperatorNotEqual<uint32_t>(); }
TEST(WTF_Bitmap, Hash_uint32_t) { testBitmapHash<uint32_t>(); }

#if CPU(REGISTER64)

TEST(WTF_Bitmap, Size_uint64_t) { testBitmapSize<uint64_t>(); }
TEST(WTF_Bitmap, ConstructedEmpty_uint64_t) { testBitmapConstructedEmpty<uint64_t>(); }
TEST(WTF_Bitmap, SetGet_uint64_t) { testBitmapSetGet<uint64_t>(); }
TEST(WTF_Bitmap, TestAndSet_uint64_t) { testBitmapTestAndSet<uint64_t>(); }
TEST(WTF_Bitmap, TestAndClear_uint64_t) { testBitmapTestAndClear<uint64_t>(); }
TEST(WTF_Bitmap, ConcurrentTestAndSet_uint64_t) { testBitmapConcurrentTestAndSet<uint64_t>(); }
TEST(WTF_Bitmap, ConcurrentTestAndClear_uint64_t) { testBitmapConcurrentTestAndClear<uint64_t>(); }
TEST(WTF_Bitmap, Clear_uint64_t) { testBitmapClear<uint64_t>(); }
TEST(WTF_Bitmap, ClearAll_uint64_t) { testBitmapClearAll<uint64_t>(); }
TEST(WTF_Bitmap, Invert_uint64_t) { testBitmapInvert<uint64_t>(); }
TEST(WTF_Bitmap, FindRunOfZeros_uint64_t) { testBitmapFindRunOfZeros<uint64_t>(); }
TEST(WTF_Bitmap, Count_uint64_t) { testBitmapCount<uint64_t>(); }
TEST(WTF_Bitmap, IsEmpty_uint64_t) { testBitmapIsEmpty<uint64_t>(); }
TEST(WTF_Bitmap, IsFull_uint64_t) { testBitmapIsFull<uint64_t>(); }
TEST(WTF_Bitmap, Merge_uint64_t) { testBitmapMerge<uint64_t>(); }
TEST(WTF_Bitmap, Filter_uint64_t) { testBitmapFilter<uint64_t>(); }
TEST(WTF_Bitmap, Exclude_uint64_t) { testBitmapExclude<uint64_t>(); }
TEST(WTF_Bitmap, ConcurrentFilter_uint64_t) { testBitmapConcurrentFilter<uint64_t>(); }
TEST(WTF_Bitmap, Subsumes_uint64_t) { testBitmapSubsumes<uint64_t>(); }
TEST(WTF_Bitmap, ForEachSetBit_uint64_t) { testBitmapForEachSetBit<uint64_t>(); }
TEST(WTF_Bitmap, FindBit_uint64_t) { testBitmapFindBit<uint64_t>(); }
TEST(WTF_Bitmap, Iteration_uint64_t) { testBitmapIteration<uint64_t>(); }
TEST(WTF_Bitmap, MergeAndClear_uint64_t) { testBitmapMergeAndClear<uint64_t>(); }
TEST(WTF_Bitmap, SetAndClear_uint64_t) { testBitmapSetAndClear<uint64_t>(); }
TEST(WTF_Bitmap, OperatorEqualAccess_uint64_t) { testBitmapOperatorEqual<uint64_t>(); }
TEST(WTF_Bitmap, OperatorNotEqualAccess_uint64_t) { testBitmapOperatorNotEqual<uint64_t>(); }
TEST(WTF_Bitmap, Hash_uint64_t) { testBitmapHash<uint64_t>(); }

#endif // CPU(REGISTER64)

} // namespace TestWebKitAPI
