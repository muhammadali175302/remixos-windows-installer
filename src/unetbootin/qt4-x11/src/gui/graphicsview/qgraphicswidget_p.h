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

#ifndef QGRAPHICSWIDGET_P_H
#define QGRAPHICSWIDGET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include <private/qobject_p.h>
#include "qgraphicsitem_p.h"
#include "qgraphicswidget.h"
#include <QtGui/qfont.h>
#include <QtGui/qpalette.h>
#include <QtGui/qsizepolicy.h>
#include <QtGui/qstyle.h>

QT_BEGIN_NAMESPACE

class QGraphicsLayout;
class QStyleOptionTitleBar;

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

class Q_GUI_EXPORT QGraphicsWidgetPrivate : public QGraphicsItemPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsWidget)
public:
    QGraphicsWidgetPrivate()
        : leftMargin(0),
          topMargin(0),
          rightMargin(0),
          bottomMargin(0),
          leftLayoutItemMargin(0),
          topLayoutItemMargin(0),
          rightLayoutItemMargin(0),
          bottomLayoutItemMargin(0),
          layout(0),
          inheritedPaletteResolveMask(0),
          inheritedFontResolveMask(0),
          inSetGeometry(0),
          focusPolicy(Qt::NoFocus),
          focusNext(0),
          focusPrev(0),
          focusChild(0),
          windowFlags(0),
          hoveredSubControl(QStyle::SC_None),
          grabbedSection(Qt::NoSection),
          buttonMouseOver(false),
          buttonSunken(false),
          setWindowFrameMargins(false),
          leftWindowFrameMargin(0),
          topWindowFrameMargin(0),
          rightWindowFrameMargin(0),
          bottomWindowFrameMargin(0)
    { }

    void init(QGraphicsItem *parentItem, Qt::WindowFlags wFlags);
    qreal titleBarHeight(const QStyleOptionTitleBar &options) const;

    // Margins
    qreal leftMargin;
    qreal topMargin;
    qreal rightMargin;
    qreal bottomMargin;
    QRectF contentsRect;

    // Layout item margins
    void getLayoutItemMargins(qreal *left, qreal *top, qreal *right, qreal *bottom) const;
    void setLayoutItemMargins(qreal left, qreal top, qreal right, qreal bottom);
    void setLayoutItemMargins(QStyle::SubElement element, const QStyleOption *opt = 0);

    void fixFocusChainBeforeReparenting(QGraphicsWidget *newParent, QGraphicsScene *newScene = 0);
    void setLayout_helper(QGraphicsLayout *l);

    qreal leftLayoutItemMargin;
    qreal topLayoutItemMargin;
    qreal rightLayoutItemMargin;
    qreal bottomLayoutItemMargin;

    // Layouts
    QGraphicsLayout *layout;
    void setLayoutDirection_helper(Qt::LayoutDirection direction);
    void resolveLayoutDirection();

    // Style
    QPalette palette;
    uint inheritedPaletteResolveMask;
    void setPalette_helper(const QPalette &palette);
    void resolvePalette(uint inheritedMask);
    void updatePalette(const QPalette &palette);
    QPalette naturalWidgetPalette() const;
    QFont font;
    uint inheritedFontResolveMask;
    void setFont_helper(const QFont &font);
    void resolveFont(uint inheritedMask);
    void updateFont(const QFont &font);
    QFont naturalWidgetFont() const;

    // Window specific
    void initStyleOptionTitleBar(QStyleOptionTitleBar *option);
    void adjustWindowFlags(Qt::WindowFlags *wFlags);
    void windowFrameMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void windowFrameMousePressEvent(QGraphicsSceneMouseEvent *event);
    void windowFrameMouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void windowFrameHoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void windowFrameHoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    bool hasDecoration() const;

    // State
    inline int attributeToBitIndex(Qt::WidgetAttribute att) const
    {
        int bit = -1;
        switch (att) {
        case Qt::WA_SetLayoutDirection: bit = 0; break;
        case Qt::WA_RightToLeft: bit = 1; break;
        case Qt::WA_SetStyle: bit = 2; break;
        case Qt::WA_Resized: bit = 3; break;
        case Qt::WA_DeleteOnClose: bit = 4; break;
        case Qt::WA_NoSystemBackground: bit = 5; break;
        case Qt::WA_OpaquePaintEvent: bit = 6; break;
        case Qt::WA_SetPalette: bit = 7; break;
        case Qt::WA_SetFont: bit = 8; break;
        case Qt::WA_WindowPropagation: bit = 9; break;
        default: break;
        }
        return bit;
    }
    inline void setAttribute(Qt::WidgetAttribute att, bool value)
    {
        int bit = attributeToBitIndex(att);
        if (bit == -1) {
            qWarning("QGraphicsWidget::setAttribute: unsupported attribute %d", int(att));
            return;
        }
        if (value)
            attributes |= (1 << bit);
        else
            attributes &= ~(1 << bit);
    }
    inline bool testAttribute(Qt::WidgetAttribute att) const
    {
        int bit = attributeToBitIndex(att);
        if (bit == -1)
            return false;
        return (attributes & (1 << bit)) != 0;
    }
    quint32 attributes : 10;
    quint32 inSetGeometry : 1;

    // Focus
    Qt::FocusPolicy focusPolicy;
    QGraphicsWidget *focusNext;
    QGraphicsWidget *focusPrev;
    QGraphicsWidget *focusChild;
    void setFocusWidget();
    void clearFocusWidget();

    // Windows
    Qt::WindowFlags windowFlags;
    QString windowTitle;
    QStyle::SubControl hoveredSubControl;
    Qt::WindowFrameSection grabbedSection;
    uint buttonMouseOver : 1;
    uint buttonSunken : 1;
    QPointF mouseDelta; // to compensate for small error when interactively resizing
    QRectF startGeometry;
    QRect buttonRect;

    bool setWindowFrameMargins;
    qreal leftWindowFrameMargin;
    qreal topWindowFrameMargin;
    qreal rightWindowFrameMargin;
    qreal bottomWindowFrameMargin;

#ifndef QT_NO_ACTION
    QList<QAction *> actions;
#endif
};

#endif //!defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

QT_END_NAMESPACE

#endif //QGRAPHICSWIDGET_P_H

