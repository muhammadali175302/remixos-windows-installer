/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtSql module of the Qt Toolkit.
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

#include "qsqlquery.h"

//#define QT_DEBUG_SQL

#include "qatomic.h"
#include "qsqlrecord.h"
#include "qsqlresult.h"
#include "qsqldriver.h"
#include "qsqldatabase.h"
#include "private/qsqlnulldriver_p.h"
#include "qvector.h"
#include "qmap.h"

QT_BEGIN_NAMESPACE

class QSqlQueryPrivate
{
public:
    QSqlQueryPrivate(QSqlResult* result);
    ~QSqlQueryPrivate();
    QAtomicInt ref;
    QSqlResult* sqlResult;
    QSql::NumericalPrecisionPolicy precisionPolicy;

    static QSqlQueryPrivate* shared_null();
};

Q_GLOBAL_STATIC_WITH_ARGS(QSqlQueryPrivate, nullQueryPrivate, (0))
Q_GLOBAL_STATIC(QSqlNullDriver, nullDriver)
Q_GLOBAL_STATIC_WITH_ARGS(QSqlNullResult, nullResult, (nullDriver()))

QSqlQueryPrivate* QSqlQueryPrivate::shared_null()
{
    QSqlQueryPrivate *null = nullQueryPrivate();
    null->ref.ref();
    return null;
}

/*!
\internal
*/
QSqlQueryPrivate::QSqlQueryPrivate(QSqlResult* result)
    : ref(1), sqlResult(result), precisionPolicy(QSql::HighPrecision)
{
    if (!sqlResult)
        sqlResult = nullResult();
}

QSqlQueryPrivate::~QSqlQueryPrivate()
{
    QSqlResult *nr = nullResult();
    if (!nr || sqlResult == nr)
        return;
    delete sqlResult;
}

/*!
    \class QSqlQuery
    \brief The QSqlQuery class provides a means of executing and
    manipulating SQL statements.

    \ingroup database
    \ingroup shared
    \mainclass
    \module sql

    QSqlQuery encapsulates the functionality involved in creating,
    navigating and retrieving data from SQL queries which are
    executed on a \l QSqlDatabase. It can be used to execute DML
    (data manipulation language) statements, such as \c SELECT, \c
    INSERT, \c UPDATE and \c DELETE, as well as DDL (data definition
    language) statements, such as \c{CREATE} \c{TABLE}. It can also
    be used to execute database-specific commands which are not
    standard SQL (e.g. \c{SET DATESTYLE=ISO} for PostgreSQL).

    Successfully executed SQL statements set the query's state to
    active; isActive() then returns true. Otherwise the query's state
    is set to inactive. In either case, when executing a new SQL
    statement, the query is positioned on an invalid record; an
    active query must be navigated to a valid record (so that
    isValid() returns true) before values can be retrieved.

    \target QSqlQuery examples

    Navigating records is performed with the following functions:

    \list
    \o next()
    \o previous()
    \o first()
    \o last()
    \o seek()
    \endlist

    These functions allow the programmer to move forward, backward
    or arbitrarily through the records returned by the query. If you
    only need to move forward through the results (e.g., by using
    next()), you can use setForwardOnly(), which will save a
    significant amount of memory overhead and improve performance on
    some databases. Once an active query is positioned on a valid
    record, data can be retrieved using value(). All data is
    transferred from the SQL backend using QVariants.

    For example:

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 7

    To access the data returned by a query, use value(int). Each
    field in the data returned by a \c SELECT statement is accessed
    by passing the field's position in the statement, starting from
    0. This makes using \c{SELECT *} queries inadvisable because the
    order of the fields returned is indeterminate.

    For the sake of efficiency, there are no functions to access a
    field by name (unless you use prepared queries with names, as
    explained below). To convert a field name into an index, use
    record().\l{QSqlRecord::indexOf()}{indexOf()}, for example:

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 8

    QSqlQuery supports prepared query execution and the binding of
    parameter values to placeholders. Some databases don't support
    these features, so for those, Qt emulates the required
    functionality. For example, the Oracle and ODBC drivers have
    proper prepared query support, and Qt makes use of it; but for
    databases that don't have this support, Qt implements the feature
    itself, e.g. by replacing placeholders with actual values when a
    query is executed. Use numRowsAffected() to find out how many rows
    were affected by a non-\c SELECT query, and size() to find how
    many were retrieved by a \c SELECT.

    Oracle databases identify placeholders by using a colon-name
    syntax, e.g \c{:name}. ODBC simply uses \c ? characters. Qt
    supports both syntaxes, with the restriction that you can't mix
    them in the same query.

    You can retrieve the values of all the fields in a single variable
    (a map) using boundValues().

    \section1 Approaches to Binding Values

    Below we present the same example using each of the four
    different binding approaches, as well as one example of binding
    values to a stored procedure.

    \bold{Named binding using named placeholders:}

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 9

    \bold{Positional binding using named placeholders:}

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 10

    \bold{Binding values using positional placeholders (version 1):}

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 11

    \bold{Binding values using positional placeholders (version 2):}

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 12

    \bold{Binding values to a stored procedure:}

    This code calls a stored procedure called \c AsciiToInt(), passing
    it a character through its in parameter, and taking its result in
    the out parameter.

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 13

    Note that unbound parameters will retain their values.

    Stored procedures that uses the return statement to return values,
    or return multiple result sets, are not fully supported. For specific
    details see \l{SQL Database Drivers}.

    \warning You must load the SQL driver and open the connection before a
    QSqlQuery is created. Also, the connection must remain open while the
    query exists; otherwise, the behavior of QSqlQuery is undefined.

    \sa QSqlDatabase, QSqlQueryModel, QSqlTableModel, QVariant
*/

