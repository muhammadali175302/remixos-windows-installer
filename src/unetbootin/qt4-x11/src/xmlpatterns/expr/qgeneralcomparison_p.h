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

#ifndef Patternist_GeneralComparison_H
#define Patternist_GeneralComparison_H

#include "qatomiccomparator_p.h"
#include "qpaircontainer_p.h"
#include "qcomparisonplatform_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Implements XPath 2.0's general comparions, such as the <tt>=</tt> operator.
     *
     * ComparisonPlatform is inherited with @c protected scope because ComparisonPlatform
     * must access members of GeneralComparison.
     *
     * @see <a href="http://www.w3.org/TR/xpath20/#id-general-comparisons">XML Path Language
     * (XPath) 2.0, 3.5.2 General Comparisons</a>
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class GeneralComparison : public PairContainer,
                              public ComparisonPlatform<GeneralComparison,
                                                        true /* We want to report errors. */,
                                                        AtomicComparator::AsGeneralComparison>
    {
    public:
        GeneralComparison(const Expression::Ptr &op1,
                          const AtomicComparator::Operator op,
                          const Expression::Ptr &op2,
                          const bool isBackwardsCompat = false);

        virtual bool evaluateEBV(const DynamicContext::Ptr &) const;
        virtual Expression::Ptr typeCheck(const StaticContext::Ptr &context,
                                          const SequenceType::Ptr &reqType);

        virtual SequenceType::List expectedOperandTypes() const;
        virtual SequenceType::Ptr staticType() const;

        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        /**
         * @returns always IDGeneralComparison
         */
        virtual ID id() const;

        virtual QList<QExplicitlySharedDataPointer<OptimizationPass> > optimizationPasses() const;

        /**
         * @returns the operator that this GeneralComparison is using.
         */
        inline AtomicComparator::Operator operatorID() const
        {
            return m_operator;
        }

        /**
         * Overridden to optimize case-insensitive compares.
         */
        virtual Expression::Ptr compress(const StaticContext::Ptr &context);

    private:
        static inline void updateType(ItemType::Ptr &type,
                                      const Expression::Ptr &source);
        AtomicComparator::Ptr fetchGeneralComparator(Expression::Ptr &op1,
                                                     Expression::Ptr &op2,
                                                     const ReportContext::Ptr &context) const;
        bool generalCompare(const Item &op1,
                            const Item &op2,
                            const DynamicContext::Ptr &context) const;

        const AtomicComparator::Operator m_operator;
        const bool m_isBackwardsCompat;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
