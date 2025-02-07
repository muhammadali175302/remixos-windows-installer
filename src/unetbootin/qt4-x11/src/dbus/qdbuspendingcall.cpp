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

#include "qdbuspendingcall.h"
#include "qdbuspendingcall_p.h"

#include "qdbusconnection_p.h"
#include "qdbusmetatype_p.h"
#include "qcoreapplication.h"
#include "qcoreevent.h"
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QDBusPendingCall
    \inmodule QtDBus
    \since 4.5

    \brief The QDBusPendingCall class refers to one pending asynchronous call

    A QDBusPendingCall object is a reference to a method call that was
    sent over D-Bus without waiting for a reply. QDBusPendingCall is an
    opaque type, meant to be used as a handle for a pending reply.

    In most programs, the QDBusPendingCall class will not be used
    directly. It can be safely replaced with the template-based
    QDBusPendingReply, in order to access the contents of the reply or
    wait for it to be complete.

    The QDBusPendingCallWatcher class allows one to connect to a signal
    that will indicate when the reply has arrived or if the call has
    timed out. It also provides the
    QDBusPendingCallWatcher::waitForFinished() method which will suspend
    the execution of the program until the reply has arrived.

    \note If you create a copy of a QDBusPendingCall object, all
          information will be shared among the many copies. Therefore,
          QDBusPendingCall is an explicitly-shared object and does not
          provide a method of detaching the copies (since they refer
          to the same pending call)

    \sa QDBusPendingReply, QDBusPendingCallWatcher,
        QDBusAbstractInterface::asyncCall()
*/

/*!
    \class QDBusPendingCallWatcher
    \inmodule QtDBus
    \since 4.5

    \brief The QDBusPendingCallWatcher class provides a convenient way for
    waiting for asynchronous replies

    The QDBusPendingCallWatcher provides the finished() signal that will be
    emitted when a reply arrives.

    It is usually used like the following example:

    \snippet doc/src/snippets/code/src.qdbus.qdbuspendingcall.cpp 0

    Note that it is not necessary to keep the original QDBusPendingCall
    object around since QDBusPendingCallWatcher inherits from that class
    too.

    The slot connected to by the above code could be something similar
    to the following:

    \snippet doc/src/snippets/code/src.qdbus.qdbuspendingcall.cpp 1

    Note the use of QDBusPendingReply to validate the argument types in
    the reply. If the reply did not contain exactly two arguments
    (one string and one QByteArray), QDBusPendingReply::isError() will
    return true.

    \sa QDBusPendingReply, QDBusAbstractInterface::asyncCall()
*/

/*!
    \fn void QDBusPendingCallWatcher::finished(QDBusPendingCallWatcher *self)

    This signal is emitted when the pending call has finished and its
    reply is available. The \a self parameter is a pointer to the
    object itself, passed for convenience so that the slot can access
    the properties and determine the contents of the reply.
*/

void QDBusPendingCallWatcherHelper::add(QDBusPendingCallWatcher *watcher)
{
    connect(this, SIGNAL(finished()), watcher, SLOT(_q_finished()), Qt::QueuedConnection);
}

QDBusPendingCallPrivate::~QDBusPendingCallPrivate()
{
    if (pending) {
        q_dbus_pending_call_cancel(pending);
        q_dbus_pending_call_unref(pending);
    }
    delete watcherHelper;
}

bool QDBusPendingCallPrivate::setReplyCallback(QObject *target, const char *member)
{
    receiver = target;
    metaTypes.clear();
    methodIdx = -1;
    if (!target)
        return true;;           // unsetting

    if (!member || !*member) {
        // would not be able to deliver a reply
        qWarning("QDBusPendingCall::setReplyCallback: error: cannot deliver a reply to %s::%s (%s)",
                 target ? target->metaObject()->className() : "(null)",
                 member ? member + 1 : "(null)",
                 target ? qPrintable(target->objectName()) : "no name");
        return false;
    }

    methodIdx = QDBusConnectionPrivate::findSlot(target, member + 1, metaTypes);
    if (methodIdx == -1) {
        QByteArray normalizedName = QMetaObject::normalizedSignature(member + 1);
        methodIdx = QDBusConnectionPrivate::findSlot(target, normalizedName, metaTypes);
    }
    if (methodIdx == -1) {
        // would not be able to deliver a reply
        qWarning("QDBusPendingCall::setReplyCallback: error: cannot deliver a reply to %s::%s (%s)",
                 target->metaObject()->className(),
                 member + 1, qPrintable(target->objectName()));
        return false;
    }

    // success
    // construct the expected signature
    int count = metaTypes.count() - 1;
    if (count == 1 && metaTypes.at(1) == QDBusMetaTypeId::message) {
        // wildcard slot, can receive anything, so don't set the signature
        return true;
    }

    if (metaTypes.at(count) == QDBusMetaTypeId::message)
        --count;

    // QList<int> is actually a vector
    // kids, don't try this at home
    setMetaTypes(count, count ? &metaTypes.at(1) : 0);
    return true;
}

