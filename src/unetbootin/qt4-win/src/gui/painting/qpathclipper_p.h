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

#ifndef QPATHCLIPPER_P_H
#define QPATHCLIPPER_P_H

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

#include <QtGui/qpainterpath.h>
#include <QtCore/qlist.h>

#include <private/qbezier_p.h>
#include <private/qdatabuffer_p.h>
#include <stdio.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QWingedEdge;

class Q_AUTOTEST_EXPORT QPathClipper
{
public:
    enum Operation {
        BoolAnd,
        BoolOr,
        BoolSub,
        Simplify
    };
public:
    QPathClipper(const QPainterPath &subject,
                 const QPainterPath &clip);

    QPainterPath clip(Operation op = BoolAnd);

    bool intersect();
    bool contains();

private:
    Q_DISABLE_COPY(QPathClipper)

    enum ClipperMode {
        ClipMode, // do the full clip
        CheckMode // for contains/intersects (only interested in whether the result path is non-empty)
    };

    bool handleCrossingEdges(QWingedEdge &list, qreal y, ClipperMode mode);
    bool doClip(QWingedEdge &list, ClipperMode mode);

    QPainterPath subjectPath;
    QPainterPath clipPath;
    Operation op;

    int aMask;
    int bMask;
};

struct QPathVertex
{
public:
    QPathVertex(const QPointF &p = QPointF(), int e = -1);
    operator QPointF() const;

    int edge;

    qreal x;
    qreal y;
};

class QPathEdge
{
public:
    enum Traversal {
        RightTraversal,
        LeftTraversal
    };

    enum Direction {
        Forward,
        Backward
    };

    enum Type {
        Line,
        Curve
    };

    QPathEdge(int a = -1, int b = -1);

    mutable int flag;

    int windingA;
    int windingB;

    int first;
    int second;

    qreal angle;
    qreal invAngle;

    const QBezier *bezier;
    qreal t0;
    qreal t1;

    int next(Traversal traversal, Direction direction) const;

    void setNext(Traversal traversal, Direction direction, int next);
    void setNext(Direction direction, int next);

    Direction directionTo(int vertex) const;
    int vertex(Direction direction) const;

    bool isBezier() const;

private:
    int m_next[2][2];
};

class QPathSegments
{
public:
    struct Intersection {
        int vertex;
        qreal t;

        int next;

        bool operator<(const Intersection &o) const {
            return t < o.t;
        }
    };

    struct Segment {
        Segment(int pathId, int vertexA, int vertexB, int bezierIndex = -1)
            : path(pathId)
            , bezier(bezierIndex)
            , va(vertexA)
            , vb(vertexB)
            , intersection(-1)
        {
        }

        int path;
        int bezier;

        // vertices
        int va;
        int vb;

        // intersection index
        int intersection;

        QRectF bounds;
    };


    QPathSegments();

    void setPath(const QPainterPath &path);
    void addPath(const QPainterPath &path);

    int intersections() const;
    int segments() const;
    int points() const;

    const Segment &segmentAt(int index) const;
    const QLineF lineAt(int index) const;
    const QBezier *bezierAt(int index) const;
    const QRectF &elementBounds(int index) const;
    int pathId(int index) const;

    const QPointF &pointAt(int vertex) const;
    int addPoint(const QPointF &point);

    const Intersection *intersectionAt(int index) const;
    void addIntersection(int index, const Intersection &intersection);

    void mergePoints();

private:
    QDataBuffer<QPointF> m_points;
    QDataBuffer<Segment> m_segments;
    QDataBuffer<QBezier> m_beziers;
    QDataBuffer<Intersection> m_intersections;

    int m_pathId;
};

class Q_AUTOTEST_EXPORT QWingedEdge
{
public:
    struct TraversalStatus
    {
        int edge;
        QPathEdge::Traversal traversal;
        QPathEdge::Direction direction;

        void flipDirection();
        void flipTraversal();

        void flip();
    };

    QWingedEdge();
    QWingedEdge(const QPainterPath &subject, const QPainterPath &clip);

    void simplify();
    QPainterPath toPath() const;

    int edgeCount() const;

    QPathEdge *edge(int edge);
    const QPathEdge *edge(int edge) const;

    int vertexCount() const;

    int addVertex(const QPointF &p);

    QPathVertex *vertex(int vertex);
    const QPathVertex *vertex(int vertex) const;

    TraversalStatus next(const TraversalStatus &status) const;

    int addEdge(const QPointF &a, const QPointF &b, const QBezier *bezier = 0, qreal t0 = 0, qreal t1 = 1);
    int addEdge(int vertexA, int vertexB, const QBezier *bezier = 0, qreal t0 = 0, qreal t1 = 1);

    bool isInside(qreal x, qreal y) const;

    static QPathEdge::Traversal flip(QPathEdge::Traversal traversal);
    static QPathEdge::Direction flip(QPathEdge::Direction direction);

private:
    void intersectAndAdd();

    void printNode(int i, FILE *handle);

    QBezier bezierFromIndex(int index) const;

    void removeEdge(int ei);
    void addBezierEdge(const QBezier *bezier, const QPointF &a, const QPointF &b, qreal alphaA, qreal alphaB, int path);
    void addBezierEdge(const QBezier *bezier, int vertexA, int vertexB, qreal alphaA, qreal alphaB, int path);

