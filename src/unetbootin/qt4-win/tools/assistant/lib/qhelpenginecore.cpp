/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "qhelpenginecore.h"
#include "qhelpengine_p.h"
#include "qhelpdbreader_p.h"
#include "qhelpcollectionhandler_p.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QLibrary>
#include <QtCore/QPluginLoader>
#include <QtCore/QFileInfo>
#include <QtGui/QApplication>
#include <QtSql/QSqlQuery>

QT_BEGIN_NAMESPACE

void QHelpEngineCorePrivate::init(const QString &collectionFile,
                                  QHelpEngineCore *helpEngineCore)
{
    q = helpEngineCore;
    collectionHandler = new QHelpCollectionHandler(collectionFile, helpEngineCore);
    connect(collectionHandler, SIGNAL(error(const QString&)),
        this, SLOT(errorReceived(const QString&)));
    needsSetup = true;
}

QHelpEngineCorePrivate::~QHelpEngineCorePrivate()
{
    delete collectionHandler;
    clearMaps();
}

void QHelpEngineCorePrivate::clearMaps()
{
    QMap<QString, QHelpDBReader*>::iterator it = readerMap.begin();
    while (it != readerMap.end()) {
        delete it.value();
        ++it;
    }
    readerMap.clear();
    fileNameReaderMap.clear();
    virtualFolderMap.clear();
}

bool QHelpEngineCorePrivate::setup()
{
    error.clear();
    if (!needsSetup)
        return true;

    needsSetup = false;    
    emit q->setupStarted();
    clearMaps();

    if (!collectionHandler->openCollectionFile()) {
        emit q->setupFinished();
        return false;
    }

    const QHelpCollectionHandler::DocInfoList docList =
        collectionHandler->registeredDocumentations();
    QFileInfo fi(collectionHandler->collectionFile());
    QString absFileName;
    foreach(const QHelpCollectionHandler::DocInfo info, docList) {
        if (QDir::isAbsolutePath(info.fileName))
            absFileName = info.fileName;
        else
            absFileName = fi.absolutePath() + QDir::separator() + info.fileName;
        QHelpDBReader *reader = new QHelpDBReader(absFileName,
            uniquifyConnectionName(info.fileName, this), this);
        if (!reader->init()) {
            emit q->warning(tr("Cannot open documentation file %1!")
                .arg(absFileName));
            continue;
        }

        readerMap.insert(info.namespaceName, reader);
        fileNameReaderMap.insert(absFileName, reader);
        virtualFolderMap.insert(info.folderName, reader);
    }
    q->currentFilter();
    emit q->setupFinished();
    return true;
}

void QHelpEngineCorePrivate::errorReceived(const QString &msg)
{
    error = msg;
}



/*!
    \class QHelpEngineCore
    \inmodule QtHelp
    \brief The QHelpEngineCore class provides the core functionality
    of the help system.
    
    Before the help engine can be used, it must be initialized by
    calling setupData(). At the beginning of the setup process the
    signal setupStarted() is emitted. From this point on until
    the signal setupFinished() is emitted, is the help data in an
    undefined meaning unusable state.

    The core help engine can be used to perform different tasks.
    By calling linksForIdentifier() the engine returns
    urls specifying the file locations inside the help system. The
    actual file data can then be retrived by calling fileData(). In
    contrast to all other functions in this class, linksForIdentifier()
    depends on the currently set custom filter. Depending on the filter,
    the function may return different hits.

    Every help engine can contain any number of custom filters. A custom
    filter is defined by a name and set of filter attributes and can be
    added to the help engine by calling addCustomFilter(). Analogous,
    it is removed by calling removeCustomFilter(). customFilters() returns
    all defined filters.

    The help engine also offers the possiblity to set and read values
    in a persistant way comparable to ini files or Windows registry
    entries. For more information see setValue() or value().

    This class does not offer any GUI components or functionality for
    indices or contents. If you need one of those use QHelpEngine
    instead.
*/

/*!
    \fn void QHelpEngineCore::setupStarted()

    This signal is emitted when setup is started.
*/

/*!
    \fn void QHelpEngineCore::setupFinished()

    This signal is emitted when the setup is complete.
*/

/*!
    \fn void QHelpEngineCore::currentFilterChanged(const QString &newFilter)

    This signal is emitted when the current filter is changed to
    \a newFilter.
*/

/*!
    \fn void QHelpEngineCore::warning(const QString &msg)

    This signal is emitted when a non critical error occurs.
    The warning message is stored in \a msg.
*/

