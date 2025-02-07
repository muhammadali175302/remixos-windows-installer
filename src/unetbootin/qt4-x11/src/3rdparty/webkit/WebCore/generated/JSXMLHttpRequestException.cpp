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

#include "JSXMLHttpRequestException.h"

#include <wtf/GetPtr.h>

#include "KURL.h"
#include "XMLHttpRequestException.h"

#include <runtime/Error.h>
#include <runtime/JSNumberCell.h>
#include <runtime/JSString.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSXMLHttpRequestException)

/* Hash table */

static const HashTableValue JSXMLHttpRequestExceptionTableValues[5] =
{
    { "code", DontDelete|ReadOnly, (intptr_t)jsXMLHttpRequestExceptionCode, (intptr_t)0 },
    { "name", DontDelete|ReadOnly, (intptr_t)jsXMLHttpRequestExceptionName, (intptr_t)0 },
    { "message", DontDelete|ReadOnly, (intptr_t)jsXMLHttpRequestExceptionMessage, (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsXMLHttpRequestExceptionConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSXMLHttpRequestExceptionTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 127, JSXMLHttpRequestExceptionTableValues, 0 };
#else
    { 10, 7, JSXMLHttpRequestExceptionTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSXMLHttpRequestExceptionConstructorTableValues[3] =
{
    { "NETWORK_ERR", DontDelete|ReadOnly, (intptr_t)jsXMLHttpRequestExceptionNETWORK_ERR, (intptr_t)0 },
    { "ABORT_ERR", DontDelete|ReadOnly, (intptr_t)jsXMLHttpRequestExceptionABORT_ERR, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSXMLHttpRequestExceptionConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 1, JSXMLHttpRequestExceptionConstructorTableValues, 0 };
#else
    { 4, 3, JSXMLHttpRequestExceptionConstructorTableValues, 0 };
#endif

class JSXMLHttpRequestExceptionConstructor : public DOMObject {
public:
    JSXMLHttpRequestExceptionConstructor(ExecState* exec)
        : DOMObject(JSXMLHttpRequestExceptionConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSXMLHttpRequestExceptionPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSXMLHttpRequestExceptionConstructor::s_info = { "XMLHttpRequestExceptionConstructor", 0, &JSXMLHttpRequestExceptionConstructorTable, 0 };

bool JSXMLHttpRequestExceptionConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSXMLHttpRequestExceptionConstructor, DOMObject>(exec, &JSXMLHttpRequestExceptionConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSXMLHttpRequestExceptionPrototypeTableValues[4] =
{
    { "NETWORK_ERR", DontDelete|ReadOnly, (intptr_t)jsXMLHttpRequestExceptionNETWORK_ERR, (intptr_t)0 },
    { "ABORT_ERR", DontDelete|ReadOnly, (intptr_t)jsXMLHttpRequestExceptionABORT_ERR, (intptr_t)0 },
    { "toString", DontDelete|DontEnum|Function, (intptr_t)jsXMLHttpRequestExceptionPrototypeFunctionToString, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSXMLHttpRequestExceptionPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 3, JSXMLHttpRequestExceptionPrototypeTableValues, 0 };
#else
    { 8, 7, JSXMLHttpRequestExceptionPrototypeTableValues, 0 };
#endif

const ClassInfo JSXMLHttpRequestExceptionPrototype::s_info = { "XMLHttpRequestExceptionPrototype", 0, &JSXMLHttpRequestExceptionPrototypeTable, 0 };

JSObject* JSXMLHttpRequestExceptionPrototype::self(ExecState* exec)
{
    return getDOMPrototype<JSXMLHttpRequestException>(exec);
}

bool JSXMLHttpRequestExceptionPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticPropertySlot<JSXMLHttpRequestExceptionPrototype, JSObject>(exec, &JSXMLHttpRequestExceptionPrototypeTable, this, propertyName, slot);
}

const ClassInfo JSXMLHttpRequestException::s_info = { "XMLHttpRequestException", 0, &JSXMLHttpRequestExceptionTable, 0 };

JSXMLHttpRequestException::JSXMLHttpRequestException(PassRefPtr<Structure> structure, PassRefPtr<XMLHttpRequestException> impl)
    : DOMObject(structure)
    , m_impl(impl)
{
}

JSXMLHttpRequestException::~JSXMLHttpRequestException()
{
    forgetDOMObject(*Heap::heap(this)->globalData(), m_impl.get());

}

JSObject* JSXMLHttpRequestException::createPrototype(ExecState* exec)
{
    return new (exec) JSXMLHttpRequestExceptionPrototype(JSXMLHttpRequestExceptionPrototype::createStructure(exec->lexicalGlobalObject()->objectPrototype()));
}

bool JSXMLHttpRequestException::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSXMLHttpRequestException, Base>(exec, &JSXMLHttpRequestExceptionTable, this, propertyName, slot);
}

JSValuePtr jsXMLHttpRequestExceptionCode(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    XMLHttpRequestException* imp = static_cast<XMLHttpRequestException*>(static_cast<JSXMLHttpRequestException*>(asObject(slot.slotBase()))->impl());
    return jsNumber(exec, imp->code());
}

JSValuePtr jsXMLHttpRequestExceptionName(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    XMLHttpRequestException* imp = static_cast<XMLHttpRequestException*>(static_cast<JSXMLHttpRequestException*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->name());
}

JSValuePtr jsXMLHttpRequestExceptionMessage(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    XMLHttpRequestException* imp = static_cast<XMLHttpRequestException*>(static_cast<JSXMLHttpRequestException*>(asObject(slot.slotBase()))->impl());
    return jsString(exec, imp->message());
}

JSValuePtr jsXMLHttpRequestExceptionConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSXMLHttpRequestException*>(asObject(slot.slotBase()))->getConstructor(exec);
}
JSValuePtr JSXMLHttpRequestException::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSXMLHttpRequestExceptionConstructor>(exec);
}

JSValuePtr jsXMLHttpRequestExceptionPrototypeFunctionToString(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSXMLHttpRequestException::s_info))
        return throwError(exec, TypeError);
    JSXMLHttpRequestException* castedThisObj = static_cast<JSXMLHttpRequestException*>(asObject(thisValue));
    XMLHttpRequestException* imp = static_cast<XMLHttpRequestException*>(castedThisObj->impl());


    JSC::JSValuePtr result = jsString(exec, imp->toString());
    return result;
}

// Constant getters

JSValuePtr jsXMLHttpRequestExceptionNETWORK_ERR(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(101));
}

JSValuePtr jsXMLHttpRequestExceptionABORT_ERR(ExecState* exec, const Identifier&, const PropertySlot&)
{
    return jsNumber(exec, static_cast<int>(102));
}

JSC::JSValuePtr toJS(JSC::ExecState* exec, XMLHttpRequestException* object)
{
    return getDOMObjectWrapper<JSXMLHttpRequestException>(exec, object);
}
XMLHttpRequestException* toXMLHttpRequestException(JSC::JSValuePtr value)
{
    return value->isObject(&JSXMLHttpRequestException::s_info) ? static_cast<JSXMLHttpRequestException*>(asObject(value))->impl() : 0;
}

}