/*!
    Constructs a QSqlQuery object which uses the QSqlResult \a result
    to communicate with a database.
*/

QSqlQuery::QSqlQuery(QSqlResult *result)
{
    d = new QSqlQueryPrivate(result);
}

/*!
    Destroys the object and frees any allocated resources.
*/

QSqlQuery::~QSqlQuery()
{
    if (!d->ref.deref())
        delete d;
}

/*!
    Constructs a copy of \a other.
*/

QSqlQuery::QSqlQuery(const QSqlQuery& other)
{
    d = other.d;
    d->ref.ref();
}

/*!
    \internal
*/
static void qInit(QSqlQuery *q, const QString& query, QSqlDatabase db)
{
    QSqlDatabase database = db;
    if (!database.isValid())
        database = QSqlDatabase::database(QLatin1String(QSqlDatabase::defaultConnection), false);
    if (database.isValid()) {
        *q = QSqlQuery(database.driver()->createResult());
    }
    if (!query.isEmpty())
        q->exec(query);
}

/*!
    Constructs a QSqlQuery object using the SQL \a query and the
    database \a db. If \a db is not specified, the application's
    default database is used. If \a query is not an empty string, it
    will be executed.

    \sa QSqlDatabase
*/
QSqlQuery::QSqlQuery(const QString& query, QSqlDatabase db)
{
    d = QSqlQueryPrivate::shared_null();
    qInit(this, query, db);
}

/*!
    Constructs a QSqlQuery object using the database \a db.

    \sa QSqlDatabase
*/

QSqlQuery::QSqlQuery(QSqlDatabase db)
{
    d = QSqlQueryPrivate::shared_null();
    qInit(this, QString(), db);
}


/*!
    Assigns \a other to this object.
*/

QSqlQuery& QSqlQuery::operator=(const QSqlQuery& other)
{
    qAtomicAssign(d, other.d);
    return *this;
}

/*!
    Returns true if the query is active and positioned on a valid
    record and the \a field is NULL; otherwise returns false. Note
    that for some drivers, isNull() will not return accurate
    information until after an attempt is made to retrieve data.

    \sa isActive(), isValid(), value()
*/

bool QSqlQuery::isNull(int field) const
{
    if (d->sqlResult->isActive() && d->sqlResult->isValid())
        return d->sqlResult->isNull(field);
    return true;
}

