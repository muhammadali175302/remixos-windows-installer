/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qcoreapplication.h"
#include "qcoreapplication_p.h"

#include "qabstracteventdispatcher.h"
#include "qcoreevent.h"
#include "qeventloop.h"
#include "qcorecmdlineargs_p.h"
#include <qdatastream.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <private/qprocess_p.h>
#include <qtextcodec.h>
#include <qthread.h>
#include <qthreadstorage.h>
#include <private/qthread_p.h>
#include <qlibraryinfo.h>
#include <private/qfactoryloader_p.h>

#ifdef Q_OS_UNIX
#  if !defined(QT_NO_GLIB)
#    include "qeventdispatcher_glib_p.h"
#  endif
#  include "qeventdispatcher_unix_p.h"
#endif

#ifdef Q_OS_WIN
#  include "qeventdispatcher_win_p.h"
#endif

#ifdef Q_OS_MAC
#  include "qcore_mac_p.h"
#endif

#include <stdlib.h>

#ifdef Q_OS_UNIX
#  include <locale.h>
#endif

QT_BEGIN_NAMESPACE

#if defined(Q_WS_WIN) || defined(Q_WS_MAC)
extern QString qAppFileName();
#endif

#if !defined(Q_OS_WIN)
#ifdef Q_OS_MAC
QString QCoreApplicationPrivate::macMenuBarName()
{
    QString bundleName;
    CFTypeRef string = CFBundleGetValueForInfoDictionaryKey(CFBundleGetMainBundle(), CFSTR("CFBundleName"));
    if (string)
        bundleName = QCFString::toQString(static_cast<CFStringRef>(string));
    return bundleName;
}
#endif
QString QCoreApplicationPrivate::appName() const
{
    static QString applName;
#ifdef Q_OS_MAC
    applName = macMenuBarName();
#endif
    if (applName.isEmpty() && argv[0]) {
        char *p = strrchr(argv[0], '/');
        applName = QString::fromLocal8Bit(p ? p + 1 : argv[0]);
    }
    return applName;
}
#endif

bool QCoreApplicationPrivate::checkInstance(const char *function)
{
    bool b = (QCoreApplication::self != 0);
    if (!b)
        qWarning("QApplication::%s: Please instantiate the QApplication object first", function);
    return b;
}

// Support for introspection

QSignalSpyCallbackSet Q_CORE_EXPORT qt_signal_spy_callback_set = { 0, 0, 0, 0 };

void qt_register_signal_spy_callbacks(const QSignalSpyCallbackSet &callback_set)
{
    qt_signal_spy_callback_set = callback_set;
}

extern "C" void Q_CORE_EXPORT qt_startup_hook()
{
}

typedef QList<QtCleanUpFunction> QVFuncList;
Q_GLOBAL_STATIC(QVFuncList, postRList)

void qAddPostRoutine(QtCleanUpFunction p)
{
    QVFuncList *list = postRList();
    if (!list)
        return;
    list->prepend(p);
}

void qRemovePostRoutine(QtCleanUpFunction p)
{
    QVFuncList *list = postRList();
    if (!list)
        return;
    list->removeAll(p);
}

void Q_CORE_EXPORT qt_call_post_routines()
{
    QVFuncList *list = postRList();
    if (!list)
        return;
    while (!list->isEmpty())
        (list->takeFirst())();
}


// app starting up if false
bool QCoreApplicationPrivate::is_app_running = false;
 // app closing down if true
bool QCoreApplicationPrivate::is_app_closing = false;


Q_CORE_EXPORT uint qGlobalPostedEventsCount()
{
    return QThreadData::current()->postEventList.size();
}


void qt_set_current_thread_to_main_thread()
{
    QCoreApplicationPrivate::theMainThread = QThread::currentThread();
}



QCoreApplication *QCoreApplication::self = 0;
QAbstractEventDispatcher *QCoreApplicationPrivate::eventDispatcher = 0;
uint QCoreApplicationPrivate::attribs;

#ifdef Q_OS_UNIX
Qt::HANDLE qt_application_thread_id = 0;
#endif

struct QCoreApplicationData {
    QCoreApplicationData() {
#ifndef QT_NO_LIBRARY
        app_libpaths = 0;
#endif
    }
    ~QCoreApplicationData() {
#ifndef QT_NO_LIBRARY
        delete app_libpaths;
#endif

        // cleanup the QAdoptedThread created for the main() thread
        QThreadData *data = QThreadData::get2(QCoreApplicationPrivate::theMainThread);
        QCoreApplicationPrivate::theMainThread = 0;
        data->deref(); // deletes the data and the adopted thread
    }
    QString orgName, orgDomain, application;
    QString applicationVersion;

#ifndef QT_NO_LIBRARY
    QStringList *app_libpaths;
#endif

};

Q_GLOBAL_STATIC(QCoreApplicationData, coreappdata)

QCoreApplicationPrivate::QCoreApplicationPrivate(int &aargc, char **aargv)
    : QObjectPrivate(), argc(aargc), argv(aargv), application_type(0), eventFilter(0),
      in_exec(false)
{
    static const char *const empty = "";
    if (argc == 0 || argv == 0) {
        argc = 0;
        argv = (char **)&empty; // ouch! careful with QCoreApplication::argv()!
    }
    QCoreApplicationPrivate::is_app_closing = false;

#ifdef Q_OS_UNIX
    qt_application_thread_id = QThread::currentThreadId();
#endif

    // note: this call to QThread::currentThread() may end up setting theMainThread!
    if (QThread::currentThread() != theMainThread)
        qWarning("WARNING: QApplication was not created in the main() thread.");
}

QCoreApplicationPrivate::~QCoreApplicationPrivate()
{
#ifndef QT_NO_THREAD
    void *data = &threadData->tls;
    QThreadStorageData::finish((void **)data);
#endif

    // need to clear the state of the mainData, just in case a new QCoreApplication comes along.
    QMutexLocker locker(&threadData->postEventList.mutex);
    for (int i = 0; i < threadData->postEventList.size(); ++i) {
        const QPostEvent &pe = threadData->postEventList.at(i);
        if (pe.event) {
            --pe.receiver->d_func()->postedEvents;
            pe.event->posted = false;
            delete pe.event;
        }
    }
    threadData->postEventList.clear();
    threadData->postEventList.recursion = 0;
    threadData->quitNow = false;
}

void QCoreApplicationPrivate::createEventDispatcher()
{
    Q_Q(QCoreApplication);
#if defined(Q_OS_UNIX)
#  if !defined(QT_NO_GLIB)
    if (qgetenv("QT_NO_GLIB").isEmpty() && QEventDispatcherGlib::versionSupported())
        eventDispatcher = new QEventDispatcherGlib(q);
    else
#  endif
        eventDispatcher = new QEventDispatcherUNIX(q);
#elif defined(Q_OS_WIN)
    eventDispatcher = new QEventDispatcherWin32(q);
#else
#  error "QEventDispatcher not yet ported to this platform"
#endif
}

QThread *QCoreApplicationPrivate::theMainThread = 0;
QThread *QCoreApplicationPrivate::mainThread()
{
    Q_ASSERT(theMainThread != 0);
    return theMainThread;
}

#if !defined (QT_NO_DEBUG) || defined (QT_MAC_FRAMEWORK_BUILD)
void QCoreApplicationPrivate::checkReceiverThread(QObject *receiver)
{
    QThread *currentThread = QThread::currentThread();
    QThread *thr = receiver->thread();
    Q_ASSERT_X(currentThread == thr || !thr,
               "QCoreApplication::sendEvent",
               QString::fromLatin1("Cannot send events to objects owned by a different thread. "
                                   "Current thread %1. Receiver '%2' (of type '%3') was created in thread %4")
               .arg(QString::number((quintptr) currentThread, 16))
               .arg(receiver->objectName())
               .arg(QLatin1String(receiver->metaObject()->className()))
               .arg(QString::number((quintptr) thr, 16))
               .toLocal8Bit().data());
    Q_UNUSED(currentThread);
    Q_UNUSED(thr);
}
#endif

void QCoreApplicationPrivate::appendApplicationPathToLibraryPaths()
{
#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    QStringList *app_libpaths = coreappdata()->app_libpaths;
    Q_ASSERT(app_libpaths);
    QString app_location( QCoreApplication::applicationFilePath() );
    app_location.truncate(app_location.lastIndexOf(QLatin1Char('/')));
    app_location = QDir(app_location).canonicalPath();
    if (app_location !=  QLibraryInfo::location(QLibraryInfo::PluginsPath) && QFile::exists(app_location) && !app_libpaths->contains(app_location))
        app_libpaths->append(app_location);
#endif
}

