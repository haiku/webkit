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

#include "config.h"

#include <WebCore/PrivateClickMeasurement.h>
#include <wtf/URL.h>
#include <wtf/WallTime.h>

using namespace WebCore;

namespace TestWebKitAPI {

constexpr uint32_t min6BitValue { 0 };

const URL webKitURL { { }, "https://webkit.org"_s };
const URL exampleURL { { }, "https://example.com"_s };
const URL emptyURL { };

// Positive test cases.

TEST(PrivateClickMeasurement, ValidMinValues)
{
    PrivateClickMeasurement attribution { PrivateClickMeasurement::SourceID(min6BitValue), PrivateClickMeasurement::SourceSite { webKitURL }, PrivateClickMeasurement::AttributeOnSite { exampleURL } };
    attribution.attributeAndGetEarliestTimeToSend(PrivateClickMeasurement::AttributionTriggerData(min6BitValue, PrivateClickMeasurement::Priority(min6BitValue)));

    auto attributionURL = attribution.reportURL();
    
    ASSERT_EQ(attributionURL.string(), "https://webkit.org/.well-known/private-click-measurement/");

    ASSERT_EQ(attribution.json()->toJSONString(), "{\"source-engagement-type\":\"click\",\"source-site\":\"webkit.org\",\"source-id\":0,\"attributed-on-site\":\"example.com\",\"trigger-data\":0,\"report-version\":1}");
}

TEST(PrivateClickMeasurement, ValidMidValues)
{
    PrivateClickMeasurement attribution { PrivateClickMeasurement::SourceID((uint32_t)12), PrivateClickMeasurement::SourceSite { webKitURL }, PrivateClickMeasurement::AttributeOnSite { exampleURL } };
    attribution.attributeAndGetEarliestTimeToSend(PrivateClickMeasurement::AttributionTriggerData((uint32_t)44, PrivateClickMeasurement::Priority((uint32_t)22)));

    auto attributionURL = attribution.reportURL();
    
    ASSERT_EQ(attributionURL.string(), "https://webkit.org/.well-known/private-click-measurement/");

    ASSERT_EQ(attribution.json()->toJSONString(), "{\"source-engagement-type\":\"click\",\"source-site\":\"webkit.org\",\"source-id\":12,\"attributed-on-site\":\"example.com\",\"trigger-data\":44,\"report-version\":1}");
}

TEST(PrivateClickMeasurement, ValidMaxValues)
{
    PrivateClickMeasurement attribution { PrivateClickMeasurement::SourceID(PrivateClickMeasurement::MaxEntropy), PrivateClickMeasurement::SourceSite { webKitURL }, PrivateClickMeasurement::AttributeOnSite { exampleURL } };
    attribution.attributeAndGetEarliestTimeToSend(PrivateClickMeasurement::AttributionTriggerData(PrivateClickMeasurement::MaxEntropy, PrivateClickMeasurement::Priority(PrivateClickMeasurement::MaxEntropy)));

    auto attributionURL = attribution.reportURL();
    
    ASSERT_EQ(attributionURL.string(), "https://webkit.org/.well-known/private-click-measurement/");

    ASSERT_EQ(attribution.json()->toJSONString(), "{\"source-engagement-type\":\"click\",\"source-site\":\"webkit.org\",\"source-id\":63,\"attributed-on-site\":\"example.com\",\"trigger-data\":63,\"report-version\":1}");
}

TEST(PrivateClickMeasurement, EarliestTimeToSendAttributionMinimumDelay)
{
    PrivateClickMeasurement attribution { PrivateClickMeasurement::SourceID(PrivateClickMeasurement::MaxEntropy), PrivateClickMeasurement::SourceSite { webKitURL }, PrivateClickMeasurement::AttributeOnSite { exampleURL } };
    auto now = WallTime::now();
    attribution.attributeAndGetEarliestTimeToSend(PrivateClickMeasurement::AttributionTriggerData(PrivateClickMeasurement::MaxEntropy, PrivateClickMeasurement::Priority(PrivateClickMeasurement::MaxEntropy)));
    auto earliestTimeToSend = attribution.earliestTimeToSend();
    ASSERT_TRUE(earliestTimeToSend);
    ASSERT_TRUE(earliestTimeToSend.value().secondsSinceEpoch() - 24_h >= now.secondsSinceEpoch());
}

TEST(PrivateClickMeasurement, ValidConversionURLs)
{
    const URL conversionURLWithoutPriority { { }, "https://webkit.org/.well-known/private-click-measurement/22"_s };
    auto optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithoutPriority);
    ASSERT_TRUE(optionalConversion);
    ASSERT_EQ(optionalConversion->data, (uint32_t)22);

    const URL conversionURLWithoutPriorityMaxEntropy { { }, "https://webkit.org/.well-known/private-click-measurement/63"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithoutPriorityMaxEntropy);
    ASSERT_TRUE(optionalConversion);
    ASSERT_EQ(optionalConversion->data, (uint32_t)63);
    
