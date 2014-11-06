/*
 * Copyright (c) 2009, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef RenderSVGModelObject_h
#define RenderSVGModelObject_h

#include "core/rendering/RenderObject.h"
#include "core/rendering/svg/SVGRenderSupport.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGRect.h"

namespace WebCore {

// Most renderers in the SVG rendering tree will inherit from this class
// but not all. (e.g. RenderSVGForeignObject, RenderSVGBlock) thus methods
// required by SVG renders need to be declared on RenderObject, but shared
// logic can go in this class or in SVGRenderSupport.

class SVGElement;

class RenderSVGModelObject : public RenderObject {
public:
    explicit RenderSVGModelObject(SVGElement*);

    virtual LayoutRect clippedOverflowRectForRepaint(const RenderLayerModelObject* repaintContainer) const OVERRIDE;
    virtual void computeFloatRectForRepaint(const RenderLayerModelObject* repaintContainer, FloatRect&, bool fixed = false) const OVERRIDE FINAL;
    virtual LayoutRect outlineBoundsForRepaint(const RenderLayerModelObject* repaintContainer, const RenderGeometryMap*) const OVERRIDE FINAL;

    virtual void absoluteRects(Vector<IntRect>&, const LayoutPoint& accumulatedOffset) const OVERRIDE FINAL;
    virtual void absoluteQuads(Vector<FloatQuad>&, bool* wasFixed) const;

    virtual void mapLocalToContainer(const RenderLayerModelObject* repaintContainer, TransformState&, MapCoordinatesFlags = ApplyContainerFlip, bool* wasFixed = 0) const OVERRIDE FINAL;
    virtual const RenderObject* pushMappingToContainer(const RenderLayerModelObject* ancestorToStopAt, RenderGeometryMap&) const OVERRIDE FINAL;
    virtual void styleDidChange(StyleDifference, const RenderStyle* oldStyle);

    static bool checkIntersection(RenderObject*, const SVGRect&);
    static bool checkEnclosure(RenderObject*, const SVGRect&);

    virtual void computeLayerHitTestRects(LayerHitTestRects&) const OVERRIDE;

    SVGElement* element() const { return toSVGElement(RenderObject::node()); }

protected:
    virtual void addLayerHitTestRects(LayerHitTestRects&, const RenderLayer* currentCompositedLayer, const LayoutPoint& layerOffset, const LayoutRect& containerRect) const OVERRIDE;
    virtual void willBeDestroyed();

private:
    // RenderSVGModelObject subclasses should use element() instead.
    void node() const WTF_DELETED_FUNCTION;

    // This method should never be called, SVG uses a different nodeAtPoint method
    bool nodeAtPoint(const HitTestRequest&, HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) OVERRIDE;
    virtual void absoluteFocusRingQuads(Vector<FloatQuad>&) OVERRIDE FINAL;
};

}

#endif
