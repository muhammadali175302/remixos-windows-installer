/*
    Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
                  2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>

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

#include "config.h"

#if ENABLE(SVG)
#include "SVGStopElement.h"

#include "Document.h"
#include "RenderSVGGradientStop.h"
#include "SVGNames.h"

namespace WebCore {

SVGStopElement::SVGStopElement(const QualifiedName& tagName, Document* doc)
    : SVGStyledElement(tagName, doc)
    , m_offset(0.0f)
{
}

SVGStopElement::~SVGStopElement()
{
}

ANIMATED_PROPERTY_DEFINITIONS(SVGStopElement, float, Number, number, Offset, offset, SVGNames::offsetAttr.localName(), m_offset)

void SVGStopElement::parseMappedAttribute(MappedAttribute* attr)
{
    if (attr->name() == SVGNames::offsetAttr) {
        const String& value = attr->value();
        if (value.endsWith("%"))
            setOffsetBaseValue(value.left(value.length() - 1).toFloat() / 100.0f);
        else
            setOffsetBaseValue(value.toFloat());
    } else
        SVGStyledElement::parseMappedAttribute(attr);
}

RenderObject* SVGStopElement::createRenderer(RenderArena* arena, RenderStyle* style)
{
    return new (arena) RenderSVGGradientStop(this);
}

void SVGStopElement::notifyAttributeChange() const
{
    if (!attached() || ownerDocument()->parsing())
        return;

    const_cast<SVGStopElement*>(this)->recalcStyle(Force);
    SVGStyledElement::notifyAttributeChange();
}

}

#endif // ENABLE(SVG)

// vim:ts=4:noet
