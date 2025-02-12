/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/*!
    \class QGLColormap
    \brief The QGLColormap class is used for installing custom colormaps into
    QGLWidgets.

    \module OpenGL
    \ingroup multimedia
    \ingroup shared

    QGLColormap provides a platform independent way of specifying and
    installing indexed colormaps into QGLWidgets. QGLColormap is
    especially useful when using the OpenGL color-index mode.

    Under X11 you must use an X server that supports either a \c
    PseudoColor or \c DirectColor visual class. If your X server
    currently only provides a \c GrayScale, \c TrueColor, \c
    StaticColor or \c StaticGray visual, you will not be able to
    allocate colorcells for writing. If this is the case, try setting
    your X server to 8 bit mode. It should then provide you with at
    least a \c PseudoColor visual. Note that you may experience
    colormap flashing if your X server is running in 8 bit mode.

    Under Windows the size of the colormap is always set to 256
    colors. Note that under Windows you can also install colormaps
    in child widgets.

    This class uses \l{implicit sharing} as a memory and speed
    optimization.

    Example of use:
    \snippet doc/src/snippets/code/src.opengl.qglcolormap.cpp 0

    \sa QGLWidget::setColormap(), QGLWidget::colormap()
*/

/*!
    \fn Qt::HANDLE QGLColormap::handle()

    \internal

    Returns the handle for this color map.
*/

/*!
    \fn void QGLColormap::setHandle(Qt::HANDLE handle)

    \internal

    Sets the handle for this color map to \a handle.
*/

#include "qglcolormap.h"

QT_BEGIN_NAMESPACE

QGLColormap::QGLColormapData QGLColormap::shared_null = { Q_BASIC_ATOMIC_INITIALIZER(1), 0, 0 };

/*!
    Construct a QGLColormap.
*/
QGLColormap::QGLColormap()
    : d(&shared_null)
{
    d->ref.ref();
}


/*!
    Construct a shallow copy of \a map.
*/
QGLColormap::QGLColormap(const QGLColormap &map)
    : d(map.d)
{
    d->ref.ref();
}

/*!
    Dereferences the QGLColormap and deletes it if this was the last
    reference to it.
*/
QGLColormap::~QGLColormap()
{
    if (!d->ref.deref())
        cleanup(d);
}

void QGLColormap::cleanup(QGLColormap::QGLColormapData *x)
{
    delete x->cells;
    x->cells = 0;
    delete x;
}

/*!
    Assign a shallow copy of \a map to this QGLColormap.
*/
QGLColormap & QGLColormap::operator=(const QGLColormap &map)
{
    map.d->ref.ref();
    if (!d->ref.deref())
        cleanup(d);
    d = map.d;
    return *this;
}

/*!
    \fn void QGLColormap::detach()
    \internal

    Detaches this QGLColormap from the shared block.
*/

void QGLColormap::detach_helper()
{
    QGLColormapData *x = new QGLColormapData;
    x->ref = 1;
    x->cmapHandle = 0;
    x->cells = 0;
    if (d->cells) {
        x->cells = new QVector<QRgb>(256);
        *x->cells = *d->cells;
    }
    if (!d->ref.deref())
        cleanup(d);
    d = x;
}

/*!
    Set cell at index \a idx in the colormap to color \a color.
*/
void QGLColormap::setEntry(int idx, QRgb color)
{
    detach();
    if (!d->cells)
        d->cells = new QVector<QRgb>(256);
    d->cells->insert(idx, color);
}

/*!
    Set an array of cells in this colormap. \a count is the number of
    colors that should be set, \a colors is the array of colors, and
    \a base is the starting index.
*/
void QGLColormap::setEntries(int count, const QRgb *colors, int base)
{
    detach();
    if (!d->cells)
        d->cells = new QVector<QRgb>(256);

    Q_ASSERT_X(!colors || base >= 0 || base + count < d->cells->size(), "QGLColormap::setEntries",
               "preconditions not met");
    for (int i = base; i < base + count; ++i)
        setEntry(i, colors[i]);
}

/*!
    Returns the QRgb value in the colorcell with index \a idx.
*/
QRgb QGLColormap::entryRgb(int idx) const
{
    if (d == &shared_null || !d->cells)
        return 0;
    else
        return d->cells->at(idx);
}

/*!
    \overload

    Set the cell with index \a idx in the colormap to color \a color.
*/
void QGLColormap::setEntry(int idx, const QColor &color)
{
    setEntry(idx, color.rgb());
}

/*!
    Returns the QRgb value in the colorcell with index \a idx.
*/
QColor QGLColormap::entryColor(int idx) const
{
    if (d == &shared_null || !d->cells)
        return QColor();
    else
        return QColor(d->cells->at(idx));
}

/*!
    Returns true if the colormap is empty; otherwise returns false. A
    colormap with no color values set is considered to be empty.
*/
bool QGLColormap::isEmpty() const
{
    return d == &shared_null || d->cells == 0 || d->cells->size() == 0 || d->cmapHandle == 0;
}


/*!
    Returns the number of colorcells in the colormap.
*/
int QGLColormap::size() const
{
    return d->cells ? d->cells->size() : 0;
}

/*!
    Returns the index of the color \a color. If \a color is not in the
    map, -1 is returned.
*/
int QGLColormap::find(QRgb color) const
{
    if (d->cells)
        return d->cells->indexOf(color);
    return -1;
}

/*!
    Returns the index of the color that is the closest match to color
    \a color.
*/
int QGLColormap::findNearest(QRgb color) const
{
    int idx = find(color);
    if (idx >= 0)
        return idx;
    int mapSize = size();
    int mindist = 200000;
    int r = qRed(color);
    int g = qGreen(color);
    int b = qBlue(color);
    int rx, gx, bx, dist;
    for (int i = 0; i < mapSize; ++i) {
        QRgb ci = d->cells->at(i);
        rx = r - qRed(ci);
        gx = g - qGreen(ci);
        bx = b - qBlue(ci);
        dist = rx * rx + gx * gx + bx * bx;        // calculate distance
        if (dist < mindist) {                // minimal?
            mindist = dist;
            idx = i;
        }
    }
    return idx;
}

QT_END_NAMESPACE
