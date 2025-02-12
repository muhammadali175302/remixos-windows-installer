/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QICON_H
#define QICON_H

#include <QtCore/qglobal.h>
#include <QtCore/qsize.h>
#include <QtGui/qpixmap.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QIconPrivate;
class QIconEngine;
class QIconEngineV2;

class Q_GUI_EXPORT QIcon
{
public:
    enum Mode { Normal, Disabled, Active, Selected };
    enum State { On, Off };

    QIcon();
    QIcon(const QPixmap &pixmap);
    QIcon(const QIcon &other);
    explicit QIcon(const QString &fileName); // file or resource name
    explicit QIcon(QIconEngine *engine);
    explicit QIcon(QIconEngineV2 *engine);
    ~QIcon();
    QIcon &operator=(const QIcon &other);
    operator QVariant() const;

    QPixmap pixmap(const QSize &size, Mode mode = Normal, State state = Off) const;
    inline QPixmap pixmap(int w, int h, Mode mode = Normal, State state = Off) const
        { return pixmap(QSize(w, h), mode, state); }
    inline QPixmap pixmap(int extent, Mode mode = Normal, State state = Off) const
        { return pixmap(QSize(extent, extent), mode, state); }

    QSize actualSize(const QSize &size, Mode mode = Normal, State state = Off) const;

    void paint(QPainter *painter, const QRect &rect, Qt::Alignment alignment = Qt::AlignCenter, Mode mode = Normal, State state = Off) const;
    inline void paint(QPainter *painter, int x, int y, int w, int h, Qt::Alignment alignment = Qt::AlignCenter, Mode mode = Normal, State state = Off) const
        { paint(painter, QRect(x, y, w, h), alignment, mode, state); }

    bool isNull() const;
    bool isDetached() const;
    void detach();

    int serialNumber() const;
    qint64 cacheKey() const;

    void addPixmap(const QPixmap &pixmap, Mode mode = Normal, State state = Off);
    void addFile(const QString &fileName, const QSize &size = QSize(), Mode mode = Normal, State state = Off);

#ifdef QT3_SUPPORT
    enum Size { Small, Large, Automatic = Small };
    static QT3_SUPPORT void setPixmapSize(Size which, const QSize &size);
    static QT3_SUPPORT QSize pixmapSize(Size which);
    inline QT3_SUPPORT void reset(const QPixmap &pixmap, Size /*size*/) { *this = QIcon(pixmap); }
    inline QT3_SUPPORT void setPixmap(const QPixmap &pixmap, Size, Mode mode = Normal, State state = Off)
        { addPixmap(pixmap, mode, state); }
    inline QT3_SUPPORT void setPixmap(const QString &fileName, Size, Mode mode = Normal, State state = Off)
        { addPixmap(QPixmap(fileName), mode, state); }
    QT3_SUPPORT QPixmap pixmap(Size size, Mode mode, State state = Off) const;
    QT3_SUPPORT QPixmap pixmap(Size size, bool enabled, State state = Off) const;
    QT3_SUPPORT QPixmap pixmap() const;
#endif

    Q_DUMMY_COMPARISON_OPERATOR(QIcon)

private:
    QIconPrivate *d;
#if !defined(QT_NO_DATASTREAM)
    friend Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QIcon &);
    friend Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QIcon &);
#endif

public:
    typedef QIconPrivate * DataPtr;
    inline DataPtr &data_ptr() { return d; }
};

Q_DECLARE_SHARED(QIcon)
Q_DECLARE_TYPEINFO(QIcon, Q_MOVABLE_TYPE);

#if !defined(QT_NO_DATASTREAM)
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QIcon &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QIcon &);
#endif

#ifdef QT3_SUPPORT
typedef QIcon QIconSet;
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QICON_H
