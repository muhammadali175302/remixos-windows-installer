/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QSIZE_H
#define QSIZE_H

#include <QtCore/qnamespace.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class Q_CORE_EXPORT QSize
{
public:
    QSize();
    QSize(int w, int h);

    bool isNull() const;
    bool isEmpty() const;
    bool isValid() const;

    int width() const;
    int height() const;
    void setWidth(int w);
    void setHeight(int h);
    void transpose();

    void scale(int w, int h, Qt::AspectRatioMode mode);
    void scale(const QSize &s, Qt::AspectRatioMode mode);

    QSize expandedTo(const QSize &) const;
    QSize boundedTo(const QSize &) const;

    int &rwidth();
    int &rheight();

    QSize &operator+=(const QSize &);
    QSize &operator-=(const QSize &);
    QSize &operator*=(qreal c);
    QSize &operator/=(qreal c);

    friend inline bool operator==(const QSize &, const QSize &);
    friend inline bool operator!=(const QSize &, const QSize &);
    friend inline const QSize operator+(const QSize &, const QSize &);
    friend inline const QSize operator-(const QSize &, const QSize &);
    friend inline const QSize operator*(const QSize &, qreal);
    friend inline const QSize operator*(qreal, const QSize &);
    friend inline const QSize operator/(const QSize &, qreal);

private:
    int wd;
    int ht;
};
Q_DECLARE_TYPEINFO(QSize, Q_MOVABLE_TYPE);

/*****************************************************************************
  QSize stream functions
 *****************************************************************************/

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QSize &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QSize &);


/*****************************************************************************
  QSize inline functions
 *****************************************************************************/

inline QSize::QSize()
{ wd = ht = -1; }

inline QSize::QSize(int w, int h)
{ wd = w; ht = h; }

inline bool QSize::isNull() const
{ return wd==0 && ht==0; }

inline bool QSize::isEmpty() const
{ return wd<1 || ht<1; }

inline bool QSize::isValid() const
{ return wd>=0 && ht>=0; }

inline int QSize::width() const
{ return wd; }

inline int QSize::height() const
{ return ht; }

inline void QSize::setWidth(int w)
{ wd = w; }

inline void QSize::setHeight(int h)
{ ht = h; }

inline void QSize::scale(int w, int h, Qt::AspectRatioMode mode)
{ scale(QSize(w, h), mode); }

inline int &QSize::rwidth()
{ return wd; }

inline int &QSize::rheight()
{ return ht; }

inline QSize &QSize::operator+=(const QSize &s)
{ wd+=s.wd; ht+=s.ht; return *this; }

inline QSize &QSize::operator-=(const QSize &s)
{ wd-=s.wd; ht-=s.ht; return *this; }

inline QSize &QSize::operator*=(qreal c)
{ wd = qRound(wd*c); ht = qRound(ht*c); return *this; }

inline bool operator==(const QSize &s1, const QSize &s2)
{ return s1.wd == s2.wd && s1.ht == s2.ht; }

inline bool operator!=(const QSize &s1, const QSize &s2)
{ return s1.wd != s2.wd || s1.ht != s2.ht; }

inline const QSize operator+(const QSize & s1, const QSize & s2)
{ return QSize(s1.wd+s2.wd, s1.ht+s2.ht); }

inline const QSize operator-(const QSize &s1, const QSize &s2)
{ return QSize(s1.wd-s2.wd, s1.ht-s2.ht); }

inline const QSize operator*(const QSize &s, qreal c)
{ return QSize(qRound(s.wd*c), qRound(s.ht*c)); }

inline const QSize operator*(qreal c, const QSize &s)
{ return QSize(qRound(s.wd*c), qRound(s.ht*c)); }

inline QSize &QSize::operator/=(qreal c)
{
    Q_ASSERT(!qFuzzyCompare(c + 1, 1));
    wd = qRound(wd/c); ht = qRound(ht/c);
    return *this;
}

inline const QSize operator/(const QSize &s, qreal c)
{
    Q_ASSERT(!qFuzzyCompare(c + 1, 1));
    return QSize(qRound(s.wd/c), qRound(s.ht/c));
}

inline QSize QSize::expandedTo(const QSize & otherSize) const
{
    return QSize(qMax(wd,otherSize.wd), qMax(ht,otherSize.ht));
}

inline QSize QSize::boundedTo(const QSize & otherSize) const
{
    return QSize(qMin(wd,otherSize.wd), qMin(ht,otherSize.ht));
}

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QSize &);
#endif


class Q_CORE_EXPORT QSizeF
{
public:
    QSizeF();
    QSizeF(const QSize &sz);
    QSizeF(qreal w, qreal h);

    bool isNull() const;
    bool isEmpty() const;
    bool isValid() const;

    qreal width() const;
    qreal height() const;
    void setWidth(qreal w);
    void setHeight(qreal h);
    void transpose();