/*!
    Constructs a new core help engine with a \a parent. The help engine
    uses the information stored in the \a collectionFile to provide help.
    If the collection file does not exist yet, it'll be created.
*/
QHelpEngineCore::QHelpEngineCore(const QString &collectionFile, QObject *parent)
    : QObject(parent)
{
    d = new QHelpEngineCorePrivate();
    d->init(collectionFile, this);    
}

/*!
    \internal
*/
QHelpEngineCore::QHelpEngineCore(QHelpEngineCorePrivate *helpEngineCorePrivate,
                                 QObject *parent)
    : QObject(parent)
{
    d = helpEngineCorePrivate;
}

/*!
    Destructs the help engine.
*/
QHelpEngineCore::~QHelpEngineCore()
{
    delete d;
}

/*!
    Returns the absolute file path of the collection file.
*/
QString QHelpEngineCore::collectionFile() const
{
    return d->collectionHandler->collectionFile();
}

/*!
    Sets the file \a fileName as the collection file for
    the help engine. Calling this function will leave the
    help engine in an invalid state, meaning setupData() or
    any getter function has to be called in order to setup
    the help engine again.
*/
void QHelpEngineCore::setCollectionFile(const QString &fileName)
{
    if (fileName == collectionFile())
        return;

    if (d->collectionHandler) {
        delete d->collectionHandler;
        d->collectionHandler = 0;
        d->clearMaps();
    }
    d->init(fileName, this);
    d->needsSetup = true;    
}

/*!
    Sets up the help engine by processing the information found
    in the collection file. By calling the function, the help
    engine is forced to initialize itself immediately. Most of
    the times, this function does not have to be called
    explicitely, since getter functions which depend on a set up
    help engine do that themselves. If the set up was successful, 
    true is returned otherwise false.
*/
bool QHelpEngineCore::setupData()
{
    d->needsSetup = true;
    return d->setup();    
}

/*!
    Creates the file \a fileName and copies all contents from
    the current collection file into the newly created file,
    and returns true if successful; otherwise returns false.

    The copying process makes sure that file references to Qt
    Collection files (\c{.qch}) files are updated accordingly.
*/
bool QHelpEngineCore::copyCollectionFile(const QString &fileName)
{
    if (!d->setup())
        return false;
    return d->collectionHandler->copyCollectionFile(fileName);
}

/*!
    Returns the namespace name of a documentation set
    specified by its file name. If the file is not valid,
    an empty string is returned.

    \sa documentationFileName()
*/
QString QHelpEngineCore::namespaceName(const QString &documentationFileName)
{
    QHelpDBReader reader(documentationFileName, QLatin1String("GetNamespaceName"), 0);
    if (reader.init())
        return reader.namespaceName();
    return QString();
}

/*!
    Registers the documentation set contained in the file
    \a documentationFileName. One documentation set, uniquely
    identified by its namespace can only be registered once.
    True is returned if the registration was successful, otherwise
    false.

    \sa unregisterDocumentation(), error()
*/
bool QHelpEngineCore::registerDocumentation(const QString &documentationFileName)
{
    d->error.clear();
    d->needsSetup = true;
    return d->collectionHandler->registerDocumentation(documentationFileName);
}

/*!
    Unregisters the documentation set identified by its 
    \a namespaceName from the help collection. Returns true 
    on success, otherwise false.

    \sa registerDocumentation(), error()
*/
bool QHelpEngineCore::unregisterDocumentation(const QString &namespaceName)
{
    d->error.clear();
    d->needsSetup = true;
    return d->collectionHandler->unregisterDocumentation(namespaceName);
}

/*!
    Returns the absolute file name of the Qt documentation file (.qch)
    identified by the \a namespaceName. If there is no documentation
    with the specified namespace registered, an empty string is returned.

    \sa namespaceName()
*/
QString QHelpEngineCore::documentationFileName(const QString &namespaceName)
{
    QString res;
    if (!d->setup())
        return res;
    const QHelpCollectionHandler::DocInfoList docList = d->collectionHandler->registeredDocumentations();
    foreach(const QHelpCollectionHandler::DocInfo info, docList) {
        if (info.namespaceName == namespaceName) {
            QFileInfo fi(d->collectionHandler->collectionFile());
            fi.setFile(fi.absolutePath() + QDir::separator() + info.fileName);
            res = QDir::cleanPath(fi.absoluteFilePath());
            break;
        }        
    }
    return res;
}

/*!
    Returns a list of all documentations registered in the collection
    file. The returned names are the namespaces of the documentation
    sets.
*/
QStringList QHelpEngineCore::registeredDocumentations() const
{
    QStringList list;
    if (!d->setup())
        return list;
    const QHelpCollectionHandler::DocInfoList docList = d->collectionHandler->registeredDocumentations();
    foreach(const QHelpCollectionHandler::DocInfo info, docList) {
        list.append(info.namespaceName);
    }
    return list;
}