/*!
    Executes the SQL in \a query. Returns true and sets the query
    state to active if the query was successful; otherwise returns
    false. The \a query string must use syntax appropriate for the
    SQL database being queried (for example, standard SQL).

    After the query is executed, the query is positioned on an \e
    invalid record and must be navigated to a valid record before
    data values can be retrieved (for example, using next()).

    Note that the last error for this query is reset when exec() is
    called.

    Example:

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 34

    \sa isActive(), isValid(), next(), previous(), first(), last(),
        seek()
*/

bool QSqlQuery::exec(const QString& query)
{
    if (d->ref != 1) {
        bool fo = isForwardOnly();
        *this = QSqlQuery(driver()->createResult());
        d->sqlResult->setNumericalPrecisionPolicy(d->precisionPolicy);
        setForwardOnly(fo);
    } else {
        d->sqlResult->clear();
        d->sqlResult->setActive(false);
        d->sqlResult->setLastError(QSqlError());
        d->sqlResult->setAt(QSql::BeforeFirstRow);
        d->sqlResult->setNumericalPrecisionPolicy(d->precisionPolicy);
    }
    d->sqlResult->setQuery(query.trimmed());
    if (!driver()->isOpen() || driver()->isOpenError()) {
        qWarning("QSqlQuery::exec: database not open");
        return false;
    }
    if (query.isEmpty()) {
        qWarning("QSqlQuery::exec: empty query");
        return false;
    }
#ifdef QT_DEBUG_SQL
    qDebug("\n QSqlQuery: %s", query.toLocal8Bit().constData());
#endif
    return d->sqlResult->reset(query);
}

/*!
    Returns the value of field \a index in the current record.

    The fields are numbered from left to right using the text of the
    \c SELECT statement, e.g. in

    \snippet doc/src/snippets/code/src.sql.kernel.qsqlquery.cpp 0

    field 0 is \c forename and field 1 is \c
    surname. Using \c{SELECT *} is not recommended because the order
    of the fields in the query is undefined.

    An invalid QVariant is returned if field \a index does not
    exist, if the query is inactive, or if the query is positioned on
    an invalid record.

    \sa previous() next() first() last() seek() isActive() isValid()
*/

QVariant QSqlQuery::value(int index) const
{
    if (isActive() && isValid() && (index > QSql::BeforeFirstRow))
        return d->sqlResult->data(index);
    qWarning("QSqlQuery::value: not positioned on a valid record");
    return QVariant();
}

/*!
    Returns the current internal position of the query. The first
    record is at position zero. If the position is invalid, the
    function returns QSql::BeforeFirstRow or
    QSql::AfterLastRow, which are special negative values.

    \sa previous() next() first() last() seek() isActive() isValid()
*/

int QSqlQuery::at() const
{
    return d->sqlResult->at();
}

/*!
    Returns the text of the current query being used, or an empty
    string if there is no current query text.

    \sa executedQuery()
*/

QString QSqlQuery::lastQuery() const
{
    return d->sqlResult->lastQuery();
}

/*!
    Returns the database driver associated with the query.
*/

const QSqlDriver *QSqlQuery::driver() const
{
    return d->sqlResult->driver();
}

/*!
    Returns the result associated with the query.
*/

const QSqlResult* QSqlQuery::result() const
{
    return d->sqlResult;
}

