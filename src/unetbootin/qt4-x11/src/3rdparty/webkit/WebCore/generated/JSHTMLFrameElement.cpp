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

#include "JSHTMLFrameElement.h"

#include <wtf/GetPtr.h>

#include "DOMWindow.h"
#include "Document.h"
#include "HTMLFrameElement.h"
#include "JSDOMBinding.h"
#include "JSDOMWindow.h"
#include "JSDocument.h"
#include "JSSVGDocument.h"
#include "KURL.h"
#include "SVGDocument.h"

#include <runtime/Error.h>
#include <runtime/JSNumberCell.h>
#include <runtime/JSString.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSHTMLFrameElement)

/* Hash table */

static const HashTableValue JSHTMLFrameElementTableValues[15] =
{
    { "frameBorder", DontDelete, (intptr_t)jsHTMLFrameElementFrameBorder, (intptr_t)setJSHTMLFrameElementFrameBorder },
    { "longDesc", DontDelete, (intptr_t)jsHTMLFrameElementLongDesc, (intptr_t)setJSHTMLFrameElementLongDesc },
    { "marginHeight", DontDelete, (intptr_t)jsHTMLFrameElementMarginHeight, (intptr_t)setJSHTMLFrameElementMarginHeight },
    { "marginWidth", DontDelete, (intptr_t)jsHTMLFrameElementMarginWidth, (intptr_t)setJSHTMLFrameElementMarginWidth },
    { "name", DontDelete, (intptr_t)jsHTMLFrameElementName, (intptr_t)setJSHTMLFrameElementName },
    { "noResize", DontDelete, (intptr_t)jsHTMLFrameElementNoResize, (intptr_t)setJSHTMLFrameElementNoResize },
    { "scrolling", DontDelete, (intptr_t)jsHTMLFrameElementScrolling, (intptr_t)setJSHTMLFrameElementScrolling },
    { "src", DontDelete, (intptr_t)jsHTMLFrameElementSrc, (intptr_t)setJSHTMLFrameElementSrc },
    { "contentDocument", DontDelete|ReadOnly, (intptr_t)jsHTMLFrameElementContentDocument, (intptr_t)0 },
    { "contentWindow", DontDelete|ReadOnly, (intptr_t)jsHTMLFrameElementContentWindow, (intptr_t)0 },
    { "location", DontDelete, (intptr_t)jsHTMLFrameElementLocation, (intptr_t)setJSHTMLFrameElementLocation },
    { "width", DontDelete|ReadOnly, (intptr_t)jsHTMLFrameElementWidth, (intptr_t)0 },
    { "height", DontDelete|ReadOnly, (intptr_t)jsHTMLFrameElementHeight, (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsHTMLFrameElementConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLFrameElementTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 16383, JSHTMLFrameElementTableValues, 0 };
#else
    { 36, 31, JSHTMLFrameElementTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSHTMLFrameElementConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLFrameElementConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSHTMLFrameElementConstructorTableValues, 0 };
#else
    { 1, 0, JSHTMLFrameElementConstructorTableValues, 0 };
#endif

class JSHTMLFrameElementConstructor : public DOMObject {
public:
    JSHTMLFrameElementConstructor(ExecState* exec)
        : DOMObject(JSHTMLFrameElementConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSHTMLFrameElementPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSHTMLFrameElementConstructor::s_info = { "HTMLFrameElementConstructor", 0, &JSHTMLFrameElementConstructorTable, 0 };

bool JSHTMLFrameElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLFrameElementConstructor, DOMObject>(exec, &JSHTMLFrameElementConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSHTMLFrameElementPrototypeTableValues[2] =
{
    { "getSVGDocument", DontDelete|Function, (intptr_t)jsHTMLFrameElementPrototypeFunctionGetSVGDocument, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSHTMLFrameElementPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSHTMLFrameElementPrototypeTableValues, 0 };
#else
    { 2, 1, JSHTMLFrameElementPrototypeTableValues, 0 };
#endif

const ClassInfo JSHTMLFrameElementPrototype::s_info = { "HTMLFrameElementPrototype", 0, &JSHTMLFrameElementPrototypeTable, 0 };

JSObject* JSHTMLFrameElementPrototype::self(ExecState* exec)
{
    return getDOMPrototype<JSHTMLFrameElement>(exec);
}

bool JSHTMLFrameElementPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSHTMLFrameElementPrototypeTable, this, propertyName, slot);
}

const ClassInfo JSHTMLFrameElement::s_info = { "HTMLFrameElement", &JSHTMLElement::s_info, &JSHTMLFrameElementTable, 0 };

JSHTMLFrameElement::JSHTMLFrameElement(PassRefPtr<Structure> structure, PassRefPtr<HTMLFrameElement> impl)
    : JSHTMLElement(structure, impl)
{
}

JSObject* JSHTMLFrameElement::createPrototype(ExecState* exec)
{
    return new (exec) JSHTMLFrameElementPrototype(JSHTMLFrameElementPrototype::createStructure(JSHTMLElementPrototype::self(exec)));
}

bool JSHTMLFrameElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLFrameElement, Base>(exec, &JSHTMLFrameElementTable, this, propertyName, slot);
}

JSValuePtr jsHTMLFrameElementFrameBorder(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->frameBorder());
}

JSValuePtr jsHTMLFrameElementLongDesc(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->longDesc());
}

JSValuePtr jsHTMLFrameElementMarginHeight(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->marginHeight());
}