/*!
    Returns a list of custom filters.

    \sa addCustomFilter(), removeCustomFilter()
*/
QStringList QHelpEngineCore::customFilters() const
{
    if (!d->setup())
        return QStringList();
    return d->collectionHandler->customFilters();
}

/*!
    Adds the new custom filter \a filterName. The filter attributes
    are specified by \a attributes. The function returns false if 
    the filter can not be added, e.g. when the filter already exists.

    \sa customFilters(), removeCustomFilter()
*/
bool QHelpEngineCore::addCustomFilter(const QString &filterName,
                                      const QStringList &attributes)
{
    d->error.clear();
    d->needsSetup = true;
    return d->collectionHandler->addCustomFilter(filterName,
        attributes);
}

/*!
    Returns true if the filter \a filterName was removed successfully,
    otherwise false.

    \sa addCustomFilter(), customFilters()
*/
bool QHelpEngineCore::removeCustomFilter(const QString &filterName)
{
    d->error.clear();
    d->needsSetup = true;
    return d->collectionHandler->removeCustomFilter(filterName);
}

/*!
    Returns a list of all defined filter attributes.
*/
QStringList QHelpEngineCore::filterAttributes() const
{
    if (!d->setup())
        return QStringList();
    return d->collectionHandler->filterAttributes();
}

/*!
    Returns a list of filter attributes used by the custom
    filter \a filterName.
*/
QStringList QHelpEngineCore::filterAttributes(const QString &filterName) const
{
    if (!d->setup())
        return QStringList();
    return d->collectionHandler->filterAttributes(filterName);
}

/*!
    Returns the name of the current custom filter.
*/
QString QHelpEngineCore::currentFilter() const
{
    if (!d->setup())
        return QString();

    if (d->currentFilter.isEmpty()) {
        QString filter = 
            d->collectionHandler->customValue(QLatin1String("CurrentFilter"),
                QString()).toString();
        if (!filter.isEmpty()
            && d->collectionHandler->customFilters().contains(filter))
            d->currentFilter = filter;
    }
    return d->currentFilter;
}

/*!
    Sets the filter used by the help engine to \a filterName.
*/
void QHelpEngineCore::setCurrentFilter(const QString &filterName)
{
    if (!d->setup() || filterName == d->currentFilter)
        return;
    d->currentFilter = filterName;
    d->collectionHandler->setCustomValue(QLatin1String("CurrentFilter"),
        d->currentFilter);
    emit currentFilterChanged(d->currentFilter);    
}

/*!
    Returns a list of filter attributes for the different filter
    sections defined in the documentation set with the namespace
    \a namespaceName.
*/
QList<QStringList> QHelpEngineCore::filterAttributeSets(const QString &namespaceName) const
{
    if (d->setup()) {
        QHelpDBReader *reader = d->readerMap.value(namespaceName);
        if (reader)
            return reader->filterAttributeSets();
    }
    return QList<QStringList>();
}

/*!
    Returns a list of files contained in the documentation set \a
    namespaceName. The files can be filtered by \a filterAttributes as
    well as by their extension \a extensionFilter (e.g. 'html').
*/
QList<QUrl> QHelpEngineCore::files(const QString namespaceName,
                                   const QStringList &filterAttributes,
                                   const QString &extensionFilter)
{    
    QList<QUrl> res;
    if (!d->setup())
        return res;
    QHelpDBReader *reader = d->readerMap.value(namespaceName);
    if (!reader) {
        d->error = tr("The specified namespace does not exist!");
        return res;
    }

    QUrl url;
    url.setScheme(QLatin1String("qthelp"));
    url.setAuthority(namespaceName);

    const QStringList files = reader->files(filterAttributes, extensionFilter);
    foreach (const QString file, files) {
        url.setPath(QLatin1String("/") + file);
        res.append(url);
    }
    return res;
}

