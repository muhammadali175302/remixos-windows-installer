/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtNetwork module of the Qt Toolkit.
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

#ifndef QABSTRACTSOCKET_P_H
#define QABSTRACTSOCKET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "QtNetwork/qabstractsocket.h"
#include "QtCore/qbytearray.h"
#include "QtCore/qlist.h"
#include "QtCore/qtimer.h"
#include "private/qringbuffer_p.h"
#include "private/qiodevice_p.h"
#include "private/qnativesocketengine_p.h"
#include "qnetworkproxy.h"

QT_BEGIN_NAMESPACE

class QHostInfo;

class QAbstractSocketPrivate : public QIODevicePrivate, public QAbstractSocketEngineReceiver
{
    Q_DECLARE_PUBLIC(QAbstractSocket)
public:
    QAbstractSocketPrivate();
    virtual ~QAbstractSocketPrivate();

    // from QAbstractSocketEngineReceiver
    inline void readNotification() { canReadNotification(); }
    inline void writeNotification() { canWriteNotification(); }
    inline void exceptionNotification() {}
    void connectionNotification();
#ifndef QT_NO_NETWORKPROXY
    inline void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator) {
        Q_Q(QAbstractSocket);
        q->proxyAuthenticationRequired(proxy, authenticator);
    }
#endif

    bool canReadNotification();
    bool canWriteNotification();

    // slots
    void _q_connectToNextAddress();
    void _q_startConnecting(const QHostInfo &hostInfo);
    void _q_testConnection();
    void _q_abortConnectionAttempt();

    bool readSocketNotifierCalled;
    bool readSocketNotifierState;
    bool readSocketNotifierStateSet;

    bool emittedReadyRead;
    bool emittedBytesWritten;

    bool abortCalled;
    bool closeCalled;
    bool pendingClose;

    QString hostName;
    quint16 port;
    QHostAddress host;
    QList<QHostAddress> addresses;

    quint16 localPort;
    quint16 peerPort;
    QHostAddress localAddress;
    QHostAddress peerAddress;
    QString peerName;

    QAbstractSocketEngine *socketEngine;
    int cachedSocketDescriptor;

#ifndef QT_NO_NETWORKPROXY
    QNetworkProxy proxy;
    QNetworkProxy proxyInUse;
    void resolveProxy(const QString &hostName, quint16 port);
#else
    inline void resolveProxy(const QString &, quint16) { }
#endif
    inline void resolveProxy(quint16 port) { resolveProxy(QString(), port); }

    void resetSocketLayer();
    bool flush();

    bool initSocketLayer(QAbstractSocket::NetworkLayerProtocol protocol);
    void startConnectingByName(const QString &host);
    void fetchConnectionParameters();
    void setupSocketNotifiers();
    bool readFromSocket();

#ifdef Q_OS_LINUX
    qint64 addToBytesAvailable;
#endif
    qint64 readBufferMaxSize;
    QRingBuffer readBuffer;
    QRingBuffer writeBuffer;

    bool isBuffered;
    int blockingTimeout;

    QTimer *connectTimer;
    int connectTimeElapsed;

    int hostLookupId;

    QAbstractSocket::SocketType socketType;
    QAbstractSocket::SocketState state;

    QAbstractSocket::SocketError socketError;
};

QT_END_NAMESPACE

#endif // QABSTRACTSOCKET_P_H