/*!
    Retrieves the record at position \a index, if available, and
    positions the query on the retrieved record. The first record is
    at position 0. Note that the query must be in an active state and
    isSelect() must return true before calling this function.

    If \a relative is false (the default), the following rules apply:

    \list
    \o If \a index is negative, the result is positioned before the
    first record and false is returned.
    \o Otherwise, an attempt is made to move to the record at position
    \a index. If the record at position \a index could not be retrieved, the
    result is positioned after the last record and false is returned. If
    the record is successfully retrieved, true is returned.
    \endlist

    If \a relative is true, the following rules apply:

    \list
    \o If the result is currently positioned before the first
    record or on the first record, and \a index is negative, there is no
    change, and false is returned.
    \o If the result is currently located after the last record, and
    \a index is positive, there is no change, and false is returned.
    \o If the result is currently located somewhere in the middle,
    and the relative offset \a index moves the result below zero, the
    result is positioned before the first record and false is
    returned.
    \o Otherwise, an attempt is made to move to the record \a index
    records ahead of the current record (or \a index records behind the
    current record if \a index is negative). If the record at offset \a index
    could not be retrieved, the result is positioned after the last
    record if \a index >= 0, (or before the first record if \a index is
    negative), and false is returned. If the record is successfully
    retrieved, true is returned.
    \endlist

    \sa next() previous() first() last() at() isActive() isValid()
*/
bool QSqlQuery::seek(int index, bool relative)
{
    if (!isSelect() || !isActive())
        return false;
    int actualIdx;
    if (!relative) { // arbitrary seek
        if (index < 0) {
            d->sqlResult->setAt(QSql::BeforeFirstRow);
            return false;
        }
        actualIdx = index;
    } else {
        switch (at()) { // relative seek
        case QSql::BeforeFirstRow:
            if (index > 0)
                actualIdx = index;
            else {
                return false;
            }
            break;
        case QSql::AfterLastRow:
            if (index < 0) {
                d->sqlResult->fetchLast();
                actualIdx = at() + index;
            } else {
                return false;
            }
            break;
        default:
            if ((at() + index) < 0) {
                d->sqlResult->setAt(QSql::BeforeFirstRow);
                return false;
            }
            actualIdx = at() + index;
            break;
        }
    }
    // let drivers optimize
    if (isForwardOnly() && actualIdx < at()) {
        qWarning("QSqlQuery::seek: cannot seek backwards in a forward only query");
        return false;
    }
    if (actualIdx == (at() + 1) && at() != QSql::BeforeFirstRow) {
        if (!d->sqlResult->fetchNext()) {
            d->sqlResult->setAt(QSql::AfterLastRow);
            return false;
        }
        return true;
    }
    if (actualIdx == (at() - 1)) {
        if (!d->sqlResult->fetchPrevious()) {
            d->sqlResult->setAt(QSql::BeforeFirstRow);
            return false;
        }
        return true;
    }
    if (!d->sqlResult->fetch(actualIdx)) {
        d->sqlResult->setAt(QSql::AfterLastRow);
        return false;
    }
    return true;
}

/*!
    Retrieves the next record in the result, if available, and
    positions the query on the retrieved record. Note that the result
    must be in an active state and isSelect() must return true before
    calling this function or it will do nothing and return false.

    The following rules apply:

    \list
    \o If the result is currently located before the first
    record, e.g. immediately after a query is executed, an attempt is
    made to retrieve the first record.

    \o If the result is currently located after the last record,
    there is no change and false is returned.

    \o If the result is located somewhere in the middle, an attempt
    is made to retrieve the next record.
    \endlist

    If the record could not be retrieved, the result is positioned after
    the last record and false is returned. If the record is successfully
    retrieved, true is returned.

    \sa previous() first() last() seek() at() isActive() isValid()
*/

bool QSqlQuery::next()
{
    if (!isSelect() || !isActive())
        return false;
    bool b = false;
    switch (at()) {
    case QSql::BeforeFirstRow:
        b = d->sqlResult->fetchFirst();
        return b;
    case QSql::AfterLastRow:
        return false;
    default:
        if (!d->sqlResult->fetchNext()) {
            d->sqlResult->setAt(QSql::AfterLastRow);
            return false;
        }
        return true;
    }
}

/*!
    Retrieves the previous record in the result, if available, and
    positions the query on the retrieved record. Note that the result
    must be in an active state and isSelect() must return true before
    calling this function or it will do nothing and return false.

    The following rules apply:

    \list
    \o If the result is currently located before the first record,
    there is no change and false is returned.

    \o If the result is currently located after the last record, an
    attempt is made to retrieve the last record.

    \o If the result is somewhere in the middle, an attempt is made
    to retrieve the previous record.
    \endlist

    If the record could not be retrieved, the result is positioned
    before the first record and false is returned. If the record is
    successfully retrieved, true is returned.

    \sa next() first() last() seek() at() isActive() isValid()
*/

