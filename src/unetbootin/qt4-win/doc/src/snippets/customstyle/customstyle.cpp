/****************************************************************************
**
** Copyright (C) 2004-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QtGui>

#include "customstyle.h"

CustomStyle::CustomStyle()
{
//! [0]
    QSpinBox *spinBox = qobject_cast<QSpinBox *>(widget);
    if (spinBox) {
//! [0] //! [1]
    }
//! [1]
}

//! [2]
void CustomStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                                QPainter *painter, const QWidget *widget) const
{
    if (element == PE_IndicatorSpinUp || element == PE_IndicatorSpinDown) {
	QPolygon points(3);
	int x = option->rect.x();
	int y = option->rect.y();
	int w = option->rect.width() / 2;
	int h = option->rect.height() / 2;
	x += (option->rect.width() - w) / 2;
	y += (option->rect.height() - h) / 2;

	if (element == PE_IndicatorSpinUp) {
	    points[0] = QPoint(x, y + h);
	    points[1] = QPoint(x + w, y + h);
	    points[2] = QPoint(x + w / 2, y);
	} else { // PE_SpinBoxDown
	    points[0] = QPoint(x, y);
	    points[1] = QPoint(x + w, y);
	    points[2] = QPoint(x + w / 2, y + h);
	}

	if (option->state & State_Enabled) {
	    painter->setPen(option->palette.mid().color());
	    painter->setBrush(option->palette.buttonText());
	} else {
	    painter->setPen(option->palette.buttonText().color());
	    painter->setBrush(option->palette.mid());
	}
	painter->drawPolygon(points);
    } else {
	QWindowsStyle::drawPrimitive(element, option, painter, widget);
//! [2] //! [3]
    }
//! [3] //! [4]
}
//! [4]
