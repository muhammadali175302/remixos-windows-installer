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

#ifndef QSPLITTER_P_H
#define QSPLITTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qframe_p.h"
#include "qrubberband.h"

QT_BEGIN_NAMESPACE

static const uint Default = 2;

class QSplitterLayoutStruct
{
public:
    QRect rect;
    int sizer;
    uint collapsed : 1;
    uint collapsible : 2;
    QWidget *widget;
    QSplitterHandle *handle;

    QSplitterLayoutStruct() : sizer(-1), collapsed(false), collapsible(Default), widget(0), handle(0) {}
    ~QSplitterLayoutStruct() { delete handle; }
    int getWidgetSize(Qt::Orientation orient);
    int getHandleSize(Qt::Orientation orient);
    int pick(const QSize &size, Qt::Orientation orient)
    { return (orient == Qt::Horizontal) ? size.width() : size.height(); }
};

class QSplitterPrivate : public QFramePrivate
{
    Q_DECLARE_PUBLIC(QSplitter)
public:
    QSplitterPrivate() : rubberBand(0), opaque(true), firstShow(true),
                         childrenCollapsible(true), compatMode(false), handleWidth(0), blockChildAdd(false) {}

    QPointer<QRubberBand> rubberBand;
    mutable QList<QSplitterLayoutStruct *> list;
    Qt::Orientation orient;
    bool opaque : 8;
    bool firstShow : 8;
    bool childrenCollapsible : 8;
    bool compatMode : 8;
    int handleWidth;
    bool blockChildAdd;

    inline int pick(const QPoint &pos) const
    { return orient == Qt::Horizontal ? pos.x() : pos.y(); }
    inline int pick(const QSize &s) const
    { return orient == Qt::Horizontal ? s.width() : s.height(); }

    inline int trans(const QPoint &pos) const
    { return orient == Qt::Vertical ? pos.x() : pos.y(); }
    inline int trans(const QSize &s) const
    { return orient == Qt::Vertical ? s.width() : s.height(); }

    void init();
    void recalc(bool update = false);
    void doResize();
    void storeSizes();
    void getRange(int index, int *, int *, int *, int *) const;
    void addContribution(int, int *, int *, bool) const;
    int adjustPos(int, int, int *, int *, int *, int *) const;
    bool collapsible(QSplitterLayoutStruct *) const;
    bool collapsible(int index) const
    { return (index < 0 || index >= list.size()) ? true : collapsible(list.at(index)); }
    QSplitterLayoutStruct *findWidget(QWidget *) const;
    void insertWidget_helper(int index, QWidget *widget, bool show);
    QSplitterLayoutStruct *insertWidget(int index, QWidget *);
    void doMove(bool backwards, int pos, int index, int delta,
                bool mayCollapse, int *positions, int *widths);
    void setGeo(QSplitterLayoutStruct *s, int pos, int size, bool allowCollapse);
    int findWidgetJustBeforeOrJustAfter(int index, int delta, int &collapsibleSize) const;
    void updateHandles();
    void setSizes_helper(const QList<int> &sizes, bool clampNegativeSize = false);

};

class QSplitterHandlePrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QSplitterHandle)
public:
    QSplitterHandlePrivate() : orient(Qt::Horizontal), opaq(false), s(0), mouseOffset(0) {}

    inline int pick(const QPoint &pos) const
    { return orient == Qt::Horizontal ? pos.x() : pos.y(); }

    Qt::Orientation orient;
    bool opaq;
    QSplitter *s;
    bool hover;
    int mouseOffset;
};

QT_END_NAMESPACE

#endif