/*!
    Returns an invalid URL if the file \a url cannot be found.
    If the file exists, either the same url is returned or a
    different url if the file is located in a different namespace
    which is merged via a common virtual folder.
*/
QUrl QHelpEngineCore::findFile(const QUrl &url) const
{    
    QUrl res;
    if (!d->setup() || !url.isValid() || url.toString().count(QLatin1Char('/')) < 4
        || url.scheme() != QLatin1String("qthelp"))
        return res;

    QString ns = url.authority();
    QString filePath = QDir::cleanPath(url.path());
    if (filePath.startsWith(QLatin1Char('/')))
        filePath = filePath.mid(1);
    QString virtualFolder = filePath.mid(0, filePath.indexOf(QLatin1Char('/'), 1));
    filePath = filePath.mid(virtualFolder.length()+1);
    
    QHelpDBReader *defaultReader = 0;
    if (d->readerMap.contains(ns)) {
        defaultReader = d->readerMap.value(ns);
        if (defaultReader->fileExists(virtualFolder, filePath))
            return url;        
    }

    QStringList filterAtts = filterAttributes(currentFilter());
    foreach (QHelpDBReader *reader, d->virtualFolderMap.values(virtualFolder)) {
        if (reader == defaultReader)
            continue;
        if (reader->fileExists(virtualFolder, filePath, filterAtts)) {
            res = url;
            res.setAuthority(reader->namespaceName());
            return res;
        }
    }

    foreach (QHelpDBReader *reader, d->virtualFolderMap.values(virtualFolder)) {
        if (reader == defaultReader)
            continue;
        if (reader->fileExists(virtualFolder, filePath)) {
            res = url;
            res.setAuthority(reader->namespaceName());
            break;
        }
    }

    return res;
}

/*!
    Returns the data of the file specified by \a url. If the
    file does not exist, an empty QByteArray is returned.

    /sa findFile()
*/
QByteArray QHelpEngineCore::fileData(const QUrl &url) const
{
    if (!d->setup() || !url.isValid() || url.toString().count(QLatin1Char('/')) < 4
        || url.scheme() != QLatin1String("qthelp"))
        return QByteArray();

    QString ns = url.authority();
    QString filePath = QDir::cleanPath(url.path());
    if (filePath.startsWith(QLatin1Char('/')))
        filePath = filePath.mid(1);
    QString virtualFolder = filePath.mid(0, filePath.indexOf(QLatin1Char('/'), 1));
    filePath = filePath.mid(virtualFolder.length()+1);

    QByteArray ba;
    QHelpDBReader *defaultReader = 0;
    if (d->readerMap.contains(ns)) {
        defaultReader = d->readerMap.value(ns);
        ba = defaultReader->fileData(virtualFolder, filePath);
    }

    if (ba.isEmpty()) {
        foreach (QHelpDBReader *reader, d->virtualFolderMap.values(virtualFolder)) {
            if (reader == defaultReader)
                continue;
            ba = reader->fileData(virtualFolder, filePath);
            if (!ba.isEmpty())
                return ba;
        }
    } 
    return ba;
}

/*!
    Returns a map of hits found for the \a id. A hit contains the
    title of the document and the url where the keyword is located.
    The result depends on the current filter, meaning only the keywords
    registered for the current filter will be returned.
*/
QMap<QString, QUrl> QHelpEngineCore::linksForIdentifier(const QString &id) const
{
    QMap<QString, QUrl> linkMap;
    if (!d->setup())
        return linkMap;

    QStringList atts = filterAttributes(d->currentFilter);
    foreach (QHelpDBReader *reader, d->readerMap)
        reader->linksForIdentifier(id, atts, linkMap);

    return linkMap;
}

/*!
    Removes the \a key from the settings section in the
    collection file. Returns true if the value was removed
    successfully, otherwise false.

    \sa customValue(), setCustomValue()
*/
bool QHelpEngineCore::removeCustomValue(const QString &key)
{
    d->error.clear();
    return d->collectionHandler->removeCustomValue(key);
}

/*!
    Returns the value assigned to the \a key. If the requested
    key does not exist, the specified \a defaultValue is
    returned.

    \sa setCustomValue(), removeCustomValue()
*/
QVariant QHelpEngineCore::customValue(const QString &key, const QVariant &defaultValue) const
{
    if (!d->setup())
        return QVariant();
    return d->collectionHandler->customValue(key, defaultValue);
}

/*!
    Save the \a value under the \a key. If the key already exist,
    the value will be overwritten. Returns true if the value was
    saved successfully, otherwise false.

    \sa customValue(), removeCustomValue()
*/
bool QHelpEngineCore::setCustomValue(const QString &key, const QVariant &value)
{
    d->error.clear();
    return d->collectionHandler->setCustomValue(key, value);
}

/*!
    Returns the meta data for the documentation file \a
    documentationFileName. If there is no data available for
    \a name, an invalid QVariant() is returned. The meta
    data is defined when creating the documention file and
    cannot be modified later. Common meta data includes e.g.
    the author of the documentation.
*/
QVariant QHelpEngineCore::metaData(const QString &documentationFileName,
                                   const QString &name)
{
    QHelpDBReader reader(documentationFileName, QLatin1String("GetMetaData"), 0);
    
    if (reader.init())
        return reader.metaData(name);
    return QVariant();
}

/*!
    Returns a description of the last error that occured.
*/
QString QHelpEngineCore::error() const
{
    return d->error;
}

QT_END_NAMESPACE
