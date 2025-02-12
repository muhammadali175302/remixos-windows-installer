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

#include "propertyeditor.h"

#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtvariantproperty.h"
#include "designerpropertymanager.h"
#include "qdesigner_propertysheet_p.h"
#include "formwindowbase_p.h"

#include "newdynamicpropertydialog.h"
#include "dynamicpropertysheet.h"
#include "shared_enums_p.h"

// sdk
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QDesignerWidgetDataBaseInterface>
// shared
#include <qdesigner_utils_p.h>
#include <qdesigner_propertycommand_p.h>
#include <metadatabase_p.h>
#include <iconloader_p.h>

#include <QtGui/QAction>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QApplication>
#include <QtGui/QVBoxLayout>
#include <QtGui/QScrollArea>
#include <QtGui/QStackedWidget>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QActionGroup>
#include <QtGui/QLabel>
#include <QtGui/QAbstractButton>

#include <QtCore/QSettings>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>

static const char *SettingsGroupC = "PropertyEditor";
#if QT_VERSION > 0x040400
static const char *ViewKeyC = "View";
#endif
static const char *ColorKeyC = "Colored";
static const char *SortedKeyC = "Sorted";
static const char *ExpansionKeyC = "ExpandedItems";

enum SettingsView { TreeView, ButtonView };

QT_BEGIN_NAMESPACE

// ---------------------------------------------------------------------------------