bool QSqlQuery::previous()
{
    if (!isSelect() || !isActive())
        return false;
    if (isForwardOnly()) {
        qWarning("QSqlQuery::seek: cannot seek backwards in a forward only query");
        return false;
    }

    bool b = false;
    switch (at()) {
    case QSql::BeforeFirstRow:
        return false;
    case QSql::AfterLastRow:
        b = d->sqlResult->fetchLast();
        return b;
    default:
        if (!d->sqlResult->fetchPrevious()) {
            d->sqlResult->setAt(QSql::BeforeFirstRow);
            return false;
        }
        return true;
    }
}

/*!
    Retrieves the first record in the result, if available, and
    positions the query on the retrieved record. Note that the result
    must be in an active state and isSelect() must return true before
    calling this function or it will do nothing and return false.
    Returns true if successful. If unsuccessful the query position is
    set to an invalid position and false is returned.

    \sa next() previous() last() seek() at() isActive() isValid()
*/

bool QSqlQuery::first()
{
    if (!isSelect() || !isActive())
        return false;
    if (isForwardOnly() && at() > QSql::BeforeFirstRow) {
        qWarning("QSqlQuery::seek: cannot seek backwards in a forward only query");
        return false;
    }
    bool b = false;
    b = d->sqlResult->fetchFirst();
    return b;
}

/*!
    Retrieves the last record in the result, if available, and
    positions the query on the retrieved record. Note that the result
    must be in an active state and isSelect() must return true before
    calling this function or it will do nothing and return false.
    Returns true if successful. If unsuccessful the query position is
    set to an invalid position and false is returned.

    \sa next() previous() first() seek() at() isActive() isValid()
*/

bool QSqlQuery::last()
{
    if (!isSelect() || !isActive())
        return false;
    bool b = false;
    b = d->sqlResult->fetchLast();
    return b;
}

/*!
    Returns the size of the result (number of rows returned), or -1
    if the size cannot be determined or if the database does not
    support reporting information about query sizes. Note that for
    non-\c SELECT statements (isSelect() returns false), size() will
    return -1. If the query is not active (isActive() returns false),
    -1 is returned.

    To determine the number of rows affected by a non-\c SELECT
    statement, use numRowsAffected().

    \sa isActive() numRowsAffected() QSqlDriver::hasFeature()
*/
int QSqlQuery::size() const
{
    if (isActive() && d->sqlResult->driver()->hasFeature(QSqlDriver::QuerySize))
        return d->sqlResult->size();
    return -1;
}

/*!
    Returns the number of rows affected by the result's SQL statement,
    or -1 if it cannot be determined. Note that for \c SELECT
    statements, the value is undefined; use size() instead. If the
    query is not active (isActive() returns false), -1 is returned.

    \sa size() QSqlDriver::hasFeature()
*/

int QSqlQuery::numRowsAffected() const
{
    if (isActive())
        return d->sqlResult->numRowsAffected();
    return -1;
}

/*!
    Returns error information about the last error (if any) that
    occurred with this query.

    \sa QSqlError, QSqlDatabase::lastError()
*/

QSqlError QSqlQuery::lastError() const
{
    return d->sqlResult->lastError();
}

/*!
    Returns true if the query is currently positioned on a valid
    record; otherwise returns false.
*/

bool QSqlQuery::isValid() const
{
    return d->sqlResult->isValid();
}

/*!
    Returns true if the query is currently active; otherwise returns
    false.
*/

bool QSqlQuery::isActive() const
{
    return d->sqlResult->isActive();
}

/*!
    Returns true if the current query is a \c SELECT statement;
    otherwise returns false.
*/

bool QSqlQuery::isSelect() const
{
    return d->sqlResult->isSelect();
}

/*!
    Returns true if you can only scroll forward through a result set;
    otherwise returns false.

    \sa setForwardOnly(), next()
*/
bool QSqlQuery::isForwardOnly() const
{
    return d->sqlResult->isForwardOnly();
}

/*!
    Sets forward only mode to \a forward. If \a forward is true,
    only next() and seek() with positive values, are allowed for
    navigating the results.

    Forward only mode can be (depending on the driver) more memory
    efficient since results do not need to be cached. It will also
    improve performance on some databases. For this to be true, you
    must call \c setForwardMode() before the query is prepared or
    executed. Note that the constructor that takes a query and a
    database may execute the query.

    Forward only mode is off by default.

    \sa isForwardOnly(), next(), seek()
*/
void QSqlQuery::setForwardOnly(bool forward)
{
    d->sqlResult->setForwardOnly(forward);
}

