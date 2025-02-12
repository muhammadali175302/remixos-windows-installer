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

#include "JSRect.h"

#include <wtf/GetPtr.h>

#include "CSSPrimitiveValue.h"
#include "JSCSSPrimitiveValue.h"
#include "Rect.h"

#include <runtime/JSNumberCell.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSRect)

/* Hash table */

static const HashTableValue JSRectTableValues[6] =
{
    { "top", DontDelete|ReadOnly, (intptr_t)jsRectTop, (intptr_t)0 },
    { "right", DontDelete|ReadOnly, (intptr_t)jsRectRight, (intptr_t)0 },
    { "bottom", DontDelete|ReadOnly, (intptr_t)jsRectBottom, (intptr_t)0 },
    { "left", DontDelete|ReadOnly, (intptr_t)jsRectLeft, (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsRectConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSRectTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 7, JSRectTableValues, 0 };
#else
    { 16, 15, JSRectTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSRectConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSRectConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSRectConstructorTableValues, 0 };
#else
    { 1, 0, JSRectConstructorTableValues, 0 };
#endif

class JSRectConstructor : public DOMObject {
public:
    JSRectConstructor(ExecState* exec)
        : DOMObject(JSRectConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSRectPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSRectConstructor::s_info = { "RectConstructor", 0, &JSRectConstructorTable, 0 };

bool JSRectConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSRectConstructor, DOMObject>(exec, &JSRectConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSRectPrototypeTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSRectPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSRectPrototypeTableValues, 0 };
#else
    { 1, 0, JSRectPrototypeTableValues, 0 };
#endif

const ClassInfo JSRectPrototype::s_info = { "RectPrototype", 0, &JSRectPrototypeTable, 0 };

JSObject* JSRectPrototype::self(ExecState* exec)
{
    return getDOMPrototype<JSRect>(exec);
}

const ClassInfo JSRect::s_info = { "Rect", 0, &JSRectTable, 0 };

JSRect::JSRect(PassRefPtr<Structure> structure, PassRefPtr<Rect> impl)
    : DOMObject(structure)
    , m_impl(impl)
{
}

JSRect::~JSRect()
{
    forgetDOMObject(*Heap::heap(this)->globalData(), m_impl.get());

}

JSObject* JSRect::createPrototype(ExecState* exec)
{
    return new (exec) JSRectPrototype(JSRectPrototype::createStructure(exec->lexicalGlobalObject()->objectPrototype()));
}

bool JSRect::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSRect, Base>(exec, &JSRectTable, this, propertyName, slot);
}

JSValuePtr jsRectTop(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    Rect* imp = static_cast<Rect*>(static_cast<JSRect*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->top()));
}

JSValuePtr jsRectRight(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    Rect* imp = static_cast<Rect*>(static_cast<JSRect*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->right()));
}

JSValuePtr jsRectBottom(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    Rect* imp = static_cast<Rect*>(static_cast<JSRect*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->bottom()));
}

JSValuePtr jsRectLeft(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    Rect* imp = static_cast<Rect*>(static_cast<JSRect*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->left()));
}

JSValuePtr jsRectConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSRect*>(asObject(slot.slotBase()))->getConstructor(exec);
}
JSValuePtr JSRect::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSRectConstructor>(exec);
}

JSC::JSValuePtr toJS(JSC::ExecState* exec, Rect* object)
{
    return getDOMObjectWrapper<JSRect>(exec, object);
}
Rect* toRect(JSC::JSValuePtr value)
{
    return value->isObject(&JSRect::s_info) ? static_cast<JSRect*>(asObject(value))->impl() : 0;
}

}