namespace qdesigner_internal {

// A pair <ValidationMode, bool hasComment>.
typedef QPair<TextPropertyValidationMode, bool> StringPropertyParameters;

// Return a pair of validation mode and flag indicating whether property has a comment
// for textual properties.

static StringPropertyParameters textPropertyValidationMode(const QObject *object,const QString &propertyName,
                                                           QVariant::Type type, bool isMainContainer)
{
    if (type == QVariant::ByteArray) {
        return StringPropertyParameters(ValidationMultiLine, false);
    }
    // object name - no comment
    if (propertyName == QLatin1String("objectName")) {
        const TextPropertyValidationMode vm =  isMainContainer ? ValidationObjectNameScope : ValidationObjectName;
        return StringPropertyParameters(vm, false);
    }

    // Accessibility. Both are texts the narrator reads
    if (propertyName == QLatin1String("accessibleDescription") || propertyName == QLatin1String("accessibleName"))
        return StringPropertyParameters(ValidationRichText, true);

    // Any names
    if (propertyName == QLatin1String("buddy") || propertyName.endsWith(QLatin1String("Name")))
        return StringPropertyParameters(ValidationObjectName, false);

    // Multi line?
    if (propertyName == QLatin1String("styleSheet"))
        return StringPropertyParameters(ValidationStyleSheet, false);

    if (propertyName == QLatin1String("description")) // QCommandLinkButton
        return StringPropertyParameters(ValidationMultiLine, true);

    if (propertyName == QLatin1String("toolTip")         || propertyName.endsWith(QLatin1String("ToolTip")) ||
        propertyName == QLatin1String("whatsThis")       || propertyName == QLatin1String("iconText") ||
        propertyName == QLatin1String("windowIconText")  || propertyName == QLatin1String("html"))
        return StringPropertyParameters(ValidationRichText, true);

    // text: Check according to widget type.
    if (propertyName == QLatin1String("text")) {
        if (qobject_cast<const QAction *>(object) || qobject_cast<const QLineEdit *>(object))
            return StringPropertyParameters(ValidationSingleLine, true);
        if (qobject_cast<const QAbstractButton *>(object))
            return StringPropertyParameters(ValidationMultiLine, true);
         return StringPropertyParameters(ValidationRichText, true);
    }

    if (propertyName == QLatin1String("plainText")) // QPlainTextEdit
        return StringPropertyParameters(ValidationMultiLine, true);

#ifdef Q_OS_WIN // No translation for the active X "control" property
    if (propertyName == QLatin1String("control") && !qstrcmp(object->metaObject()->className(), "QAxWidget"))
        return StringPropertyParameters(ValidationSingleLine, false);
#endif

    // default to single
    return StringPropertyParameters(ValidationSingleLine, true);
}

// ----------- PropertyEditor::Strings

PropertyEditor::Strings::Strings() :
    m_commentName(PropertyEditor::tr("comment")),
    m_fontProperty(QLatin1String("font")),
    m_qLayoutWidget(QLatin1String("QLayoutWidget")),
    m_designerPrefix(QLatin1String("QDesigner")),
    m_layout(QLatin1String("Layout")),
    m_validationModeAttribute(QLatin1String("validationMode")),
    m_fontAttribute(QLatin1String("font")),
    m_superPaletteAttribute(QLatin1String("superPalette")),
    m_enumNamesAttribute(QLatin1String("enumNames")),
    m_resettableAttribute(QLatin1String("resettable")),
    m_flagsAttribute(QLatin1String("flags"))
{
    m_alignmentProperties.insert(QLatin1String("alignment"));
    m_alignmentProperties.insert(QLatin1String("layoutLabelAlignment")); // QFormLayout
    m_alignmentProperties.insert(QLatin1String("layoutFormAlignment"));
}

// ----------- PropertyEditor

QDesignerMetaDataBaseItemInterface* PropertyEditor::metaDataBaseItem() const
{
    QObject *o = object();
    if (!o)
        return 0;
    QDesignerMetaDataBaseInterface *db = core()->metaDataBase();
    if (!db)
        return 0;
    return db->item(o);
}

void PropertyEditor::addCommentProperty(QtVariantProperty *property, const QString &propertyName)
{
    if (m_propertyToComment.contains(property))
        return;

    QtVariantProperty *commentProperty = m_propertyManager->addProperty(QVariant::String, m_strings.m_commentName);
    commentProperty->setValue(propertyComment(m_core, m_object, propertyName));
    property->addSubProperty(commentProperty);
    m_propertyToComment[property] = commentProperty;
    m_commentToProperty[commentProperty] = property;
}

void PropertyEditor::setupStringProperty(QtVariantProperty *property, const QString &propertyName,
                                         const QVariant &value, bool isMainContainer)
{
    const StringPropertyParameters params = textPropertyValidationMode(m_object, propertyName, value.type(), isMainContainer);
    // Does a meta DB entry exist - add comment
    const bool hasComment = params.second && metaDataBaseItem();
    const QString stringValue = value.type() == QVariant::ByteArray ? QString::fromUtf8(value.toByteArray()) : value.toString();
    property->setAttribute(m_strings.m_validationModeAttribute, params.first);
    // assuming comment cannot appear or disappear for the same property in different object instance
    if (hasComment)
        addCommentProperty(property, propertyName);
}

void PropertyEditor::setupPaletteProperty(QtVariantProperty *property)
{
    QPalette value = qvariant_cast<QPalette>(property->value());
    QPalette superPalette = QPalette();
    QWidget *currentWidget = qobject_cast<QWidget *>(m_object);
    if (currentWidget) {
        if (currentWidget->isWindow())
            superPalette = QApplication::palette(currentWidget);
        else {
            if (currentWidget->parentWidget())
                superPalette = currentWidget->parentWidget()->palette();
        }
    }
    m_updatingBrowser = true;
    property->setAttribute(m_strings.m_superPaletteAttribute, superPalette);
    m_updatingBrowser = false;
}

static inline QToolButton *createDropDownButton(QAction *defaultAction, QWidget *parent = 0)
{
    QToolButton *rc = new QToolButton(parent);
    rc->setDefaultAction(defaultAction);
    rc->setPopupMode(QToolButton::InstantPopup);
    return rc;
}

PropertyEditor::PropertyEditor(QDesignerFormEditorInterface *core, QWidget *parent, Qt::WindowFlags flags)  :
    QDesignerPropertyEditor(parent, flags),
    m_core(core),
    m_propertySheet(0),
    m_currentBrowser(0),
    m_treeBrowser(0),
    m_propertyManager(new DesignerPropertyManager(m_core, this)),
    m_dynamicGroup(0),
    m_updatingBrowser(false),
    m_stackedWidget(new QStackedWidget),
    m_buttonIndex(-1),
    m_treeIndex(-1),
    m_addDynamicAction(new QAction(createIconSet(QLatin1String("plus.png")), tr("Add Dynamic Property..."), this)),
    m_removeDynamicAction(new QAction(createIconSet(QLatin1String("minus.png")), tr("Remove Dynamic Property"), this)),
    m_sortingAction(new QAction(createIconSet(QLatin1String("sort.png")), tr("Sorting"), this)),
    m_coloringAction(new QAction(createIconSet(QLatin1String("color.png")), tr("Color Groups"), this)),
    m_treeAction(new QAction(tr("Tree View"), this)),
    m_buttonAction(new QAction(tr("Drop Down Button View"), this)),
    m_classLabel(new QLabel),
    m_sorting(false),
    m_coloring(false),
    m_dynamicColor(QColor(191, 207, 255)),
    m_layoutColor(QColor(255, 191, 191))
{
    m_colors.reserve(6);
    m_colors.push_back(QColor(255, 230, 191));
    m_colors.push_back(QColor(255, 255, 191));
    m_colors.push_back(QColor(191, 255, 191));
    m_colors.push_back(QColor(199, 255, 255));
    m_colors.push_back(QColor(234, 191, 255));
    m_colors.push_back(QColor(255, 191, 239));

    QToolBar *toolBar = new QToolBar;

    QActionGroup *actionGroup = new QActionGroup(this);

    m_treeAction->setCheckable(true);
    m_treeAction->setIcon(createIconSet(QLatin1String("widgets/listview.png")));
    m_buttonAction->setCheckable(true);
    m_buttonAction->setIcon(createIconSet(QLatin1String("dropdownbutton.png")));

    actionGroup->addAction(m_treeAction);
    actionGroup->addAction(m_buttonAction);
    connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotViewTriggered(QAction*)));

    QWidget *classWidget = new QWidget;
    classWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    QHBoxLayout *l = new QHBoxLayout(classWidget);
    l->setMargin(0);
    l->addWidget(m_classLabel);
    m_classLabel->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed));

    // Add actions
    QActionGroup *addDynamicActionGroup = new QActionGroup(this);
    connect(addDynamicActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotAddDynamicProperty(QAction*)));

    QMenu *addDynamicActionMenu = new QMenu(this);
    m_addDynamicAction->setMenu(addDynamicActionMenu);
    m_addDynamicAction->setEnabled(false);
    QAction *addDynamicAction = addDynamicActionGroup->addAction(tr("String..."));
    addDynamicAction->setData(static_cast<int>(QVariant::String));
    addDynamicActionMenu->addAction(addDynamicAction);
    addDynamicAction = addDynamicActionGroup->addAction(tr("Bool..."));
    addDynamicAction->setData(static_cast<int>(QVariant::Bool));
    addDynamicActionMenu->addAction(addDynamicAction);
    addDynamicActionMenu->addSeparator();
    addDynamicAction = addDynamicActionGroup->addAction(tr("Other..."));
    addDynamicAction->setData(static_cast<int>(QVariant::Invalid));
    addDynamicActionMenu->addAction(addDynamicAction);
    // remove
    m_removeDynamicAction->setEnabled(false);
    connect(m_removeDynamicAction, SIGNAL(triggered()), this, SLOT(slotRemoveDynamicProperty()));
    // Configure
    QAction *configureAction = new QAction(tr("Configure Property Editor"), this);
    configureAction->setIcon(createIconSet(QLatin1String("configure.png")));
    QMenu *configureMenu = new QMenu(this);
    configureAction->setMenu(configureMenu);

    m_sortingAction->setCheckable(true);
    connect(m_sortingAction, SIGNAL(toggled(bool)), this, SLOT(slotSorting(bool)));

    m_coloringAction->setCheckable(true);
    connect(m_coloringAction, SIGNAL(toggled(bool)), this, SLOT(slotColoring(bool)));

    configureMenu->addAction(m_sortingAction);
    configureMenu->addAction(m_coloringAction);
