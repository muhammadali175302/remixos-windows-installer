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


#if ENABLE(SVG) && ENABLE(SVG_FONTS)

#include "SVGElement.h"
#include "JSSVGAltGlyphElement.h"

#include <wtf/GetPtr.h>

#include "JSSVGAnimatedString.h"
#include "KURL.h"
#include "SVGAltGlyphElement.h"

#include <runtime/JSString.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSSVGAltGlyphElement)

/* Hash table */

static const HashTableValue JSSVGAltGlyphElementTableValues[4] =
{
    { "glyphRef", DontDelete, (intptr_t)jsSVGAltGlyphElementGlyphRef, (intptr_t)setJSSVGAltGlyphElementGlyphRef },
    { "format", DontDelete, (intptr_t)jsSVGAltGlyphElementFormat, (intptr_t)setJSSVGAltGlyphElementFormat },
    { "href", DontDelete|ReadOnly, (intptr_t)jsSVGAltGlyphElementHref, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSSVGAltGlyphElementTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 3, JSSVGAltGlyphElementTableValues, 0 };
#else
    { 8, 7, JSSVGAltGlyphElementTableValues, 0 };
#endif

/* Hash table for prototype */

static const HashTableValue JSSVGAltGlyphElementPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSSVGAltGlyphElementPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGAltGlyphElementPrototypeTableValues, 0 };
#else
    { 1, 0, JSSVGAltGlyphElementPrototypeTableValues, 0 };
#endif

const ClassInfo JSSVGAltGlyphElementPrototype::s_info = { "SVGAltGlyphElementPrototype", 0, &JSSVGAltGlyphElementPrototypeTable, 0 };

JSObject* JSSVGAltGlyphElementPrototype::self(ExecState* exec)
{
    return getDOMPrototype<JSSVGAltGlyphElement>(exec);
}

const ClassInfo JSSVGAltGlyphElement::s_info = { "SVGAltGlyphElement", &JSSVGTextPositioningElement::s_info, &JSSVGAltGlyphElementTable, 0 };

JSSVGAltGlyphElement::JSSVGAltGlyphElement(PassRefPtr<Structure> structure, PassRefPtr<SVGAltGlyphElement> impl)
    : JSSVGTextPositioningElement(structure, impl)
{
}

JSObject* JSSVGAltGlyphElement::createPrototype(ExecState* exec)
{
    return new (exec) JSSVGAltGlyphElementPrototype(JSSVGAltGlyphElementPrototype::createStructure(JSSVGTextPositioningElementPrototype::self(exec)));
}

bool JSSVGAltGlyphElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGAltGlyphElement, Base>(exec, &JSSVGAltGlyphElementTable, this, propertyName, slot);
}

JSValuePtr jsSVGAltGlyphElementGlyphRef(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    SVGAltGlyphElement* imp = static_cast<SVGAltGlyphElement*>(static_cast<JSSVGAltGlyphElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->glyphRef());
}

JSValuePtr jsSVGAltGlyphElementFormat(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    SVGAltGlyphElement* imp = static_cast<SVGAltGlyphElement*>(static_cast<JSSVGAltGlyphElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->format());
}

JSValuePtr jsSVGAltGlyphElementHref(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    SVGAltGlyphElement* imp = static_cast<SVGAltGlyphElement*>(static_cast<JSSVGAltGlyphElement*>(asObject(slot.slotBase()))->impl());
    RefPtr<SVGAnimatedString> obj = imp->hrefAnimated();
    return toJS(exec, obj.get(), imp);
}

void JSSVGAltGlyphElement::put(ExecState* exec, const Identifier& propertyName, JSValuePtr value, PutPropertySlot& slot)
{
    lookupPut<JSSVGAltGlyphElement, Base>(exec, propertyName, value, &JSSVGAltGlyphElementTable, this, slot);
}

void setJSSVGAltGlyphElementGlyphRef(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    SVGAltGlyphElement* imp = static_cast<SVGAltGlyphElement*>(static_cast<JSSVGAltGlyphElement*>(thisObject)->impl());
    ExceptionCode ec = 0;
    imp->setGlyphRef(value->toString(exec), ec);
    setDOMException(exec, ec);
}

void setJSSVGAltGlyphElementFormat(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    SVGAltGlyphElement* imp = static_cast<SVGAltGlyphElement*>(static_cast<JSSVGAltGlyphElement*>(thisObject)->impl());
    ExceptionCode ec = 0;
    imp->setFormat(value->toString(exec), ec);
    setDOMException(exec, ec);
}


}

#endif // ENABLE(SVG) && ENABLE(SVG_FONTS)
