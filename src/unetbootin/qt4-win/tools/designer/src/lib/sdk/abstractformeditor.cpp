/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#include "abstractformeditor.h"
#include "abstractdialoggui_p.h"
#include "abstractintrospection_p.h"

#include <QtDesigner/QDesignerWidgetBoxInterface>
#include <QtDesigner/QDesignerPropertyEditorInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerWidgetDataBaseInterface>
#include <QtDesigner/QDesignerWidgetFactoryInterface>
#include <QtDesigner/QDesignerObjectInspectorInterface>
#include <QtDesigner/QDesignerBrushManagerInterface>
#include <QtDesigner/QDesignerIntegrationInterface>
#include <QtDesigner/QDesignerIconCacheInterface>
#include <QtDesigner/QDesignerActionEditorInterface>
#include <pluginmanager_p.h>
#include <qtresourcemodel_p.h>
#include <qtgradientmanager.h>
#include <QtDesigner/QDesignerPromotionInterface>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterfacePrivate {
public:
    QDesignerFormEditorInterfacePrivate();
    ~QDesignerFormEditorInterfacePrivate();


    QPointer<QWidget> m_topLevel;
    QPointer<QDesignerWidgetBoxInterface> m_widgetBox;
    QPointer<QDesignerPropertyEditorInterface> m_propertyEditor;
    QPointer<QDesignerFormWindowManagerInterface> m_formWindowManager;
    QPointer<QExtensionManager> m_extensionManager;
    QPointer<QDesignerMetaDataBaseInterface> m_metaDataBase;
    QPointer<QDesignerWidgetDataBaseInterface> m_widgetDataBase;
    QPointer<QDesignerWidgetFactoryInterface> m_widgetFactory;
    QPointer<QDesignerObjectInspectorInterface> m_objectInspector;
    QPointer<QDesignerBrushManagerInterface> m_brushManager;
    QPointer<QDesignerIntegrationInterface> m_integration;
    QPointer<QDesignerIconCacheInterface> m_iconCache;
    QPointer<QDesignerActionEditorInterface> m_actionEditor;
    QDesignerPluginManager *m_pluginManager;
    QDesignerPromotionInterface *m_promotion;
    QDesignerIntrospectionInterface *m_introspection;
    QDesignerDialogGuiInterface *m_dialogGui;
    QPointer<QtResourceModel> m_resourceModel;
    QPointer<QtGradientManager> m_gradientManager; // instantiated and deleted by designer_integration
};

QDesignerFormEditorInterfacePrivate::QDesignerFormEditorInterfacePrivate() :
    m_pluginManager(0),
    m_promotion(0),
    m_introspection(0),
    m_dialogGui(0),
    m_resourceModel(0),
    m_gradientManager(0)
{
}

QDesignerFormEditorInterfacePrivate::~QDesignerFormEditorInterfacePrivate()
{
    delete m_formWindowManager;
    delete m_promotion;
    delete m_introspection;
    delete m_dialogGui;
    delete m_resourceModel;
}
/*!
    \class QDesignerFormEditorInterface

    \brief The QDesignerFormEditorInterface class allows you to access
    Qt Designer's various components.

    \inmodule QtDesigner

    \QD's current QDesignerFormEditorInterface object holds
    information about all \QD's components: The action editor, the
    object inspector, the property editor, the widget box, and the
    extension and form window managers. QDesignerFormEditorInterface
    contains a collection of functions that provides interfaces to all
    these components. They are typically used to query (and
    manipulate) the respective component. For example:

    \snippet doc/src/snippets/code/tools.designer.src.lib.sdk.abstractformeditor.cpp 0

    QDesignerFormEditorInterface is not intended to be instantiated
    directly. A pointer to \QD's current QDesignerFormEditorInterface
    object (\c formEditor in the example above) is provided by the
    QDesignerCustomWidgetInterface::initialize() function's
    parameter. When implementing a custom widget plugin, you must
    subclass the QDesignerCustomWidgetInterface to expose your plugin
    to \QD.

    QDesignerFormEditorInterface also provides functions that can set
    the action editor, property editor, object inspector and widget
    box. These are only useful if you want to provide your own custom
    components.

    Finally, QDesignerFormEditorInterface provides the topLevel()
    function that returns \QD's top-level widget.

    \sa QDesignerCustomWidgetInterface
*/

/*!
    Constructs a QDesignerFormEditorInterface object with the given \a
    parent.
*/

QDesignerFormEditorInterface::QDesignerFormEditorInterface(QObject *parent)
    : QObject(parent),
      d(new QDesignerFormEditorInterfacePrivate)
{
}

