/*
    Copyright (C) 2006 Apple Computer, Inc.

    This file is part of the WebKit project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef SVGForeignObjectElement_h
#define SVGForeignObjectElement_h

#if ENABLE(SVG) && ENABLE(SVG_EXPERIMENTAL_FEATURES)

#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGURIReference.h"
#include "SVGStyledTransformableElement.h"
#include "SVGExternalResourcesRequired.h"

namespace WebCore
{
    class SVGLength;
    class SVGDocument;

    class SVGForeignObjectElement : public SVGStyledTransformableElement,
                                public SVGTests,
                                public SVGLangSpace,
                                public SVGExternalResourcesRequired,
                                public SVGURIReference
    {
    public:
        SVGForeignObjectElement(const QualifiedName&, Document*);
        virtual ~SVGForeignObjectElement();
        
        virtual bool isValid() const { return SVGTests::isValid(); }
        virtual void parseMappedAttribute(MappedAttribute*);

        bool childShouldCreateRenderer(Node*) const;
        virtual RenderObject* createRenderer(RenderArena* arena, RenderStyle* style);

    protected:
        virtual const SVGElement* contextElement() const { return this; }

    private:
        ANIMATED_PROPERTY_FORWARD_DECLARATIONS(SVGExternalResourcesRequired, bool, ExternalResourcesRequired, externalResourcesRequired) 
        ANIMATED_PROPERTY_FORWARD_DECLARATIONS(SVGURIReference, String, Href, href)

        ANIMATED_PROPERTY_DECLARATIONS(SVGForeignObjectElement, SVGLength, SVGLength, X, x)
        ANIMATED_PROPERTY_DECLARATIONS(SVGForeignObjectElement, SVGLength, SVGLength, Y, y)
        ANIMATED_PROPERTY_DECLARATIONS(SVGForeignObjectElement, SVGLength, SVGLength, Width, width)
        ANIMATED_PROPERTY_DECLARATIONS(SVGForeignObjectElement, SVGLength, SVGLength, Height, height)
    };

} // namespace WebCore

#endif // ENABLE(SVG)
#endif

// vim:ts=4:noet
