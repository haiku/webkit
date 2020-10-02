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
#include "SimpleRange.h"

#include "CharacterData.h"
#include "NodeTraversal.h"
#include "ShadowRoot.h"

namespace WebCore {

SimpleRange::SimpleRange(const BoundaryPoint& start, const BoundaryPoint& end)
    : start(start)
    , end(end)
{
}

SimpleRange::SimpleRange(BoundaryPoint&& start, BoundaryPoint&& end)
    : start(WTFMove(start))
    , end(WTFMove(end))
{
}

bool operator==(const SimpleRange& a, const SimpleRange& b)
{
    return a.start == b.start && a.end == b.end;
}

// FIXME: Create BoundaryPoint.cpp and move this there.
Optional<BoundaryPoint> makeBoundaryPointBeforeNode(Node& node)
{
    auto parent = node.parentNode();
    if (!parent)
        return WTF::nullopt;
    return BoundaryPoint { *parent, node.computeNodeIndex() };
}

// FIXME: Create BoundaryPoint.cpp and move this there.
Optional<BoundaryPoint> makeBoundaryPointAfterNode(Node& node)
{
    auto parent = node.parentNode();
    if (!parent)
        return WTF::nullopt;
    return BoundaryPoint { *parent, node.computeNodeIndex() + 1 };
}

// FIXME: Create BoundaryPoint.cpp and move this there.
static bool isOffsetBeforeChild(ContainerNode& container, unsigned offset, Node& child)
{
    if (!offset)
        return true;
    // If the container is not the parent, the child is part of a shadow tree, which we sort between offset 0 and offset 1.
    if (child.parentNode() != &container)
        return false;
    unsigned currentOffset = 0;
    for (auto currentChild = container.firstChild(); currentChild && currentChild != &child; currentChild = currentChild->nextSibling()) {
        if (offset <= ++currentOffset)
            return true;
    }
    return false;
}

// FIXME: Create BoundaryPoint.cpp and move this there.
// FIXME: Once we move to C++20, replace with the C++20 <=> operator.
// FIXME: This could return std::strong_ordering if we had that, or the equivalent.
static PartialOrdering order(unsigned a, unsigned b)
{
    if (a < b)
        return PartialOrdering::less;
    if (a > b)
        return PartialOrdering::greater;
    return PartialOrdering::equivalent;
}

// FIXME: Create BoundaryPoint.cpp and move this there.
PartialOrdering documentOrder(const BoundaryPoint& a, const BoundaryPoint& b)
{
    if (a.container.ptr() == b.container.ptr())
        return order(a.offset, b.offset);

    for (auto ancestor = b.container.ptr(); ancestor; ) {
        auto nextAncestor = ancestor->parentInComposedTree();
        if (nextAncestor == a.container.ptr())
            return isOffsetBeforeChild(*nextAncestor, a.offset, *ancestor) ? PartialOrdering::less : PartialOrdering::greater;
        ancestor = nextAncestor;
    }

    for (auto ancestor = a.container.ptr(); ancestor; ) {
        auto nextAncestor = ancestor->parentInComposedTree();
        if (nextAncestor == b.container.ptr())
            return isOffsetBeforeChild(*nextAncestor, b.offset, *ancestor) ? PartialOrdering::greater : PartialOrdering::less;
        ancestor = nextAncestor;
    }

    return documentOrder(a.container, b.container);
}

Optional<SimpleRange> makeRangeSelectingNode(Node& node)
{
    auto parent = node.parentNode();
    if (!parent)
        return WTF::nullopt;
    unsigned offset = node.computeNodeIndex();
    return SimpleRange { { *parent, offset }, { *parent, offset + 1 } };
}

SimpleRange makeRangeSelectingNodeContents(Node& node)
{
    return { makeBoundaryPointBeforeNodeContents(node), makeBoundaryPointAfterNodeContents(node) };
}

OffsetRange characterDataOffsetRange(const SimpleRange& range, const Node& node)
{
    return { &node == range.start.container.ptr() ? range.start.offset : 0,
        &node == range.end.container.ptr() ? range.end.offset : std::numeric_limits<unsigned>::max() };
}

static RefPtr<Node> firstIntersectingNode(const SimpleRange& range)
{
    if (range.start.container->isCharacterDataNode())
        return range.start.container.ptr();
    if (auto child = range.start.container->traverseToChildAt(range.start.offset))
        return child;
    return NodeTraversal::nextSkippingChildren(range.start.container);
}

static RefPtr<Node> nodePastLastIntersectingNode(const SimpleRange& range)
{
    if (range.end.container->isCharacterDataNode())
        return NodeTraversal::nextSkippingChildren(range.end.container);
    if (auto child = range.end.container->traverseToChildAt(range.end.offset))
        return child;
    return NodeTraversal::nextSkippingChildren(range.end.container);
}

static RefPtr<Node> firstIntersectingNodeWithDeprecatedZeroOffsetStartQuirk(const SimpleRange& range)
{
    if (range.start.container->isCharacterDataNode())
        return range.start.container.ptr();
    if (auto child = range.start.container->traverseToChildAt(range.start.offset))
        return child;
    if (!range.start.offset)
        return range.start.container.ptr();
    return NodeTraversal::nextSkippingChildren(range.start.container);
}

IntersectingNodeIterator::IntersectingNodeIterator(const SimpleRange& range)
    : m_node(firstIntersectingNode(range))
    , m_pastLastNode(nodePastLastIntersectingNode(range))
{
    enforceEndInvariant();
}

IntersectingNodeIterator::IntersectingNodeIterator(const SimpleRange& range, QuirkFlag)
    : m_node(firstIntersectingNodeWithDeprecatedZeroOffsetStartQuirk(range))
    , m_pastLastNode(nodePastLastIntersectingNode(range))
{
    enforceEndInvariant();
}

void IntersectingNodeIterator::advance()
{
    ASSERT(m_node);
    m_node = NodeTraversal::next(*m_node);
    enforceEndInvariant();
}

void IntersectingNodeIterator::advanceSkippingChildren()
{
    ASSERT(m_node);
    m_node = m_node->contains(m_pastLastNode.get()) ? nullptr : NodeTraversal::nextSkippingChildren(*m_node);
    enforceEndInvariant();
}

void IntersectingNodeIterator::enforceEndInvariant()
{
    if (m_node == m_pastLastNode || !m_node) {
        m_node = nullptr;
        m_pastLastNode = nullptr;
    }
}

RefPtr<Node> commonInclusiveAncestor(const SimpleRange& range)
{
    return commonInclusiveAncestor(range.start.container, range.end.container);
}

bool isPointInRange(const SimpleRange& range, const BoundaryPoint& point)
{
    return is_lteq(documentOrder(range.start, point)) && is_lteq(documentOrder(point, range.end));
}

bool isPointInRange(const SimpleRange& range, const Optional<BoundaryPoint>& point)
{
    return point && isPointInRange(range, *point);
}

PartialOrdering documentOrder(const SimpleRange& range, const BoundaryPoint& point)
{
    if (auto order = documentOrder(range.start, point); !is_lt(order))
        return order;
    if (auto order = documentOrder(range.end, point); !is_gt(order))
        return order;
    return PartialOrdering::equivalent;
}

PartialOrdering documentOrder(const BoundaryPoint& point, const SimpleRange& range)
{
    if (auto order = documentOrder(point, range.start); !is_gt(order))
        return order;
    if (auto order = documentOrder(point, range.end); !is_lt(order))
        return order;
    return PartialOrdering::equivalent;
}

bool contains(const SimpleRange& outerRange, const SimpleRange& innerRange)
{
    return is_lteq(documentOrder(outerRange.start, innerRange.start)) && is_gteq(documentOrder(outerRange.end, innerRange.end));
}

bool intersects(const SimpleRange& a, const SimpleRange& b)
{
    return is_lteq(documentOrder(a.start, b.end)) && is_lteq(documentOrder(b.start, a.end));
}

static bool compareByDocumentOrder(const BoundaryPoint& a, const BoundaryPoint& b)
{
    return is_lt(documentOrder(a, b));
}

SimpleRange unionRange(const SimpleRange& a, const SimpleRange& b)
{
    return { std::min(a.start, b.start, compareByDocumentOrder), std::max(a.end, b.end, compareByDocumentOrder) };
}

Optional<SimpleRange> intersection(const Optional<SimpleRange>& a, const Optional<SimpleRange>& b)
{
    // FIXME: Can this be done with fewer calls to documentOrder?
    if (!a || !b || !intersects(*a, *b))
        return WTF::nullopt;
    return { { std::max(a->start, b->start, compareByDocumentOrder), std::min(a->end, b->end, compareByDocumentOrder) } };
}

bool contains(const SimpleRange& range, const Node& node)
{
    // FIXME: Consider a more efficient algorithm that avoids always computing the node index.
    // FIXME: Does this const_cast point to a design problem?
    auto nodeRange = makeRangeSelectingNode(const_cast<Node&>(node));
    return nodeRange && contains(range, *nodeRange);
}

bool intersects(const SimpleRange& range, const Node& node)
{
    // FIXME: Consider a more efficient algorithm that avoids always computing the node index.
    // FIXME: Does this const_cast point to a design problem?
    auto nodeRange = makeRangeSelectingNode(const_cast<Node&>(node));
    if (!nodeRange)
        return node.contains(range.start.container.ptr());
    return is_lt(documentOrder(nodeRange->start, range.end)) && is_lt(documentOrder(range.start, nodeRange->end));

}

}
