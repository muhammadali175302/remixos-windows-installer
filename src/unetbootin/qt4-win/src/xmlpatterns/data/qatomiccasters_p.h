/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_AtomicCasters_H
#define Patternist_AtomicCasters_H

#include "qatomiccaster_p.h"
#include "qdecimal_p.h"
#include "qderivedinteger_p.h"
#include "qderivedstring_p.h"
#include "qinteger_p.h"
#include "qvalidationerror_p.h"

/**
 * @file
 * @short Contains classes sub-classing AtomicCaster and which
 * are responsible of casting an atomic value to another type.
 */

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{

    /**
     * @short Casts any atomic value to @c xs:string.
     *
     * This class uses Item::stringValue() for retrieving a string
     * representation, and thus supports casting from atomic values
     * of any type.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template<TypeOfDerivedString DerivedType>
    class ToStringCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const
        {
            Q_ASSERT(from);
            return DerivedString<DerivedType>::fromLexical(context->namePool(), from.stringValue());
        }
    };

    /**
     * @short Casts any atomic value to @c xs:untypedAtomic.
     *
     * This class uses Item::stringValue() for retrieving a string
     * representation, and thus supports casting from atomic values
     * of any type. The implementation is similar to ToStringCaster.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class ToUntypedAtomicCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a string value to @c xs:anyURI.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class ToAnyURICaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:hexBinary atomic value to @c xs:base64Binary.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class HexBinaryToBase64BinaryCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:base64Binary atomic value to @c xs:hexBinary.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class Base64BinaryToHexBinaryCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:base64Binary.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToBase64BinaryCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:hexBinary.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToHexBinaryCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts any @c numeric value to @c xs:boolean.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class NumericToBooleanCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts any string value, @c xs:string or @c xs:untypedAtomic, to @c xs:boolean.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToBooleanCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c numeric value, such as @c xs:double or @c xs:decimal, to @c xs:integer or
     * @c xs:decimal, depending on IsInteger.
     *
     * castFrom() uses Numeric::toInteger() for doing the actual casting.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template <const bool IsInteger>
    class NumericToDecimalCaster : public AtomicCaster
    {
    public:
        /**
         * Used by NumericToDerivedIntegerCaster in addition to this class.
         */
        static inline QString errorMessage()
        {
            return QtXmlPatterns::tr("When casting to %1 from %2, the source value cannot be %3.");
        }

        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const
        {
            const ItemType::Ptr t(from.type());
            const Numeric *const num = from.template as<Numeric>();

            if(BuiltinTypes::xsDouble->xdtTypeMatches(t) || BuiltinTypes::xsFloat->xdtTypeMatches(t))
            {
                if(num->isInf() || num->isNaN())
                {
                    return ValidationError::createError(errorMessage()
                                                        .arg(formatType(context->namePool(), IsInteger ? BuiltinTypes::xsInteger : BuiltinTypes::xsDecimal))
                                                        .arg(formatType(context->namePool(), t))
                                                        .arg(formatData(num->stringValue())),
                                                        ReportContext::FOCA0002);
                }
            }

            if(IsInteger)
                return Integer::fromValue(num->toInteger());
            else
                return toItem(Decimal::fromValue(num->toDecimal()));
        }
    };

    /**
     * @short Casts a string value, @c xs:string or @c xs:untypedAtomic, to @c xs:decimal.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToDecimalCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a string value, @c xs:string or @c xs:untypedAtomic, to @c xs:integer.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToIntegerCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a value of type @c xs:boolean to @c xs:decimal.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class BooleanToDecimalCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a value of type @c xs:boolean to @c xs:integer.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class BooleanToIntegerCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a value to itself. Essentially, this AtomicCaster does nothing.
     *
     * Casting a value to the type of itself is defined to be a noop,
     * no operation. When it can be statically detected that will be done,
     * CastAs rewrites itself appropriately during compilation, but
     * in some cases insufficent data is available at compile time and then
     * is this class need on a case-per-case base at evaluation time.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class SelfToSelfCaster : public AtomicCaster
    {
    public:

        /**
         * This function simply returns @p from.
         */
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:gYear.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToGYearCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:gDay.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToGDayCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:gMonth.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToGMonthCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:gYearMonth.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToGYearMonthCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:gYearMonth.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToGMonthDayCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:dateTime.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToDateTimeCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:time.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToTimeCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:date.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToDateCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:duration.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToDurationCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:dayTimeDuration.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToDayTimeDurationCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:string or @c xs:untypedAtomic atomic value to @c xs:yearMonthDuration.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class StringToYearMonthDurationCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };


    /**
     * @short Casts a @c xs:date or @c xs:dateTime atomic value to @c xs:gYear.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDateTimeToGYearCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:date or @c xs:dateTime atomic value to @c xs:gYearMonth.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDateTimeToGYearMonthCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:date or @c xs:dateTime atomic value to @c xs:gMonth.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDateTimeToGMonthCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:date or @c xs:dateTime atomic value to @c xs:gMonthDay.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDateTimeToGMonthDayCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts a @c xs:date or @c xs:dateTime atomic value to @c xs:gDay.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDateTimeToGDayCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts an AbstractDateTime instance to DateTime.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDateTimeToDateTimeCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts an AbstractDateTime instance to SchemaTime.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDateTimeToDateCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts an AbstractDateTime instance to SchemaTime.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDateTimeToTimeCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts an AbstractDuration instance to Duration.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDurationToDurationCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts an AbstractDuration instance to DayTimeDuration.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDurationToDayTimeDurationCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts an AbstractDuration instance to YearMonthDuration.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    class AbstractDurationToYearMonthDurationCaster : public AtomicCaster
    {
    public:
        virtual Item castFrom(const Item &from,
                              const QExplicitlySharedDataPointer<DynamicContext> &context) const;
    };

    /**
     * @short Casts an @c xs:string instance to a derived type of @c xs:integer.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template<TypeOfDerivedInteger type>
    class StringToDerivedIntegerCaster : public AtomicCaster
    {
    public:
        virtual Item
        castFrom(const Item &from,
                 const QExplicitlySharedDataPointer<DynamicContext> &context) const
        {
            return DerivedInteger<type>::fromLexical(context->namePool(), from.stringValue());
        }
    };

    /**
     * @short Casts an @c xs:boolean instance to a derived type of @c xs:integer.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template<TypeOfDerivedInteger type>
    class BooleanToDerivedIntegerCaster : public AtomicCaster
    {
    public:
        virtual Item
        castFrom(const Item &from,
                 const QExplicitlySharedDataPointer<DynamicContext> &context) const
        {
            return DerivedInteger<type>::fromValue(context->namePool(), from.template as<AtomicValue>()->evaluateEBV(context) ? 1 : 0);
        }
    };

    /**
     * @short Casts an @c xs:boolean instance to a derived type of @c xs:integer.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template<TypeOfDerivedString type>
    class AnyToDerivedStringCaster : public AtomicCaster
    {
    public:
        virtual Item
        castFrom(const Item &from,
                 const QExplicitlySharedDataPointer<DynamicContext> &context) const
        {
            return DerivedString<type>::fromLexical(context->namePool(), from.stringValue());
        }
    };

    /**
     * @short Casts any @c numeric instance to a derived type of @c xs:integer.
     *
     * @ingroup Patternist_xdm
     * @author Frans Englich <fenglich@trolltech.com>
     */
    template<TypeOfDerivedInteger type>
    class NumericToDerivedIntegerCaster : public AtomicCaster
    {
    public:
        virtual Item
        castFrom(const Item &from,
                 const QExplicitlySharedDataPointer<DynamicContext> &context) const
        {
            const ItemType::Ptr t(from.type());
            const Numeric *const num = from.template as<Numeric>();

            if(BuiltinTypes::xsDouble->xdtTypeMatches(t) || BuiltinTypes::xsFloat->xdtTypeMatches(t))
            {
                if(num->isInf() || num->isNaN())
                {
                    return ValidationError::createError(NumericToDecimalCaster<false>::errorMessage()
                                                        .arg(formatType(context->namePool(), DerivedInteger<type>::itemType()))
                                                        .arg(formatType(context->namePool(), t))
                                                        .arg(formatData(num->stringValue())));
                }
            }

            return toItem(DerivedInteger<type>::fromValue(context->namePool(), from.template as<Numeric>()->toInteger()));
        }
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
