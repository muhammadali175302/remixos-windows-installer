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

#include "qfontcombobox.h"

#ifndef QT_NO_FONTCOMBOBOX

#include <qstringlistmodel.h>
#include <qitemdelegate.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qevent.h>
#include <private/qcombobox_p.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

extern QObject *qt_fontdatabase_private();

static QFontDatabase::WritingSystem writingSystemForFont(const QFont &font, bool *hasLatin)
{
    *hasLatin = true;

    QList<QFontDatabase::WritingSystem> writingSystems = QFontDatabase().writingSystems(font.family());
//     qDebug() << font.family() << writingSystems;

    // this just confuses the algorithm below. Vietnamese is Latin with lots of special chars
    writingSystems.removeAll(QFontDatabase::Vietnamese);

    QFontDatabase::WritingSystem system = QFontDatabase::Any;

    if (!writingSystems.contains(QFontDatabase::Latin)) {
        *hasLatin = false;
        // we need to show something
        if (writingSystems.count())
            system = writingSystems.last();
    } else {
        writingSystems.removeAll(QFontDatabase::Latin);
    }

    if (writingSystems.isEmpty())
        return system;

    if (writingSystems.count() == 1 && writingSystems.at(0) > QFontDatabase::Cyrillic) {
        system = writingSystems.at(0);
        return system;
    }

    if (writingSystems.count() <= 2
        && writingSystems.last() > QFontDatabase::Armenian
        && writingSystems.last() < QFontDatabase::Vietnamese) {
        system = writingSystems.last();
        return system;
    }

    if (writingSystems.count() <= 5
        && writingSystems.last() >= QFontDatabase::SimplifiedChinese
        && writingSystems.last() <= QFontDatabase::Korean)
        system = writingSystems.last();

    return system;
}

class QFontFamilyDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit QFontFamilyDelegate(QObject *parent);

    // painting
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    QIcon truetype;
    QIcon bitmap;
    QFontDatabase::WritingSystem writingSystem;
};

QFontFamilyDelegate::QFontFamilyDelegate(QObject *parent)
    : QAbstractItemDelegate(parent)
{
    truetype = QIcon(QLatin1String(":/trolltech/styles/commonstyle/images/fonttruetype-16.png"));
    bitmap = QIcon(QLatin1String(":/trolltech/styles/commonstyle/images/fontbitmap-16.png"));
    writingSystem = QFontDatabase::Any;
}

void QFontFamilyDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    QFont font(option.font);
    font.setPointSize(QFontInfo(font).pointSize() * 3 / 2);
    QFont font2 = font;
    font2.setFamily(text);

    bool hasLatin;
    QFontDatabase::WritingSystem system = writingSystemForFont(font2, &hasLatin);
    if (hasLatin)
        font = font2;

    QRect r = option.rect;

    if (option.state & QStyle::State_Selected) {
        painter->save();
        painter->setBrush(option.palette.highlight());
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);
        painter->setPen(QPen(option.palette.highlightedText(), 0));
    }

    const QIcon *icon = &bitmap;
    if (QFontDatabase().isSmoothlyScalable(text)) {
        icon = &truetype;
    }
    QSize actualSize = icon->actualSize(r.size());

    icon->paint(painter, r, Qt::AlignLeft|Qt::AlignVCenter);
    if (option.direction == Qt::RightToLeft)
        r.setRight(r.right() - actualSize.width() - 4);
    else
        r.setLeft(r.left() + actualSize.width() + 4);

    QFont old = painter->font();
    painter->setFont(font);
    painter->drawText(r, Qt::AlignVCenter|Qt::AlignLeading|Qt::TextSingleLine, text);

    if (writingSystem != QFontDatabase::Any)
        system = writingSystem;

    if (system != QFontDatabase::Any) {
        int w = painter->fontMetrics().width(text + QLatin1String("  "));
        painter->setFont(font2);
        QString sample = QFontDatabase().writingSystemSample(system);
        if (option.direction == Qt::RightToLeft)
            r.setRight(r.right() - w);
        else
            r.setLeft(r.left() + w);
        painter->drawText(r, Qt::AlignVCenter|Qt::AlignLeading|Qt::TextSingleLine, sample);
    }
    painter->setFont(old);

    if (option.state & QStyle::State_Selected)
        painter->restore();

}