/*!
    Destroys the QDesignerFormEditorInterface object.
*/
QDesignerFormEditorInterface::~QDesignerFormEditorInterface()
{
    delete d;
}

/*!
    Returns an interface to \QD's widget box.

    \sa setWidgetBox()
*/
QDesignerWidgetBoxInterface *QDesignerFormEditorInterface::widgetBox() const
{
    return d->m_widgetBox;
}

/*!
    Sets \QD's widget box to be the specified \a widgetBox.

    \sa widgetBox()
*/
void QDesignerFormEditorInterface::setWidgetBox(QDesignerWidgetBoxInterface *widgetBox)
{
    d->m_widgetBox = widgetBox;
}

/*!
    Returns an interface to \QD's property editor.

    \sa setPropertyEditor()
*/
QDesignerPropertyEditorInterface *QDesignerFormEditorInterface::propertyEditor() const
{
    return d->m_propertyEditor;
}

/*!
    Sets \QD's property editor to be the specified \a propertyEditor.

    \sa propertyEditor()
*/
void QDesignerFormEditorInterface::setPropertyEditor(QDesignerPropertyEditorInterface *propertyEditor)
{
    d->m_propertyEditor = propertyEditor;
}

/*!
    Returns an interface to \QD's action editor.

    \sa setActionEditor()
*/
QDesignerActionEditorInterface *QDesignerFormEditorInterface::actionEditor() const
{
    return d->m_actionEditor;
}

/*!
    Sets \QD's action editor to be the specified \a actionEditor.

    \sa actionEditor()
*/
void QDesignerFormEditorInterface::setActionEditor(QDesignerActionEditorInterface *actionEditor)
{
    d->m_actionEditor = actionEditor;
}

/*!
    Returns \QD's top-level widget.
*/
QWidget *QDesignerFormEditorInterface::topLevel() const
{
    return d->m_topLevel;
}

/*!
    \internal
*/
void QDesignerFormEditorInterface::setTopLevel(QWidget *topLevel)
{
    d->m_topLevel = topLevel;
}

/*!
    Returns an interface to \QD's form window manager.
*/
QDesignerFormWindowManagerInterface *QDesignerFormEditorInterface::formWindowManager() const
{
    return d->m_formWindowManager;
}

/*!
    \internal
*/
void QDesignerFormEditorInterface::setFormManager(QDesignerFormWindowManagerInterface *formWindowManager)
{
    d->m_formWindowManager = formWindowManager;
}

/*!
    Returns an interface to \QD's extension manager.
*/
QExtensionManager *QDesignerFormEditorInterface::extensionManager() const
{
    return d->m_extensionManager;
}

/*!
    \internal
*/
void QDesignerFormEditorInterface::setExtensionManager(QExtensionManager *extensionManager)
{
    d->m_extensionManager = extensionManager;
}

/*!
    \internal

    Returns an interface to the meta database used by the form editor.
*/
QDesignerMetaDataBaseInterface *QDesignerFormEditorInterface::metaDataBase() const
{
    return d->m_metaDataBase;
}

/*!
    \internal
*/
void QDesignerFormEditorInterface::setMetaDataBase(QDesignerMetaDataBaseInterface *metaDataBase)
{
    d->m_metaDataBase = metaDataBase;
}

/*!
    \internal

    Returns an interface to the widget database used by the form editor.
*/
QDesignerWidgetDataBaseInterface *QDesignerFormEditorInterface::widgetDataBase() const
{
    return d->m_widgetDataBase;
}

/*!
    \internal
*/
void QDesignerFormEditorInterface::setWidgetDataBase(QDesignerWidgetDataBaseInterface *widgetDataBase)
{
    d->m_widgetDataBase = widgetDataBase;
}

/*!
    \internal

    Returns an interface to the designer promotion handler.
*/

QDesignerPromotionInterface *QDesignerFormEditorInterface::promotion() const
{
    return d->m_promotion;
}

/*!
    \internal

    Sets the designer promotion handler.
*/

void QDesignerFormEditorInterface::setPromotion(QDesignerPromotionInterface *promotion)
{
    if (d->m_promotion)
        delete d->m_promotion;
    d->m_promotion = promotion;
}

/*!
    \internal

    Returns an interface to the widget factory used by the form editor
    to create widgets for the form.
*/
QDesignerWidgetFactoryInterface *QDesignerFormEditorInterface::widgetFactory() const
{
    return d->m_widgetFactory;
}

/*!
    \internal
*/
void QDesignerFormEditorInterface::setWidgetFactory(QDesignerWidgetFactoryInterface *widgetFactory)
{
    d->m_widgetFactory = widgetFactory;
}

