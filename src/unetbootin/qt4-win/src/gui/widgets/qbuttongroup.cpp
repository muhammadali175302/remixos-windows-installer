/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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



/*!
    \class QButtonGroup qbuttongroup.h
    \brief The QButtonGroup class provides a container to organize groups of
    button widgets.

    \ingroup organizers
    \ingroup geomanagement
    \ingroup appearance
    \mainclass

    QButtonGroup provides an abstract container into which button widgets can
    be placed. It does not provide a visual representation of this container
    (see QGroupBox for a container widget), but instead manages the states of
    each of the buttons in the group.

    An \l exclusive button group switches off all checkable (toggle)
    buttons except the one that was clicked. By default, a button
    group is exclusive. The buttons in a button group are usually
    checkable QPushButton's, \l{QCheckBox}es (normally for
    non-exclusive button groups), or \l{QRadioButton}s. If you create
    an exclusive button group, you should ensure that one of the
    buttons in the group is initially checked; otherwise, the group
    will initially be in a state where no buttons are checked.

    A button is added to the group with addButton(). It can be removed
    from the group with removeButton(). If the group is exclusive, the
    currently checked button is available as checkedButton(). If a
    button is clicked the buttonClicked() signal is emitted. For a
    checkable button in an exclusive group this means that the button
    was checked. The list of buttons in the group is returned by
    buttons().

    In addition, QButtonGroup can map between integers and
    buttons. You can assign an integer id to a button with setId(),
    and retrieve it with id(). The id of the currently checked button
    is available with checkedId(), and there is an overloaded signal
    buttonClicked() which emits the id of the button. The purpose of
    the mapping mechanism is to simplify the representation of enum
    values in a user interface.

    \sa QGroupBox QPushButton, QCheckBox, QRadioButton
*/

/*!
    \fn QButtonGroup::QButtonGroup(QObject *parent)

    Constructs a new, empty button group with the given \a parent.

    \sa addButton() setExclusive()
*/

/*!
    \fn QButtonGroup::~QButtonGroup()

    Destroys the button group.
*/

/*!
    \property QButtonGroup::exclusive
    \brief whether the button group is exclusive

    If this property is true then only one button in the group can be checked
    at any given time. The user can click on any button to check it, and that
    button will replace the existing one as the checked button in the group.

    In an exclusive group, the user cannot uncheck the currently checked button
    by clicking on it; instead, another button in the group must be clicked
    to set the new checked button for that group.

    By default, this property is true.
*/

/*!
    \fn void QButtonGroup::buttonClicked(QAbstractButton *button)

    This signal is emitted when the given \a button is clicked. A
    button is clicked when it is first pressed and then released, when
    its shortcut key is typed, or programmatically when
    QAbstractButton::click() or QAbstractButton::animateClick() is
    called.


    \sa checkedButton(), QAbstractButton::clicked()
*/

/*!
    \fn void QButtonGroup::buttonClicked(int id)

    This signal is emitted when a button with the given \a id is
    clicked.

    \sa checkedButton(), QAbstractButton::clicked()
*/

/*!
    \fn void QButtonGroup::buttonPressed(QAbstractButton *button)
    \since 4.2

    This signal is emitted when the given \a button is pressed down. 

    \sa QAbstractButton::pressed()
*/

/*!
    \fn void QButtonGroup::buttonPressed(int id)
    \since 4.2

    This signal is emitted when a button with the given \a id is
    pressed down.

    \sa QAbstractButton::pressed()
*/

/*!
    \fn void QButtonGroup::buttonReleased(QAbstractButton *button)
    \since 4.2

    This signal is emitted when the given \a button is released. 

    \sa QAbstractButton::released()
*/

/*!
    \fn void QButtonGroup::buttonReleased(int id)
    \since 4.2

    This signal is emitted when a button with the given \a id is
    released.

    \sa QAbstractButton::released()
*/

/*!
    \fn void QButtonGroup::addButton(QAbstractButton *button, int id = -1);

    Adds the given \a button to the button group, with the given \a id.

    \sa removeButton() buttons()
*/

/*!
    \fn void QButtonGroup::removeButton(QAbstractButton *button);

    Removes the given \a button from the button group.

    \sa addButton() buttons()
*/

/*!
    \fn QList<QAbstractButton*> QButtonGroup::buttons() const

    Returns the list of this groups's buttons. This may be empty.

    \sa addButton(), removeButton()
*/

/*!
    \fn QAbstractButton *QButtonGroup::checkedButton() const;

    Returns the button group's checked button, or 0 if no buttons are
    checked.

    \sa buttonClicked()
*/

/*!
    \fn QAbstractButton *QButtonGroup::button(int id) const;
    \since 4.1

    Returns the button with the specified \a id, or 0 if no such button
    exists.
*/

/*!
    \fn void QButtonGroup::setId(QAbstractButton *button, int id)
    \since 4.1

    Sets the \a id for the specified \a button.

    \sa id()
*/

/*!
    \fn int QButtonGroup::id(QAbstractButton *button) const;
    \since 4.1

    Returns the id for the specified \a button, or -1 if no such button
    exists.

    \sa setId()
*/

/*!
    \fn int QButtonGroup::checkedId() const;
    \since 4.1

    Returns the id of the checkedButton(), or -1 if no button is checked.

    \sa setId()
*/


/*! \fn void QButtonGroup::insert(QAbstractButton *b)

    Use addButton() instead.
*/

/*! \fn void QButtonGroup::remove(QAbstractButton *b)

    Use removeButton() instead.
*/