QString qAppName()
{
    if (!QCoreApplicationPrivate::checkInstance("qAppName"))
        return QString();
    return QCoreApplication::instance()->d_func()->appName();
}

/*!
    \class QCoreApplication
    \brief The QCoreApplication class provides an event loop for console Qt
    applications.

    \ingroup application
    \mainclass

    This class is used by non-GUI applications to provide their event
    loop. For non-GUI application that uses Qt, there should be exactly
    one QCoreApplication object. For GUI applications, see
    QApplication.

    QCoreApplication contains the main event loop, where all events
    from the operating system (e.g., timer and network events) and
    other sources are processed and dispatched. It also handles the
    application's initialization and finalization, as well as
    system-wide and application-wide settings.

    The command line arguments which QCoreApplication's constructor
    should be called with are accessible using arguments(). The
    event loop is started with a call to exec(). Long running
    operations can call processEvents() to keep the application
    responsive.

    Some Qt classes, such as QString, can be used without a
    QCoreApplication object. However, in general, we recommend that
    you create a QCoreApplication or a QApplication object in your \c
    main() function as early as possible. The application will enter
    the event loop when exec() is called. exit() will not return
    until the event loop exits, e.g., when quit() is called.

    An application has an applicationDirPath() and an
    applicationFilePath(). Translation files can be added or removed
    using installTranslator() and removeTranslator(). Application
    strings can be translated using translate(). The QObject::tr()
    and QObject::trUtf8() functions are implemented in terms of
    translate().

    The class provides a quit() slot and an aboutToQuit() signal.

    Several static convenience functions are also provided. The
    QCoreApplication object is available from instance(). Events can
    be sent or posted using sendEvent(), postEvent(), and
    sendPostedEvents(). Pending events can be removed with
    removePostedEvents() or flushed with flush(). Library paths (see
    QLibrary) can be retrieved with libraryPaths() and manipulated by
    setLibraryPaths(), addLibraryPath(), and removeLibraryPath().

    \sa QApplication, QAbstractEventDispatcher, QEventLoop,
    {Semaphores Example}, {Wait Conditions Example}
*/

/*!
    \fn static QCoreApplication *QCoreApplication::instance()

    Returns a pointer to the application's QCoreApplication (or
    QApplication) instance.
*/

/*!\internal
 */
QCoreApplication::QCoreApplication(QCoreApplicationPrivate &p)
    : QObject(p, 0)
{
    init();
    // note: it is the subclasses' job to call
    // QCoreApplicationPrivate::eventDispatcher->startingUp();
}

/*!
    Flushes the platform specific event queues.

    If you are doing graphical changes inside a loop that does not
    return to the event loop on asynchronous window systems like X11
    or double buffered window systems like Mac OS X, and you want to
    visualize these changes immediately (e.g. Splash Screens), call
    this function.

    \sa sendPostedEvents()
*/
void QCoreApplication::flush()
{
    if (self && self->d_func()->eventDispatcher)
        self->d_func()->eventDispatcher->flush();
}

/*!
    Constructs a Qt kernel application. Kernel applications are
    applications without a graphical user interface. These type of
    applications are used at the console or as server processes.

    The \a argc and \a argv arguments are processed by the application,
    and made available in a more convenient form by the arguments()
    function.

    \warning The data pointed to by \a argc and \a argv must stay
    valid for the entire lifetime of the QCoreApplication object.
*/
QCoreApplication::QCoreApplication(int &argc, char **argv)
    : QObject(*new QCoreApplicationPrivate(argc, argv))
{
    init();
    QCoreApplicationPrivate::eventDispatcher->startingUp();
}

extern void set_winapp_name();

// ### move to QCoreApplicationPrivate constructor?
void QCoreApplication::init()
{
    Q_D(QCoreApplication);

#ifdef Q_OS_UNIX
    setlocale(LC_ALL, "");                // use correct char set mapping
#endif

#ifdef Q_WS_WIN
    // Get the application name/instance if qWinMain() was not invoked
    set_winapp_name();
#endif

    Q_ASSERT_X(!self, "QCoreApplication", "there should be only one application object");
    QCoreApplication::self = this;

#ifndef QT_NO_THREAD
    QThread::initialize();
#endif

    // use the event dispatcher created by the app programmer (if any)
    if (!QCoreApplicationPrivate::eventDispatcher)
        QCoreApplicationPrivate::eventDispatcher = d->threadData->eventDispatcher;
    // otherwise we create one
    if (!QCoreApplicationPrivate::eventDispatcher)
        d->createEventDispatcher();
    Q_ASSERT(QCoreApplicationPrivate::eventDispatcher != 0);

    if (!QCoreApplicationPrivate::eventDispatcher->parent())
        QCoreApplicationPrivate::eventDispatcher->moveToThread(d->threadData->thread);

    d->threadData->eventDispatcher = QCoreApplicationPrivate::eventDispatcher;

#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    if (!coreappdata()->app_libpaths) {
        // make sure that library paths is initialized
        libraryPaths();
    } else {
        d->appendApplicationPathToLibraryPaths();
    }
#endif

#if defined(Q_OS_UNIX) && !(defined(QT_NO_PROCESS))
    // Make sure the process manager thread object is created in the main
    // thread.
    QProcessPrivate::initializeProcessManager();
#endif

#ifdef QT_EVAL
    extern void qt_core_eval_init(uint);
    qt_core_eval_init(d->application_type);
#endif

    qt_startup_hook();
}

/*!
    Destroys the QCoreApplication object.
*/
QCoreApplication::~QCoreApplication()
{
    qt_call_post_routines();

    self = 0;
    QCoreApplicationPrivate::is_app_closing = true;
    QCoreApplicationPrivate::is_app_running = false;

#ifndef QT_NO_THREAD
    QThread::cleanup();
#endif

    d_func()->threadData->eventDispatcher = 0;
    if (QCoreApplicationPrivate::eventDispatcher)
        QCoreApplicationPrivate::eventDispatcher->closingDown();
    QCoreApplicationPrivate::eventDispatcher = 0;

#ifndef QT_NO_LIBRARY
    delete coreappdata()->app_libpaths;
    coreappdata()->app_libpaths = 0;
#endif
}


/*!
    Sets the attribute \a attribute if \a on is true;
    otherwise clears the attribute.

    One of the attributes that can be set with this method is
    Qt::AA_ImmediateWidgetCreation. It tells Qt to create toplevel
    windows immediately. Normally, resources for widgets are allocated
    on demand to improve efficiency and minimize resource usage.
    Therefore, if it is important to minimize resource consumption, do
    not set this attribute.

    \sa testAttribute()
*/
void QCoreApplication::setAttribute(Qt::ApplicationAttribute attribute, bool on)
{
    if (on)
        QCoreApplicationPrivate::attribs |= 1 << attribute;
    else
        QCoreApplicationPrivate::attribs &= ~(1 << attribute);
}

/*!
  Returns true if attribute \a attribute is set;
  otherwise returns false.

  \sa setAttribute()
 */
bool QCoreApplication::testAttribute(Qt::ApplicationAttribute attribute)
{
    return QCoreApplicationPrivate::testAttribute(attribute);
}


/*!
  \internal

  This function is here to make it possible for Qt extensions to
  hook into event notification without subclassing QApplication
*/
bool QCoreApplication::notifyInternal(QObject *receiver, QEvent *event)
{
    // Make it possible for Qt Jambi and QSA to hook into events even
    // though QApplication is subclassed...
    bool result = false;
    void *cbdata[] = { receiver, event, &result };
    if (QInternal::activateCallbacks(QInternal::EventNotifyCallback, cbdata)) {
        return result;
    }

    // Qt enforces the rule that events can only be sent to objects in
    // the current thread, so receiver->d_func()->threadData is
    // equivalent to QThreadData::current(), just without the function
    // call overhead.
    QObjectPrivate *d = receiver->d_func();
    QThreadData *threadData = d->threadData;
    ++threadData->loopLevel;

    int deleteWatch = 0;
    int *oldDeleteWatch = QObjectPrivate::setDeleteWatch(d, &deleteWatch);

    bool inEvent = d->inEventHandler;
    d->inEventHandler = true;

#if defined(QT_NO_EXCEPTIONS)
    bool returnValue = notify(receiver, event);
#else
    bool returnValue;
    try {
        returnValue = notify(receiver, event);
    } catch(...) {
        --threadData->loopLevel;
        throw;
    }
#endif

    // Restore the previous state if the object was not deleted..
    if (!deleteWatch) {
        d->inEventHandler = inEvent;
    }
    QObjectPrivate::resetDeleteWatch(d, oldDeleteWatch, deleteWatch);

    --threadData->loopLevel;
    return returnValue;
}


