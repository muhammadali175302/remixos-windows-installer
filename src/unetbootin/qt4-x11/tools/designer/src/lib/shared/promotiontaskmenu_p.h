/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef PROMOTIONTASKMENU_H
#define PROMOTIONTASKMENU_H

#include "shared_global_p.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;

class QAction;
class QMenu;
class QWidget;
class QSignalMapper;

namespace qdesigner_internal {

// A helper class for creating promotion context menus and handling promotion actions.

class QDESIGNER_SHARED_EXPORT PromotionTaskMenu: public QObject
{
    Q_OBJECT
public:
    enum Mode {
        ModeSingleWidget,
        ModeManagedMultiSelection,
        ModeUnmanagedMultiSelection
    };

    explicit PromotionTaskMenu(QWidget *widget,Mode mode = ModeManagedMultiSelection, QObject *parent = 0);

    Mode mode() const;
    void setMode(Mode m);

    void setWidget(QWidget *widget);

    // Set menu labels
    void setPromoteLabel(const QString &promoteLabel);
    void setEditPromoteToLabel(const QString &promoteEditLabel);
    // Defaults to "Demote to %1".arg(class).
    void setDemoteLabel(const QString &demoteLabel);

    typedef QList<QAction*> ActionList;

    enum AddFlags { LeadingSeparator = 1, TrailingSeparator = 2, SuppressGlobalEdit = 4};

    // Adds a list of promotion actions according to the current promotion state of the widget.
    void addActions(QDesignerFormWindowInterface *fw, unsigned flags, ActionList &actionList);
    // Convenience that finds the form window.
    void addActions(unsigned flags, ActionList &actionList);

    void addActions(QDesignerFormWindowInterface *fw, unsigned flags, QMenu *menu);
    void addActions(unsigned flags, QMenu *menu);

    // Pop up the editor in a global context.
    static void editPromotedWidgets(QDesignerFormEditorInterface *core, QWidget* parent);

private slots:
    void slotPromoteToCustomWidget(const QString &customClassName);
    void slotDemoteFromCustomWidget();
    void slotEditPromotedWidgets();
    void slotEditPromoteTo();
    void slotEditSignalsSlots();

private:
    void promoteTo(QDesignerFormWindowInterface *fw, const QString &customClassName);

    enum PromotionState { NotApplicable, NoHomogenousSelection, CanPromote, CanDemote };
    PromotionState createPromotionActions(QDesignerFormWindowInterface *formWindow);
    QDesignerFormWindowInterface *formWindow() const;

    typedef QList<QPointer<QWidget> > PromotionSelectionList;
    PromotionSelectionList promotionSelectionList(QDesignerFormWindowInterface *formWindow) const;

    Mode m_mode;

    QPointer<QWidget> m_widget;

    QSignalMapper *m_promotionMapper;
    // Per-Widget actions
    QList<QAction *> m_promotionActions;

    QAction *m_globalEditAction;
    QAction *m_EditPromoteToAction;
    QAction *m_EditSignalsSlotsAction;

    QString m_promoteLabel;
    QString m_demoteLabel;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // PROMOTIONTASKMENU_H