    const URL conversionURLWithoutPriorityAndLeadingZero { { }, "https://webkit.org/.well-known/private-click-measurement/02"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithoutPriorityAndLeadingZero);
    ASSERT_TRUE(optionalConversion);
    ASSERT_EQ(optionalConversion->data, (uint32_t)2);

    const URL conversionURLWithPriority { { }, "https://webkit.org/.well-known/private-click-measurement/22/12"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithPriority);
    ASSERT_TRUE(optionalConversion);
    ASSERT_EQ(optionalConversion->data, (uint32_t)22);
    ASSERT_EQ(optionalConversion->priority, (uint32_t)12);

    const URL conversionURLWithPriorityMaxEntropy { { }, "https://webkit.org/.well-known/private-click-measurement/63/63"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithPriorityMaxEntropy);
    ASSERT_TRUE(optionalConversion);
    ASSERT_EQ(optionalConversion->data, (uint32_t)63);
    ASSERT_EQ(optionalConversion->priority, (uint32_t)63);
    
    const URL conversionURLWithPriorityAndLeadingZero { { }, "https://webkit.org/.well-known/private-click-measurement/22/02"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithPriorityAndLeadingZero);
    ASSERT_TRUE(optionalConversion);
    ASSERT_EQ(optionalConversion->data, (uint32_t)22);
    ASSERT_EQ(optionalConversion->priority, (uint32_t)2);
}

// Negative test cases.

TEST(PrivateClickMeasurement, InvalidCampaignId)
{
    PrivateClickMeasurement attribution { PrivateClickMeasurement::SourceID(PrivateClickMeasurement::MaxEntropy + 1), PrivateClickMeasurement::SourceSite { webKitURL }, PrivateClickMeasurement::AttributeOnSite { exampleURL } };
    attribution.attributeAndGetEarliestTimeToSend(PrivateClickMeasurement::AttributionTriggerData(PrivateClickMeasurement::MaxEntropy, PrivateClickMeasurement::Priority(PrivateClickMeasurement::MaxEntropy)));

    ASSERT_TRUE(attribution.reportURL().isEmpty());
}

TEST(PrivateClickMeasurement, InvalidSourceHost)
{
    PrivateClickMeasurement attribution { PrivateClickMeasurement::SourceID(PrivateClickMeasurement::MaxEntropy), PrivateClickMeasurement::SourceSite { emptyURL }, PrivateClickMeasurement::AttributeOnSite { exampleURL } };
    attribution.attributeAndGetEarliestTimeToSend(PrivateClickMeasurement::AttributionTriggerData(PrivateClickMeasurement::MaxEntropy, PrivateClickMeasurement::Priority(PrivateClickMeasurement::MaxEntropy)));

    ASSERT_TRUE(attribution.reportURL().isEmpty());
}

TEST(PrivateClickMeasurement, InvalidDestinationHost)
{
    PrivateClickMeasurement attribution { PrivateClickMeasurement::SourceID(PrivateClickMeasurement::MaxEntropy + 1), PrivateClickMeasurement::SourceSite { webKitURL }, PrivateClickMeasurement::AttributeOnSite { emptyURL } };
    attribution.attributeAndGetEarliestTimeToSend(PrivateClickMeasurement::AttributionTriggerData(PrivateClickMeasurement::MaxEntropy, PrivateClickMeasurement::Priority(PrivateClickMeasurement::MaxEntropy)));

    ASSERT_TRUE(attribution.reportURL().isEmpty());
}

TEST(PrivateClickMeasurement, InvalidConversionData)
{
    PrivateClickMeasurement attribution { PrivateClickMeasurement::SourceID(PrivateClickMeasurement::MaxEntropy), PrivateClickMeasurement::SourceSite { webKitURL }, PrivateClickMeasurement::AttributeOnSite { exampleURL } };
    attribution.attributeAndGetEarliestTimeToSend(PrivateClickMeasurement::AttributionTriggerData((PrivateClickMeasurement::MaxEntropy + 1), PrivateClickMeasurement::Priority(PrivateClickMeasurement::MaxEntropy)));

    ASSERT_TRUE(attribution.reportURL().isEmpty());
}

TEST(PrivateClickMeasurement, InvalidPriority)
{
    PrivateClickMeasurement attribution { PrivateClickMeasurement::SourceID(PrivateClickMeasurement::MaxEntropy), PrivateClickMeasurement::SourceSite { webKitURL }, PrivateClickMeasurement::AttributeOnSite { exampleURL } };
    attribution.attributeAndGetEarliestTimeToSend(PrivateClickMeasurement::AttributionTriggerData(PrivateClickMeasurement::MaxEntropy, PrivateClickMeasurement::Priority(PrivateClickMeasurement::MaxEntropy + 1)));

    ASSERT_TRUE(attribution.reportURL().isEmpty());
}

