/*
    Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>

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

#ifndef SVGRenderingIntent_h
#define SVGRenderingIntent_h

#if ENABLE(SVG)

#include "Shared.h"

namespace WebCore {

class SVGRenderingIntent : public Shared<SVGRenderingIntent>
{
public:
    enum SVGRenderingIntentType {
        RENDERING_INTENT_UNKNOWN                  = 0,
        RENDERING_INTENT_AUTO                     = 1,
        RENDERING_INTENT_PERCEPTUAL               = 2,
        RENDERING_INTENT_RELATIVE_COLORIMETRIC    = 3,
        RENDERING_INTENT_SATURATION               = 4,
        RENDERING_INTENT_ABSOLUTE_COLORIMETRIC    = 5
    };

    SVGRenderingIntent() { } 
    ~SVGRenderingIntent() { }
};

} // namespace WebCore

#endif // ENABLE(SVG)
#endif // SVGRenderingIntent_h

// vim:ts=4:noet
