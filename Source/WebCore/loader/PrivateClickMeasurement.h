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

#pragma once

#include "RegistrableDomain.h"
#include <wtf/CompletionHandler.h>
#include <wtf/Forward.h>
#include <wtf/JSONValues.h>
#include <wtf/Optional.h>
#include <wtf/URL.h>
#include <wtf/WallTime.h>
#include <wtf/text/StringHash.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class PrivateClickMeasurement {
public:
    using PriorityValue = uint32_t;

    static constexpr uint32_t MaxEntropy = 63;

    struct SourceID {
        SourceID() = default;
        explicit SourceID(uint32_t id)
            : id { id }
        {
        }
        
        bool isValid() const
        {
            return id <= MaxEntropy;
        }
        
        uint32_t id { 0 };
    };

    struct SourceSite {
        SourceSite() = default;
        explicit SourceSite(const URL& url)
            : registrableDomain { url }
        {
        }

        explicit SourceSite(const RegistrableDomain& domain)
            : registrableDomain { domain }
        {
        }

        explicit SourceSite(WTF::HashTableDeletedValueType)
            : registrableDomain(WTF::HashTableDeletedValue)
        {
        }

        bool operator==(const SourceSite& other) const
        {
            return registrableDomain == other.registrableDomain;
        }

        bool matches(const URL& url) const
        {
            return registrableDomain.matches(url);
        }

        bool isHashTableDeletedValue() const
        {
            return registrableDomain.isHashTableDeletedValue();
        }

        static SourceSite deletedValue()
        {
            return SourceSite { WTF::HashTableDeletedValue };
        }

        static void constructDeletedValue(SourceSite& sourceSite)
        {
            new (&sourceSite) SourceSite;
            sourceSite = SourceSite::deletedValue();
        }

        void deleteValue()
        {
            registrableDomain = RegistrableDomain { WTF::HashTableDeletedValue };
        }

        bool isDeletedValue() const
        {
            return isHashTableDeletedValue();
        }

        RegistrableDomain registrableDomain;
    };

    struct SourceSiteHash {
        static unsigned hash(const SourceSite& sourceSite)
        {
            return sourceSite.registrableDomain.hash();
        }
        
        static bool equal(const SourceSite& a, const SourceSite& b)
        {
            return a == b;
        }

        static const bool safeToCompareToEmptyOrDeleted = false;
    };

    struct AttributeOnSite {
        AttributeOnSite() = default;
        explicit AttributeOnSite(const URL& url)
            : registrableDomain { RegistrableDomain { url } }
        {
        }

        explicit AttributeOnSite(WTF::HashTableDeletedValueType)
            : registrableDomain { WTF::HashTableDeletedValue }
        {
        }

        explicit AttributeOnSite(RegistrableDomain&& domain)
            : registrableDomain { WTFMove(domain) }
        {
        }
        
        bool operator==(const AttributeOnSite& other) const
        {
            return registrableDomain == other.registrableDomain;
        }

        bool matches(const URL& url) const
        {
            return registrableDomain == RegistrableDomain { url };
        }
        
        bool isHashTableDeletedValue() const
        {
            return registrableDomain.isHashTableDeletedValue();
        }

        static AttributeOnSite deletedValue()
        {
            return AttributeOnSite { WTF::HashTableDeletedValue };
        }

        static void constructDeletedValue(AttributeOnSite& attributeOnSite)
        {
            new (&attributeOnSite) AttributeOnSite;
            attributeOnSite = AttributeOnSite::deletedValue();
        }

        void deleteValue()
        {
            registrableDomain = RegistrableDomain { WTF::HashTableDeletedValue };
        }

        bool isDeletedValue() const
        {
            return isHashTableDeletedValue();
        }

        RegistrableDomain registrableDomain;
    };

    struct AttributeOnSiteHash {
        static unsigned hash(const AttributeOnSite& attributeOnSite)
        {
            return attributeOnSite.registrableDomain.hash();
        }
        
        static bool equal(const AttributeOnSite& a, const AttributeOnSite& b)
        {
            return a == b;
        }

        static const bool safeToCompareToEmptyOrDeleted = false;
    };

    struct Priority {
        explicit Priority(PriorityValue value)
        : value { value }
        {
        }
        
        PriorityValue value;
    };
    
    struct AttributionTriggerData {
        enum class WasSent : bool { No, Yes };
        
        AttributionTriggerData(uint32_t data, Priority priority, WasSent wasSent = WasSent::No)
            : data { data }
            , priority { priority.value }
            , wasSent { wasSent }
        {
        }

        bool isValid() const
        {
            return data <= MaxEntropy && priority <= MaxEntropy;
        }
        
        uint32_t data;
        PriorityValue priority;
        WasSent wasSent = WasSent::No;

        template<class Encoder> void encode(Encoder&) const;
        template<class Decoder> static Optional<AttributionTriggerData> decode(Decoder&);
    };

    PrivateClickMeasurement() = default;
    PrivateClickMeasurement(SourceID sourceID, const SourceSite& sourceSite, const AttributeOnSite& attributeOnSite, WallTime timeOfAdClick = WallTime::now())
        : m_sourceID { sourceID }
        , m_sourceSite { sourceSite }
        , m_attributeOnSite { attributeOnSite }
        , m_timeOfAdClick { timeOfAdClick }
    {
    }

    WEBCORE_EXPORT static const Seconds maxAge();
    WEBCORE_EXPORT static Expected<AttributionTriggerData, String> parseAttributionRequest(const URL& redirectURL);
    WEBCORE_EXPORT Optional<Seconds> attributeAndGetEarliestTimeToSend(AttributionTriggerData&&);
    WEBCORE_EXPORT bool hasHigherPriorityThan(const PrivateClickMeasurement&) const;
    WEBCORE_EXPORT URL reportURL() const;
    WEBCORE_EXPORT Ref<JSON::Object> json() const;
    const SourceSite& sourceSite() const { return m_sourceSite; };
    const AttributeOnSite& attributeOnSite() const { return m_attributeOnSite; };
    WallTime timeOfAdClick() const { return m_timeOfAdClick; }
    Optional<WallTime> earliestTimeToSend() const { return m_earliestTimeToSend; };
    void setEarliestTimeToSend(WallTime time) { m_earliestTimeToSend = time; }
    SourceID sourceID() { return m_sourceID; }
    Optional<AttributionTriggerData> attributionTriggerData() { return m_attributionTriggerData; }
    void setAttribution(AttributionTriggerData&& attributionTriggerData) { m_attributionTriggerData = WTFMove(attributionTriggerData); }

    template<class Encoder> void encode(Encoder&) const;
    template<class Decoder> static Optional<PrivateClickMeasurement> decode(Decoder&);

