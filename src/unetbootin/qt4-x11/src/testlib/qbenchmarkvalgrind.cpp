
/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#include "QtTest/private/qbenchmark_p.h"

#ifdef QTESTLIB_USE_VALGRIND

#include "QtTest/private/qbenchmarkvalgrind_p.h"
#include <QtCore/qstringlist.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qprocess.h>
#include <QtCore/qdir.h>
#include <QtCore/qset.h>
#include "3rdparty/callgrind_p.h"

// Returns true iff a sufficiently recent valgrind is available.
bool QBenchmarkValgrindUtils::haveValgrind()
{
#ifdef NVALGRIND
    return false;
#else
    QProcess process;
    QStringList args;
    args << QLatin1String("--version");
    process.start(QLatin1String("valgrind"), args);
    if (!process.waitForFinished(-1))
        return false;
    const QByteArray out = process.readAllStandardOutput();
    const QRegExp rx(QLatin1String("^valgrind-([0-9]).([0-9]).[0-9]"));
    if (rx.indexIn(QLatin1String(out.data())) == -1)
        return false;
    bool ok;
    const int major = rx.cap(1).toInt(&ok);
    if (!ok)
        return false;
    const int minor = rx.cap(2).toInt(&ok);
    if (!ok)
        return false;
//    return (major > 3 || (major == 3 && minor >= 3)); // v >= 3.3 for --callgrind-out-file option
    Q_UNUSED(major);
    Q_UNUSED(minor);
    return true; // skip version restriction for now
#endif
}

// Reruns this program through callgrind.
// Returns true upon success, otherwise false.
bool QBenchmarkValgrindUtils::rerunThroughCallgrind(const QStringList &origAppArgs, int &exitCode)
{
    if (!QBenchmarkValgrindUtils::runCallgrindSubProcess(origAppArgs, exitCode)) {
        qWarning("failed to run callgrind subprocess");
        return false;
    }
    return true;
}

static void dumpOutput(const QByteArray &data, FILE *fh)
{
    QFile file;
    file.open(fh, QIODevice::WriteOnly);
    file.write(data);
}

qint64 QBenchmarkValgrindUtils::extractResult(const QString &fileName)
{
    QFile file(fileName);
    const bool openOk = file.open(QIODevice::ReadOnly | QIODevice::Text);
    Q_ASSERT(openOk);

    qint64 val = -1;
    bool valSeen = false;
    const QRegExp rxValue(QLatin1String("^summary: (\\d+)"));
    while (!file.atEnd()) {
        const QString line(QLatin1String(file.readLine()));
        if (rxValue.indexIn(line) != -1) {
            Q_ASSERT(rxValue.numCaptures() == 1);
            bool ok;
            val = rxValue.cap(1).toLongLong(&ok);
            Q_ASSERT(ok);
            valSeen = true;
            break;
        }
    }
    Q_ASSERT(valSeen);
    return val;
}

// Gets the newest file name (i.e. the one with the highest integer suffix).
QString QBenchmarkValgrindUtils::getNewestFileName()
{
    QStringList nameFilters;
    QString base = QBenchmarkGlobalData::current->callgrindOutFileBase;
    Q_ASSERT(!base.isEmpty());

    nameFilters << QString(QLatin1String("%1.*")).arg(base);
    QFileInfoList fiList = QDir().entryInfoList(nameFilters, QDir::Files | QDir::Readable);
    Q_ASSERT(!fiList.empty());
    int hiSuffix = -1;
    QFileInfo lastFileInfo;
    const QString pattern = QString(QLatin1String("%1.(\\d+)")).arg(base);
    const QRegExp rx(pattern);
    foreach (QFileInfo fileInfo, fiList) {
        const int index = rx.indexIn(fileInfo.fileName());
        Q_ASSERT(index == 0);
        bool ok;
        const int suffix = rx.cap(1).toInt(&ok);
        Q_ASSERT(ok);
        Q_ASSERT(suffix >= 0);
        if (suffix > hiSuffix) {
            lastFileInfo = fileInfo;
            hiSuffix = suffix;
        }
    }

    return lastFileInfo.fileName();
}