#if QT_VERSION > 0x040400
    configureMenu->addSeparator();
    configureMenu->addAction(m_treeAction);
    configureMenu->addAction(m_buttonAction);
#endif
    // Assemble toolbar
    toolBar->addWidget(classWidget);
    toolBar->addWidget(createDropDownButton(m_addDynamicAction));
    toolBar->addAction(m_removeDynamicAction);
    toolBar->addSeparator();
    toolBar->addWidget(createDropDownButton(configureAction));
    // Views
    QScrollArea *buttonScroll = new QScrollArea(m_stackedWidget);
    m_buttonBrowser = new QtButtonPropertyBrowser(buttonScroll);
    buttonScroll->setWidgetResizable(true);
    buttonScroll->setWidget(m_buttonBrowser);
    m_buttonIndex = m_stackedWidget->addWidget(buttonScroll);
    connect(m_buttonBrowser, SIGNAL(currentItemChanged(QtBrowserItem*)), this, SLOT(slotCurrentItemChanged(QtBrowserItem*)));

    m_treeBrowser = new QtTreePropertyBrowser(m_stackedWidget);
    m_treeBrowser->setRootIsDecorated(false);
    m_treeBrowser->setMarkPropertiesWithoutValue(true);
    m_treeBrowser->setResizeMode(QtTreePropertyBrowser::Interactive);
    m_treeIndex = m_stackedWidget->addWidget(m_treeBrowser);
    connect(m_treeBrowser, SIGNAL(currentItemChanged(QtBrowserItem*)), this, SLOT(slotCurrentItemChanged(QtBrowserItem*)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(toolBar);
    layout->addWidget(m_stackedWidget);
    layout->setMargin(0);
    layout->setSpacing(0);

    m_treeFactory = new DesignerEditorFactory(m_core, this);
    m_treeFactory->setSpacing(0);
    m_groupFactory = new DesignerEditorFactory(m_core, this);
    QtVariantPropertyManager *variantManager = m_propertyManager;
    m_buttonBrowser->setFactoryForManager(variantManager, m_groupFactory);
    m_treeBrowser->setFactoryForManager(variantManager, m_treeFactory);

    m_stackedWidget->setCurrentIndex(m_treeIndex);
    m_currentBrowser = m_treeBrowser;
    m_treeAction->setChecked(true);

    connect(m_groupFactory, SIGNAL(resetProperty(QtProperty*)), this, SLOT(slotResetProperty(QtProperty*)));
    connect(m_treeFactory, SIGNAL(resetProperty(QtProperty*)), this, SLOT(slotResetProperty(QtProperty*)));
    connect(variantManager, SIGNAL(valueChanged(QtProperty*,QVariant,bool)), this, SLOT(slotValueChanged(QtProperty*,QVariant,bool)));

    // retrieve initial settings
    QSettings settings;
    settings.beginGroup(QLatin1String(SettingsGroupC));
#if QT_VERSION > 0x040400
    const SettingsView view = settings.value(QLatin1String(ViewKeyC), TreeView).toInt() == TreeView ? TreeView :  ButtonView;
#endif
    // Coloring not available unless treeview and not sorted
    m_sorting = settings.value(QLatin1String(SortedKeyC), false).toBool();
    m_coloring = settings.value(QLatin1String(ColorKeyC), true).toBool();
    const QVariantMap expansionState = settings.value(QLatin1String(ExpansionKeyC), QVariantMap()).toMap();
    settings.endGroup();
    // Apply settings
    m_sortingAction->setChecked(m_sorting);
    m_coloringAction->setChecked(m_coloring);
#if QT_VERSION > 0x040400
    switch (view) {
    case TreeView:
        m_currentBrowser = m_treeBrowser;
        m_stackedWidget->setCurrentIndex(m_treeIndex);
        m_treeAction->setChecked(true);
        break;
    case ButtonView:
        m_currentBrowser = m_buttonBrowser;
        m_stackedWidget->setCurrentIndex(m_buttonIndex);
        m_buttonAction->setChecked(true);
        break;
    }
#endif
    // Restore expansionState from QVariant map
    if (!expansionState.empty()) {
        const QVariantMap::const_iterator cend = expansionState.constEnd();
        for (QVariantMap::const_iterator it = expansionState.constBegin(); it != cend; ++it)
            m_expansionState.insert(it.key(), it.value().toBool());
    }
    updateActionsState();
}

PropertyEditor::~PropertyEditor()
{
    storeExpansionState();
    saveSettings();
}

void PropertyEditor::saveSettings() const
{
    QSettings settings;
    settings.beginGroup(QLatin1String(SettingsGroupC));
#if QT_VERSION > 0x040400
    settings.setValue(QLatin1String(ViewKeyC), QVariant(m_treeAction->isChecked() ? TreeView : ButtonView));
#endif
    settings.setValue(QLatin1String(ColorKeyC), QVariant(m_coloring));
    settings.setValue(QLatin1String(SortedKeyC), QVariant(m_sorting));
    // Save last expansionState as QVariant map
    QVariantMap expansionState;
    if (!m_expansionState.empty()) {
        const QMap<QString, bool>::const_iterator cend = m_expansionState.constEnd();
        for (QMap<QString, bool>::const_iterator it = m_expansionState.constBegin(); it != cend; ++it)
            expansionState.insert(it.key(), QVariant(it.value()));
    }
    settings.setValue(QLatin1String(ExpansionKeyC), expansionState);
    settings.endGroup();
}

void PropertyEditor::setExpanded(QtBrowserItem *item, bool expanded)
{
    if (m_buttonBrowser == m_currentBrowser)
        m_buttonBrowser->setExpanded(item, expanded);
    else if (m_treeBrowser == m_currentBrowser)
        m_treeBrowser->setExpanded(item, expanded);
}

bool PropertyEditor::isExpanded(QtBrowserItem *item)
{
    if (m_buttonBrowser == m_currentBrowser)
        return m_buttonBrowser->isExpanded(item);
    else if (m_treeBrowser == m_currentBrowser)
        return m_treeBrowser->isExpanded(item);
    return false;
}

/* Default handling of items not found in the map:
 * - Top-level items (classes) are assumed to be expanded
 * - Anything below (properties) is assumed to be collapsed
 * That is, the map is required, the state cannot be stored in a set */

void PropertyEditor::storePropertiesExpansionState(const QList<QtBrowserItem *> &items)
{
    const QChar bar = QLatin1Char('|');
    QListIterator<QtBrowserItem *> itProperty(items);
    while (itProperty.hasNext()) {
        QtBrowserItem *propertyItem = itProperty.next();
        if (!propertyItem->children().empty()) {
            QtProperty *property = propertyItem->property();
            const QString propertyName = property->propertyName();
            const QMap<QtProperty *, QString>::const_iterator itGroup = m_propertyToGroup.constFind(property);
            if (itGroup != m_propertyToGroup.constEnd()) {
                QString key = itGroup.value();
                key += bar;
                key += propertyName;
                m_expansionState[key] = isExpanded(propertyItem);
            }
        }
    }
}

void PropertyEditor::storeExpansionState()
{
    const QList<QtBrowserItem *> items = m_currentBrowser->topLevelItems();
    if (m_sorting) {
        storePropertiesExpansionState(items);
    } else {
        QListIterator<QtBrowserItem *> itGroup(items);
        while (itGroup.hasNext()) {
            QtBrowserItem *item = itGroup.next();
            const QString groupName = item->property()->propertyName();
            QList<QtBrowserItem *> propertyItems = item->children();
            if (!propertyItems.empty())
                m_expansionState[groupName] = isExpanded(item);

            // properties stuff here
            storePropertiesExpansionState(propertyItems);
        }
    }
}

void PropertyEditor::collapseAll()
{
    QList<QtBrowserItem *> items = m_currentBrowser->topLevelItems();
    QListIterator<QtBrowserItem *> itGroup(items);
    while (itGroup.hasNext())
        setExpanded(itGroup.next(), false);
}

void PropertyEditor::applyPropertiesExpansionState(const QList<QtBrowserItem *> &items)
{
    const QChar bar = QLatin1Char('|');
    QListIterator<QtBrowserItem *> itProperty(items);
    while (itProperty.hasNext()) {
        const QMap<QString, bool>::const_iterator excend = m_expansionState.constEnd();
        QtBrowserItem *propertyItem = itProperty.next();
        QtProperty *property = propertyItem->property();
        const QString propertyName = property->propertyName();
        const QMap<QtProperty *, QString>::const_iterator itGroup = m_propertyToGroup.constFind(property);
        if (itGroup != m_propertyToGroup.constEnd()) {
            QString key = itGroup.value();
            key += bar;
            key += propertyName;
            const QMap<QString, bool>::const_iterator pit = m_expansionState.constFind(key);
            if (pit != excend)
                setExpanded(propertyItem, pit.value());
            else
                setExpanded(propertyItem, false);
        }
    }
}

void PropertyEditor::applyExpansionState()
{
    const QList<QtBrowserItem *> items = m_currentBrowser->topLevelItems();
    if (m_sorting) {
        applyPropertiesExpansionState(items);
    } else {
        QListIterator<QtBrowserItem *> itTopLevel(items);
        const QMap<QString, bool>::const_iterator excend = m_expansionState.constEnd();
        while (itTopLevel.hasNext()) {
            QtBrowserItem *item = itTopLevel.next();
            const QString groupName = item->property()->propertyName();
            const QMap<QString, bool>::const_iterator git = m_expansionState.constFind(groupName);
            if (git != excend)
                setExpanded(item, git.value());
            else
                setExpanded(item, true);
            // properties stuff here
            applyPropertiesExpansionState(item->children());
        }
    }
}

void PropertyEditor::clearView()
{
    m_currentBrowser->clear();
}

QColor PropertyEditor::propertyColor(QtProperty *property) const
{
    if (!m_coloring)
        return QColor();

    QtProperty *groupProperty = property;

    QMap<QtProperty *, QString>::ConstIterator itProp = m_propertyToGroup.constFind(property);
    if (itProp != m_propertyToGroup.constEnd())
        groupProperty = m_nameToGroup.value(itProp.value());

    const int groupIdx = m_groups.indexOf(groupProperty);
    if (groupIdx != -1) {
        if (groupProperty == m_dynamicGroup)
            return m_dynamicColor;
        if (isLayoutGroup(groupProperty))
            return m_layoutColor;
        return m_colors[groupIdx % m_colors.count()];
    }
    return QColor();
}

void PropertyEditor::fillView()
{
    if (m_sorting) {
        QMapIterator<QString, QtVariantProperty *> itProperty(m_nameToProperty);
        while (itProperty.hasNext()) {
            QtVariantProperty *property = itProperty.next().value();
            m_currentBrowser->addProperty(property);
        }
    } else {
        QListIterator<QtProperty *> itGroup(m_groups);
        while (itGroup.hasNext()) {
            QtProperty *group = itGroup.next();
            QtBrowserItem *item = m_currentBrowser->addProperty(group);
            if (m_currentBrowser == m_treeBrowser)
                m_treeBrowser->setBackgroundColor(item, propertyColor(group));
            group->setModified(m_currentBrowser == m_treeBrowser);
        }
    }
}

bool PropertyEditor::isLayoutGroup(QtProperty *group) const
{
   return group->propertyName() == m_strings.m_layout;
}

void PropertyEditor::updateActionsState()
{
    m_coloringAction->setEnabled(m_treeAction->isChecked() && !m_sortingAction->isChecked());
}

void PropertyEditor::slotViewTriggered(QAction *action)
{
    storeExpansionState();
    collapseAll();
    const bool wasEnabled = updatesEnabled();
    setUpdatesEnabled(false);
    clearView();
    int idx = 0;
    if (action == m_treeAction) {
        m_currentBrowser = m_treeBrowser;
        idx = m_treeIndex;
    } else if (action == m_buttonAction) {
        m_currentBrowser = m_buttonBrowser;
        idx = m_buttonIndex;
    }
    fillView();
    m_stackedWidget->setCurrentIndex(idx);
    applyExpansionState();
    setUpdatesEnabled(wasEnabled);
    updateActionsState();
}

void PropertyEditor::slotSorting(bool sort)
{
    if (sort == m_sorting)
        return;

    storeExpansionState();
    m_sorting = sort;
    collapseAll();
    const bool wasEnabled = updatesEnabled();
    setUpdatesEnabled(false);
    clearView();
    m_treeBrowser->setRootIsDecorated(sort);
    fillView();
    applyExpansionState();
    setUpdatesEnabled(wasEnabled);
    updateActionsState();
}

void PropertyEditor::slotColoring(bool coloring)
{
    if (coloring == m_coloring)
        return;

    m_coloring = coloring;
    if (m_currentBrowser == m_treeBrowser) {
        QList<QtBrowserItem *> items = m_treeBrowser->topLevelItems();
        QListIterator<QtBrowserItem *> itItem(items);
        while (itItem.hasNext()) {
            QtBrowserItem *item = itItem.next();
            m_treeBrowser->setBackgroundColor(item, propertyColor(item->property()));
        }
    }
}

void PropertyEditor::slotAddDynamicProperty(QAction *action)
{
    if (!m_propertySheet)
        return;

    const QDesignerDynamicPropertySheetExtension *dynamicSheet =
            qt_extension<QDesignerDynamicPropertySheetExtension*>(m_core->extensionManager(), m_object);

    if (!dynamicSheet)
        return;

    QString newName;
    QVariant newValue;
    { // Make sure the dialog is closed before the signal is emitted.
        const QVariant::Type type = static_cast<QVariant::Type>(action->data().toInt());
        NewDynamicPropertyDialog dlg(core()->dialogGui(), m_currentBrowser);
        if (type != QVariant::Invalid)
            dlg.setPropertyType(type);

        QStringList reservedNames;
        const int propertyCount = m_propertySheet->count();
        for (int i = 0; i < propertyCount; i++) {
            if (!dynamicSheet->isDynamicProperty(i) || m_propertySheet->isVisible(i))
                reservedNames.append(m_propertySheet->propertyName(i));
        }
        dlg.setReservedNames(reservedNames);
        if (dlg.exec() == QDialog::Rejected)
            return;
        newName = dlg.propertyName();
        newValue = dlg.propertyValue();
    }
    m_recentlyAddedDynamicProperty = newName;
    emit addDynamicProperty(newName, newValue);
}

QDesignerFormEditorInterface *PropertyEditor::core() const
{
    return m_core;
}

bool PropertyEditor::isReadOnly() const
{
    return false;
}

void PropertyEditor::setReadOnly(bool /*readOnly*/)
{
    qDebug() << "PropertyEditor::setReadOnly() request";
}

void PropertyEditor::setPropertyValue(const QString &name, const QVariant &value, bool changed)
{
    const QMap<QString, QtVariantProperty*>::const_iterator it = m_nameToProperty.constFind(name);
    if (it == m_nameToProperty.constEnd())
        return;
    QtVariantProperty *property = it.value();
    updateBrowserValue(property, value);
    property->setModified(changed);
}

void PropertyEditor::setPropertyComment(const QString &name, const QString &value)
{
    const QMap<QString, QtVariantProperty*>::const_iterator it = m_nameToProperty.constFind(name);
    if (it == m_nameToProperty.constEnd())
        return;

    QtVariantProperty *property = it.value();
    const QMap<QtVariantProperty *, QtVariantProperty *>::const_iterator cit = m_propertyToComment.constFind(property);
    if (cit == m_propertyToComment.constEnd())
        return;
    QtVariantProperty *commentProperty = cit.value();
    updateBrowserValue(commentProperty, value);
}

/* Quick update that assumes the actual count of properties has not changed
 * N/A when for example executing a layout command and margin properties appear. */
void PropertyEditor::updatePropertySheet()
{
    if (!m_propertySheet)
        return;

    updateToolBarLabel();

    const int propertyCount = m_propertySheet->count();
    const  QMap<QString, QtVariantProperty*>::const_iterator npcend = m_nameToProperty.constEnd();
    for (int i = 0; i < propertyCount; ++i) {
        const QString propertyName = m_propertySheet->propertyName(i);
        QMap<QString, QtVariantProperty*>::const_iterator it = m_nameToProperty.constFind(propertyName);
        if (it != npcend)
            updateBrowserValue(it.value(), m_propertySheet->property(i));
    }
}

static inline QLayout *layoutOfQLayoutWidget(QObject *o)
{
    if (o->isWidgetType() && !qstrcmp(o->metaObject()->className(), "QLayoutWidget"))
        return static_cast<QWidget*>(o)->layout();
    return 0;
}

void PropertyEditor::updateToolBarLabel()
{
    QString objectName;
    QString className;
    if (m_object) {
        if (QLayout *l = layoutOfQLayoutWidget(m_object))
            objectName = l->objectName();
        else
            objectName = m_object->objectName();
        className = realClassName(m_object);
    }

    m_classLabel->setText(tr("%1\n%2").arg(objectName).arg(className));
    m_classLabel->setToolTip(tr("Object: %1\nClass: %2").arg(objectName).arg(className));

}

void PropertyEditor::updateBrowserValue(QtVariantProperty *property, const QVariant &value)
{
    QVariant v = value;
    const int type = property->propertyType();
    if (type == QtVariantPropertyManager::enumTypeId()) {
        const PropertySheetEnumValue e = qvariant_cast<PropertySheetEnumValue>(v);
        v = e.metaEnum.keys().indexOf(e.metaEnum.valueToKey(e.value));
    } else if (type == DesignerPropertyManager::designerFlagTypeId()) {
        const PropertySheetFlagValue f = qvariant_cast<PropertySheetFlagValue>(v);
        v = QVariant(f.value);
    } else if (type == DesignerPropertyManager::designerAlignmentTypeId()) {
        const PropertySheetFlagValue f = qvariant_cast<PropertySheetFlagValue>(v);
        v = QVariant(f.value);
    }
    QDesignerPropertySheet *sheet = qobject_cast<QDesignerPropertySheet*>(m_core->extensionManager()->extension(m_object, Q_TYPEID(QDesignerPropertySheetExtension)));
    int index = -1;
    if (sheet)
        index = sheet->indexOf(property->propertyName());
    if (sheet && m_propertyToGroup.contains(property)) { // don't do it for comments since property sheet doesn't keep them
        property->setEnabled(sheet->isEnabled(index));
    }

    // Rich text string property with comment: Store/Update the font the rich text editor dialog starts out with
    if (type == QVariant::String && !property->subProperties().empty()) {
        const int fontIndex = m_propertySheet->indexOf(m_strings.m_fontProperty);
        if (fontIndex != -1)
            property->setAttribute(m_strings.m_fontAttribute, m_propertySheet->property(fontIndex));
    }

    m_updatingBrowser = true;
    property->setValue(v);
    if (sheet && sheet->isResourceProperty(index))
        property->setAttribute(QLatin1String("defaultResource"), sheet->defaultResourceProperty(index));
    m_updatingBrowser = false;
}

int PropertyEditor::toBrowserType(const QVariant &value, const QString &propertyName) const
{
    if (qVariantCanConvert<PropertySheetFlagValue>(value)) {
        if (m_strings.m_alignmentProperties.contains(propertyName))
            return DesignerPropertyManager::designerAlignmentTypeId();
        return DesignerPropertyManager::designerFlagTypeId();
    }
    if (qVariantCanConvert<PropertySheetEnumValue>(value))
        return DesignerPropertyManager::enumTypeId();

    if (value.type() == QVariant::ByteArray)
        return QVariant::String;
    return value.userType();
}

QString PropertyEditor::realClassName(QObject *object) const
{
    if (!object)
        return 0;

    QString className = QLatin1String(object->metaObject()->className());
    const QDesignerWidgetDataBaseInterface *db = core()->widgetDataBase();
    if (QDesignerWidgetDataBaseItemInterface *widgetItem = db->item(db->indexOfObject(object, true))) {
        className = widgetItem->name();

        if (object->isWidgetType() && className == m_strings.m_qLayoutWidget
                && static_cast<QWidget*>(object)->layout()) {
            className = QLatin1String(static_cast<QWidget*>(object)->layout()->metaObject()->className());
        }
    }

    if (className.startsWith(m_strings.m_designerPrefix))
        className.remove(1, m_strings.m_designerPrefix.size() - 1);

    return className;
}

static QString msgUnsupportedType(const QString &propertyName, unsigned type)
{
    QString rc;
    QTextStream str(&rc);
    str << "The property \"" << propertyName << "\" of type " << type;
    if (type == QVariant::Invalid) {
        str << " (invalid) ";
    } else {
        if (type < QVariant::UserType) {
            if (const char *typeName = QVariant::typeToName(static_cast<QVariant::Type>(type)))
                str << " (" << typeName << ") ";
        } else {
            str << " (user type) ";
        }
    }
    str << " is not supported yet!";
    return rc;
}

void PropertyEditor::setObject(QObject *object)
{
    QDesignerFormWindowInterface *oldFormWindow = QDesignerFormWindowInterface::findFormWindow(m_object);
    // In the first setObject() call following the addition of a dynamic property, focus and edit it.
    const bool editNewDynamicProperty = object != 0 && m_object == object && !m_recentlyAddedDynamicProperty.isEmpty();
    m_object = object;
    m_propertyManager->setObject(object);
    QDesignerFormWindowInterface *formWindow = QDesignerFormWindowInterface::findFormWindow(m_object);
    FormWindowBase *fwb = qobject_cast<FormWindowBase *>(formWindow);
    m_treeFactory->setFormWindowBase(fwb);
    m_groupFactory->setFormWindowBase(fwb);

    storeExpansionState();

    const bool wasEnabled = updatesEnabled();
    setUpdatesEnabled(false);

    updateToolBarLabel();

    QMap<QString, QtVariantProperty *> toRemove = m_nameToProperty;

    const QDesignerDynamicPropertySheetExtension *dynamicSheet =
            qt_extension<QDesignerDynamicPropertySheetExtension*>(m_core->extensionManager(), m_object);
    const QDesignerPropertySheet *sheet = qobject_cast<QDesignerPropertySheet*>(m_core->extensionManager()->extension(m_object, Q_TYPEID(QDesignerPropertySheetExtension)));

    // Optimizization: Instead of rebuilding the complete list every time, compile a list of properties to remove,
    // remove them, traverse the sheet, in case property exists just set a value, otherwise - create it.
    QExtensionManager *m = m_core->extensionManager();

    m_propertySheet = qobject_cast<QDesignerPropertySheetExtension*>(m->extension(object, Q_TYPEID(QDesignerPropertySheetExtension)));
    if (m_propertySheet) {
        const int propertyCount = m_propertySheet->count();
        for (int i = 0; i < propertyCount; ++i) {
            if (!m_propertySheet->isVisible(i))
                continue;

            const QString propertyName = m_propertySheet->propertyName(i);
            if (m_propertySheet->indexOf(propertyName) != i)
                continue;
            const QString groupName = m_propertySheet->propertyGroup(i);
            const QMap<QString, QtVariantProperty *>::const_iterator rit = toRemove.constFind(propertyName);
            if (rit != toRemove.constEnd()) {
                QtVariantProperty *property = rit.value();
                if (m_propertyToGroup.value(property) == groupName && toBrowserType(m_propertySheet->property(i), propertyName) == property->propertyType())
                    toRemove.remove(propertyName);
            }
        }
    }

    QMapIterator<QString, QtVariantProperty *> itRemove(toRemove);
    while (itRemove.hasNext()) {
        itRemove.next();

        QtVariantProperty *property = itRemove.value();
        delete property;
        if (m_propertyToComment.contains(property)) {
            QtVariantProperty *commentProperty = m_propertyToComment.value(property);
            delete commentProperty;
            m_commentToProperty.remove(commentProperty);
            m_propertyToComment.remove(property);
        }
        m_nameToProperty.remove(itRemove.key());
        m_propertyToGroup.remove(property);
    }

    if (oldFormWindow != formWindow)
        reloadResourceProperties();

    bool isMainContainer = false;
    if (QWidget *widget = qobject_cast<QWidget*>(object)) {
        if (QDesignerFormWindowInterface *fw = QDesignerFormWindowInterface::findFormWindow(widget)) {
            isMainContainer = (fw->mainContainer() == widget);
        }
    }
    m_groups.clear();

    if (m_propertySheet) {
        QtProperty *lastProperty = 0;
        QtProperty *lastGroup = 0;
        const int propertyCount = m_propertySheet->count();
        for (int i = 0; i < propertyCount; ++i) {
            if (!m_propertySheet->isVisible(i))
                continue;

            const QString propertyName = m_propertySheet->propertyName(i);
            if (m_propertySheet->indexOf(propertyName) != i)
                continue;
            const QVariant value = m_propertySheet->property(i);

            const int type = toBrowserType(value, propertyName);

            QtVariantProperty *property = m_nameToProperty.value(propertyName, 0);
            bool newProperty = property == 0;
            if (newProperty) {
                property = m_propertyManager->addProperty(type, propertyName);
                if (property) {
                    newProperty = true;
                    if (type == DesignerPropertyManager::enumTypeId()) {
                        const PropertySheetEnumValue e = qvariant_cast<PropertySheetEnumValue>(value);
                        QStringList names;
                        QStringListIterator it(e.metaEnum.keys());
                        while (it.hasNext())
                            names.append(it.next());
                        m_updatingBrowser = true;
                        property->setAttribute(m_strings.m_enumNamesAttribute, names);
                        m_updatingBrowser = false;
                    } else if (type == DesignerPropertyManager::designerFlagTypeId()) {
                        const PropertySheetFlagValue f = qvariant_cast<PropertySheetFlagValue>(value);
                        QList<QPair<QString, uint> > flags;
                        QStringListIterator it(f.metaFlags.keys());
                        while (it.hasNext()) {
                            const QString name = it.next();
                            const uint val = f.metaFlags.keyToValue(name);
                            flags.append(qMakePair(name, val));
                        }
                        m_updatingBrowser = true;
                        QVariant v;
                        qVariantSetValue(v, flags);
                        property->setAttribute(m_strings.m_flagsAttribute, v);
                        m_updatingBrowser = false;
                    }
                }
            }

            if (property != 0) {
                const bool dynamicProperty = (dynamicSheet && dynamicSheet->isDynamicProperty(i))
                            || (sheet && sheet->isDefaultDynamicProperty(i));
                switch (type) {
                case QVariant::String:
                    setupStringProperty(property, propertyName, value, isMainContainer);
                    break;
                case QVariant::Palette:
                    setupPaletteProperty(property);
                    break;
                case QVariant::KeySequence:
                    addCommentProperty(property, propertyName);
                    break;
                default:
                    break;
                }
                property->setAttribute(m_strings.m_resettableAttribute, m_propertySheet->hasReset(i));

                const QString groupName = m_propertySheet->propertyGroup(i);
                QtVariantProperty *groupProperty = 0;

                if (newProperty) {
                    QMap<QString, QtVariantProperty*>::const_iterator itPrev = m_nameToProperty.insert(propertyName, property);
                    m_propertyToGroup[property] = groupName;
                    if (m_sorting) {
                        QtProperty *previous = 0;
                        if (itPrev != m_nameToProperty.constBegin())
                            previous = (--itPrev).value();
                        m_currentBrowser->insertProperty(property, previous);
                    }
                }
                const QMap<QString, QtVariantProperty*>::const_iterator gnit = m_nameToGroup.constFind(groupName);
                if (gnit != m_nameToGroup.constEnd()) {
                    groupProperty = gnit.value();
                } else {
                    groupProperty = m_propertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), groupName);
                    QtBrowserItem *item = 0;
                    if (!m_sorting)
                         item = m_currentBrowser->insertProperty(groupProperty, lastGroup);
                    m_nameToGroup[groupName] = groupProperty;
                    m_groups.append(groupProperty);
                    if (dynamicProperty)
                        m_dynamicGroup = groupProperty;
                    if (m_currentBrowser == m_treeBrowser && item) {
                        m_treeBrowser->setBackgroundColor(item, propertyColor(groupProperty));
                        groupProperty->setModified(true);
                    }
                }

                if (lastGroup != groupProperty) {
                    lastProperty = 0;
                    lastGroup = groupProperty;
                }
                if (!m_groups.contains(groupProperty))
                    m_groups.append(groupProperty);
                if (newProperty)
                    groupProperty->insertSubProperty(property, lastProperty);

                lastProperty = property;

                updateBrowserValue(property, value);
                const QMap<QtVariantProperty *, QtVariantProperty *>::const_iterator cit = m_propertyToComment.constFind(property);
                if (cit != m_propertyToComment.constEnd()) {
                    updateBrowserValue(cit.value(), propertyComment(m_core, m_object, propertyName));
                }
                property->setModified(m_propertySheet->isChanged(i));
                if (propertyName == QLatin1String("geometry") && type == QVariant::Rect) {
                    QList<QtProperty *> subProperties = property->subProperties();
                    foreach (QtProperty *subProperty, subProperties) {
                        const QString subPropertyName = subProperty->propertyName();
                        if (subPropertyName == QLatin1String("X") || subPropertyName == QLatin1String("Y"))
                            subProperty->setEnabled(!isMainContainer);
                    }
                }
            } else {
                qWarning(msgUnsupportedType(propertyName, type).toUtf8());
            }
        }
    }
    QMap<QString, QtVariantProperty *> groups = m_nameToGroup;
    QMapIterator<QString, QtVariantProperty *> itGroup(groups);
    while (itGroup.hasNext()) {
        QtVariantProperty *groupProperty = itGroup.next().value();
        if (groupProperty->subProperties().empty()) {
            if (groupProperty == m_dynamicGroup)
                m_dynamicGroup = 0;
            delete groupProperty;
            m_nameToGroup.remove(itGroup.key());
        }
    }
    const bool addEnabled = dynamicSheet ? dynamicSheet->dynamicPropertiesAllowed() : false;
    m_addDynamicAction->setEnabled(addEnabled);
    m_removeDynamicAction->setEnabled(false);
    applyExpansionState();
    setUpdatesEnabled(wasEnabled);
    // In the first setObject() call following the addition of a dynamic property, focus and edit it.
    if (editNewDynamicProperty) {
        // Have QApplication process the events related to completely closing the modal 'add' dialog,
        // otherwise, we cannot focus the property editor in docked mode.
        QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        editProperty(m_recentlyAddedDynamicProperty);
    }
    m_recentlyAddedDynamicProperty.clear();
}