private:
    bool isValid() const;
    static bool debugModeEnabled();

    SourceID m_sourceID;
    SourceSite m_sourceSite;
    AttributeOnSite m_attributeOnSite;
    WallTime m_timeOfAdClick;

    Optional<AttributionTriggerData> m_attributionTriggerData;
    Optional<WallTime> m_earliestTimeToSend;
};

template<class Encoder>
void PrivateClickMeasurement::encode(Encoder& encoder) const
{
    encoder << m_sourceID.id << m_sourceSite.registrableDomain << m_attributeOnSite.registrableDomain << m_timeOfAdClick << m_attributionTriggerData << m_earliestTimeToSend;
}

template<class Decoder>
Optional<PrivateClickMeasurement> PrivateClickMeasurement::decode(Decoder& decoder)
{
    Optional<uint32_t> sourceID;
    decoder >> sourceID;
    if (!sourceID)
        return WTF::nullopt;
    
    Optional<RegistrableDomain> sourceRegistrableDomain;
    decoder >> sourceRegistrableDomain;
    if (!sourceRegistrableDomain)
        return WTF::nullopt;
    
    Optional<RegistrableDomain> attributeOnRegistrableDomain;
    decoder >> attributeOnRegistrableDomain;
    if (!attributeOnRegistrableDomain)
        return WTF::nullopt;
    
    Optional<WallTime> timeOfAdClick;
    decoder >> timeOfAdClick;
    if (!timeOfAdClick)
        return WTF::nullopt;
    
    Optional<Optional<AttributionTriggerData>> attributionTriggerData;
    decoder >> attributionTriggerData;
    if (!attributionTriggerData)
        return WTF::nullopt;
    
    Optional<Optional<WallTime>> earliestTimeToSend;
    decoder >> earliestTimeToSend;
    if (!earliestTimeToSend)
        return WTF::nullopt;
    
    PrivateClickMeasurement attribution { SourceID { WTFMove(*sourceID) }, SourceSite { WTFMove(*sourceRegistrableDomain) }, AttributeOnSite { WTFMove(*attributeOnRegistrableDomain) } };
    attribution.m_attributionTriggerData = WTFMove(*attributionTriggerData);
    attribution.m_earliestTimeToSend = WTFMove(*earliestTimeToSend);
    
    return attribution;
}

template<class Encoder>
void PrivateClickMeasurement::AttributionTriggerData::encode(Encoder& encoder) const
{
    encoder << data << priority << wasSent;
}

template<class Decoder>
Optional<PrivateClickMeasurement::AttributionTriggerData> PrivateClickMeasurement::AttributionTriggerData::decode(Decoder& decoder)
{
    Optional<uint32_t> data;
    decoder >> data;
    if (!data)
        return WTF::nullopt;
    
    Optional<PriorityValue> priority;
    decoder >> priority;
    if (!priority)
        return WTF::nullopt;
    
    Optional<WasSent> wasSent;
    decoder >> wasSent;
    if (!wasSent)
        return WTF::nullopt;
    
    return AttributionTriggerData { WTFMove(*data), Priority { *priority }, *wasSent };
}

} // namespace WebCore

namespace WTF {
template<typename T> struct DefaultHash;

template<> struct DefaultHash<WebCore::PrivateClickMeasurement::SourceSite> : WebCore::PrivateClickMeasurement::SourceSiteHash { };
template<> struct HashTraits<WebCore::PrivateClickMeasurement::SourceSite> : GenericHashTraits<WebCore::PrivateClickMeasurement::SourceSite> {
    static WebCore::PrivateClickMeasurement::SourceSite emptyValue() { return { }; }
    static void constructDeletedValue(WebCore::PrivateClickMeasurement::SourceSite& slot) { WebCore::PrivateClickMeasurement::SourceSite::constructDeletedValue(slot); }
    static bool isDeletedValue(const WebCore::PrivateClickMeasurement::SourceSite& slot) { return slot.isDeletedValue(); }
};

template<> struct DefaultHash<WebCore::PrivateClickMeasurement::AttributeOnSite> : WebCore::PrivateClickMeasurement::AttributeOnSiteHash { };
template<> struct HashTraits<WebCore::PrivateClickMeasurement::AttributeOnSite> : GenericHashTraits<WebCore::PrivateClickMeasurement::AttributeOnSite> {
    static WebCore::PrivateClickMeasurement::AttributeOnSite emptyValue() { return { }; }
    static void constructDeletedValue(WebCore::PrivateClickMeasurement::AttributeOnSite& slot) { WebCore::PrivateClickMeasurement::AttributeOnSite::constructDeletedValue(slot); }
    static bool isDeletedValue(const WebCore::PrivateClickMeasurement::AttributeOnSite& slot) { return slot.isDeletedValue(); }
};
}