/*!
  Sends \a event to \a receiver: \a {receiver}->event(\a event).
  Returns the value that is returned from the receiver's event handler.

  For certain types of events (e.g. mouse and key events),
  the event will be propagated to the receiver's parent and so on up to
  the top-level object if the receiver is not interested in the event
  (i.e., it returns false).

  There are five different ways that events can be processed;
  reimplementing this virtual function is just one of them. All five
  approaches are listed below:
  \list 1
  \i Reimplementing paintEvent(), mousePressEvent() and so
  on. This is the commonest, easiest and least powerful way.

  \i Reimplementing this function. This is very powerful, providing
  complete control; but only one subclass can be active at a time.

  \i Installing an event filter on QCoreApplication::instance(). Such
  an event filter is able to process all events for all widgets, so
  it's just as powerful as reimplementing notify(); furthermore, it's
  possible to have more than one application-global event filter.
  Global event filters even see mouse events for
  \l{QWidget::isEnabled()}{disabled widgets}.

  \i Reimplementing QObject::event() (as QWidget does). If you do
  this you get Tab key presses, and you get to see the events before
  any widget-specific event filters.

  \i Installing an event filter on the object. Such an event filter
  gets all the events except Tab and Shift-Tab key presses.
  \endlist

  \sa QObject::event(), installEventFilter()
*/

bool QCoreApplication::notify(QObject *receiver, QEvent *event)
{
    Q_D(QCoreApplication);
    // no events are delivered after ~QCoreApplication() has started
    if (QCoreApplicationPrivate::is_app_closing)
        return true;

    if (receiver == 0) {                        // serious error
        qWarning("QCoreApplication::notify: Unexpected null receiver");
        return true;
    }

#ifndef QT_NO_DEBUG
    d->checkReceiverThread(receiver);
#endif

#ifdef QT3_SUPPORT
    if (event->type() == QEvent::ChildRemoved && !receiver->d_func()->pendingChildInsertedEvents.isEmpty())
        receiver->d_func()->removePendingChildInsertedEvents(static_cast<QChildEvent *>(event)->child());
#endif // QT3_SUPPORT

    return receiver->isWidgetType() ? false : d->notify_helper(receiver, event);
}

bool QCoreApplicationPrivate::sendThroughApplicationEventFilters(QObject *receiver, QEvent *event)
{
    for (int i = 0; i < eventFilters.size(); ++i) {
        register QObject *obj = eventFilters.at(i);
        if (!obj)
            continue;
        if (obj->d_func()->threadData != threadData) {
            qWarning("QCoreApplication: Application event filter cannot be in a different thread.");
            continue;
        }
        if (obj->eventFilter(receiver, event))
            return true;
    }
    return false;
}

bool QCoreApplicationPrivate::sendThroughObjectEventFilters(QObject *receiver, QEvent *event)
{
    Q_Q(QCoreApplication);
   if (receiver != q) {
        for (int i = 0; i < receiver->d_func()->eventFilters.size(); ++i) {
            register QObject *obj = receiver->d_func()->eventFilters.at(i);
            if (!obj)
                continue;
            if (obj->d_func()->threadData != receiver->d_func()->threadData) {
                qWarning("QCoreApplication: Object event filter cannot be in a different thread.");
                continue;
            }
            if (obj->eventFilter(receiver, event))
                return true;
        }
    }
    return false;
}

/*!\internal

  Helper function called by notify()
 */
bool QCoreApplicationPrivate::notify_helper(QObject *receiver, QEvent * event)
{
    // send to all application event filters
    if (sendThroughApplicationEventFilters(receiver, event))
        return true;
    // send to all receiver event filters
    if (sendThroughObjectEventFilters(receiver, event))
        return true;
    // deliver the event
    return receiver->event(event);
}

/*!
  Returns true if an application object has not been created yet;
  otherwise returns false.

  \sa closingDown()
*/

bool QCoreApplication::startingUp()
{
    return !QCoreApplicationPrivate::is_app_running;
}

/*!
  Returns true if the application objects are being destroyed;
  otherwise returns false.

  \sa startingUp()
*/

bool QCoreApplication::closingDown()
{
    return QCoreApplicationPrivate::is_app_closing;
}


/*!
    Processes all pending events for the calling thread according to
    the specified \a flags until there are no more events to process.

    You can call this function occasionally when your program is busy
    performing a long operation (e.g. copying a file).

    In event you are running a local loop which calls this function
    continuously, without an event loop, the
    \l{QEvent::DeferredDelete}{DeferredDelete} events will
    not be processed. This can affect the behaviour of widgets,
    e.g. QToolTip, that rely on \l{QEvent::DeferredDelete}{DeferredDelete}
    events to function properly. An alternative would be to call
    \l{QCoreApplication::sendPostedEvents()}{sendPostedEvents()} from
    within that local loop.

    Calling this function processes events only for the calling thread.

    \threadsafe

    \sa exec(), QTimer, QEventLoop::processEvents(), flush(), sendPostedEvents()
*/
void QCoreApplication::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    QThreadData *data = QThreadData::current();
    if (!data->eventDispatcher)
        return;
    if (flags & QEventLoop::DeferredDeletion)
        QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    data->eventDispatcher->processEvents(flags);
}

/*!
    \overload

    Processes pending events for the calling thread for \a maxtime
    milliseconds or until there are no more events to process,
    whichever is shorter.

    You can call this function occasionally when you program is busy
    doing a long operation (e.g. copying a file).

    Calling this function processes events only for the calling thread.

    \threadsafe

    \sa exec(), QTimer, QEventLoop::processEvents()
*/
void QCoreApplication::processEvents(QEventLoop::ProcessEventsFlags flags, int maxtime)
{
    QThreadData *data = QThreadData::current();
    if (!data->eventDispatcher)
        return;
    QTime start;
    start.start();
    if (flags & QEventLoop::DeferredDeletion)
        QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    while (data->eventDispatcher->processEvents(flags & ~QEventLoop::WaitForMoreEvents)) {
        if (start.elapsed() > maxtime)
            break;
        if (flags & QEventLoop::DeferredDeletion)
            QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    }
}

/*****************************************************************************
  Main event loop wrappers
 *****************************************************************************/

/*!
    Enters the main event loop and waits until exit() is called.
    Returns the value that was set to exit() (which is 0 if exit() is
    called via quit()).

    It is necessary to call this function to start event handling. The
    main event loop receives events from the window system and
    dispatches these to the application widgets.

    To make your application perform idle processing (i.e. executing a
    special function whenever there are no pending events), use a
    QTimer with 0 timeout. More advanced idle processing schemes can
    be achieved using processEvents().

    \sa quit(), exit(), processEvents(), QApplication::exec()
*/
int QCoreApplication::exec()
{
    if (!QCoreApplicationPrivate::checkInstance("exec"))
        return -1;

    QThreadData *threadData = self->d_func()->threadData;
    if (threadData != QThreadData::current()) {
        qWarning("%s::exec: Must be called from the main thread", self->metaObject()->className());
        return -1;
    }
    if (!threadData->eventLoops.isEmpty()) {
        qWarning("QCoreApplication::exec: The event loop is already running");
        return -1;
    }

    threadData->quitNow = false;
    QEventLoop eventLoop;
    self->d_func()->in_exec = true;
    int returnCode = eventLoop.exec();
    threadData->quitNow = false;
    if (self) {
        self->d_func()->in_exec = false;
        emit self->aboutToQuit();
        sendPostedEvents(0, QEvent::DeferredDelete);
    }
    return returnCode;
}

/*!
  Tells the application to exit with a return code.

    After this function has been called, the application leaves the
    main event loop and returns from the call to exec(). The exec()
    function returns \a returnCode. If the event loop is not running,
    this function does nothing.

  By convention, a \a returnCode of 0 means success, and any non-zero
  value indicates an error.

  Note that unlike the C library function of the same name, this
  function \e does return to the caller -- it is event processing that
  stops.

  \sa quit(), exec()
*/
void QCoreApplication::exit(int returnCode)
{
    if (!self)
        return;
    QThreadData *data = self->d_func()->threadData;
    data->quitNow = true;
    for (int i = 0; i < data->eventLoops.size(); ++i) {
        QEventLoop *eventLoop = data->eventLoops.at(i);
        eventLoop->exit(returnCode);
    }
}

/*****************************************************************************
  QCoreApplication management of posted events
 *****************************************************************************/

