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

#ifndef QLCDNUMBER_H
#define QLCDNUMBER_H

#include <QtGui/qframe.h>
#include <QtCore/qbitarray.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_LCDNUMBER

class QLCDNumberPrivate;
class Q_GUI_EXPORT QLCDNumber : public QFrame // LCD number widget
{
    Q_OBJECT
    Q_ENUMS(Mode SegmentStyle)
    Q_PROPERTY(bool smallDecimalPoint READ smallDecimalPoint WRITE setSmallDecimalPoint)
    Q_PROPERTY(int numDigits READ numDigits WRITE setNumDigits)
    Q_PROPERTY(Mode mode READ mode WRITE setMode)
    Q_PROPERTY(SegmentStyle segmentStyle READ segmentStyle WRITE setSegmentStyle)
    Q_PROPERTY(double value READ value WRITE display)
    Q_PROPERTY(int intValue READ intValue WRITE display)

public:
    explicit QLCDNumber(QWidget* parent = 0);
    explicit QLCDNumber(uint numDigits, QWidget* parent = 0);
    ~QLCDNumber();

    enum Mode {
        Hex, Dec, Oct, Bin
#if defined(QT3_SUPPORT) && !defined(Q_MOC_RUN)
        , HEX = Hex, DEC = Dec, OCT = Oct, BIN = Bin
#endif
    };
    enum SegmentStyle {
        Outline, Filled, Flat
    };

    bool smallDecimalPoint() const;

    int numDigits() const;
    void setNumDigits(int nDigits);

    bool checkOverflow(double num) const;
    bool checkOverflow(int num) const;

    Mode mode() const;
    void setMode(Mode);

    SegmentStyle segmentStyle() const;
    void setSegmentStyle(SegmentStyle);

    double value() const;
    int intValue() const;

    QSize sizeHint() const;

public Q_SLOTS:
    void display(const QString &str);
    void display(int num);
    void display(double num);
    void setHexMode();
    void setDecMode();
    void setOctMode();
    void setBinMode();
    void setSmallDecimalPoint(bool);

Q_SIGNALS:
    void overflow();

protected:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *);

public:
#ifdef QT3_SUPPORT
    QT3_SUPPORT_CONSTRUCTOR QLCDNumber(QWidget* parent, const char* name);
    QT3_SUPPORT_CONSTRUCTOR QLCDNumber(uint numDigits, QWidget* parent, const char* name);
    
    QT3_SUPPORT void setMargin(int margin) { setContentsMargins(margin, margin, margin, margin); }
    QT3_SUPPORT int margin() const 
    { int margin; int dummy; getContentsMargins(&margin, &dummy, &dummy, &dummy);  return margin; }    
#endif

private:
    Q_DISABLE_COPY(QLCDNumber)
    Q_DECLARE_PRIVATE(QLCDNumber)
};

#endif // QT_NO_LCDNUMBER

QT_END_NAMESPACE

QT_END_HEADER

#endif // QLCDNUMBER_H