void PropertyEditor::reloadResourceProperties()
{
    m_updatingBrowser = true;
    m_propertyManager->reloadResourceProperties();
    m_updatingBrowser = false;
}

QtBrowserItem *PropertyEditor::nonFakePropertyBrowserItem(QtBrowserItem *item) const
{
    // Top-level properties are QObject/QWidget groups, etc. Find first item property below
    // which should be nonfake
    const QList<QtBrowserItem *> topLevelItems = m_currentBrowser->topLevelItems();
    do {
        if (topLevelItems.contains(item->parent()))
            return item;
        item = item->parent();
    } while (item);
    return 0;
}

QString PropertyEditor::currentPropertyName() const
{
    if (QtBrowserItem *browserItem = m_currentBrowser->currentItem())
        if (QtBrowserItem *topLevelItem = nonFakePropertyBrowserItem(browserItem)) {
            return topLevelItem->property()->propertyName();
        }
    return QString();
}

void PropertyEditor::slotResetProperty(QtProperty *property)
{
    QDesignerFormWindowInterface *form = m_core->formWindowManager()->activeFormWindow();
    if (!form)
        return;

    if (m_propertyManager->resetFontSubProperty(property))
        return;

    if (m_propertyManager->resetIconSubProperty(property))
        return;

    if (!m_propertyToGroup.contains(property))
        return;

    emit resetProperty(property->propertyName());
}

