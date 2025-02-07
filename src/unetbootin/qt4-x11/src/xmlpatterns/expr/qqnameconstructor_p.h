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

#ifndef Patternist_QNameConstructor_H
#define Patternist_QNameConstructor_H

#include "qsinglecontainer_p.h"
#include "qbuiltintypes_p.h"
#include "qpatternistlocale_p.h"
#include "qxpathhelper_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * @short Creates an @c xs:QName value from a lexical QName using
     * statically known namespace bindings.
     *
     * @see QQNameValue
     * @see QXmlUtils
     * @author Frans Englich <fenglich@trolltech.com>
     * @ingroup Patternist_expressions
     */
    class QNameConstructor : public SingleContainer
    {
    public:

        QNameConstructor(const Expression::Ptr &source,
                         const NamespaceResolver::Ptr &nsResolver);

        virtual Item evaluateSingleton(const DynamicContext::Ptr &) const;

        virtual SequenceType::List expectedOperandTypes() const;

        virtual SequenceType::Ptr staticType() const;

        virtual ExpressionVisitorResult::Ptr accept(const ExpressionVisitor::Ptr &visitor) const;

        /**
         * Expands @p lexicalQName, which is a lexical representation of a QName such as "x:body", into
         * a QName using @p nsResolver to supply the namespace bindings.
         *
         * If @p lexicalQName is lexically invalid @p InvalidQName is raised via @p context, or if
         * no namespace binding does not exists for a prefix(if any) in @p lexicalQName, @p NoBinding
         * is raised via @p context.
         *
         * If @p asForAttribute is @c true, the name is considered to be for an
         * attribute in some way, and @p lexicalQName will not pick up the
         * default namespace if it doesn't have a prefix.
         *
         * @p nsResolver is parameterized meaning the function can be instantiated with either
         * DynamicContext or StaticContext.
         *
         * @see QQNameValue
         * @see QXmlUtils
         */
        template<typename TReportContext,
                 const ReportContext::ErrorCode InvalidQName,
                 const ReportContext::ErrorCode NoBinding>
        static
        QXmlName expandQName(const QString &lexicalQName,
                             const TReportContext &context,
                             const NamespaceResolver::Ptr &nsResolver,
                             const SourceLocationReflection *const r,
                             const bool asForAttribute = false);

        /**
         * Resolves the namespace prefix @p prefix to its namespace if it exists, or
         * raised ReportContext::XPST0081 otherwise.
         *
         * @returns the namespace URI corresponding to @p prefix
         */
        static QXmlName::NamespaceCode namespaceForPrefix(const QXmlName::PrefixCode prefix,
                                                          const StaticContext::Ptr &context,
                                                          const SourceLocationReflection *const r);

        virtual const SourceLocationReflection *actualReflection() const;

    private:
        const NamespaceResolver::Ptr m_nsResolver;
    };

    template<typename TReportContext,
             const ReportContext::ErrorCode InvalidQName,
             const ReportContext::ErrorCode NoBinding>
    QXmlName QNameConstructor::expandQName(const QString &lexicalQName,
                                           const TReportContext &context,
                                           const NamespaceResolver::Ptr &nsResolver,
                                           const SourceLocationReflection *const r,
                                           const bool asForAttribute)
    {
        Q_ASSERT(nsResolver);
        Q_ASSERT(context);

        if(XPathHelper::isQName(lexicalQName))
        {
            QString prefix;
            QString local;
            XPathHelper::splitQName(lexicalQName, prefix, local);
            const QXmlName::NamespaceCode nsCode = asForAttribute && prefix.isEmpty() ? QXmlName::NamespaceCode(StandardNamespaces::empty)
                                                                                   : (nsResolver->lookupNamespaceURI(context->namePool()->allocatePrefix(prefix)));

            if(nsCode == NamespaceResolver::NoBinding)
            {
                context->error(QtXmlPatterns::tr("No namespace binding exists for "
                                  "the prefix %1 in %2").arg(formatKeyword(prefix),
                                                             formatKeyword(lexicalQName)),
                               NoBinding,
                               r);
                return QXmlName(); /* Silence compiler warning. */
            }
            else
                return context->namePool()->allocateQName(context->namePool()->stringForNamespace(nsCode), local, prefix);
        }
        else
        {
            context->error(QtXmlPatterns::tr("%1 is an invalid %2")
                              .arg(formatData(lexicalQName))
                              .arg(formatType(context->namePool(), BuiltinTypes::xsQName)),
                           InvalidQName,
                           r);
            return QXmlName(); /* Silence compiler warning. */
        }
    }
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