TEST(PrivateClickMeasurement, InvalidMissingConversion)
{
    PrivateClickMeasurement attribution { PrivateClickMeasurement::SourceID(PrivateClickMeasurement::MaxEntropy), PrivateClickMeasurement::SourceSite { webKitURL }, PrivateClickMeasurement::AttributeOnSite { exampleURL } };

    ASSERT_TRUE(attribution.reportURL().isEmpty());
    ASSERT_FALSE(attribution.earliestTimeToSend());
}

TEST(PrivateClickMeasurement, InvalidConversionURLs)
{
    const URL conversionURLWithSingleDigitConversionData { { }, "https://webkit.org/.well-known/private-click-measurement/2"_s };
    auto optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithSingleDigitConversionData);
    ASSERT_FALSE(optionalConversion);
    
    const URL conversionURLWithNonNumeralConversionData { { }, "https://webkit.org/.well-known/private-click-measurement/2s"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithNonNumeralConversionData);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithNegativeConversionData { { }, "https://webkit.org/.well-known/private-click-measurement/-2"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithNegativeConversionData);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithTooLargeConversionData { { }, "https://webkit.org/.well-known/private-click-measurement/64"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithTooLargeConversionData);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithSingleDigitPriority { { }, "https://webkit.org/.well-known/private-click-measurement/22/2"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithSingleDigitPriority);
    ASSERT_FALSE(optionalConversion);
    
    const URL conversionURLWithNonNumeralPriority { { }, "https://webkit.org/.well-known/private-click-measurement/22/2s"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithNonNumeralPriority);
    ASSERT_FALSE(optionalConversion);
    
    const URL conversionURLWithNegativePriority { { }, "https://webkit.org/.well-known/private-click-measurement/22/-2"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithNegativePriority);
    ASSERT_FALSE(optionalConversion);
    
    const URL conversionURLWithTooLargePriority { { }, "https://webkit.org/.well-known/private-click-measurement/22/64"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithTooLargePriority);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithTooLargeConversionDataAndPriority { { }, "https://webkit.org/.well-known/private-click-measurement/64/22"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithTooLargeConversionDataAndPriority);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithTooLargeConversionDataAndTooLargePriority { { }, "https://webkit.org/.well-known/private-click-measurement/64/64"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithTooLargeConversionDataAndTooLargePriority);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithExtraLeadingSlash = { { }, "https://webkit.org/.well-known/private-click-measurement//22/12"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithExtraLeadingSlash);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithExtraTrailingSlash = { { }, "https://webkit.org/.well-known/private-click-measurement/22/12/"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithExtraTrailingSlash);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithTrailingQuestionMark = { { }, "https://webkit.org/.well-known/private-click-measurement/22/12?"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithTrailingQuestionMark);
    ASSERT_FALSE(optionalConversion);
}

TEST(PrivateClickMeasurement, InvalidConversionWithDisallowedURLComponents)
{
    // Protocol.
    const URL conversionURLWithHttpProtocol { { }, "http://webkit.org/.well-known/private-click-measurement/2"_s };
    auto optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithHttpProtocol);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithWssProtocol { { }, "wss://webkit.org/.well-known/private-click-measurement/2"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithWssProtocol);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithFileProtocol { { }, "file:///.well-known/private-click-measurement/2"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithFileProtocol);
    ASSERT_FALSE(optionalConversion);

    // Username and password.
    const URL conversionURLWithUserName { { }, "https://user@webkit.org/.well-known/private-click-measurement/2"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithUserName);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithPassword = { { }, "https://:pwd@webkit.org/.well-known/private-click-measurement/22/12?"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithPassword);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithUsernameAndPassword = { { }, "https://user:pwd@webkit.org/.well-known/private-click-measurement/22/12?"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithUsernameAndPassword);
    ASSERT_FALSE(optionalConversion);

    // Query string.
    const URL conversionURLWithTrailingQuestionMark = { { }, "https://webkit.org/.well-known/private-click-measurement/22/12?"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithTrailingQuestionMark);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithQueryString = { { }, "https://webkit.org/.well-known/private-click-measurement/22/12?extra=data"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithQueryString);
    ASSERT_FALSE(optionalConversion);
    
    // Fragment.
    const URL conversionURLWithTrailingHash = { { }, "https://webkit.org/.well-known/private-click-measurement/22/12#"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithTrailingHash);
    ASSERT_FALSE(optionalConversion);

    const URL conversionURLWithFragment = { { }, "https://webkit.org/.well-known/private-click-measurement/22/12#fragment"_s };
    optionalConversion = PrivateClickMeasurement::parseAttributionRequest(conversionURLWithFragment);
    ASSERT_FALSE(optionalConversion);
}

} // namespace TestWebKitAPI