/*!
    \fn bool QCoreApplication::sendEvent(QObject *receiver, QEvent *event)

    Sends event \a event directly to receiver \a receiver, using the
    notify() function. Returns the value that was returned from the
    event handler.

    The event is \e not deleted when the event has been sent. The normal
    approach is to create the event on the stack, for example:

    \snippet doc/src/snippets/code/src.corelib.kernel.qcoreapplication.cpp 0

    \sa postEvent(), notify()
*/

/*!
    Adds the event \a event, with the object \a receiver as the receiver of the
    event, to an event queue and returns immediately.

    The event must be allocated on the heap since the post event queue
    will take ownership of the event and delete it once it has been posted.
    It is \e {not safe} to modify or delete the event after it has been posted.

    When control returns to the main event loop, all events that are
    stored in the queue will be sent using the notify() function.

    Events are processed in the order posted. For more control over the processing order,
    use the postEvent() overload below, which takes a priority argument. This function
    posts all event with a Qt::NormalEventPriority.

    \threadsafe

    \sa sendEvent(), notify(), sendPostedEvents()
*/

void QCoreApplication::postEvent(QObject *receiver, QEvent *event)
{
    postEvent(receiver, event, Qt::NormalEventPriority);
}


/*!
    \overload
    \since 4.3

    Adds the event \a event, with the object \a receiver as the receiver of the
    event, to an event queue and returns immediately.

    The event must be allocated on the heap since the post event queue
    will take ownership of the event and delete it once it has been posted.
    It is \e {not safe} to modify or delete the event after it has been posted.

    When control returns to the main event loop, all events that are
    stored in the queue will be sent using the notify() function.

    Events are sorted in descending \a priority order, i.e. events
    with a high \a priority are queued before events with a lower \a
    priority. The \a priority can be any integer value, i.e. between
    INT_MAX and INT_MIN, inclusive; see Qt::EventPriority for more
    details. Events with equal \a priority will be processed in the
    order posted.

    \threadsafe

    \sa sendEvent(), notify(), sendPostedEvents(), Qt::EventPriority
*/
void QCoreApplication::postEvent(QObject *receiver, QEvent *event, int priority)
{
    if (receiver == 0) {
        qWarning("QCoreApplication::postEvent: Unexpected null receiver");
        delete event;
        return;
    }

    QThreadData * volatile * pdata = &receiver->d_func()->threadData;
    QThreadData *data = *pdata;
    if (!data) {
        // posting during destruction? just delete the event to prevent a leak
        delete event;
        return;
    }

    // lock the post event mutex
    data->postEventList.mutex.lock();

    // if object has moved to another thread, follow it
    while (data != *pdata) {
        data->postEventList.mutex.unlock();

        data = *pdata;
        if (!data) {
            // posting during destruction? just delete the event to prevent a leak
            delete event;
            return;
        }

        data->postEventList.mutex.lock();
    }

    // if this is one of the compressible events, do compression
    if (receiver->d_func()->postedEvents
        && self && self->compressEvent(event, receiver, &data->postEventList)) {
        data->postEventList.mutex.unlock();
        return;
    }

    event->posted = true;
    ++receiver->d_func()->postedEvents;
    if (event->type() == QEvent::DeferredDelete && data == QThreadData::current()) {
        // remember the current running eventloop for DeferredDelete
        // events posted in the receiver's thread
        event->d = reinterpret_cast<QEventPrivate *>(quintptr(data->loopLevel));
    }

    if (data->postEventList.isEmpty() || data->postEventList.last().priority >= priority) {
        // optimization: we can simply append if the last event in
        // the queue has higher or equal priority
        data->postEventList.append(QPostEvent(receiver, event, priority));
    } else {
        // insert event in descending priority order, using upper
        // bound for a given priority (to ensure proper ordering
        // of events with the same priority)
        QPostEventList::iterator begin = data->postEventList.begin()
                                         + data->postEventList.insertionOffset,
                                   end = data->postEventList.end();
        QPostEventList::iterator at = qUpperBound(begin, end, priority);
        data->postEventList.insert(at, QPostEvent(receiver, event, priority));
    }
    data->canWait = false;
    data->postEventList.mutex.unlock();

    if (data->eventDispatcher)
        data->eventDispatcher->wakeUp();
}

/*!
  \internal
  Returns true if \a event was compressed away (possibly deleted) and should not be added to the list.
*/
bool QCoreApplication::compressEvent(QEvent *event, QObject *receiver, QPostEventList *postedEvents)
{
#ifdef Q_WS_WIN
    Q_ASSERT(event);
    Q_ASSERT(receiver);
    Q_ASSERT(postedEvents);

    // compress posted timers to this object.
    if (event->type() == QEvent::Timer && receiver->d_func()->postedEvents > 0) {
        int timerId = ((QTimerEvent *) event)->timerId();
        for (int i=0; i<postedEvents->size(); ++i) {
            const QPostEvent &e = postedEvents->at(i);
            if (e.receiver == receiver && e.event && e.event->type() == QEvent::Timer
                && ((QTimerEvent *) e.event)->timerId() == timerId) {
                delete event;
                return true;
            }
        }
    } else
#endif
        if ((event->type() == QEvent::DeferredDelete
             || event->type() == QEvent::Quit)
            && receiver->d_func()->postedEvents > 0) {
            for (int i = 0; i < postedEvents->size(); ++i) {
                const QPostEvent &cur = postedEvents->at(i);
                if (cur.receiver != receiver
                    || cur.event == 0
                    || cur.event->type() != event->type())
                    continue;
                // found an event for this receiver
                delete event;
                return true;
            }
        }
    return false;
}

/*!
  \fn void QCoreApplication::sendPostedEvents()
  \overload

    Dispatches all posted events, i.e. empties the event queue.
*/

/*!
  Immediately dispatches all events which have been previously queued
  with QCoreApplication::postEvent() and which are for the object \a receiver
  and have the event type \a event_type.

  Events from the window system are \e not dispatched by this
  function, but by processEvents().

  If \a receiver is null, the events of \a event_type are sent for all
  objects. If \a event_type is 0, all the events are sent for \a receiver.

  \note This method must be called from the same thread as its QObject parameter, \a receiver.

  \sa flush(), postEvent()
*/

void QCoreApplication::sendPostedEvents(QObject *receiver, int event_type)
{
    QThreadData *data = QThreadData::current();

    QCoreApplicationPrivate::sendPostedEvents(receiver, event_type, data);
}

void QCoreApplicationPrivate::sendPostedEvents(QObject *receiver, int event_type,
                                               QThreadData *data)
{
    if (event_type == -1) {
        // we were called by an obsolete event dispatcher.
        event_type = 0;
    }

    if (receiver && receiver->d_func()->threadData != data) {
        qWarning("QCoreApplication::sendPostedEvents: Cannot send "
                 "posted events for objects in another thread");
        return;
    }

    ++data->postEventList.recursion;

#ifdef QT3_SUPPORT
    if (event_type == QEvent::ChildInserted) {
        if (receiver) {
            // optimize sendPostedEvents(w, QEvent::ChildInserted) calls away
            receiver->d_func()->sendPendingChildInsertedEvents();
            --data->postEventList.recursion;
            return;
        }

        // ChildInserted events are sent in response to *Request
        event_type = QEvent::ChildInsertedRequest;
    }
#endif

    QMutexLocker locker(&data->postEventList.mutex);

    // by default, we assume that the event dispatcher can go to sleep after
    // processing all events. if any new events are posted while we send
    // events, canWait will be set to false.
    data->canWait = (data->postEventList.size() == 0);

    if (data->postEventList.size() == 0 || (receiver && !receiver->d_func()->postedEvents)) {
        --data->postEventList.recursion;
        return;
    }

    data->canWait = true;

    // okay. here is the tricky loop. be careful about optimizing
    // this, it looks the way it does for good reasons.
    int startOffset = data->postEventList.startOffset;
    int &i = (!event_type && !receiver) ? data->postEventList.startOffset : startOffset;
    data->postEventList.insertionOffset = data->postEventList.size();

    while (i < data->postEventList.size()) {
        // avoid live-lock
        if (i >= data->postEventList.insertionOffset)
            break;

        const QPostEvent &pe = data->postEventList.at(i);
        ++i;

        if (!pe.event)
            continue;
        if ((receiver && receiver != pe.receiver) || (event_type && event_type != pe.event->type())) {
            data->canWait = false;
            continue;
        }

        if (pe.event->type() == QEvent::DeferredDelete) {
            // DeferredDelete events are only sent when we are explicitly asked to
            // (s.a. QEvent::DeferredDelete), and then only if the event loop that
            // posted the event has returned.
            const bool allowDeferredDelete =
                (quintptr(pe.event->d) > unsigned(data->loopLevel)
                 || (!quintptr(pe.event->d) && data->loopLevel > 0)
                 || (event_type == QEvent::DeferredDelete
                     && quintptr(pe.event->d) == unsigned(data->loopLevel)));
            if (!allowDeferredDelete) {
                // cannot send deferred delete
                if (!event_type && !receiver) {
                    // don't lose the event
                    data->postEventList.append(pe);
                    const_cast<QPostEvent &>(pe).event = 0;
                }
                continue;
            }
        }

        // first, we diddle the event so that we can deliver
        // it, and that no one will try to touch it later.
        pe.event->posted = false;
        QEvent * e = pe.event;
        QObject * r = pe.receiver;

        --r->d_func()->postedEvents;
        Q_ASSERT(r->d_func()->postedEvents >= 0);

        // next, update the data structure so that we're ready
        // for the next event.
        const_cast<QPostEvent &>(pe).event = 0;

        locker.unlock();
        // after all that work, it's time to deliver the event.
#ifdef QT_NO_EXCEPTIONS
        QCoreApplication::sendEvent(r, e);
#else
        try {
            QCoreApplication::sendEvent(r, e);
        } catch (...) {
            delete e;
            locker.relock();

            // since we were interrupted, we need another pass to make sure we clean everything up
            data->canWait = false;

            // uglehack: copied from below
            --data->postEventList.recursion;
            if (!data->postEventList.recursion && !data->canWait && data->eventDispatcher)
                data->eventDispatcher->wakeUp();
            throw;              // rethrow
        }
#endif

        delete e;
        locker.relock();

        // careful when adding anything below this point - the
        // sendEvent() call might invalidate any invariants this
        // function depends on.
    }

    --data->postEventList.recursion;
    if (!data->postEventList.recursion && !data->canWait && data->eventDispatcher)
        data->eventDispatcher->wakeUp();

    // clear the global list, i.e. remove everything that was
    // delivered.
    if (!event_type && !receiver && data->postEventList.startOffset >= 0) {
        const QPostEventList::iterator it = data->postEventList.begin();
        data->postEventList.erase(it, it + data->postEventList.startOffset);
        data->postEventList.insertionOffset -= data->postEventList.startOffset;
        Q_ASSERT(data->postEventList.insertionOffset >= 0);
        data->postEventList.startOffset = 0;
    }
}