void QDBusPendingCallPrivate::setMetaTypes(int count, const int *types)
{
    expectedReplyCount = count;
    if (count == 0) {
        expectedReplySignature = QLatin1String(""); // not null
        return;
    }

    QByteArray sig;
    sig.reserve(count + count / 2);
    for (int i = 0; i < count; ++i) {
        const char *typeSig = QDBusMetaType::typeToSignature(types[i]);
        if (!typeSig) {
            qFatal("QDBusPendingReply: type %s is not registered with QtDBus",
                   QMetaType::typeName(types[i]));
        }
        sig += typeSig;
    }

    expectedReplySignature = QString::fromLatin1(sig);
}

void QDBusPendingCallPrivate::checkReceivedSignature()
{
    if (replyMessage.type() == QDBusMessage::InvalidMessage)
        return;                 // not yet finished - no message to
                                // validate against
    if (replyMessage.type() == QDBusMessage::ErrorMessage)
        return;                 // we don't have to check the signature of an error reply

    if (expectedReplySignature.isNull())
        return;                 // no signature to validate against

    // can't use startsWith here because a null string doesn't start or end with an empty string
    if (!replyMessage.signature().indexOf(expectedReplySignature) == 0) {
        QString errorMsg = QLatin1String("Unexpected reply signature: got \"%1\", "
                                         "expected \"%2\"");
        replyMessage = QDBusMessage::createError(
            QDBusError::InvalidSignature,
            errorMsg.arg(replyMessage.signature(), expectedReplySignature));

    }
}

void QDBusPendingCallPrivate::waitForFinished()
{
    if (replyMessage.type() != QDBusMessage::InvalidMessage)
        return;                 // already finished

    connection->waitForFinished(this);
}

/*!
    Creates a copy of the \a other pending asynchronous call. Note
    that both objects will refer to the same pending call.
*/
QDBusPendingCall::QDBusPendingCall(const QDBusPendingCall &other)
    : d(other.d)
{
}

/*!
    \internal
*/
QDBusPendingCall::QDBusPendingCall(QDBusPendingCallPrivate *dd)
    : d(dd)
{
}

/*!
    Destroys this copy of the QDBusPendingCall object. If this copy is
    also the last copy of a pending asynchronous call, the call will
    be canceled and no further notifications will be received. There
    will be no way of accessing the reply's contents when it arrives.
*/
QDBusPendingCall::~QDBusPendingCall()
{
    // d deleted by QExplicitlySharedDataPointer
}


/*!
    Creates a copy of the \a other pending asynchronous call and drops
    the reference to the previously-referenced call. Note that both
    objects will refer to the same pending call after this function.

    If this object contained the last reference of a pending
    asynchronous call, the call will be canceled and no further
    notifications will be received. There will be no way of accessing
    the reply's contents when it arrives.
*/
QDBusPendingCall &QDBusPendingCall::operator=(const QDBusPendingCall &other)
{
    d = other.d;
    return *this;
}

/*!
    \fn bool QDBusPendingCallWatcher::isFinished() const

    Returns true if the pending call has finished processing and the
    reply has been received.

    Note that this function only changes state if you call
    waitForFinished() or if an external D-Bus event happens, which in
    general only happens if you return to the event loop execution.

    \sa QDBusPendingReply::isFinished()
*/
/*!
    \fn bool QDBusPendingReply::isFinished() const

    Returns true if the pending call has finished processing and the
    reply has been received. If this function returns true, the
    isError(), error() and reply() methods should return valid
    information.

    Note that this function only changes state if you call
    waitForFinished() or if an external D-Bus event happens, which in
    general only happens if you return to the event loop execution.

    \sa QDBusPendingCallWatcher::isFinished()
*/