QSize QFontFamilyDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    QFont font(option.font);
//     font.setFamily(text);
    font.setPointSize(QFontInfo(font).pointSize() * 3/2);
    QFontMetrics fontMetrics(font);
    return QSize(fontMetrics.width(text), fontMetrics.lineSpacing());
}


class QFontComboBoxPrivate : public QComboBoxPrivate
{
public:
    inline QFontComboBoxPrivate() { filters = QFontComboBox::AllFonts; }

    QFontComboBox::FontFilters filters;
    QFont currentFont;

    void _q_updateModel();
    void _q_currentChanged(const QString &);

    Q_DECLARE_PUBLIC(QFontComboBox)
};


void QFontComboBoxPrivate::_q_updateModel()
{
    Q_Q(QFontComboBox);
    const int scalableMask = (QFontComboBox::ScalableFonts | QFontComboBox::NonScalableFonts);
    const int spacingMask = (QFontComboBox::ProportionalFonts | QFontComboBox::MonospacedFonts);

    QStringListModel *m = qobject_cast<QStringListModel *>(q->model());
    if (!m)
        return;
    QFontFamilyDelegate *delegate = qobject_cast<QFontFamilyDelegate *>(q->view()->itemDelegate());
    QFontDatabase::WritingSystem system = delegate ? delegate->writingSystem : QFontDatabase::Any;

    QFontDatabase fdb;
    QStringList list = fdb.families(system);
    QStringList result;

    int offset = 0;
    QFontInfo fi(currentFont);

    for (int i = 0; i < list.size(); ++i) {
        if ((filters & scalableMask) && (filters & scalableMask) != scalableMask) {
            if (bool(filters & QFontComboBox::ScalableFonts) != fdb.isSmoothlyScalable(list.at(i)))
                continue;
        }
        if ((filters & spacingMask) && (filters & spacingMask) != spacingMask) {
            if (bool(filters & QFontComboBox::MonospacedFonts) != fdb.isFixedPitch(list.at(i)))
                continue;
        }
        result += list.at(i);
        if (list.at(i) == fi.family() || list.at(i).startsWith(fi.family() + QLatin1String(" [")))
            offset = result.count() - 1;
    }
    list = result;

    m->setStringList(list);
    if (list.isEmpty()) {
        if (currentFont != QFont()) {
            currentFont = QFont();
            emit q->currentFontChanged(currentFont);
        }
    } else {
        q->setCurrentIndex(offset);
    }
}


void QFontComboBoxPrivate::_q_currentChanged(const QString &text)
{
    Q_Q(QFontComboBox);
    QFont newFont(text);
    if (currentFont != newFont) {
        currentFont = newFont;
        emit q->currentFontChanged(currentFont);
    }
}

/*!
    \class QFontComboBox
    \brief The QFontComboBox widget is a combobox that lets the user
    select a font family.

    \since 4.2
    \ingroup basicwidgets
    \ingroup text

    The combobox is populated with an alphabetized list of font
    family names, such as Arial, Helvetica, and Times New Roman.
    Family names are displayed using the actual font when possible.
    For fonts such as Symbol, where the name is not representable in
    the font itself, a sample of the font is displayed next to the
    family name.

    QFontComboBox is often used in toolbars, in conjunction with a
    QComboBox for controlling the font size and two \l{QToolButton}s
    for bold and italic.

    When the user selects a new font, the currentFontChanged() signal
    is emitted in addition to currentIndexChanged().

    Call setWritingSystem() to tell QFontComboBox to show only fonts
    that support a given writing system, and setFontFilters() to
    filter out certain types of fonts as e.g. non scalable fonts or
    monospaced fonts.

    \image windowsxp-fontcombobox.png Screenshot of QFontComboBox on Windows XP

    \sa QComboBox, QFont, QFontInfo, QFontMetrics, QFontDatabase, {Character Map Example}
*/

/*!
    \fn void QFontComboBox::setWritingSystem(QFontDatabase::WritingSystem script)
*/