/*!
    Returns a QSqlRecord containing the field information for the
    current query. If the query points to a valid row (isValid()
    returns true), the record is populated with the row's values.
    An empty record is returned when there is no active query
    (isActive() returns false).

    To retrieve values from a query, value() should be used since
    its index-based lookup is faster.

    In the following example, a \c{SELECT * FROM} query is executed.
    Since the order of the columns is not defined, QSqlRecord::indexOf()
    is used to obtain the index of a column.

    \snippet doc/src/snippets/code/src.sql.kernel.qsqlquery.cpp 1

    \sa value()
*/
QSqlRecord QSqlQuery::record() const
{
    QSqlRecord rec = d->sqlResult->record();

    if (isValid()) {
        for (int i = 0; i < rec.count(); ++i)
            rec.setValue(i, value(i));
    }
    return rec;
}

/*!
    Clears the result set and releases any resources held by the
    query. You should rarely if ever need to call this function.
*/
void QSqlQuery::clear()
{
    *this = QSqlQuery(driver()->createResult());
}

/*!
    Prepares the SQL query \a query for execution. Returns true if the
    query is prepared successfully; otherwise returns false.

    The query may
    contain placeholders for binding values. Both Oracle style
    colon-name (e.g., \c{:surname}), and ODBC style (\c{?})
    placeholders are supported; but they cannot be mixed in the same
    query. See the \l{QSqlQuery examples}{Detailed Description} for examples.

    Portability note: Some databases choose to delay preparing a query until
    it is executed the first time. In this case, preparing a syntactically wrong
    query succeeds, but every consecutive exec() will fail.

    Example:

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 9

    \sa exec(), bindValue(), addBindValue()
*/
bool QSqlQuery::prepare(const QString& query)
{
    if (d->ref != 1) {
        bool fo = isForwardOnly();
        *this = QSqlQuery(driver()->createResult());
        setForwardOnly(fo);
        d->sqlResult->setNumericalPrecisionPolicy(d->precisionPolicy);
    } else {
        d->sqlResult->setActive(false);
        d->sqlResult->setLastError(QSqlError());
        d->sqlResult->setAt(QSql::BeforeFirstRow);
        d->sqlResult->setNumericalPrecisionPolicy(d->precisionPolicy);
    }
    if (!driver()) {
        qWarning("QSqlQuery::prepare: no driver");
        return false;
    }
    if (!driver()->isOpen() || driver()->isOpenError()) {
        qWarning("QSqlQuery::prepare: database not open");
        return false;
    }
    if (query.isEmpty()) {
        qWarning("QSqlQuery::prepare: empty query");
        return false;
    }
#ifdef QT_DEBUG_SQL
    qDebug("\n QSqlQuery::prepare: %s", query.toLocal8Bit().constData());
#endif
    return d->sqlResult->savePrepare(query);
}

/*!
    \overload

    Executes a previously prepared SQL query. Returns true if the
    query executed successfully; otherwise returns false.

    Note that the last error for this query is reset when exec() is
    called.

    \sa prepare() bindValue() addBindValue() boundValue() boundValues()
*/
bool QSqlQuery::exec()
{
    d->sqlResult->resetBindCount();

    if (d->sqlResult->lastError().isValid())
        d->sqlResult->setLastError(QSqlError());

    return d->sqlResult->exec();
}

/*! \enum QSqlQuery::BatchExecutionMode

    \value ValuesAsRows - Updates multiple rows. Treats every entry in a QVariantList as a value for updating the next row.
    \value ValuesAsColumns - Updates a single row. Treats every entry in a QVariantList as a single value of an array type.
*/

