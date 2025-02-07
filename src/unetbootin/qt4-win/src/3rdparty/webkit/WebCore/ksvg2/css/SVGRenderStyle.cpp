/*
    Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
                  2004, 2005 Rob Buis <buis@kde.org>

    Based on khtml code by:
    Copyright (C) 1999 Antti Koivisto (koivisto@kde.org)
    Copyright (C) 1999-2003 Lars Knoll (knoll@kde.org)
    Copyright (C) 2002-2003 Dirk Mueller (mueller@kde.org)
    Copyright (C) 2002 Apple Computer, Inc.

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
#include "SVGRenderStyle.h"

namespace WebCore {

SVGRenderStyle *SVGRenderStyle::s_defaultStyle = 0;

SVGRenderStyle::SVGRenderStyle()
{
    if (!s_defaultStyle)    
        s_defaultStyle = new SVGRenderStyle(true);

    fill = s_defaultStyle->fill;
    stroke = s_defaultStyle->stroke;
    text = s_defaultStyle->text;
    stops = s_defaultStyle->stops;
    clip = s_defaultStyle->clip;
    mask = s_defaultStyle->mask;
    misc = s_defaultStyle->misc;
    markers = s_defaultStyle->markers;

    setBitDefaults();
}

SVGRenderStyle::SVGRenderStyle(bool)
{
    setBitDefaults();

    fill.init();
    stroke.init();
    text.init();
    stops.init();
    clip.init();
    mask.init();
    misc.init();
    markers.init();
}

SVGRenderStyle::SVGRenderStyle(const SVGRenderStyle &other) : Shared<SVGRenderStyle>()
{
    fill = other.fill;
    stroke = other.stroke;
    text = other.text;
    stops = other.stops;
    clip = other.clip;
    mask = other.mask;
    misc = other.misc;
    markers = other.markers;

    svg_inherited_flags = other.svg_inherited_flags;
    svg_noninherited_flags = other.svg_noninherited_flags;
}

SVGRenderStyle::~SVGRenderStyle()
{
}

bool SVGRenderStyle::operator==(const SVGRenderStyle& o) const
{
    return (fill == o.fill && stroke == o.stroke && text == o.text &&
        stops == o.stops && clip == o.clip && mask == o.mask &&
        misc == o.misc && markers == o.markers &&
        svg_inherited_flags == o.svg_inherited_flags &&
        svg_noninherited_flags == o.svg_noninherited_flags);
}

bool SVGRenderStyle::inheritedNotEqual(const SVGRenderStyle* other) const
{
    return (fill != other->fill
            || stroke != other->stroke
            || markers != other->markers
            || text != other->text
            || svg_inherited_flags != other->svg_inherited_flags);
}

void SVGRenderStyle::inheritFrom(const SVGRenderStyle* svgInheritParent)
{
    if (!svgInheritParent)
        return;

    fill = svgInheritParent->fill;
    stroke = svgInheritParent->stroke;
    markers = svgInheritParent->markers;
    text = svgInheritParent->text;

    svg_inherited_flags = svgInheritParent->svg_inherited_flags;
}

}

#endif // ENABLE(SVG)

// vim:ts=4:noet