void PropertyEditor::slotValueChanged(QtProperty *property, const QVariant &value, bool enableSubPropertyHandling)
{
    if (m_updatingBrowser)
        return;

    if (!m_propertySheet)
        return;

    QtVariantProperty *varProp = m_propertyManager->variantProperty(property);

    if (!varProp)
        return;

    if (m_commentToProperty.contains(varProp)) {
        QtVariantProperty *commentParentProperty = m_commentToProperty.value(varProp);
        emit propertyCommentChanged(commentParentProperty->propertyName(), value.toString());
        return;
    }

    if (!m_propertyToGroup.contains(property))
        return;

    if (varProp->propertyType() == QtVariantPropertyManager::enumTypeId()) {
        PropertySheetEnumValue e = qvariant_cast<PropertySheetEnumValue>(m_propertySheet->property(m_propertySheet->indexOf(property->propertyName())));
        const int val = value.toInt();
        const QString valName = varProp->attributeValue(m_strings.m_enumNamesAttribute).toStringList().at(val);
        bool ok = false;
        e.value = e.metaEnum.parseEnum(valName, &ok);
        Q_ASSERT(ok);
        QVariant v;
        qVariantSetValue(v, e);
        emit propertyValueChanged(property->propertyName(), v, true);
        return;
    }

    emit propertyValueChanged(property->propertyName(), value, enableSubPropertyHandling);
}

