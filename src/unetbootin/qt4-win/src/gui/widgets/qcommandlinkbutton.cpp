/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include "qcommandlinkbutton.h"
#include "qstylepainter.h"
#include "qstyleoption.h"
#include "qtextdocument.h"
#include "qtextlayout.h"
#include "qcolor.h"
#include "qfont.h"

#include "private/qpushbutton_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCommandLinkButton qcommandlinkbutton.h
    \since 4.4
    \brief The QCommandLinkButton widget provides a Vista style command link button.

    \ingroup basicwidgets
    \mainclass

    The command link is a new control that was introduced by Windows Vista. It's
    intended use is similar to that of a radio button in that it is used to choose
    between a set of mutually exclusive options. Command link buttons should not
    be used by themselves but rather as an alternative to radio buttons in
    Wizards and dialogs and makes pressing the "next" button redundant.
    The appearance is generally similar to that of a flat pushbutton, but
    it allows for a descriptive text in addition to the normal button text.
    By default it will also carry an arrow icon, indicating that pressing the
    control will open another window or page.

    \sa QPushButton QRadioButton
*/


/*!
    \property QCommandLinkButton::description
    \brief A descriptive label to complement the button text

    Setting this property will set a descriptive text on the
    button, complementing the text label. This will usually
    be displayed in a smaller font than the primary text.
*/

class QCommandLinkButtonPrivate : public QPushButtonPrivate
{
    Q_DECLARE_PUBLIC(QCommandLinkButton)

public:
    QCommandLinkButtonPrivate()
        : QPushButtonPrivate(){}

    void init();
    qreal titleSize() const;
    bool usingVistaStyle() const;

    QFont titleFont() const;
    QFont descriptionFont() const;

    QRect titleRect() const;
    QRect descriptionRect() const;

    int textOffset() const;
    int descriptionOffset() const;
    int descriptionHeight(int width) const;
    QColor mergedColors(const QColor &a, const QColor &b, int value) const;

    int topMargin() const { return 10; }
    int leftMargin() const { return 7; }
    int rightMargin() const { return 4; }
    int bottomMargin() const { return 4; }

    QString description;
    QColor currentColor;
};

// Mix colors a and b with a ratio in the range [0-255]
QColor QCommandLinkButtonPrivate::mergedColors(const QColor &a, const QColor &b, int value = 50) const
{
    Q_ASSERT(value >= 0);
    Q_ASSERT(value <= 255);
    QColor tmp = a;
    tmp.setRed((tmp.red() * value) / 255 + (b.red() * (255 - value)) / 255);
    tmp.setGreen((tmp.green() * value) / 255 + (b.green() * (255 - value)) / 255);
    tmp.setBlue((tmp.blue() * value) / 255 + (b.blue() * (255 - value)) / 255);
    return tmp;
}

QFont QCommandLinkButtonPrivate::titleFont() const
{
    Q_Q(const QCommandLinkButton);
    QFont font = q->font();
    if (usingVistaStyle()) {
        font.setPointSizeF(12.0);
    } else {
        font.setBold(true);
        font.setPointSizeF(9.0);
    }
    return font;
}

QFont QCommandLinkButtonPrivate::descriptionFont() const
{
    Q_Q(const QCommandLinkButton);
    QFont font = q->font();
    font.setPointSizeF(9.0);
    return font;
}

QRect QCommandLinkButtonPrivate::titleRect() const
{
    Q_Q(const QCommandLinkButton);
    return q->rect().adjusted(textOffset(), topMargin(),
                              -rightMargin(), 0);
}

QRect QCommandLinkButtonPrivate::descriptionRect() const
{
    Q_Q(const QCommandLinkButton);
    return q->rect().adjusted(textOffset(), descriptionOffset(),
                              -rightMargin(), -bottomMargin());
}

int QCommandLinkButtonPrivate::textOffset() const
{
    Q_Q(const QCommandLinkButton);
    return q->icon().actualSize(q->iconSize()).width() + leftMargin() + 6;
}

int QCommandLinkButtonPrivate::descriptionOffset() const
{
    QFontMetrics fm(titleFont());
    return topMargin() + fm.height();
}

bool QCommandLinkButtonPrivate::usingVistaStyle() const
{
    Q_Q(const QCommandLinkButton);
    //### This is a hack to detect if we are indeed running Vista style themed and not in classic
    // When we add api to query for this, we should change this implementation to use it.
    return q->style()->inherits("QWindowsVistaStyle")
        && !q->style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal);
}

void QCommandLinkButtonPrivate::init()
{
    Q_Q(QCommandLinkButton);
    QPushButtonPrivate::init();
    q->setAttribute(Qt::WA_Hover);

    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Preferred, QSizePolicy::PushButton);
    policy.setHeightForWidth(true);
    q->setSizePolicy(policy);

    q->setIconSize(QSize(20, 20));
    q->setIcon(q->style()->standardIcon(QStyle::SP_CommandLink));
}

