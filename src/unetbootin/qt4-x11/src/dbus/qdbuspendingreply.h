/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDBus module of the Qt Toolkit.
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

#ifndef QDBUSPENDINGREPLY_H
#define QDBUSPENDINGREPLY_H

#include <QtCore/qglobal.h>
#include <QtDBus/qdbusmacros.h>
#include <QtDBus/qdbusargument.h>
#include <QtDBus/qdbuspendingcall.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(DBus)

class QDBUS_EXPORT QDBusPendingReplyData: public QDBusPendingCall
{
protected:
    QDBusPendingReplyData();
    ~QDBusPendingReplyData();
    void assign(const QDBusPendingCall &call);
    void assign(const QDBusMessage &message);

    QVariant argumentAt(int index) const;
    void setMetaTypes(int count, const int *metaTypes);
};

namespace QDBusPendingReplyTypes {
    template<int Index,
             typename T1, typename T2, typename T3, typename T4,
             typename T5, typename T6, typename T7, typename T8>
    struct Select
    {
        typedef Select<Index - 1, T2, T3, T4, T5, T6, T7, T8, void> Next;
        typedef typename Next::Type Type;
    };
    template<typename T1, typename T2, typename T3, typename T4,
             typename T5, typename T6, typename T7, typename T8>
    struct Select<0, T1, T2, T3, T4, T5, T6, T7, T8>
    {
        typedef T1 Type;
    };

    template<typename T1> inline int metaTypeFor(T1 * = 0)
    { return qMetaTypeId<T1>(); }
    // specialise for QVariant, allowing it to be used in place of QDBusVariant
    template<> inline int metaTypeFor<QVariant>(QVariant *)
    { return qMetaTypeId<QDBusVariant>(); }

    template<typename T1, typename T2, typename T3, typename T4,
             typename T5, typename T6, typename T7, typename T8>
    struct ForEach
    {
        typedef ForEach<T2, T3, T4, T5, T6, T7, T8, void> Next;
        enum { Total = Next::Total + 1 };
        static inline void fillMetaTypes(int *p)
        {
            *p = metaTypeFor<T1>();
            Next::fillMetaTypes(++p);
        }
    };
    template<>
    struct ForEach<void, void, void, void,   void, void, void, void>
    {
        enum { Total = 0 };
        static inline void fillMetaTypes(int *)
        { }
    };
} // namespace QDBusPendingReplyTypes

template<typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void,
         typename T5 = void, typename T6 = void, typename T7 = void, typename T8 = void>
class QDBusPendingReply:
#ifdef Q_QDOC
    public QDBusPendingCall
#else
    public QDBusPendingReplyData
#endif
{
    typedef QDBusPendingReplyTypes::ForEach<T1, T2, T3, T4, T5, T6, T7, T8> ForEach;
    template<int Index> struct Select :
    QDBusPendingReplyTypes::Select<Index, T1, T2, T3, T4, T5, T6, T7, T8>
    {
    };

public:
    enum { Count = ForEach::Total };

    inline QDBusPendingReply()
    { }
    inline QDBusPendingReply(const QDBusPendingReply &other)
        : QDBusPendingReplyData(other)
    { }
    inline QDBusPendingReply(const QDBusPendingCall &call)
    { *this = call; }
    inline QDBusPendingReply(const QDBusMessage &message)
    { *this = message; }
    inline QDBusPendingReply &operator=(const QDBusPendingReply &other)
    { assign(other); return *this; }
    inline QDBusPendingReply &operator=(const QDBusPendingCall &call)
    { assign(call); return *this; }
    inline QDBusPendingReply &operator=(const QDBusMessage &message)
    { assign(message); return *this; }

    inline int count() const { return Count; }

#if defined(Q_QDOC) || defined(Q_NO_USING_KEYWORD)
    inline QVariant argumentAt(int index) const
    { return QDBusPendingReplyData::argumentAt(index); }
#else
    using QDBusPendingReplyData::argumentAt;
#endif

#if defined(Q_QDOC)
    bool isFinished() const;
    void waitForFinished();

    bool isValid() const;
    bool isError() const;
    QDBusError error() const;
    QDBusMessage reply() const;

    template<int Index> inline Type argumentAt() const;
    inline T1 value() const;
    inline operator T1() const;
#else
    template<int Index> inline
    const typename Select<Index>::Type argumentAt() const
    {
        // static assert?
        Q_ASSERT_X(Index < count() && Index >= 0, "QDBusPendingReply::argumentAt",
                   "Index out of bounds");
        typedef typename Select<Index>::Type ResultType;
        return qdbus_cast<ResultType>(argumentAt(Index));
    }

    inline typename Select<0>::Type value() const
    {
        return argumentAt<0>();
    }

    inline operator typename Select<0>::Type() const
    {
        return argumentAt<0>();
    }
#endif

private:
    inline void calculateMetaTypes()
    {
        int typeIds[Count > 0 ? Count : 1]; // use at least one since zero-sized arrays aren't valid
        ForEach::fillMetaTypes(typeIds);
        setMetaTypes(Count, typeIds);
    }

    inline void assign(const QDBusPendingCall &call)
    {
        QDBusPendingReplyData::assign(call);
        calculateMetaTypes();
    }

    inline void assign(const QDBusMessage &message)
    {
        QDBusPendingReplyData::assign(message);
        calculateMetaTypes();
    }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