/*!
    \since 4.2

    Executes a previously prepared SQL query in a batch. All the bound parameters
    have to be lists of variants. If the database doesn't support batch executions,
    the driver will simulate it using conventional exec() calls.

    Returns true if the query is executed successfully; otherwise returns false.

    Example:

    \snippet doc/src/snippets/code/src.sql.kernel.qsqlquery.cpp 2

    The example above inserts four new rows into \c myTable:

    \snippet doc/src/snippets/code/src.sql.kernel.qsqlquery.cpp 3

    To bind NULL values, a null QVariant of the relevant type has to be added to
    the bound QVariantList; for example, \c {QVariant(QVariant::String)} should be
    used if you are using strings.

    Note that every bound QVariantList must contain the same amount of variants.
    Note that the type of the QVariants in a list must not change. For example,
    you cannot mix integer and string variants within a QVariantList.

    The \a mode parameter indicates how the bound QVariantList will be interpreted.
    If \a mode is \c ValuesAsRows, every variant within the QVariantList will be
    interpreted as a value for a new row. \c ValuesAsColumns is a special case
    for the Oracle driver. In this mode, every entry within a QVariantList will
    be interpreted as array-value for an IN or OUT value within a stored procedure.
    Note that this will only work if the IN or OUT value is a table-type consisting
    of only one column of a basic type, for example
    \c{TYPE myType IS TABLE OF VARCHAR(64) INDEX BY BINARY_INTEGER;}

    \sa prepare(), bindValue(), addBindValue()
*/
bool QSqlQuery::execBatch(BatchExecutionMode mode)
{
    return d->sqlResult->execBatch(mode == ValuesAsColumns);
}

/*!
    Set the placeholder \a placeholder to be bound to value \a val in
    the prepared statement. Note that the placeholder mark (e.g \c{:})
    must be included when specifying the placeholder name. If \a paramType
    is QSql::Out or QSql::InOut, the placeholder will be
    overwritten with data from the database after the exec() call.

    To bind a NULL value, use a null QVariant; for example, use
    \c {QVariant(QVariant::String)} if you are binding a string.

    \sa addBindValue(), prepare(), exec(), boundValue() boundValues()
*/
void QSqlQuery::bindValue(const QString& placeholder, const QVariant& val,
                          QSql::ParamType paramType
)
{
    d->sqlResult->bindValue(placeholder, val, paramType);
}

/*!
    \overload

    Set the placeholder in position \a pos to be bound to value \a val
    in the prepared statement. Field numbering starts at 0. If \a paramType
    is QSql::Out or QSql::InOut, the placeholder will be
    overwritten with data from the database after the exec() call.
*/
void QSqlQuery::bindValue(int pos, const QVariant& val, QSql::ParamType paramType)
{
    d->sqlResult->bindValue(pos, val, paramType);
}

/*!
    Adds the value \a val to the list of values when using positional
    value binding. The order of the addBindValue() calls determines
    which placeholder a value will be bound to in the prepared query.
    If \a paramType is QSql::Out or QSql::InOut, the placeholder will
    be overwritten with data from the database after the exec() call.

    To bind a NULL value, use a null QVariant; for example, use
    \c {QVariant(QVariant::String)} if you are binding a string.

    \sa bindValue(), prepare(), exec(), boundValue() boundValues()
*/
void QSqlQuery::addBindValue(const QVariant& val, QSql::ParamType paramType)
{
    d->sqlResult->addBindValue(val, paramType);
}

/*!
    Returns the value for the \a placeholder.

    \sa boundValues() bindValue() addBindValue()
*/
QVariant QSqlQuery::boundValue(const QString& placeholder) const
{
    return d->sqlResult->boundValue(placeholder);
}

/*!
    \overload

    Returns the value for the placeholder at position \a pos.
*/
QVariant QSqlQuery::boundValue(int pos) const
{
    return d->sqlResult->boundValue(pos);
}

/*!
    Returns a map of the bound values.

    With named binding, the bound values can be examined in the
    following ways:

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 14

    With positional binding, the code becomes:

    \snippet doc/src/snippets/sqldatabase/sqldatabase.cpp 15

    \sa boundValue() bindValue() addBindValue()
*/
QMap<QString,QVariant> QSqlQuery::boundValues() const
{
    QMap<QString,QVariant> map;

    const QVector<QVariant> values(d->sqlResult->boundValues());
    for (int i = 0; i < values.count(); ++i)
        map[d->sqlResult->boundValueName(i)] = values.at(i);
    return map;
}