/*!
  Removes all events posted using postEvent() for \a receiver.

  The events are \e not dispatched, instead they are removed from the
  queue. You should never need to call this function. If you do call it,
  be aware that killing events may cause \a receiver to break one or
  more invariants.

  \threadsafe
*/

void QCoreApplication::removePostedEvents(QObject *receiver)
{
    removePostedEvents(receiver, 0);
}

/*!
    \overload
    \since 4.3

    Removes all events of the given \a eventType that were posted
    using postEvent() for \a receiver.

    The events are \e not dispatched, instead they are removed from
    the queue. You should never need to call this function. If you do
    call it, be aware that killing events may cause \a receiver to
    break one or more invariants.

    If \a receiver is null, the events of \a eventType are removed for
    all objects. If \a eventType is 0, all the events are removed for
    \a receiver.

    \threadsafe
*/

void QCoreApplication::removePostedEvents(QObject *receiver, int eventType)
{
#ifdef QT3_SUPPORT
    if (eventType == QEvent::ChildInserted)
        eventType = QEvent::ChildInsertedRequest;
#endif

    QThreadData *data = receiver ? receiver->d_func()->threadData : QThreadData::current();
    QMutexLocker locker(&data->postEventList.mutex);

    // the QObject destructor calls this function directly.  this can
    // happen while the event loop is in the middle of posting events,
    // and when we get here, we may not have any more posted events
    // for this object.
    if (receiver && !receiver->d_func()->postedEvents)
        return;
    QCoreApplicationPrivate::removePostedEvents_unlocked(receiver, eventType, data);
}

void QCoreApplicationPrivate::removePostedEvents_unlocked(QObject *receiver,
                                                          int eventType,
                                                          QThreadData *data)
{
    int n = data->postEventList.size();
    int j = 0;

    for (int i = 0; i < n; ++i) {
        const QPostEvent &pe = data->postEventList.at(i);

        if ((!receiver || pe.receiver == receiver)
            && (pe.event && (eventType == 0 || pe.event->type() == eventType))) {
            --pe.receiver->d_func()->postedEvents;
#ifdef QT3_SUPPORT
            if (pe.event->type() == QEvent::ChildInsertedRequest)
                pe.receiver->d_func()->removePendingChildInsertedEvents(0);
#endif
            pe.event->posted = false;
            delete pe.event;
            const_cast<QPostEvent &>(pe).event = 0;
        } else if (!data->postEventList.recursion) {
            if (i != j)
                data->postEventList.swap(i, j);
            ++j;
        }
    }

#ifdef QT_DEBUG
    if (receiver && eventType == 0) {
        Q_ASSERT(!receiver->d_func()->postedEvents);
    }
#endif

    if (!data->postEventList.recursion) {
        // truncate list
        data->postEventList.erase(data->postEventList.begin() + j, data->postEventList.end());
    }
}


/*!
  Removes \a event from the queue of posted events, and emits a
  warning message if appropriate.

  \warning This function can be \e really slow. Avoid using it, if
  possible.

  \threadsafe
*/

void QCoreApplicationPrivate::removePostedEvent(QEvent * event)
{
    if (!event || !event->posted)
        return;

    QThreadData *data = QThreadData::current();

    QMutexLocker locker(&data->postEventList.mutex);

    if (data->postEventList.size() == 0) {
#if defined(QT_DEBUG)
        qDebug("QCoreApplication::removePostedEvent: Internal error: %p %d is posted",
                (void*)event, event->type());
        return;
#endif
    }

    for (int i = 0; i < data->postEventList.size(); ++i) {
        const QPostEvent & pe = data->postEventList.at(i);
        if (pe.event == event) {
#ifndef QT_NO_DEBUG
            qWarning("QCoreApplication::removePostedEvent: Event of type %d deleted while posted to %s %s",
                     event->type(),
                     pe.receiver->metaObject()->className(),
                     pe.receiver->objectName().toLocal8Bit().data());
#endif
            --pe.receiver->d_func()->postedEvents;
            pe.event->posted = false;
            delete pe.event;
            const_cast<QPostEvent &>(pe).event = 0;
            return;
        }
    }
}

/*!\reimp

*/
bool QCoreApplication::event(QEvent *e)
{
    if (e->type() == QEvent::Quit) {
        quit();
        return true;
    }
    return QObject::event(e);
}

/*! \enum QCoreApplication::Encoding

    This enum type defines the 8-bit encoding of character string
    arguments to translate():

    \value CodecForTr  The encoding specified by
                       QTextCodec::codecForTr() (Latin-1 if none has
                       been set).
    \value UnicodeUTF8  UTF-8.
    \value DefaultCodec  (Obsolete) Use CodecForTr instead.

    \sa QObject::tr(), QObject::trUtf8(), QString::fromUtf8()
*/

/*!
    Tells the application to exit with return code 0 (success).
    Equivalent to calling QCoreApplication::exit(0).

    It's common to connect the QApplication::lastWindowClosed() signal
    to quit(), and you also often connect e.g. QAbstractButton::clicked() or
    signals in QAction, QMenu, or QMenuBar to it.

    Example:

    \snippet doc/src/snippets/code/src.corelib.kernel.qcoreapplication.cpp 1

    \sa exit(), aboutToQuit(), QApplication::lastWindowClosed()
*/

void QCoreApplication::quit()
{
    exit(0);
}

/*!
  \fn void QCoreApplication::aboutToQuit()

  This signal is emitted when the application is about to quit the
  main event loop, e.g. when the event loop level drops to zero.
  This may happen either after a call to quit() from inside the
  application or when the users shuts down the entire desktop session.

  The signal is particularly useful if your application has to do some
  last-second cleanup. Note that no user interaction is possible in
  this state.

  \sa quit()
*/

#ifndef QT_NO_TRANSLATION
/*!
    Adds the translation file \a translationFile to the list of
    translation files to be used for translations.

    Multiple translation files can be installed. Translations are
    searched for in the last installed translation file on, back to
    the first installed translation file. The search stops as soon as
    a matching translation is found.

  \sa removeTranslator() translate() QTranslator::load()
*/

void QCoreApplication::installTranslator(QTranslator *translationFile)
{
    if (!translationFile)
        return;

    if (!QCoreApplicationPrivate::checkInstance("installTranslator"))
        return;
    QCoreApplicationPrivate *d = self->d_func();
    d->translators.prepend(translationFile);

#ifndef QT_NO_TRANSLATION_BUILDER
    if (translationFile->isEmpty())
        return;
#endif

    QEvent ev(QEvent::LanguageChange);
    QCoreApplication::sendEvent(self, &ev);
}