JSValuePtr jsHTMLFrameElementMarginWidth(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->marginWidth());
}

JSValuePtr jsHTMLFrameElementName(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->name());
}

JSValuePtr jsHTMLFrameElementNoResize(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsBoolean(imp->noResize());
}

JSValuePtr jsHTMLFrameElementScrolling(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->scrolling());
}

JSValuePtr jsHTMLFrameElementSrc(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->src());
}

JSValuePtr jsHTMLFrameElementContentDocument(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return checkNodeSecurity(exec, imp->contentDocument()) ? toJS(exec, WTF::getPtr(imp->contentDocument())) : jsUndefined();
}

JSValuePtr jsHTMLFrameElementContentWindow(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->contentWindow()));
}

JSValuePtr jsHTMLFrameElementLocation(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->location());
}

JSValuePtr jsHTMLFrameElementWidth(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsNumber(exec, imp->width());
}

JSValuePtr jsHTMLFrameElementHeight(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->impl());
    return jsNumber(exec, imp->height());
}

JSValuePtr jsHTMLFrameElementConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSHTMLFrameElement*>(asObject(slot.slotBase()))->getConstructor(exec);
}
void JSHTMLFrameElement::put(ExecState* exec, const Identifier& propertyName, JSValuePtr value, PutPropertySlot& slot)
{
    lookupPut<JSHTMLFrameElement, Base>(exec, propertyName, value, &JSHTMLFrameElementTable, this, slot);
}

void setJSHTMLFrameElementFrameBorder(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(thisObject)->impl());
    imp->setFrameBorder(valueToStringWithNullCheck(exec, value));
}

void setJSHTMLFrameElementLongDesc(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(thisObject)->impl());
    imp->setLongDesc(valueToStringWithNullCheck(exec, value));
}

void setJSHTMLFrameElementMarginHeight(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(thisObject)->impl());
    imp->setMarginHeight(valueToStringWithNullCheck(exec, value));
}

void setJSHTMLFrameElementMarginWidth(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(thisObject)->impl());
    imp->setMarginWidth(valueToStringWithNullCheck(exec, value));
}

void setJSHTMLFrameElementName(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(thisObject)->impl());
    imp->setName(valueToStringWithNullCheck(exec, value));
}

void setJSHTMLFrameElementNoResize(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(thisObject)->impl());
    imp->setNoResize(value->toBoolean(exec));
}

void setJSHTMLFrameElementScrolling(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(static_cast<JSHTMLFrameElement*>(thisObject)->impl());
    imp->setScrolling(valueToStringWithNullCheck(exec, value));
}

void setJSHTMLFrameElementSrc(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    static_cast<JSHTMLFrameElement*>(thisObject)->setSrc(exec, value);
}

void setJSHTMLFrameElementLocation(ExecState* exec, JSObject* thisObject, JSValuePtr value)
{
    static_cast<JSHTMLFrameElement*>(thisObject)->setLocation(exec, value);
}

JSValuePtr JSHTMLFrameElement::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSHTMLFrameElementConstructor>(exec);
}

JSValuePtr jsHTMLFrameElementPrototypeFunctionGetSVGDocument(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSHTMLFrameElement::s_info))
        return throwError(exec, TypeError);
    JSHTMLFrameElement* castedThisObj = static_cast<JSHTMLFrameElement*>(asObject(thisValue));
    HTMLFrameElement* imp = static_cast<HTMLFrameElement*>(castedThisObj->impl());
    ExceptionCode ec = 0;
    if (!checkNodeSecurity(exec, imp->getSVGDocument(ec)))
        return jsUndefined();


    JSC::JSValuePtr result = toJS(exec, WTF::getPtr(imp->getSVGDocument(ec)));
    setDOMException(exec, ec);
    return result;
}


}
