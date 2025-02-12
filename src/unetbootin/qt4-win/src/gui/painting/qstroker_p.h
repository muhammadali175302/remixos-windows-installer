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

#ifndef QSTROKER_P_H
#define QSTROKER_P_H

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

#include "QtGui/qpainterpath.h"
#include "private/qdatabuffer_p.h"
#include "private/qnumeric_p.h"

QT_BEGIN_NAMESPACE

// #define QFIXED_IS_26_6

#if defined QFIXED_IS_26_6
typedef int qfixed;
#define qt_real_to_fixed(real) qfixed(real * 64.0)
#define qt_int_to_fixed(real) qfixed(int(real) << 6)
#define qt_fixed_to_real(fixed) qreal(fixed / 64.0)
#define qt_fixed_to_int(fixed) int(fixed >> 6)
struct qfixed2d
{
    qfixed x;
    qfixed y;

    bool operator==(const qfixed2d &other) const { return x == other.x && y == other.y; }
};
#elif defined QFIXED_IS_32_32
typedef qint64 qfixed;
#define qt_real_to_fixed(real) qfixed(real * double(qint64(1) << 32))
#define qt_fixed_to_real(fixed) qreal(fixed / double(qint64(1) << 32))
struct qfixed2d
{
    qfixed x;
    qfixed y;

    bool operator==(const qfixed2d &other) const { return x == other.x && y == other.y; }
};
#elif defined QFIXED_IS_16_16
typedef int qfixed;
#define qt_real_to_fixed(real) qfixed(real * double(1 << 16))
#define qt_fixed_to_real(fixed) qreal(fixed / double(1 << 16))
struct qfixed2d
{
    qfixed x;
    qfixed y;

    bool operator==(const qfixed2d &other) const { return x == other.x && y == other.y; }
};
#else
typedef qreal qfixed;
#define qt_real_to_fixed(real) qfixed(real)
#define qt_fixed_to_real(fixed) fixed
struct qfixed2d
{
    qfixed x;
    qfixed y;

    bool operator==(const qfixed2d &other) const { return qFuzzyCompare(x, other.x)
                                                       && qFuzzyCompare(y, other.y); }
};
#endif

#define QT_PATH_KAPPA 0.5522847498

QPointF qt_curves_for_arc(const QRectF &rect, qreal startAngle, qreal sweepLength,
                          QPointF *controlPoints, int *point_count);

qreal qt_t_for_arc_angle(qreal angle);

typedef void (*qStrokerMoveToHook)(qfixed x, qfixed y, void *data);
typedef void (*qStrokerLineToHook)(qfixed x, qfixed y, void *data);
typedef void (*qStrokerCubicToHook)(qfixed c1x, qfixed c1y,
                                    qfixed c2x, qfixed c2y,
                                    qfixed ex, qfixed ey,
                                    void *data);

class Q_GUI_EXPORT QStrokerOps
{
public:
    struct Element {
        QPainterPath::ElementType type;
        qfixed x;
        qfixed y;

        inline bool isMoveTo() const { return type == QPainterPath::MoveToElement; }
        inline bool isLineTo() const { return type == QPainterPath::LineToElement; }
        inline bool isCurveTo() const { return type == QPainterPath::CurveToElement; }

        operator qfixed2d () { qfixed2d pt = { x, y }; return pt; }
    };

    QStrokerOps();
    virtual ~QStrokerOps();

    void setMoveToHook(qStrokerMoveToHook moveToHook) { m_moveTo = moveToHook; }
    void setLineToHook(qStrokerLineToHook lineToHook) { m_lineTo = lineToHook; }
    void setCubicToHook(qStrokerCubicToHook cubicToHook) { m_cubicTo = cubicToHook; }

    virtual void begin(void *customData);
    virtual void end();

    inline void moveTo(qfixed x, qfixed y);
    inline void lineTo(qfixed x, qfixed y);
    inline void cubicTo(qfixed x1, qfixed y1, qfixed x2, qfixed y2, qfixed ex, qfixed ey);

    void strokePath(const QPainterPath &path, void *data, const QTransform &matrix);
    void strokePolygon(const QPointF *points, int pointCount, bool implicit_close,
                       void *data, const QTransform &matrix);
    void strokeEllipse(const QRectF &ellipse, void *data, const QTransform &matrix);

    QRectF clipRect() const { return m_clip_rect; }
    void setClipRect(const QRectF &clip) { m_clip_rect = clip; }

protected:
    inline void emitMoveTo(qfixed x, qfixed y);
    inline void emitLineTo(qfixed x, qfixed y);
    inline void emitCubicTo(qfixed c1x, qfixed c1y, qfixed c2x, qfixed c2y, qfixed ex, qfixed ey);

    virtual void processCurrentSubpath() = 0;
    QDataBuffer<Element> m_elements;

    QRectF m_clip_rect;

private:
    void *m_customData;
    qStrokerMoveToHook m_moveTo;
    qStrokerLineToHook m_lineTo;
    qStrokerCubicToHook m_cubicTo;

};

class Q_GUI_EXPORT QStroker : public QStrokerOps
{
public:

    enum LineJoinMode {
        FlatJoin,
        SquareJoin,
        MiterJoin,
        RoundJoin,
        RoundCap,
        SvgMiterJoin
    };

    QStroker();
    ~QStroker();

    void setStrokeWidth(qfixed width) { m_strokeWidth = width; }
    qfixed strokeWidth() const { return m_strokeWidth; }

    void setCapStyle(Qt::PenCapStyle capStyle) { m_capStyle = joinModeForCap(capStyle); }
    Qt::PenCapStyle capStyle() const { return capForJoinMode(m_capStyle); }
    LineJoinMode capStyleMode() const { return m_capStyle; }