/*!
    Removes the translation file \a translationFile from the list of
    translation files used by this application. (It does not delete the
    translation file from the file system.)

    \sa installTranslator() translate(), QObject::tr()
*/

void QCoreApplication::removeTranslator(QTranslator *translationFile)
{
    if (!translationFile)
        return;
    if (!QCoreApplicationPrivate::checkInstance("removeTranslator"))
        return;
    QCoreApplicationPrivate *d = self->d_func();
    if (d->translators.removeAll(translationFile) && !self->closingDown()) {
        QEvent ev(QEvent::LanguageChange);
        QCoreApplication::sendEvent(self, &ev);
    }
}

/*!
    \overload
*/
QString QCoreApplication::translate(const char *context, const char *sourceText,
                                    const char *comment, Encoding encoding)
{
    return translate(context, sourceText, comment, encoding, -1);
}

/*!
    \reentrant

    Returns the translation text for \a sourceText, by querying the
    installed translation files. The translation files are searched
    from the most recently installed file back to the first
    installed file.

    QObject::tr() and QObject::trUtf8() provide this functionality
    more conveniently.

    \a context is typically a class name (e.g., "MyDialog") and \a
    sourceText is either English text or a short identifying text.

    \a comment is a disambiguating comment, for when the same \a
    sourceText is used in different roles within the same context. By
    default, it is null. \a encoding indicates the 8-bit encoding of
    character stings See the \l QTranslator documentation for more
    information about contexts and comments.

    \a n is used in conjunction with \c %n to support plural forms.
    See QObject::tr() for details.

    If none of the translation files contain a translation for \a
    sourceText in \a context, this function returns a QString
    equivalent of \a sourceText. The encoding of \a sourceText is
    specified by \e encoding; it defaults to CodecForTr.

    This function is not virtual. You can use alternative translation
    techniques by subclassing \l QTranslator.

    \warning This method is reentrant only if all translators are
    installed \e before calling this method. Installing or removing
    translators while performing translations is not supported. Doing
    so will most likely result in crashes or other undesirable
    behavior.

    \sa QObject::tr() installTranslator() QTextCodec::codecForTr()
*/


QString QCoreApplication::translate(const char *context, const char *sourceText,
                                    const char *comment, Encoding encoding, int n)
{
    QString result;

    if (!sourceText)
        return result;

    if (self && !self->d_func()->translators.isEmpty()) {
        QList<QTranslator*>::ConstIterator it;
        QTranslator *translationFile;
        for (it = self->d_func()->translators.constBegin(); it != self->d_func()->translators.constEnd(); ++it) {
            translationFile = *it;
            result = translationFile->translate(context, sourceText, comment, n);
            if (!result.isEmpty())
                break;
        }
    }

    if (result.isEmpty()) {
#ifdef QT_NO_TEXTCODEC
        Q_UNUSED(encoding)
#else
        if (encoding == UnicodeUTF8)
            result = QString::fromUtf8(sourceText);
        else if (QTextCodec::codecForTr() != 0)
            result = QTextCodec::codecForTr()->toUnicode(sourceText);
        else
#endif
            result = QString::fromLatin1(sourceText);
    }

    if (n >= 0) {
        int percentPos = -1;
        while ((percentPos = result.indexOf(QLatin1Char('%'), percentPos + 1)) != -1) {
            int len = 1;
            QString fmt(QLatin1String("%1"));
            if (result.mid(percentPos + len, 1).startsWith(QLatin1Char('L'))) {
                ++len;
                fmt = QLatin1String("%L1");
            }
            if (result.mid(percentPos + len, 1).startsWith(QLatin1Char('n'))) {
                ++len;
                result.replace(percentPos, len, fmt.arg(n));
            }
        }
    }
    return result;
}

bool QCoreApplicationPrivate::isTranslatorInstalled(QTranslator *translator)
{
    return QCoreApplication::self
           && QCoreApplication::self->d_func()->translators.contains(translator);
}

#endif //QT_NO_TRANSLATE

/*!
    Returns the directory that contains the application executable.

    For example, if you have installed Qt in the \c{C:\Trolltech\Qt}
    directory, and you run the \c{regexp} example, this function will
    return "C:/Trolltech/Qt/examples/tools/regexp".

    On Mac OS X this will point to the directory actually containing the
    executable, which may be inside of an application bundle (if the
    application is bundled).

    \warning On Unix, this function assumes that argv[0] contains the file
    name of the executable (which it normally does). It also assumes that
    the current directory hasn't been changed by the application.

    \sa applicationFilePath()
*/
QString QCoreApplication::applicationDirPath()
{
    if (!self) {
        qWarning("QCoreApplication::applicationDirPath: Please instantiate the QApplication object first");
        return QString();
    }

    QCoreApplicationPrivate *d = self->d_func();
    if (d->cachedApplicationDirPath == QString())
        d->cachedApplicationDirPath = QFileInfo(applicationFilePath()).path();
    return d->cachedApplicationDirPath;
}

/*!
    Returns the file path of the application executable.

    For example, if you have installed Qt in the \c{/usr/local/qt}
    directory, and you run the \c{regexp} example, this function will
    return "/usr/local/qt/examples/tools/regexp/regexp".

    \warning On Unix, this function assumes that argv[0] contains the file
    name of the executable (which it normally does). It also assumes that
    the current directory hasn't been changed by the application.

    \sa applicationDirPath()
*/
QString QCoreApplication::applicationFilePath()
{
    if (!self) {
        qWarning("QCoreApplication::applicationFilePath: Please instantiate the QApplication object first");
        return QString();
    }

    QCoreApplicationPrivate *d = self->d_func();
    if (d->cachedApplicationFilePath != QString())
        return d->cachedApplicationFilePath;

#if defined( Q_WS_WIN )
    QFileInfo filePath;
    QT_WA({
        wchar_t module_name[256];
        GetModuleFileNameW(0, module_name, sizeof(module_name) / sizeof(wchar_t));
        filePath = QString::fromUtf16((ushort *)module_name);
    }, {
        char module_name[256];
        GetModuleFileNameA(0, module_name, sizeof(module_name));
        filePath = QString::fromLocal8Bit(module_name);
    });

    d->cachedApplicationFilePath = filePath.filePath();
    return d->cachedApplicationFilePath;
#elif defined(Q_WS_MAC)
    QString qAppFileName_str = qAppFileName();
    if(!qAppFileName_str.isEmpty()) {
        QFileInfo fi(qAppFileName_str);
        d->cachedApplicationFilePath = fi.exists() ? fi.canonicalFilePath() : QString();
        return d->cachedApplicationFilePath;
    }
#endif
#if defined( Q_OS_UNIX )
#  ifdef Q_OS_LINUX
    // Try looking for a /proc/<pid>/exe symlink first which points to
    // the absolute path of the executable
    QFileInfo pfi(QString::fromLatin1("/proc/%1/exe").arg(getpid()));
    if (pfi.exists() && pfi.isSymLink()) {
        d->cachedApplicationFilePath = pfi.canonicalFilePath();
        return d->cachedApplicationFilePath;
    }
#  endif

    QString argv0 = QFile::decodeName(QByteArray(argv()[0]));
    QString absPath;

    if (!argv0.isEmpty() && argv0.at(0) == QLatin1Char('/')) {
        /*
          If argv0 starts with a slash, it is already an absolute
          file path.
        */
        absPath = argv0;
    } else if (argv0.contains(QLatin1Char('/'))) {
        /*
          If argv0 contains one or more slashes, it is a file path
          relative to the current directory.
        */
        absPath = QDir::current().absoluteFilePath(argv0);
    } else {
        /*
          Otherwise, the file path has to be determined using the
          PATH environment variable.
        */
        QByteArray pEnv = qgetenv("PATH");
        QDir currentDir = QDir::current();
        QStringList paths = QString::fromLocal8Bit(pEnv.constData()).split(QLatin1String(":"));
        for (QStringList::const_iterator p = paths.constBegin(); p != paths.constEnd(); ++p) {
            if ((*p).isEmpty())
                continue;
            QString candidate = currentDir.absoluteFilePath(*p + QLatin1Char('/') + argv0);
            QFileInfo candidate_fi(candidate);
            if (candidate_fi.exists() && !candidate_fi.isDir()) {
                absPath = candidate;
                break;
            }
        }
    }

    absPath = QDir::cleanPath(absPath);

    QFileInfo fi(absPath);
    d->cachedApplicationFilePath = fi.exists() ? fi.canonicalFilePath() : QString();
    return d->cachedApplicationFilePath;
#endif
}