/*!
    Returns the last query that was successfully executed.

    In most cases this function returns the same string as
    lastQuery(). If a prepared query with placeholders is executed on
    a DBMS that does not support it, the preparation of this query is
    emulated. The placeholders in the original query are replaced with
    their bound values to form a new query. This function returns the
    modified query. It is mostly useful for debugging purposes.

    \sa lastQuery()
*/
QString QSqlQuery::executedQuery() const
{
    return d->sqlResult->executedQuery();
}

/*!
    \fn bool QSqlQuery::prev()

    Use previous() instead.
*/

/*!
    Returns the object ID of the most recent inserted row if the
    database supports it.
    An invalid QVariant will be returned if the query did not
    insert any value or if the database does not report the id back.
    If more than one row was touched by the insert, the behavior is
    undefined.

    For MySQL databases the row's auto-increment field will
    be returned.
    
    \note For this function to work in PSQL, the table table must
    contain OIDs, which may not have been created by default.
    Check the \c default_with_oids configuration variable to be sure.

    \sa QSqlDriver::hasFeature()
*/
QVariant QSqlQuery::lastInsertId() const
{
    return d->sqlResult->lastInsertId();
}

/*!
    Instruct the database driver to return numerical values with a precision specified by
    \a precisionPolicy.

    The Oracle driver, for example, retrieves numerical values as strings by default to
    prevent the loss of precision. If the high precision doesn't matter, use this method
    to increase execution speed by bypassing string conversions.

    Note: Drivers that don't support fetching numerical values with low precision will
    ignore the precision policy. You can use QSqlDriver::hasFeature() to find out whether a
    driver supports this feature.

    Note: Setting the precision policy doesn't affect the currently active query. Call
    \l{exec()}{exec(QString)} or prepare() in order to activate the policy.

    \sa QSql::NumericalPrecisionPolicy, numericalPrecisionPolicy()
*/
void QSqlQuery::setNumericalPrecisionPolicy(QSql::NumericalPrecisionPolicy precisionPolicy)
{
    d->precisionPolicy = precisionPolicy;
}

/*!
    Returns the current precision policy.

    \sa QSql::NumericalPrecisionPolicy, setNumericalPrecisionPolicy()
*/
QSql::NumericalPrecisionPolicy QSqlQuery::numericalPrecisionPolicy() const
{
    return d->precisionPolicy;
}

/*!
    \since 4.3.2

    Instruct the database driver that no more data will be fetched from this
    query until it is re-executed. There is normally no need to call this
    function, but it may be helpful in order to free resources such as locks
    or cursors if you intend to re-use the query at a later time.
    
    Sets the query to inactive. Bound values retain their values.

    \sa prepare() exec() isActive()
*/
void QSqlQuery::finish()
{
    if (isActive()) {
        d->sqlResult->setLastError(QSqlError());
        d->sqlResult->setAt(QSql::BeforeFirstRow);
        d->sqlResult->detachFromResultSet();
        d->sqlResult->setActive(false);
    }
}

/*!
    \since 4.4
 
    Discards the current result set and navigates to the next if available.

    Some databases are capable of returning multiple result sets for stored
    procedures or SQL batches (a query strings that contains multiple
    statements). If multiple result sets are available after executing a
    query this function can be used to navigate to the next result set(s).
    
    If a new result set is available this function will return true.
    The query will be repositioned on an \e invalid record in the new
    result set and must be navigated to a valid record before data
    values can be retrieved. If a new result set isn't available the
    function returns false and the the query is set to inactive. In any
    case the old result set will be discarded.

    When one of the statements is a non-select statement a count of affected 
    rows may be available instead of a result set.

    Note that some databases, i.e. Microsoft SQL Server, requires
    non-scrollable cursors when working with multiple result sets. 
    Some databases may execute all statements at once while others may
    delay the execution until the result set is actually accessed,
    and some databases may have restrictions on which statements are
    allowed to be used in a SQL batch. 

    \sa QSqlDriver::hasFeature() setForwardOnly() next() isSelect() numRowsAffected() isActive() lastError()
*/
bool QSqlQuery::nextResult()
{
    if (isActive())
        return d->sqlResult->nextResult();
    return false;
}

QT_END_NAMESPACE
