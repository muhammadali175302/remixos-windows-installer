/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <private/qdrawhelper_p.h>

#ifdef QT_HAVE_SSE

#include <private/qdrawhelper_sse_p.h>

QT_BEGIN_NAMESPACE

CompositionFunctionSolid qt_functionForModeSolid_SSE[numCompositionFunctions] = {
    comp_func_solid_SourceOver<QSSEIntrinsics>,
    comp_func_solid_DestinationOver<QSSEIntrinsics>,
    comp_func_solid_Clear<QSSEIntrinsics>,
    comp_func_solid_Source<QSSEIntrinsics>,
    0,
    comp_func_solid_SourceIn<QSSEIntrinsics>,
    comp_func_solid_DestinationIn<QSSEIntrinsics>,
    comp_func_solid_SourceOut<QSSEIntrinsics>,
    comp_func_solid_DestinationOut<QSSEIntrinsics>,
    comp_func_solid_SourceAtop<QSSEIntrinsics>,
    comp_func_solid_DestinationAtop<QSSEIntrinsics>,
    comp_func_solid_XOR<QSSEIntrinsics>,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // svg 1.2 modes
    rasterop_solid_SourceOrDestination<QMMXIntrinsics>,
    rasterop_solid_SourceAndDestination<QMMXIntrinsics>,
    rasterop_solid_SourceXorDestination<QMMXIntrinsics>,
    rasterop_solid_NotSourceAndNotDestination<QMMXIntrinsics>,
    rasterop_solid_NotSourceOrNotDestination<QMMXIntrinsics>,
    rasterop_solid_NotSourceXorDestination<QMMXIntrinsics>,
    rasterop_solid_NotSource<QMMXIntrinsics>,
    rasterop_solid_NotSourceAndDestination<QMMXIntrinsics>,
    rasterop_solid_SourceAndNotDestination<QMMXIntrinsics>
};

CompositionFunction qt_functionForMode_SSE[numCompositionFunctions] = {
    comp_func_SourceOver<QSSEIntrinsics>,
    comp_func_DestinationOver<QSSEIntrinsics>,
    comp_func_Clear<QSSEIntrinsics>,
    comp_func_Source<QSSEIntrinsics>,
    0,
    comp_func_SourceIn<QSSEIntrinsics>,
    comp_func_DestinationIn<QSSEIntrinsics>,
    comp_func_SourceOut<QSSEIntrinsics>,
    comp_func_DestinationOut<QSSEIntrinsics>,
    comp_func_SourceAtop<QSSEIntrinsics>,
    comp_func_DestinationAtop<QSSEIntrinsics>,
    comp_func_XOR<QSSEIntrinsics>
};

void qt_blend_color_argb_sse(int count, const QSpan *spans, void *userData)
{
    qt_blend_color_argb_x86<QSSEIntrinsics>(count, spans, userData,
                                            (CompositionFunctionSolid*)qt_functionForModeSolid_SSE);
}

void qt_memfill32_sse(quint32 *dest, quint32 value, int count)
{
    return qt_memfill32_sse_template<QSSEIntrinsics>(dest, value, count);
}

void qt_bitmapblit16_sse(QRasterBuffer *rasterBuffer, int x, int y,
                         quint32 color,
                         const uchar *src,
                         int width, int height, int stride)
{
    return qt_bitmapblit16_sse_template<QSSEIntrinsics>(rasterBuffer, x,y,
                                                        color, src, width,
                                                        height, stride);
}

void qt_blend_argb32_on_argb32_sse(uchar *destPixels, int dbpl,
                                   const uchar *srcPixels, int sbpl,
                                   int w, int h,
                                   int const_alpha)
{
    const uint *src = (const uint *) srcPixels;
    uint *dst = (uint *) destPixels;

    uint ca = const_alpha - 1;

    for (int y=0; y<h; ++y) {
        comp_func_SourceOver<QSSEIntrinsics>(dst, src, w, ca);
        dst = (quint32 *)(((uchar *) dst) + dbpl);
        src = (const quint32 *)(((const uchar *) src) + sbpl);
    }
}

void qt_blend_rgb32_on_rgb32_sse(uchar *destPixels, int dbpl,
                                 const uchar *srcPixels, int sbpl,
                                 int w, int h,
                                 int const_alpha)
{
    const uint *src = (const uint *) srcPixels;
    uint *dst = (uint *) destPixels;

    uint ca = const_alpha - 1;

    for (int y=0; y<h; ++y) {
        comp_func_Source<QSSEIntrinsics>(dst, src, w, ca);
        dst = (quint32 *)(((uchar *) dst) + dbpl);
        src = (const quint32 *)(((const uchar *) src) + sbpl);
    }
}

QT_END_NAMESPACE

#endif // QT_HAVE_SSE
