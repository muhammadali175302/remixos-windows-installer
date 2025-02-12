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

#ifndef SVGStylable_h
#define SVGStylable_h
#if ENABLE(SVG)

namespace WebCore {

    class CSSValue;
    class CSSStyleDeclaration;
    class String;
    class StringImpl;

    class SVGStylable {
    public:
        SVGStylable();
        virtual ~SVGStylable();

        // 'SVGStylable' functions
        virtual CSSStyleDeclaration* style() = 0;
        virtual CSSValue* getPresentationAttribute(StringImpl* name) = 0;
    };

} // namespace WebCore

#endif // ENABLE(SVG)
#endif // SVGStylable_h

// vim:ts=4:noet