bool PropertyEditor::isDynamicProperty(const QtBrowserItem* item) const
{
    if (!item)
        return false;

    const QDesignerDynamicPropertySheetExtension *dynamicSheet =
            qt_extension<QDesignerDynamicPropertySheetExtension*>(m_core->extensionManager(), m_object);

    if (!dynamicSheet)
        return false;

    if (m_propertyToGroup.contains(item->property())
                && dynamicSheet->isDynamicProperty(m_propertySheet->indexOf(item->property()->propertyName())))
        return true;
    return false;
}

void PropertyEditor::editProperty(const QString &name)
{
    // find the browser item belonging to the property, make it current and edit it
    QtBrowserItem *browserItem = 0;
    if (QtVariantProperty *property = m_nameToProperty.value(name, 0)) {
        const QList<QtBrowserItem *> items = m_currentBrowser->items(property);
        if (items.size() == 1)
            browserItem = items.front();
    }
    if (browserItem == 0)
        return;
    m_currentBrowser->setFocus(Qt::OtherFocusReason);
    if (m_currentBrowser == m_treeBrowser) { // edit is currently only supported in tree view
        m_treeBrowser->editItem(browserItem);
    } else {
        m_currentBrowser->setCurrentItem(browserItem);
    }
}

void PropertyEditor::slotCurrentItemChanged(QtBrowserItem *item)
{
    m_removeDynamicAction->setEnabled(isDynamicProperty(item));

}

void PropertyEditor::slotRemoveDynamicProperty()
{
    if (QtBrowserItem* item = m_currentBrowser->currentItem())
        if (isDynamicProperty(item))
            emit removeDynamicProperty(item->property()->propertyName());
}
}

QT_END_NAMESPACE