/*!
    Returns the current process ID for the application.
*/
qint64 QCoreApplication::applicationPid()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    return GetCurrentProcessId();
#else
    // UNIX
    return getpid();
#endif
}

/*!
    \obsolete

    Use arguments().size() instead.
*/
int QCoreApplication::argc()
{
    if (!self) {
        qWarning("QCoreApplication::argc: Please instantiate the QApplication object first");
        return 0;
    }
    return self->d_func()->argc;
}


/*!
    \obsolete

    Use arguments() instead.
*/
char **QCoreApplication::argv()
{
    if (!self) {
        qWarning("QCoreApplication::argv: Please instantiate the QApplication object first");
        return 0;
    }
    return self->d_func()->argv;
}

/*!
    \since 4.1

    Returns the list of command-line arguments.

    arguments().at(0) is the program name, arguments().at(1) is the
    first argument, and arguments().last() is the last argument.

    Calling this function is slow - you should store the result in a variable
    when parsing the command line.

    \warning On Unix, this list is built from the argc and argv parameters passed
    to the constructor in the main() function. The string-data in argv is
    interpreted using QString::fromLocal8Bit(); hence it is not possible to
    pass i.e. Japanese command line arguments on a system that runs in a latin1
    locale. Most modern Unix systems do not have this limitation, as they are
    Unicode based.

    On NT-based Windows, this limitation does not apply either.
    On Windows, the arguments() are not built from the contents of argv/argc, as
    the content does not support Unicode. Instead, the arguments() are constructed
    from the return value of
    \l{http://msdn2.microsoft.com/en-us/library/ms683156(VS.85).aspx}{GetCommandLine()}.
*/

QStringList QCoreApplication::arguments()
{
    QStringList list;

    if (!self) {
        qWarning("QCoreApplication::arguments: Please instantiate the QApplication object first");
        return list;
    }
#ifdef Q_OS_WIN
    QString cmdline = QT_WA_INLINE(QString::fromUtf16((unsigned short *)GetCommandLineW()), QString::fromLocal8Bit(GetCommandLineA()));

#if defined(Q_OS_WINCE)
    wchar_t tempFilename[512];
    if (GetModuleFileNameW(0, tempFilename, 512))
        cmdline.prepend(QString(QLatin1String("\"")) + QString::fromUtf16((unsigned short *)tempFilename) + QString(QLatin1String("\" ")));
#endif // Q_OS_WINCE

    list = qWinCmdArgs(cmdline);
    if (self->d_func()->application_type) { // GUI app? Skip known - see qapplication.cpp
        QStringList stripped;
        for (int a = 0; a < list.count(); ++a) {
            QString arg = list.at(a);
            QByteArray l1arg = arg.toLatin1();
            if (l1arg == "-qdevel" ||
                l1arg == "-qdebug" ||
                l1arg == "-reverse" ||
                l1arg == "-widgetcount" ||
                l1arg == "-direct3d")
                ;
            else if (l1arg.startsWith("-style="))
                ;
            else if (l1arg == "-style" ||
                l1arg == "-session")
                ++a;
            else
                stripped += arg;
        }
        list = stripped;
    }
#else
    const int ac = self->d_func()->argc;
    char ** const av = self->d_func()->argv;
    for (int a = 0; a < ac; ++a) {
        list << QString::fromLocal8Bit(av[a]);
    }
#endif

    return list;
}

/*!
    \property QCoreApplication::organizationName
    \brief the name of the organization that wrote this application

    The value is used by the QSettings class when it is constructed
    using the empty constructor. This saves having to repeat this
    information each time a QSettings object is created.

    On Mac, QSettings uses organizationDomain() as the organization
    if it's not an empty string; otherwise it uses
    organizationName(). On all other platforms, QSettings uses
    organizationName() as the organization.

    \sa organizationDomain applicationName
*/

void QCoreApplication::setOrganizationName(const QString &orgName)
{
    coreappdata()->orgName = orgName;
}

QString QCoreApplication::organizationName()
{
    return coreappdata()->orgName;
}

/*!
    \property QCoreApplication::organizationDomain
    \brief the Internet domain of the organization that wrote this application

    The value is used by the QSettings class when it is constructed
    using the empty constructor. This saves having to repeat this
    information each time a QSettings object is created.

    On Mac, QSettings uses organizationDomain() as the organization
    if it's not an empty string; otherwise it uses organizationName().
    On all other platforms, QSettings uses organizationName() as the
    organization.

    \sa organizationName applicationName applicationVersion
*/
void QCoreApplication::setOrganizationDomain(const QString &orgDomain)
{
    coreappdata()->orgDomain = orgDomain;
}

QString QCoreApplication::organizationDomain()
{
    return coreappdata()->orgDomain;
}

/*!
    \property QCoreApplication::applicationName
    \brief the name of this application

    The value is used by the QSettings class when it is constructed
    using the empty constructor. This saves having to repeat this
    information each time a QSettings object is created.

    \sa organizationName organizationDomain applicationVersion
*/
void QCoreApplication::setApplicationName(const QString &application)
{
    coreappdata()->application = application;
}

QString QCoreApplication::applicationName()
{
    return coreappdata()->application;
}

/*!
    \property QCoreApplication::applicationVersion
    \since 4.4
    \brief the version of this application

    \sa applicationName organizationName organizationDomain
*/
void QCoreApplication::setApplicationVersion(const QString &version)
{
    coreappdata()->applicationVersion = version;
}

QString QCoreApplication::applicationVersion()
{
    return coreappdata()->applicationVersion;
}

#if !defined(QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)

Q_GLOBAL_STATIC_WITH_ARGS(QMutex, libraryPathMutex, (QMutex::Recursive))

/*!
    Returns a list of paths that the application will search when
    dynamically loading libraries.

    Qt provides default library paths, but they can also be set using
    a \l{Using qt.conf}{qt.conf} file. Paths specified in this file
    will override default values.

    This list will include the installation directory for plugins if
    it exists (the default installation directory for plugins is \c
    INSTALL/plugins, where \c INSTALL is the directory where Qt was
    installed).  The directory of the application executable (NOT the
    working directory) is always added, as well as the colon separated
    entries of the QT_PLUGIN_PATH environment variable.

    If you want to iterate over the list, you can use the \l foreach
    pseudo-keyword:

    \snippet doc/src/snippets/code/src.corelib.kernel.qcoreapplication.cpp 2

    \sa setLibraryPaths(), addLibraryPath(), removeLibraryPath(), QLibrary,
        {How to Create Qt Plugins}
*/
QStringList QCoreApplication::libraryPaths()
{
    QMutexLocker locker(libraryPathMutex());
    if (!coreappdata()->app_libpaths) {
        QStringList *app_libpaths = coreappdata()->app_libpaths = new QStringList;
        QString installPathPlugins =  QLibraryInfo::location(QLibraryInfo::PluginsPath);
        if (QFile::exists(installPathPlugins)) {
            // Make sure we convert from backslashes to slashes.
            installPathPlugins = QDir(installPathPlugins).canonicalPath();
            if (!app_libpaths->contains(installPathPlugins))
                app_libpaths->append(installPathPlugins);
        }

        // If QCoreApplication is not yet instantiated,
        // make sure we add the application path when we construct the QCoreApplication
        if (self) self->d_func()->appendApplicationPathToLibraryPaths();

        const QByteArray libPathEnv = qgetenv("QT_PLUGIN_PATH");
        if (!libPathEnv.isEmpty()) {
#ifdef Q_OS_WIN
            QLatin1Char pathSep(';');
#else
            QLatin1Char pathSep(':');
#endif
            QStringList paths = QString::fromLatin1(libPathEnv).split(pathSep, QString::SkipEmptyParts);
            for (QStringList::const_iterator it = paths.constBegin(); it != paths.constEnd(); ++it) {
                QString canonicalPath = QDir(*it).canonicalPath();
                if (!canonicalPath.isEmpty()
                    && !app_libpaths->contains(canonicalPath)) {
                    app_libpaths->append(canonicalPath);
                }
            }
        }
    }
    return *(coreappdata()->app_libpaths);
}



/*!

    Sets the list of directories to search when loading libraries to
    \a paths. All existing paths will be deleted and the path list
    will consist of the paths given in \a paths.

    \sa libraryPaths(), addLibraryPath(), removeLibraryPath(), QLibrary
 */