    void scale(qreal w, qreal h, Qt::AspectRatioMode mode);
    void scale(const QSizeF &s, Qt::AspectRatioMode mode);

    QSizeF expandedTo(const QSizeF &) const;
    QSizeF boundedTo(const QSizeF &) const;

    qreal &rwidth();
    qreal &rheight();

    QSizeF &operator+=(const QSizeF &);
    QSizeF &operator-=(const QSizeF &);
    QSizeF &operator*=(qreal c);
    QSizeF &operator/=(qreal c);

    friend inline bool operator==(const QSizeF &, const QSizeF &);
    friend inline bool operator!=(const QSizeF &, const QSizeF &);
    friend inline const QSizeF operator+(const QSizeF &, const QSizeF &);
    friend inline const QSizeF operator-(const QSizeF &, const QSizeF &);
    friend inline const QSizeF operator*(const QSizeF &, qreal);
    friend inline const QSizeF operator*(qreal, const QSizeF &);
    friend inline const QSizeF operator/(const QSizeF &, qreal);

    inline QSize toSize() const;

private:
    qreal wd;
    qreal ht;
};
Q_DECLARE_TYPEINFO(QSizeF, Q_MOVABLE_TYPE);


/*****************************************************************************
  QSizeF stream functions
 *****************************************************************************/

Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QSizeF &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QSizeF &);


/*****************************************************************************
  QSizeF inline functions
 *****************************************************************************/

inline QSizeF::QSizeF()
{ wd = ht = -1.; }

inline QSizeF::QSizeF(const QSize &sz)
    : wd(sz.width()), ht(sz.height())
{
}

inline QSizeF::QSizeF(qreal w, qreal h)
{ wd = w; ht = h; }

inline bool QSizeF::isNull() const
{ return qIsNull(wd) && qIsNull(ht); }

inline bool QSizeF::isEmpty() const
{ return wd <= 0. || ht <= 0.; }

inline bool QSizeF::isValid() const
{ return wd >= 0. && ht >= 0.; }

inline qreal QSizeF::width() const
{ return wd; }

inline qreal QSizeF::height() const
{ return ht; }

inline void QSizeF::setWidth(qreal w)
{ wd = w; }

inline void QSizeF::setHeight(qreal h)
{ ht = h; }

inline void QSizeF::scale(qreal w, qreal h, Qt::AspectRatioMode mode)
{ scale(QSizeF(w, h), mode); }

inline qreal &QSizeF::rwidth()
{ return wd; }

inline qreal &QSizeF::rheight()
{ return ht; }

inline QSizeF &QSizeF::operator+=(const QSizeF &s)
{ wd += s.wd; ht += s.ht; return *this; }

inline QSizeF &QSizeF::operator-=(const QSizeF &s)
{ wd -= s.wd; ht -= s.ht; return *this; }

inline QSizeF &QSizeF::operator*=(qreal c)
{ wd *= c; ht *= c; return *this; }

inline bool operator==(const QSizeF &s1, const QSizeF &s2)
{ return qFuzzyCompare(s1.wd, s2.wd) && qFuzzyCompare(s1.ht, s2.ht); }

inline bool operator!=(const QSizeF &s1, const QSizeF &s2)
{ return !qFuzzyCompare(s1.wd, s2.wd) || !qFuzzyCompare(s1.ht, s2.ht); }

inline const QSizeF operator+(const QSizeF & s1, const QSizeF & s2)
{ return QSizeF(s1.wd+s2.wd, s1.ht+s2.ht); }

inline const QSizeF operator-(const QSizeF &s1, const QSizeF &s2)
{ return QSizeF(s1.wd-s2.wd, s1.ht-s2.ht); }

inline const QSizeF operator*(const QSizeF &s, qreal c)
{ return QSizeF(s.wd*c, s.ht*c); }

inline const QSizeF operator*(qreal c, const QSizeF &s)
{ return QSizeF(s.wd*c, s.ht*c); }

inline QSizeF &QSizeF::operator/=(qreal c)
{
    Q_ASSERT(!qFuzzyCompare(c + 1, 1));
    wd = wd/c; ht = ht/c;
    return *this;
}

inline const QSizeF operator/(const QSizeF &s, qreal c)
{
    Q_ASSERT(!qFuzzyCompare(c + 1, 1));
    return QSizeF(s.wd/c, s.ht/c);
}

inline QSizeF QSizeF::expandedTo(const QSizeF & otherSize) const
{
    return QSizeF(qMax(wd,otherSize.wd), qMax(ht,otherSize.ht));
}

inline QSizeF QSizeF::boundedTo(const QSizeF & otherSize) const
{
    return QSizeF(qMin(wd,otherSize.wd), qMin(ht,otherSize.ht));
}

inline QSize QSizeF::toSize() const
{
    return QSize(qRound(wd), qRound(ht));
}

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QSizeF &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSIZE_H