    int insert(const QPathVertex &vertex);
    TraversalStatus findInsertStatus(int vertex, int edge) const;

    qreal delta(int vertex, int a, int b) const;

    QDataBuffer<QPathEdge> m_edges;
    QDataBuffer<QPathVertex> m_vertices;

    QVector<qreal> m_splitPoints;

    QPathSegments m_segments;
};

inline QPathEdge::QPathEdge(int a, int b)
    : flag(0)
    , windingA(0)
    , windingB(0)
    , first(a)
    , second(b)
    , bezier(0)
{
    m_next[0][0] = -1;
    m_next[1][0] = -1;
    m_next[0][0] = -1;
    m_next[1][0] = -1;
}

inline int QPathEdge::next(Traversal traversal, Direction direction) const
{
    return m_next[int(traversal)][int(direction)];
}

inline void QPathEdge::setNext(Traversal traversal, Direction direction, int next)
{
    m_next[int(traversal)][int(direction)] = next;
}

inline void QPathEdge::setNext(Direction direction, int next)
{
    m_next[0][int(direction)] = next;
    m_next[1][int(direction)] = next;
}

inline QPathEdge::Direction QPathEdge::directionTo(int vertex) const
{
    return first == vertex ? Backward : Forward;
}

inline int QPathEdge::vertex(Direction direction) const
{
    return direction == Backward ? first : second;
}

inline bool QPathEdge::isBezier() const
{
    return bezier >= 0;
}

inline QPathVertex::QPathVertex(const QPointF &p, int e)
    : edge(e)
    , x(p.x())
    , y(p.y())
{
}

inline QPathVertex::operator QPointF() const
{
    return QPointF(x, y);
}

inline QPathSegments::QPathSegments()
{
}

inline int QPathSegments::segments() const
{
    return m_segments.size();
}

inline int QPathSegments::points() const
{
    return m_points.size();
}

inline const QPointF &QPathSegments::pointAt(int i) const
{
    return m_points.at(i);
}

inline int QPathSegments::addPoint(const QPointF &point)
{
    m_points << point;
    return m_points.size() - 1;
}

inline const QPathSegments::Segment &QPathSegments::segmentAt(int index) const
{
    return m_segments.at(index);
}

inline const QLineF QPathSegments::lineAt(int index) const
{
    const Segment &segment = m_segments.at(index);
    return QLineF(m_points.at(segment.va), m_points.at(segment.vb));
}

inline const QBezier *QPathSegments::bezierAt(int index) const
{
    const Segment &segment = m_segments.at(index);
    if (segment.bezier >= 0)
        return &m_beziers.at(segment.bezier);
    else
        return 0;
}

inline const QRectF &QPathSegments::elementBounds(int index) const
{
    return m_segments.at(index).bounds;
}

inline int QPathSegments::pathId(int index) const
{
    return m_segments.at(index).path;
}

inline const QPathSegments::Intersection *QPathSegments::intersectionAt(int index) const
{
    const int intersection = m_segments.at(index).intersection;
    if (intersection < 0)
        return 0;
    else
        return &m_intersections.at(intersection);
}

inline int QPathSegments::intersections() const
{
    return m_intersections.size();
}

inline void QPathSegments::addIntersection(int index, const Intersection &intersection)
{
    m_intersections << intersection;

    Segment &segment = m_segments.at(index);
    if (segment.intersection < 0) {
        segment.intersection = m_intersections.size() - 1;
    } else {
        Intersection *isect = &m_intersections.at(segment.intersection);

        while (isect->next != 0)
            isect += isect->next;

        isect->next = (m_intersections.size() - 1) - (isect - m_intersections.data());
    }
}

inline void QWingedEdge::TraversalStatus::flipDirection()
{
    direction = QWingedEdge::flip(direction);
}

inline void QWingedEdge::TraversalStatus::flipTraversal()
{
    traversal = QWingedEdge::flip(traversal);
}

inline void QWingedEdge::TraversalStatus::flip()
{
    flipDirection();
    flipTraversal();
}

inline int QWingedEdge::edgeCount() const
{
    return m_edges.size();
}

inline QPathEdge *QWingedEdge::edge(int edge)
{
    return edge < 0 ? 0 : &m_edges.at(edge);
}

inline const QPathEdge *QWingedEdge::edge(int edge) const
{
    return edge < 0 ? 0 : &m_edges.at(edge);
}

inline int QWingedEdge::vertexCount() const
{
    return m_vertices.size();
}

inline int QWingedEdge::addVertex(const QPointF &p)
{
    m_vertices << p;
    return m_vertices.size() - 1;
}

inline QPathVertex *QWingedEdge::vertex(int vertex)
{
    return vertex < 0 ? 0 : &m_vertices.at(vertex);
}

inline const QPathVertex *QWingedEdge::vertex(int vertex) const
{
    return vertex < 0 ? 0 : &m_vertices.at(vertex);
}

inline QPathEdge::Traversal QWingedEdge::flip(QPathEdge::Traversal traversal)
{
    return traversal == QPathEdge::RightTraversal ? QPathEdge::LeftTraversal : QPathEdge::RightTraversal;
}

inline QPathEdge::Direction QWingedEdge::flip(QPathEdge::Direction direction)
{
    return direction == QPathEdge::Forward ? QPathEdge::Backward : QPathEdge::Forward;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPATHCLIPPER_P_H