    void setJoinStyle(Qt::PenJoinStyle style) { m_joinStyle = joinModeForJoin(style); }
    Qt::PenJoinStyle joinStyle() const { return joinForJoinMode(m_joinStyle); }
    LineJoinMode joinStyleMode() const { return m_joinStyle; }

    void setMiterLimit(qfixed length) { m_miterLimit = length; }
    qfixed miterLimit() const { return m_miterLimit; }

    void setCurveThreshold(qfixed threshold) { m_curveThreshold = threshold; }
    qfixed curveThreshold() const { return m_curveThreshold; }

    void joinPoints(qfixed x, qfixed y, const QLineF &nextLine, LineJoinMode join);
    inline void emitMoveTo(qfixed x, qfixed y);
    inline void emitLineTo(qfixed x, qfixed y);
    inline void emitCubicTo(qfixed c1x, qfixed c1y, qfixed c2x, qfixed c2y, qfixed ex, qfixed ey);

protected:
    static Qt::PenCapStyle capForJoinMode(LineJoinMode mode);
    static LineJoinMode joinModeForCap(Qt::PenCapStyle);

    static Qt::PenJoinStyle joinForJoinMode(LineJoinMode mode);
    static LineJoinMode joinModeForJoin(Qt::PenJoinStyle joinStyle);

    virtual void processCurrentSubpath();

    qfixed m_strokeWidth;
    qfixed m_miterLimit;
    qfixed m_curveThreshold;

    LineJoinMode m_capStyle;
    LineJoinMode m_joinStyle;

    qfixed m_back1X;
    qfixed m_back1Y;

    qfixed m_back2X;
    qfixed m_back2Y;
};

class Q_GUI_EXPORT QDashStroker : public QStrokerOps
{
public:
    QDashStroker(QStroker *stroker);

    QStroker *stroker() const { return m_stroker; }

    static QVector<qfixed> patternForStyle(Qt::PenStyle style);

    void setDashPattern(const QVector<qfixed> &dashPattern) { m_dashPattern = dashPattern; }
    QVector<qfixed> dashPattern() const { return m_dashPattern; }

    void setDashOffset(qreal offset) { m_dashOffset = offset; }
    qreal dashOffset() const { return m_dashOffset; }

    virtual void begin(void *data);
    virtual void end();

protected:
    virtual void processCurrentSubpath();

    QStroker *m_stroker;
    QVector<qfixed> m_dashPattern;
    qreal m_dashOffset;
};


/*******************************************************************************
 * QStrokerOps inline membmers
 */

inline void QStrokerOps::emitMoveTo(qfixed x, qfixed y)
{
    Q_ASSERT(m_moveTo);
    m_moveTo(x, y, m_customData);
}

inline void QStrokerOps::emitLineTo(qfixed x, qfixed y)
{
    Q_ASSERT(m_lineTo);
    m_lineTo(x, y, m_customData);
}

inline void QStrokerOps::emitCubicTo(qfixed c1x, qfixed c1y, qfixed c2x, qfixed c2y, qfixed ex, qfixed ey)
{
    Q_ASSERT(m_cubicTo);
    m_cubicTo(c1x, c1y, c2x, c2y, ex, ey, m_customData);
}

inline void QStrokerOps::moveTo(qfixed x, qfixed y)
{
    if (m_elements.size()>1)
        processCurrentSubpath();
    m_elements.reset();
    Element e = { QPainterPath::MoveToElement, x, y };
    m_elements.add(e);
}

inline void QStrokerOps::lineTo(qfixed x, qfixed y)
{
    Element e = { QPainterPath::LineToElement, x, y };
    m_elements.add(e);
}

inline void QStrokerOps::cubicTo(qfixed x1, qfixed y1, qfixed x2, qfixed y2, qfixed ex, qfixed ey)
{
    Element c1 = { QPainterPath::CurveToElement, x1, y1 };
    Element c2 = { QPainterPath::CurveToDataElement, x2, y2 };
    Element e = { QPainterPath::CurveToDataElement, ex, ey };
    m_elements.add(c1);
    m_elements.add(c2);
    m_elements.add(e);
}

/*******************************************************************************
 * QStroker inline members
 */
inline void QStroker::emitMoveTo(qfixed x, qfixed y)
{
    m_back2X = m_back1X;
    m_back2Y = m_back1Y;
    m_back1X = x;
    m_back1Y = y;
    QStrokerOps::emitMoveTo(x, y);
}

inline void QStroker::emitLineTo(qfixed x, qfixed y)
{
    m_back2X = m_back1X;
    m_back2Y = m_back1Y;
    m_back1X = x;
    m_back1Y = y;
    QStrokerOps::emitLineTo(x, y);
}

inline void QStroker::emitCubicTo(qfixed c1x, qfixed c1y,
                                  qfixed c2x, qfixed c2y,
                                  qfixed ex, qfixed ey)
{
    if (c2x == ex && c2y == ey) {
        if (c1x == ex && c1y == ey) {
            m_back2X = m_back1X;
            m_back2Y = m_back1Y;
        } else {
            m_back2X = c1x;
            m_back2Y = c1y;
        }
    } else {
        m_back2X = c2x;
        m_back2Y = c2y;
    }
    m_back1X = ex;
    m_back1Y = ey;
    QStrokerOps::emitCubicTo(c1x, c1y, c2x, c2y, ex, ey);
}

/*******************************************************************************
 * QDashStroker inline members
 */
inline void QDashStroker::begin(void *data)
{
    Q_ASSERT(m_stroker);
    m_stroker->begin(data);
    QStrokerOps::begin(data);
}

inline void QDashStroker::end()
{
    Q_ASSERT(m_stroker);
    QStrokerOps::end();
    m_stroker->end();
}

QT_END_NAMESPACE

#endif // QSTROKER_P_H
