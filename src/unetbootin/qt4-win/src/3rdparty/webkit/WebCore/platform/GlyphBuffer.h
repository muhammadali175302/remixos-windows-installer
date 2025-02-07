/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GlyphBuffer_h
#define GlyphBuffer_h

#include "FloatSize.h"

#if PLATFORM(CG)
#include <ApplicationServices/ApplicationServices.h>
#elif PLATFORM(CAIRO)
#include <cairo.h>
#endif

#include <wtf/Vector.h>

namespace WebCore {

typedef unsigned short Glyph;
class FontData;

#if PLATFORM(CG)
typedef Glyph GlyphBufferGlyph;
typedef CGSize GlyphBufferAdvance;
#elif PLATFORM(CAIRO)
typedef cairo_glyph_t GlyphBufferGlyph;
typedef FloatSize GlyphBufferAdvance;
#elif PLATFORM(QT)
typedef unsigned short GlyphBufferGlyph;
typedef FloatSize GlyphBufferAdvance;
#endif

class GlyphBuffer {
public:
    bool isEmpty() const { return m_fontData.isEmpty(); }
    int size() const { return m_fontData.size(); }
    
    void clear()
    {
        m_fontData.clear();
        m_glyphs.clear();
        m_advances.clear();
#if PLATFORM(WIN)
        m_offsets.clear();
#endif
    }

    GlyphBufferGlyph* glyphs(int from) { return m_glyphs.data() + from; }
    GlyphBufferAdvance* advances(int from) { return m_advances.data() + from; }
    const GlyphBufferGlyph* glyphs(int from) const { return m_glyphs.data() + from; }
    const GlyphBufferAdvance* advances(int from) const { return m_advances.data() + from; }

    const FontData* fontDataAt(int index) const { return m_fontData[index]; }
    
    void swap(int index1, int index2)
    {
        const FontData* f = m_fontData[index1];
        m_fontData[index1] = m_fontData[index2];
        m_fontData[index2] = f;

        GlyphBufferGlyph g = m_glyphs[index1];
        m_glyphs[index1] = m_glyphs[index2];
        m_glyphs[index2] = g;

        GlyphBufferAdvance s = m_advances[index1];
        m_advances[index1] = m_advances[index2];
        m_advances[index2] = s;

#if PLATFORM(WIN)
        FloatSize offset = m_offsets[index1];
        m_offsets[index1] = m_offsets[index2];
        m_offsets[index2] = offset;
#endif
    }

    Glyph glyphAt(int index) const
    {
#if PLATFORM(CG) || PLATFORM(QT)
        return m_glyphs[index];
#elif PLATFORM(CAIRO)
        return m_glyphs[index].index;
#endif
    }

    float advanceAt(int index) const
    {
#if PLATFORM(CG)
        return m_advances[index].width;
#elif PLATFORM(CAIRO) || PLATFORM(QT)
        return m_advances[index].width();
#endif
    }

    FloatSize offsetAt(int index) const
    {
#if PLATFORM(WIN)
        return m_offsets[index];
#else
        return FloatSize();
#endif
    }

    void add(Glyph glyph, const FontData* font, float width, const FloatSize* offset = 0)
    {
        m_fontData.append(font);
#if PLATFORM(CG)
        m_glyphs.append(glyph);
        CGSize advance;
        advance.width = width;
        advance.height = 0;
        m_advances.append(advance);
#elif PLATFORM(CAIRO)
        cairo_glyph_t cairoGlyph;
        cairoGlyph.index = glyph;
        m_glyphs.append(cairoGlyph);
        m_advances.append(FloatSize(width, 0));
#elif PLATFORM(QT)
        m_glyphs.append(glyph);
        m_advances.append(FloatSize(width, 0));
#endif
#if PLATFORM(WIN)
        if (offset)
            m_offsets.append(*offset);
        else
            m_offsets.append(FloatSize());
#endif
    }
    
private:
    Vector<const FontData*, 2048> m_fontData;
    Vector<GlyphBufferGlyph, 2048> m_glyphs;
    Vector<GlyphBufferAdvance, 2048> m_advances;
#if PLATFORM(WIN)
    Vector<FloatSize, 2048> m_offsets;
#endif
};

}
#endif