qint64 QBenchmarkValgrindUtils::extractLastResult()
{
    return extractResult(getNewestFileName());
}

void QBenchmarkValgrindUtils::cleanup()
{
    QStringList nameFilters;
    QString base = QBenchmarkGlobalData::current->callgrindOutFileBase;
    Q_ASSERT(!base.isEmpty());
    nameFilters
        << QString(QLatin1String("%1")).arg(base) // overall summary
        << QString(QLatin1String("%1.*")).arg(base); // individual dumps
    QFileInfoList fiList = QDir().entryInfoList(nameFilters, QDir::Files | QDir::Readable);
    foreach (QFileInfo fileInfo, fiList) {
        const bool removeOk = QFile::remove(fileInfo.fileName());
        Q_ASSERT(removeOk);
    }
}

QString QBenchmarkValgrindUtils::outFileBase(qint64 pid)
{
    return QString(QLatin1String("callgrind.out.%1")).arg(
        pid != -1 ? pid : QCoreApplication::applicationPid());
}

// Reruns this program through callgrind, storing callgrind result files in the
// current directory.
// Returns true upon success, otherwise false.
bool QBenchmarkValgrindUtils::runCallgrindSubProcess(const QStringList &origAppArgs, int &exitCode)
{
    const QString execFile(origAppArgs.at(0));
    QStringList args;
    args << QLatin1String("--tool=callgrind") << QLatin1String("--instr-atstart=yes")
         << QLatin1String("--quiet")
         << execFile << QLatin1String("-callgrindchild");

#if (defined Q_WS_QWS)
    // While running the child process, we aren't processing events, and hence aren't
    // acting as the QWS server. Therefore it's necessary to tell the child to act
    // as its own server instead of connecting to us.
    args << QLatin1String("-qws");
#endif

    // pass on original arguments that make sense (e.g. avoid wasting time producing output
    // that will be ignored anyway) ...
    for (int i = 1; i < origAppArgs.size(); ++i) {
        const QString arg(origAppArgs.at(i));
        if (arg == QLatin1String("-callgrind"))
            continue;
        args << arg; // ok to pass on
    }

    QProcess process;
    process.start(QLatin1String("valgrind"), args);
    process.waitForStarted(-1);
    QBenchmarkGlobalData::current->callgrindOutFileBase =
        QBenchmarkValgrindUtils::outFileBase(process.pid());
    const bool finishedOk = process.waitForFinished(-1);
    exitCode = process.exitCode();

    dumpOutput(process.readAllStandardOutput(), stdout);
    dumpOutput(process.readAllStandardError(), stderr);

    return finishedOk;
}

void QBenchmarkCallgrindMeasurer::start()
{
    CALLGRIND_ZERO_STATS;
}

qint64 QBenchmarkCallgrindMeasurer::checkpoint()
{
    CALLGRIND_DUMP_STATS;
    const qint64 result = QBenchmarkValgrindUtils::extractLastResult();
    return result;
}

qint64 QBenchmarkCallgrindMeasurer::stop()
{	
    return checkpoint();
}

bool QBenchmarkCallgrindMeasurer::isMeasurementAccepted(qint64 measurement)
{
    Q_UNUSED(measurement);
    return true;
}

int QBenchmarkCallgrindMeasurer::adjustIterationCount(int)
{ 
    return 1;
}

int QBenchmarkCallgrindMeasurer::adjustMedianCount(int)
{ 
    return 1;
}

bool QBenchmarkCallgrindMeasurer::needsWarmupIteration()
{ 
    return true;
}

QString QBenchmarkCallgrindMeasurer::unitText()
{
    return QLatin1String("instr. loads");
}

QString QBenchmarkCallgrindMeasurer::metricText()
{
    return QLatin1String("callgrind");
}

#endif // QTESTLIB_USE_VALGRIND
