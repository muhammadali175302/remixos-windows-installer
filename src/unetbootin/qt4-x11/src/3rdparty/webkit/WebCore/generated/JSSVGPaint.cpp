/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"


#if ENABLE(SVG)

#include "SVGElement.h"
#include "JSSVGPaint.h"

#include <wtf/GetPtr.h>

#include "KURL.h"
#include "SVGPaint.h"

#include <runtime/Error.h>
#include <runtime/JSNumberCell.h>
#include <runtime/JSString.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSSVGPaint)

/* Hash table */

static const HashTableValue JSSVGPaintTableValues[4] =
{
    { "paintType", DontDelete|ReadOnly, (intptr_t)jsSVGPaintPaintType, (intptr_t)0 },
    { "uri", DontDelete|ReadOnly, (intptr_t)jsSVGPaintUri, (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsSVGPaintConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSSVGPaintTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 31, JSSVGPaintTableValues, 0 };
#else
    { 9, 7, JSSVGPaintTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSSVGPaintConstructorTableValues[11] =
{
    { "SVG_PAINTTYPE_UNKNOWN", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_UNKNOWN, (intptr_t)0 },
    { "SVG_PAINTTYPE_RGBCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_RGBCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_RGBCOLOR_ICCCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_RGBCOLOR_ICCCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_NONE", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_NONE, (intptr_t)0 },
    { "SVG_PAINTTYPE_CURRENTCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_CURRENTCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_URI_NONE", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_URI_NONE, (intptr_t)0 },
    { "SVG_PAINTTYPE_URI_CURRENTCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_URI_CURRENTCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_URI_RGBCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_URI_RGBCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_URI", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_URI, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSSVGPaintConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 4095, JSSVGPaintConstructorTableValues, 0 };
#else
    { 35, 31, JSSVGPaintConstructorTableValues, 0 };
#endif

class JSSVGPaintConstructor : public DOMObject {
public:
    JSSVGPaintConstructor(ExecState* exec)
        : DOMObject(JSSVGPaintConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSSVGPaintPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSSVGPaintConstructor::s_info = { "SVGPaintConstructor", 0, &JSSVGPaintConstructorTable, 0 };

bool JSSVGPaintConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGPaintConstructor, DOMObject>(exec, &JSSVGPaintConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSSVGPaintPrototypeTableValues[13] =
{
    { "SVG_PAINTTYPE_UNKNOWN", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_UNKNOWN, (intptr_t)0 },
    { "SVG_PAINTTYPE_RGBCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_RGBCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_RGBCOLOR_ICCCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_RGBCOLOR_ICCCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_NONE", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_NONE, (intptr_t)0 },
    { "SVG_PAINTTYPE_CURRENTCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_CURRENTCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_URI_NONE", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_URI_NONE, (intptr_t)0 },
    { "SVG_PAINTTYPE_URI_CURRENTCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_URI_CURRENTCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_URI_RGBCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_URI_RGBCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR, (intptr_t)0 },
    { "SVG_PAINTTYPE_URI", DontDelete|ReadOnly, (intptr_t)jsSVGPaintSVG_PAINTTYPE_URI, (intptr_t)0 },
    { "setUri", DontDelete|Function, (intptr_t)jsSVGPaintPrototypeFunctionSetUri, (intptr_t)1 },
    { "setPaint", DontDelete|Function, (intptr_t)jsSVGPaintPrototypeFunctionSetPaint, (intptr_t)4 },
    { 0, 0, 0, 0 }
};

static const HashTable JSSVGPaintPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 4095, JSSVGPaintPrototypeTableValues, 0 };
#else
    { 35, 31, JSSVGPaintPrototypeTableValues, 0 };
#endif

const ClassInfo JSSVGPaintPrototype::s_info = { "SVGPaintPrototype", 0, &JSSVGPaintPrototypeTable, 0 };

JSObject* JSSVGPaintPrototype::self(ExecState* exec)
{
    return getDOMPrototype<JSSVGPaint>(exec);
}

bool JSSVGPaintPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticPropertySlot<JSSVGPaintPrototype, JSObject>(exec, &JSSVGPaintPrototypeTable, this, propertyName, slot);
}

const ClassInfo JSSVGPaint::s_info = { "SVGPaint", &JSSVGColor::s_info, &JSSVGPaintTable, 0 };

JSSVGPaint::JSSVGPaint(PassRefPtr<Structure> structure, PassRefPtr<SVGPaint> impl)
    : JSSVGColor(structure, impl)
{
}

JSObject* JSSVGPaint::createPrototype(ExecState* exec)
{
    return new (exec) JSSVGPaintPrototype(JSSVGPaintPrototype::createStructure(JSSVGColorPrototype::self(exec)));
}

bool JSSVGPaint::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGPaint, Base>(exec, &JSSVGPaintTable, this, propertyName, slot);
}

JSValuePtr jsSVGPaintPaintType(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    SVGPaint* imp = static_cast<SVGPaint*>(static_cast<JSSVGPaint*>(asObject(slot.slotBase()))->impl());
    return jsNumber(exec, imp->paintType());
}

JSValuePtr jsSVGPaintUri(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    SVGPaint* imp = static_cast<SVGPaint*>(static_cast<JSSVGPaint*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->uri());
}

JSValuePtr jsSVGPaintConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSSVGPaint*>(asObject(slot.slotBase()))->getConstructor(exec);
}
JSValuePtr JSSVGPaint::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSSVGPaintConstructor>(exec);
}

JSValuePtr jsSVGPaintPrototypeFunctionSetUri(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSSVGPaint::s_info))
        return throwError(exec, TypeError);
    JSSVGPaint* castedThisObj = static_cast<JSSVGPaint*>(asObject(thisValue));
    SVGPaint* imp = static_cast<SVGPaint*>(castedThisObj->impl());
    const UString& uri = args.at(exec, 0)->toString(exec);

    imp->setUri(uri);
    return jsUndefined();
}

JSValuePtr jsSVGPaintPrototypeFunctionSetPaint(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSSVGPaint::s_info))
        return throwError(exec, TypeError);
    JSSVGPaint* castedThisObj = static_cast<JSSVGPaint*>(asObject(thisValue));
    SVGPaint* imp = static_cast<SVGPaint*>(castedThisObj->impl());
    ExceptionCode ec = 0;
    SVGPaint::SVGPaintType paintType = static_cast<SVGPaint::SVGPaintType>(args.at(exec, 0)->toInt32(exec));
    const UString& uri = args.at(exec, 1)->toString(exec);
    const UString& rgbColor = args.at(exec, 2)->toString(exec);
    const UString& iccColor = args.at(exec, 3)->toString(exec);

    imp->setPaint(paintType, uri, rgbColor, iccColor, ec);
    setDOMException(exec, ec);
    return jsUndefined();
}

// Constant getters

JSValuePtr jsSVGPaintSVG_PAINTTYPE_UNKNOWN(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(0));
}

JSValuePtr jsSVGPaintSVG_PAINTTYPE_RGBCOLOR(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(1));
}

JSValuePtr jsSVGPaintSVG_PAINTTYPE_RGBCOLOR_ICCCOLOR(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(2));
}

JSValuePtr jsSVGPaintSVG_PAINTTYPE_NONE(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(101));
}

JSValuePtr jsSVGPaintSVG_PAINTTYPE_CURRENTCOLOR(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(102));
}

JSValuePtr jsSVGPaintSVG_PAINTTYPE_URI_NONE(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(103));
}

JSValuePtr jsSVGPaintSVG_PAINTTYPE_URI_CURRENTCOLOR(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(104));
}

JSValuePtr jsSVGPaintSVG_PAINTTYPE_URI_RGBCOLOR(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(105));
}

JSValuePtr jsSVGPaintSVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(106));
}

JSValuePtr jsSVGPaintSVG_PAINTTYPE_URI(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(107));
}


}

#endif // ENABLE(SVG)