// Calculates the height of the description text based on widget width
int QCommandLinkButtonPrivate::descriptionHeight(int widgetWidth) const
{
    // Calc width of actual paragraph
    int lineWidth = widgetWidth - textOffset() - rightMargin();

    qreal descriptionheight = 0;
    if (!description.isEmpty()) {
        QTextLayout layout(description);
        layout.setFont(descriptionFont());
        layout.beginLayout();
        while (true) {
            QTextLine line = layout.createLine();
            if (!line.isValid())
                break;
            line.setLineWidth(lineWidth);
            line.setPosition(QPointF(0, descriptionheight));
            descriptionheight += line.height();
        }
        layout.endLayout();
    }
    return qRound(descriptionheight);
}

/*!
    \reimp
 */
QSize QCommandLinkButton::minimumSizeHint() const
{
    Q_D(const QCommandLinkButton);
    QSize size = sizeHint();
    int minimumHeight = qMax(d->descriptionOffset() + d->bottomMargin(),
                             iconSize().height() + d->topMargin());
    size.setHeight(minimumHeight);
    return size;
}

/*!
    Constructs a command link with no text and a \a parent.
*/

QCommandLinkButton::QCommandLinkButton(QWidget *parent)
: QPushButton(*new QCommandLinkButtonPrivate, parent)
{
    Q_D(QCommandLinkButton);
    d->init();
}

/*!
    Constructs a command link with the parent \a parent and the text \a
    text.
*/

QCommandLinkButton::QCommandLinkButton(const QString &text, QWidget *parent)
    : QPushButton(*new QCommandLinkButtonPrivate, parent)
{
    Q_D(QCommandLinkButton);
    setText(text);
    d->init();
}

/*!
    Constructs a command link with a \a text, a \a description, and a \a parent.
*/
QCommandLinkButton::QCommandLinkButton(const QString &text, const QString &description, QWidget *parent)
    : QPushButton(*new QCommandLinkButtonPrivate, parent)
{
    Q_D(QCommandLinkButton);
    setText(text);
    setDescription(description);
    d->init();
}

/*! \reimp */
bool QCommandLinkButton::event(QEvent *e)
{
    return QPushButton::event(e);
}

/*! \reimp */
QSize QCommandLinkButton::sizeHint() const
{
//  Standard size hints from UI specs
//  Without note: 135, 41
//  With note: 135, 60
    Q_D(const QCommandLinkButton);

    QSize size = QPushButton::sizeHint();
    QFontMetrics fm(d->titleFont());
    int textWidth = qMax(fm.width(text()), 135);
    int buttonWidth = textWidth + d->textOffset() + d->rightMargin();
    int heightWithoutDescription = d->descriptionOffset() + d->bottomMargin();

    size.setWidth(qMax(size.width(), buttonWidth));
    size.setHeight(qMax(d->description.isEmpty() ? 41 : 60,
                        heightWithoutDescription + d->descriptionHeight(buttonWidth)));
    return size;
}

/*! \reimp */
int QCommandLinkButton::heightForWidth(int width) const
{
    Q_D(const QCommandLinkButton);
    int heightWithoutDescription = d->descriptionOffset() + d->bottomMargin();
    // find the width available for the description area
    return heightWithoutDescription + d->descriptionHeight(width);
}

/*! \reimp */
void QCommandLinkButton::paintEvent(QPaintEvent *)
{
    Q_D(QCommandLinkButton);
    QStylePainter p(this);
    p.save();

    QStyleOptionButton option;
    initStyleOption(&option);

    //Enable command link appearence on Vista
    option.features |= QStyleOptionButton::CommandLinkButton;
    option.text = QString();
    option.icon = QIcon(); //we draw this ourselves
    QSize pixmapSize = icon().actualSize(iconSize());

    int vOffset = isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftVertical) : 0;
    int hOffset = isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal) : 0;

    //Draw icon
    p.drawControl(QStyle::CE_PushButton, option);
    if (!icon().isNull())
        p.drawPixmap(d->leftMargin() + hOffset, d->topMargin() + vOffset,
        icon().pixmap(pixmapSize, isEnabled() ? QIcon::Normal : QIcon::Disabled, QIcon::Off));

    //Draw title
    QColor textColor = palette().buttonText().color();
    if (isEnabled() && d->usingVistaStyle()) {
        textColor = QColor(21, 28, 85);
        if (underMouse() && !isDown())
            textColor = QColor(7, 64, 229);
        //A simple text color transition
        d->currentColor = d->mergedColors(textColor, d->currentColor, 60);
        option.palette.setColor(QPalette::ButtonText, d->currentColor);
    }

    int textflags = Qt::TextShowMnemonic;
    if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &option, this))
        textflags |= Qt::TextHideMnemonic;

    p.setFont(d->titleFont());
    p.drawItemText(d->titleRect().translated(hOffset, vOffset),
                    textflags, option.palette, isEnabled(), text(), QPalette::ButtonText);

    //Draw description
    textflags |= Qt::TextWordWrap | Qt::ElideRight;
    p.setFont(d->descriptionFont());
    p.drawItemText(d->descriptionRect().translated(hOffset, vOffset), textflags,
                    option.palette, isEnabled(), description(), QPalette::ButtonText);
    p.restore();
}

void QCommandLinkButton::setDescription(const QString &description)
{
    Q_D(QCommandLinkButton);
    d->description = description;
    updateGeometry();
    update();
}

QString QCommandLinkButton::description() const
{
    Q_D(const QCommandLinkButton);
    return d->description;
}

QT_END_NAMESPACE

