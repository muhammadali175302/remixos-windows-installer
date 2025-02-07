/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_ExternalVariableLoader_H
#define Patternist_ExternalVariableLoader_H

#include "qitem_p.h"
#include "qsequencetype_p.h"
#include "qxmlname.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    class DynamicContext;

    /**
     * @short Responsible for loading and declaring available external variables.
     *
     * An external variable in XQuery is a global variable that has been declared to receive
     * its value from the XQuery implementation, as opposed to an initializing expression. Here
     * is an example of a query with an external variable declaration, followed by a ordinary
     * global variable:
     *
     * <tt> declare variable $theName external;
     * declare variable $theName := "the value";
     * "And here's the query body(a string literal)"</tt>
     *
     * An external variable declaration can also specify a sequence type:
     *
     * <tt>declare variable $theName as xs:integer external;</tt>
     *
     * This class allows the user to supply the values to external variables. When
     * an external variable declaration occur in the query,
     * announceExternalVariable() is called.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Q_AUTOTEST_EXPORT ExternalVariableLoader : public QSharedData
    {
    public:
        typedef QExplicitlySharedDataPointer<ExternalVariableLoader> Ptr;
        inline ExternalVariableLoader() {}

        virtual ~ExternalVariableLoader();

        /**
         * Called when Patternist encounters an external variable in the query. It is guaranteed
         * to be called once for each external variable appearing in a query module.
         *
         * @param name the name of the variable. Quaranteed to never be @c null.
         * @param declaredType the type that the user declared the variable to be of. Whether
         * this type matches the actual value of the variable or not is irrelevant. Patternist
         * will do the necessary error handling based on the sequence type that is returned from
         * this function. If the user didn't declare a type, the type is <tt>item()*</tt>(zero or
         * more items). Quaranteed to never be @c null.
         * @returns the sequence type of the value this ExternalVariableLoader actually supplies. However,
         * if the ExternalVariableLoader knows it cannot supply a variable by this name, @c null should be
         * returned.
         */
        virtual SequenceType::Ptr announceExternalVariable(const QXmlName name,
                                                           const SequenceType::Ptr &declaredType);

        /**
         * This function is called at runtime when the external variable by name @p name needs
         * to be evaluated. It is not defined how many times this function will be called. It
         * depends on aspects such as how the query was optimized.
         *
         * @param name the name of the variable. Quaranteed to never be @c null.
         * @param context the DynamicContext.
         * @returns the value of the variable. Remember that this value must match the
         * sequence type returned from announceExternalVariable() for the same name.
         */
        virtual Item::Iterator::Ptr evaluateSequence(const QXmlName name,
                                                     const QExplicitlySharedDataPointer<DynamicContext> &context);

        virtual Item evaluateSingleton(const QXmlName name,
                                            const QExplicitlySharedDataPointer<DynamicContext> &context);
        virtual bool evaluateEBV(const QXmlName name,
                                 const QExplicitlySharedDataPointer<DynamicContext> &context);
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
