/*
    Copyright (C) 2004, 2005 Nikolas Zimmermann <wildfox@kde.org>
                  2004, 2005, 2006 Rob Buis <buis@kde.org>

    This file is part of the KDE project

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

#ifndef SVGComponentTransferFunctionElement_h
#define SVGComponentTransferFunctionElement_h
#if ENABLE(SVG) && ENABLE(SVG_EXPERIMENTAL_FEATURES)

#include "SVGElement.h"
#include "SVGFEComponentTransfer.h"

namespace WebCore
{
    class SVGNumberList;

    class SVGComponentTransferFunctionElement : public SVGElement
    {
    public:
        SVGComponentTransferFunctionElement(const QualifiedName&, Document*);
        virtual ~SVGComponentTransferFunctionElement();

        // 'SVGComponentTransferFunctionElement' functions
        // Derived from: 'Element'
        virtual void parseMappedAttribute(MappedAttribute* attr);
        
        SVGComponentTransferFunction transferFunction() const;

    private:
        ANIMATED_PROPERTY_DECLARATIONS(SVGComponentTransferFunctionElement, int, int, Type, type)
        ANIMATED_PROPERTY_DECLARATIONS(SVGComponentTransferFunctionElement, SVGNumberList*, RefPtr<SVGNumberList>, TableValues, tableValues)
        ANIMATED_PROPERTY_DECLARATIONS(SVGComponentTransferFunctionElement, double, double, Slope, slope)
        ANIMATED_PROPERTY_DECLARATIONS(SVGComponentTransferFunctionElement, double, double, Intercept, intercept)
        ANIMATED_PROPERTY_DECLARATIONS(SVGComponentTransferFunctionElement, double, double, Amplitude, amplitude)
        ANIMATED_PROPERTY_DECLARATIONS(SVGComponentTransferFunctionElement, double, double, Exponent, exponent)
        ANIMATED_PROPERTY_DECLARATIONS(SVGComponentTransferFunctionElement, double, double, Offset, offset)
    };

} // namespace WebCore

#endif // ENABLE(SVG) && ENABLE(SVG_EXPERIMENTAL_FEATURES)
#endif

// vim:ts=4:noet