bool QDBusPendingCall::isFinished() const
{
    return d && (d->replyMessage.type() != QDBusMessage::InvalidMessage);
}

void QDBusPendingCall::waitForFinished()
{
    if (d) d->waitForFinished();
}

/*!
    \fn bool QDBusPendingReply::isValid() const

    Returns true if the reply contains a normal reply message, false
    if it contains anything else.

    If the pending call has not finished processing, this function
    return false.
*/
bool QDBusPendingCall::isValid() const
{
    return d ? d->replyMessage.type() == QDBusMessage::ReplyMessage : false;
}

/*!
    \fn bool QDBusPendingReply::isError() const

    Returns true if the reply contains an error message, false if it
    contains a normal method reply.

    If the pending call has not finished processing, this function
    also returns true.
*/
bool QDBusPendingCall::isError() const
{
    return d ? d->replyMessage.type() == QDBusMessage::ErrorMessage : true;
}

/*!
    \fn QDBusError QDBusPendingReply::error() const

    Retrieves the error content of the reply message, if it has
    finished processing. If the reply message has not finished
    processing or if it contains a normal reply message (non-error),
    this function returns an invalid QDBusError.
*/
QDBusError QDBusPendingCall::error() const
{
    if (d)
        return d->replyMessage;

    // not connected, return an error
    QDBusError err = QDBusError(QDBusError::Disconnected,
                                QLatin1String("Not connected to D-Bus server"));
    return err;
}

/*!
    \fn QDBusMessage QDBusPendingReply::reply() const

    Retrieves the reply message received for the asynchronous call
    that was sent, if it has finished processing. If the pending call
    is not finished, this function returns a QDBusMessage of type
    QDBusMessage::InvalidMessage.

    After it has finished processing, the message type will either be
    an error message or a normal method reply message.
*/
QDBusMessage QDBusPendingCall::reply() const
{
    return d ? d->replyMessage : QDBusMessage::createError(error());
}

#if 0
/*!
    Sets the slot \a member in object \a target to be called when the
    reply arrives. The slot's parameter list must match the reply
    message's arguments for it to be called.

    It may, optionally, contain a QDBusMessage final parameter. If it
    is present, the parameter will contain the reply message object.

    The callback will not be called if the reply is an error message.

    This function returns true if it could set the callback, false
    otherwise. It is not a guarantee that the callback will be
    called.

    \warning QDBusPendingCall only supports one callback per pending
             asynchronous call, even if multiple QDBusPendingCall
             objects are referencing the same pending call.
*/
bool QDBusPendingCall::setReplyCallback(QObject *target, const char *member)
{
    if (!d)
        return false;

    return d->setReplyCallback(target, member);
}
#endif


class QDBusPendingCallWatcherPrivate: public QObjectPrivate
{
public:
    void _q_finished();

    Q_DECLARE_PUBLIC(QDBusPendingCallWatcher)
};

inline void QDBusPendingCallWatcherPrivate::_q_finished()
{
    Q_Q(QDBusPendingCallWatcher);
    emit q->finished(q);
}

/*!
    Creates a QDBusPendingCallWatcher object to watch for replies on the
    asynchronous pending call \a call and sets this object's parent
    to \a parent.
*/
QDBusPendingCallWatcher::QDBusPendingCallWatcher(const QDBusPendingCall &call, QObject *parent)
    : QObject(*new QDBusPendingCallWatcherPrivate, parent), QDBusPendingCall(call)
{
    if (d) {
        if (!d->watcherHelper)
            d->watcherHelper = new QDBusPendingCallWatcherHelper;
        d->watcherHelper->add(this);
    }
}

/*!
    Destroys this object. If this QDBusPendingCallWatcher object was the
    last reference to the unfinished pending call, the call will be
    canceled.
*/
QDBusPendingCallWatcher::~QDBusPendingCallWatcher()
{
}

/*!
    \fn void QDBusPendingCallWatcher::waitForFinished()

    Suspends the execution of the calling thread until the reply is
    received and processed. After this function returns, isFinished()
    should return true, indicating the reply's contents are ready to
    be processed.

    \sa QDBusPendingReply::waitForFinished()
*/
void QDBusPendingCallWatcher::waitForFinished()
{
    if (d) {
        d->waitForFinished();

        // our signals were queued, so deliver them
        QCoreApplication::sendPostedEvents(this, QEvent::MetaCall);
    }
}
QT_END_NAMESPACE

#include "moc_qdbuspendingcall.cpp"