/*!
    \fn void QFontComboBox::setCurrentFont(const QFont &font);
*/

/*!
    Constructs a font combobox with the given \a parent.
*/
QFontComboBox::QFontComboBox(QWidget *parent)
    : QComboBox(*new QFontComboBoxPrivate, parent)
{
    Q_D(QFontComboBox);
    d->currentFont = font();
    setEditable(true);

    QStringListModel *m = new QStringListModel(this);
    setModel(m);
    setItemDelegate(new QFontFamilyDelegate(this));
    QListView *lview = qobject_cast<QListView*>(view());
    if (lview)
        lview->setUniformItemSizes(true);
    setWritingSystem(QFontDatabase::Any);

    connect(this, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(_q_currentChanged(QString)));

    // qfontdatabase.cpp
    connect(qt_fontdatabase_private(), SIGNAL(fontDatabaseChanged()),
            this, SLOT(_q_updateModel()));
}


/*!
    Destroys the combobox.
*/
QFontComboBox::~QFontComboBox()
{
}

/*!
    \property QFontComboBox::writingSystem
    \brief the writing system that serves as a filter for the combobox

    If \a script is QFontDatabase::Any (the default), all fonts are
    listed.

    \sa fontFilters
*/

void QFontComboBox::setWritingSystem(QFontDatabase::WritingSystem script)
{
    Q_D(QFontComboBox);
    QFontFamilyDelegate *delegate = qobject_cast<QFontFamilyDelegate *>(view()->itemDelegate());
    if (delegate)
        delegate->writingSystem = script;
    d->_q_updateModel();
}

QFontDatabase::WritingSystem QFontComboBox::writingSystem() const
{
    QFontFamilyDelegate *delegate = qobject_cast<QFontFamilyDelegate *>(view()->itemDelegate());
    if (delegate)
        return delegate->writingSystem;
    return QFontDatabase::Any;
}


/*!
  \enum QFontComboBox::FontFilter

  This enum can be used to only show certain types of fonts in the font combo box.

  \value AllFonts Show all fonts
  \value ScalableFonts Show scalable fonts
  \value NonScalableFonts Show non scalable fonts
  \value MonospacedFonts Show monospaced fonts
  \value ProportionalFonts Show proportional fonts
*/

/*!
    \property QFontComboBox::fontFilters
    \brief the filter for the combobox

    By default, all fonts are listed.

    \sa writingSystem
*/
void QFontComboBox::setFontFilters(FontFilters filters)
{
    Q_D(QFontComboBox);
    d->filters = filters;
    d->_q_updateModel();
}

QFontComboBox::FontFilters QFontComboBox::fontFilters() const
{
    Q_D(const QFontComboBox);
    return d->filters;
}

/*!
    \property QFontComboBox::currentFont
    \brief the currently selected font

    \sa currentFontChanged(), currentIndex, currentText
*/
QFont QFontComboBox::currentFont() const
{
    Q_D(const QFontComboBox);
    return d->currentFont;
}

void QFontComboBox::setCurrentFont(const QFont &font)
{
    Q_D(QFontComboBox);
    if (font != d->currentFont) {
        d->currentFont = font;
        emit currentFontChanged(d->currentFont);
        d->_q_updateModel();
    }
}

/*!
    \fn QFontComboBox::currentFontChanged(const QFont &font)

    This signal is emitted whenever the current font changes, with
    the new \a font.

    \sa currentFont
*/

/*!
    \reimp
*/
bool QFontComboBox::event(QEvent *e)
{
    if (e->type() == QEvent::Resize) {
        QListView *lview = qobject_cast<QListView*>(view());
        if (lview)
            lview->window()->setFixedWidth(width() * 5 / 3);
    }
    return QComboBox::event(e);
}

/*!
    \reimp
*/
QSize QFontComboBox::sizeHint() const
{
    QSize sz = QComboBox::sizeHint();
    QFontMetrics fm(font());
    sz.setWidth(fm.width(QLatin1Char('m'))*14);
    return sz;
}

QT_END_NAMESPACE

#include "qfontcombobox.moc"
#include "moc_qfontcombobox.cpp"

#endif // QT_NO_FONTCOMBOBOX
