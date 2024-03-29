/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGAnimatedTransformListPropertyTearOff_h
#define SVGAnimatedTransformListPropertyTearOff_h

#include "core/svg/SVGTransformList.h"
#include "core/svg/properties/SVGAnimatedListPropertyTearOff.h"
#include "core/svg/properties/SVGTransformListPropertyTearOff.h"

namespace WebCore {

class SVGAnimatedTransformListPropertyTearOff : public SVGAnimatedListPropertyTearOff<SVGTransformList> {
public:
    virtual SVGListPropertyTearOff<SVGTransformList>* baseVal()
    {
        if (!m_baseVal)
            m_baseVal = SVGTransformListPropertyTearOff::create(this, BaseValRole, m_values, m_wrappers);
        return static_cast<SVGListPropertyTearOff<SVGTransformList>*>(m_baseVal.get());
    }

    virtual SVGListPropertyTearOff<SVGTransformList>* animVal()
    {
        if (!m_animVal)
            m_animVal = SVGTransformListPropertyTearOff::create(this, AnimValRole, m_values, m_wrappers);
        return static_cast<SVGListPropertyTearOff<SVGTransformList>*>(m_animVal.get());
    }

    static PassRefPtr<SVGAnimatedTransformListPropertyTearOff> create(SVGElement* contextElement, const QualifiedName& attributeName, AnimatedPropertyType animatedPropertyType, SVGTransformList& values)
    {
        ASSERT(contextElement);
        return adoptRef(new SVGAnimatedTransformListPropertyTearOff(contextElement, attributeName, animatedPropertyType, values));
    }

private:
    SVGAnimatedTransformListPropertyTearOff(SVGElement* contextElement, const QualifiedName& attributeName, AnimatedPropertyType animatedPropertyType, SVGTransformList& values)
        : SVGAnimatedListPropertyTearOff<SVGTransformList>(contextElement, attributeName, animatedPropertyType, values)
    {
    }
};

}

#endif // SVGAnimatedTransformListPropertyTearOff_h