/*!
    Returns an interface to \QD's object inspector.
*/
QDesignerObjectInspectorInterface *QDesignerFormEditorInterface::objectInspector() const
{
    return d->m_objectInspector;
}

/*!
    Sets \QD's object inspector to be the specified \a
    objectInspector.

    \sa objectInspector()
*/
void QDesignerFormEditorInterface::setObjectInspector(QDesignerObjectInspectorInterface *objectInspector)
{
    d->m_objectInspector = objectInspector;
}

/*!
    \internal

    Returns an interface to the brush manager used by the palette editor.
*/
QDesignerBrushManagerInterface *QDesignerFormEditorInterface::brushManager() const
{
    return d->m_brushManager;
}

/*!
    \internal
*/
void QDesignerFormEditorInterface::setBrushManager(QDesignerBrushManagerInterface *brushManager)
{
    d->m_brushManager = brushManager;
}

/*!
    \internal

    Returns an interface to the integration.
*/
QDesignerIntegrationInterface *QDesignerFormEditorInterface::integration() const
{
    return d->m_integration;
}

/*!
    \internal
*/
void QDesignerFormEditorInterface::setIntegration(QDesignerIntegrationInterface *integration)
{
    d->m_integration = integration;
}

/*!
    \internal

    Returns an interface to the icon cache used by the form editor to
    manage icons.
*/
QDesignerIconCacheInterface *QDesignerFormEditorInterface::iconCache() const
{
    return d->m_iconCache;
}

/*!
    \internal
*/
void QDesignerFormEditorInterface::setIconCache(QDesignerIconCacheInterface *cache)
{
    d->m_iconCache = cache;
}

/*!
    \internal

    Returns the plugin manager used by the form editor.
*/
QDesignerPluginManager *QDesignerFormEditorInterface::pluginManager() const
{
    return d->m_pluginManager;
}

/*!
    \internal

    Sets the plugin manager used by the form editor to the specified
    \a pluginManager.
*/
void QDesignerFormEditorInterface::setPluginManager(QDesignerPluginManager *pluginManager)
{
    d->m_pluginManager = pluginManager;
}

/*!
    \internal
    \since 4.4
    Returns the resource model used by the form editor.
*/
QtResourceModel *QDesignerFormEditorInterface::resourceModel() const
{
    return d->m_resourceModel;
}

/*!
    \internal

    Sets the resource model used by the form editor to the specified
    \a resourceModel.
*/
void QDesignerFormEditorInterface::setResourceModel(QtResourceModel *resourceModel)
{
    d->m_resourceModel = resourceModel;
}

/*!
    \internal
    \since 4.4
    Returns the gradient manager used by the style sheet editor.
*/
QtGradientManager *QDesignerFormEditorInterface::gradientManager() const
{
    return d->m_gradientManager;
}

/*!
    \internal

    Sets the gradient manager used by the style sheet editor to the specified
    \a gradientManager.
*/
void QDesignerFormEditorInterface::setGradientManager(QtGradientManager *gradientManager)
{
    d->m_gradientManager = gradientManager;
}

/*!
    \internal
    \since 4.4
    Returns the introspection used by the form editor.
*/
QDesignerIntrospectionInterface *QDesignerFormEditorInterface::introspection() const
{
    return d->m_introspection;
}

/*!
    \internal
    \since 4.4

    Sets the introspection used by the form editor to the specified \a introspection.
*/
void QDesignerFormEditorInterface::setIntrospection(QDesignerIntrospectionInterface *introspection)
{
    if (d->m_introspection)
        delete d->m_introspection;
     d->m_introspection = introspection;
}

/*!
    \internal

    Returns the path to the resources used by the form editor.
*/
QString QDesignerFormEditorInterface::resourceLocation() const
{
#ifdef Q_WS_MAC
    return QLatin1String(":/trolltech/formeditor/images/mac");
#else
    return QLatin1String(":/trolltech/formeditor/images/win");
#endif
}

/*!
    \internal

    Returns the dialog GUI used by the form editor.
*/

QDesignerDialogGuiInterface *QDesignerFormEditorInterface::dialogGui() const
{
    return d->m_dialogGui;
}

/*!
    \internal

    Sets the dialog GUI used by the form editor to the specified \a dialogGui.
*/

void QDesignerFormEditorInterface::setDialogGui(QDesignerDialogGuiInterface *dialogGui)
{
    delete  d->m_dialogGui;
    d->m_dialogGui = dialogGui;
}

QT_END_NAMESPACE
