/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#include "qbuiltintypes_p.h"
#include "qitem_p.h"

#include "qschematime_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

SchemaTime::SchemaTime(const QDateTime &dateTime) : AbstractDateTime(dateTime)
{
}

SchemaTime::Ptr SchemaTime::fromLexical(const QString &lexical)
{
    static const CaptureTable captureTable( // STATIC DATA
        /* The extra paranthesis is a build fix for GCC 3.3. */
        (QRegExp(QLatin1String(
                "^\\s*"                             /* Any preceding whitespace. */
                "(\\d{2})"                          /* Hour part */
                ":"                                 /* Delimiter. */
                "(\\d{2})"                          /* Minutes part */
                ":"                                 /* Delimiter. */
                "(\\d{2,})"                         /* Seconds part. */
                "(?:\\.(\\d+))?"                    /* Milli seconds part. */
                "(?:(\\+|-)(\\d{2}):(\\d{2})|(Z))?" /* The zone offset, "+08:24". */
                "\\s*$"                             /* Any terminating whitespace. */))),
        /*zoneOffsetSignP*/         5,
        /*zoneOffsetHourP*/         6,
        /*zoneOffsetMinuteP*/       7,
        /*zoneOffsetUTCSymbolP*/    8,
        /*yearP*/                   -1,
        /*monthP*/                  -1,
        /*dayP*/                    -1,
        /*hourP*/                   1,
        /*minutesP*/                2,
        /*secondsP*/                3,
        /*msecondsP*/               4);

    AtomicValue::Ptr err;
    const QDateTime retval(create(err, lexical, captureTable));

    return err ? err : SchemaTime::Ptr(new SchemaTime(retval));
}

SchemaTime::Ptr SchemaTime::fromDateTime(const QDateTime &dt)
{
    Q_ASSERT(dt.isValid());
    /* Singleton value, allocated once instead of each time it's needed. */
    // STATIC DATA
    static const QDate time_defaultDate(AbstractDateTime::DefaultYear,
                                        AbstractDateTime::DefaultMonth,
                                        AbstractDateTime::DefaultDay);

    QDateTime result;
    copyTimeSpec(dt, result);

    result.setDate(time_defaultDate);
    result.setTime(dt.time());

    return SchemaTime::Ptr(new SchemaTime(result));
}

Item SchemaTime::fromValue(const QDateTime &dt) const
{
    Q_ASSERT(dt.isValid());
    return fromDateTime(dt);
}

QString SchemaTime::stringValue() const
{
    return timeToString() + zoneOffsetToString();
}

ItemType::Ptr SchemaTime::type() const
{
    return BuiltinTypes::xsTime;
}

QT_END_NAMESPACE