void QCoreApplication::setLibraryPaths(const QStringList &paths)
{
    QMutexLocker locker(libraryPathMutex());
    if (!coreappdata()->app_libpaths)
        coreappdata()->app_libpaths = new QStringList;
    *(coreappdata()->app_libpaths) = paths;
    QFactoryLoader::refreshAll();
}

/*!
  Prepends \a path to the end of the library path list. If \a path is
  empty or already in the path list, the path list is not changed.

  The default path list consists of a single entry, the installation
  directory for plugins.  The default installation directory for plugins
  is \c INSTALL/plugins, where \c INSTALL is the directory where Qt was
  installed.

  \sa removeLibraryPath(), libraryPaths(), setLibraryPaths()
 */
void QCoreApplication::addLibraryPath(const QString &path)
{
    if (path.isEmpty())
        return;

    QMutexLocker locker(libraryPathMutex());

    // make sure that library paths is initialized
    libraryPaths();

    QString canonicalPath = QDir(path).canonicalPath();
    if (!canonicalPath.isEmpty()
        && !coreappdata()->app_libpaths->contains(canonicalPath)) {
        coreappdata()->app_libpaths->prepend(canonicalPath);
        QFactoryLoader::refreshAll();
    }
}

/*!
    Removes \a path from the library path list. If \a path is empty or not
    in the path list, the list is not changed.

    \sa addLibraryPath(), libraryPaths(), setLibraryPaths()
*/
void QCoreApplication::removeLibraryPath(const QString &path)
{
    if (path.isEmpty())
        return;

    QMutexLocker locker(libraryPathMutex());

    // make sure that library paths is initialized
    libraryPaths();

    QString canonicalPath = QDir(path).canonicalPath();
    coreappdata()->app_libpaths->removeAll(canonicalPath);
    QFactoryLoader::refreshAll();
}

#endif //QT_NO_LIBRARY

/*!
    \typedef QCoreApplication::EventFilter

    A function with the following signature that can be used as an
    event filter:

    \snippet doc/src/snippets/code/src.corelib.kernel.qcoreapplication.cpp 3

    \sa setEventFilter()
*/

/*!
    \fn EventFilter QCoreApplication::setEventFilter(EventFilter filter)

    Sets the event filter \a filter. Returns a pointer to the filter
    function previously defined.

    The event filter is a function that is called for every message
    received in all threads. This does \e not include messages to
    objects that are not handled by Qt.

    The function can return true to stop the event to be processed by
    Qt, or false to continue with the standard event processing.

    Only one filter can be defined, but the filter can use the return
    value to call the previously set event filter. By default, no
    filter is set (i.e., the function returns 0).

    \sa installEventFilter()
*/
QCoreApplication::EventFilter
QCoreApplication::setEventFilter(QCoreApplication::EventFilter filter)
{
    Q_D(QCoreApplication);
    EventFilter old = d->eventFilter;
    d->eventFilter = filter;
    return old;
}

/*!
    Sends \a message through the event filter that was set by
    setEventFilter(). If no event filter has been set, this function
    returns false; otherwise, this function returns the result of the
    event filter function in the \a result parameter.

    \sa setEventFilter()
*/
bool QCoreApplication::filterEvent(void *message, long *result)
{
    Q_D(QCoreApplication);
    if (result)
        *result = 0;
    if (d->eventFilter)
        return d->eventFilter(message, result);
#ifdef Q_OS_WIN
    return winEventFilter(reinterpret_cast<MSG *>(message), result);
#else
    return false;
#endif
}

/*!
    This function returns true if there are pending events; otherwise
    returns false. Pending events can be either from the window
    system or posted events using postEvent().

    \sa QAbstractEventDispatcher::hasPendingEvents()
*/
bool QCoreApplication::hasPendingEvents()
{
    QAbstractEventDispatcher *eventDispatcher = QAbstractEventDispatcher::instance();
    if (eventDispatcher)
        return eventDispatcher->hasPendingEvents();
    return false;
}

#ifdef QT3_SUPPORT
/*! \fn void QCoreApplication::lock()

    In Qt 3, this function locked the Qt library mutex, allowing
    non-GUI threads to perform basic printing operations using
    QPainter.

    In Qt 4, this is no longer supported, since painting is only
    supported from within a paint event handler. This function does
    nothing.

    \sa QWidget::paintEvent()
*/

/*! \fn void QCoreApplication::unlock(bool wakeUpGui)

    In Qt 3, this function unlocked the Qt library mutex. The mutex
    allowed non-GUI threads to perform basic printing operations
    using QPainter.

    In Qt 4, this is no longer supported, since painting is only
    supported from within a paint event handler. This function does
    nothing.
*/

/*! \fn bool QCoreApplication::locked()

    This function does nothing. It is there to keep old code working.
    It always returns false.

    See lock() for details.
*/

/*! \fn bool QCoreApplication::tryLock()

    This function does nothing. It is there to keep old code working.
    It always returns false.

    See lock() for details.
*/

/*! \fn void QCoreApplication::processOneEvent()
    \obsolete

    Waits for an event to occur, processes it, then returns.

    This function is useful for adapting Qt to situations where the
    event processing must be grafted onto existing program loops.

    Using this function in new applications may be an indication of design
    problems.

    \sa processEvents(), exec(), QTimer
*/

/*! \obsolete

    This function enters the main event loop (recursively). Do not call
    it unless you really know what you are doing.
*/
int QCoreApplication::enter_loop()
{
    if (!QCoreApplicationPrivate::checkInstance("enter_loop"))
        return -1;
    if (QThreadData::current() != self->d_func()->threadData) {
        qWarning("QCoreApplication::enter_loop: Must be called from the main thread");
        return -1;
    }
    QEventLoop eventLoop;
    int returnCode = eventLoop.exec();
    return returnCode;
}

/*! \obsolete

    This function exits from a recursive call to the main event loop.
    Do not call it unless you are an expert.
*/
void QCoreApplication::exit_loop()
{
    if (!QCoreApplicationPrivate::checkInstance("exit_loop"))
        return;
    QThreadData *data = QThreadData::current();
    if (data != self->d_func()->threadData) {
        qWarning("QCoreApplication::exit_loop: Must be called from the main thread");
        return;
    }
    if (!data->eventLoops.isEmpty())
        data->eventLoops.top()->exit();
}

/*! \obsolete

    Returns the current loop level.
*/
int QCoreApplication::loopLevel()
{
    if (!QCoreApplicationPrivate::checkInstance("loopLevel"))
        return -1;
    return self->d_func()->threadData->eventLoops.size();
}
#endif

/*!
    \fn void QCoreApplication::watchUnixSignal(int signal, bool watch)
    \internal
*/

/*!
    \fn void QCoreApplication::unixSignal(int number)
    \internal

    This signal is emitted whenever a Unix signal is received by the
    application. The Unix signal received is specified by its \a number.
*/

/*!
    \fn void qAddPostRoutine(QtCleanUpFunction ptr)
    \relates QCoreApplication

    Adds a global routine that will be called from the QApplication
    destructor. This function is normally used to add cleanup routines
    for program-wide functionality.

    The function specified by \a ptr should take no arguments and should
    return nothing. For example:

    \snippet doc/src/snippets/code/src.corelib.kernel.qcoreapplication.cpp 4

    Note that for an application- or module-wide cleanup,
    qAddPostRoutine() is often not suitable. For example, if the
    program is split into dynamically loaded modules, the relevant
    module may be unloaded long before the QApplication destructor is
    called.

    For modules and libraries, using a reference-counted
    initialization manager or Qt's parent-child deletion mechanism may
    be better. Here is an example of a private class that uses the
    parent-child mechanism to call a cleanup function at the right
    time:

    \snippet doc/src/snippets/code/src.corelib.kernel.qcoreapplication.cpp 5

    By selecting the right parent object, this can often be made to
    clean up the module's data at the right moment.
*/

/*!
    \macro Q_DECLARE_TR_FUNCTIONS(context)
    \relates QCoreApplication

    The Q_DECLARE_TR_FUNCTIONS() macro declares and implements two
    translation functions, \c tr() and \c trUtf8(), with these
    signatures:

    \snippet doc/src/snippets/code/src.corelib.kernel.qcoreapplication.cpp 6

    This macro is useful if you want to use QObject::tr() or
    QObject::trUtf8() in classes that don't inherit from QObject.

    Q_DECLARE_TR_FUNCTIONS() must appear at the very top of the
    class definition (before the first \c{public:} or \c{protected:}).
    For example:

    \snippet doc/src/snippets/code/src.corelib.kernel.qcoreapplication.cpp 7

    The \a context parameter is normally the class name, but it can
    be any string.

    \sa Q_OBJECT, QObject::tr(), QObject::trUtf8()
*/

QT_END_NAMESPACE
