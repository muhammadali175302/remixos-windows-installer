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

#ifndef QTEXTDOCUMENTLAYOUT_P_H
#define QTEXTDOCUMENTLAYOUT_P_H

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

#include "QtGui/qabstracttextdocumentlayout.h"
#include "QtGui/qtextoption.h"
#include "QtGui/qtextobject.h"

QT_BEGIN_NAMESPACE

class QTextListFormat;

class QTextDocumentLayoutPrivate;

class Q_AUTOTEST_EXPORT QTextDocumentLayout : public QAbstractTextDocumentLayout
{
    Q_DECLARE_PRIVATE(QTextDocumentLayout)
    Q_OBJECT
    Q_PROPERTY(int cursorWidth READ cursorWidth WRITE setCursorWidth)
    Q_PROPERTY(qreal idealWidth READ idealWidth)
    Q_PROPERTY(bool contentHasAlignment READ contentHasAlignment)
public:
    explicit QTextDocumentLayout(QTextDocument *doc);

    // from the abstract layout
    void draw(QPainter *painter, const PaintContext &context);
    int hitTest(const QPointF &point, Qt::HitTestAccuracy accuracy) const;

    int pageCount() const;
    QSizeF documentSize() const;

    void setCursorWidth(int width);
    int cursorWidth() const;

    // internal, to support the ugly FixedColumnWidth wordwrap mode in QTextEdit
    void setFixedColumnWidth(int width);

    // internal for QTextEdit's NoWrap mode
    void setViewport(const QRectF &viewport);

    virtual QRectF frameBoundingRect(QTextFrame *frame) const;
    virtual QRectF blockBoundingRect(const QTextBlock &block) const;

    // ####
    int layoutStatus() const;
    int dynamicPageCount() const;
    QSizeF dynamicDocumentSize() const;
    void ensureLayouted(qreal);

    qreal idealWidth() const;

    bool contentHasAlignment() const;

protected:
    void documentChanged(int from, int oldLength, int length);
    void resizeInlineObject(QTextInlineObject item, int posInDocument, const QTextFormat &format);
    void positionInlineObject(QTextInlineObject item, int posInDocument, const QTextFormat &format);
    void drawInlineObject(QPainter *p, const QRectF &rect, QTextInlineObject item,
                          int posInDocument, const QTextFormat &format);
    virtual void timerEvent(QTimerEvent *e);
private:
    QRectF doLayout(int from, int oldLength, int length);
    void layoutFinished();
};

QT_END_NAMESPACE

#endif // QTEXTDOCUMENTLAYOUT_P_H
