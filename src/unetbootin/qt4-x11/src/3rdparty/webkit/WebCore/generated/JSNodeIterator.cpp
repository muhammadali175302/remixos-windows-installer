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

#include "JSNodeIterator.h"

#include <wtf/GetPtr.h>

#include "JSNode.h"
#include "JSNodeFilter.h"
#include "Node.h"
#include "NodeFilter.h"
#include "NodeIterator.h"

#include <runtime/Error.h>
#include <runtime/JSNumberCell.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSNodeIterator)

/* Hash table */

static const HashTableValue JSNodeIteratorTableValues[8] =
{
    { "root", DontDelete|ReadOnly, (intptr_t)jsNodeIteratorRoot, (intptr_t)0 },
    { "whatToShow", DontDelete|ReadOnly, (intptr_t)jsNodeIteratorWhatToShow, (intptr_t)0 },
    { "filter", DontDelete|ReadOnly, (intptr_t)jsNodeIteratorFilter, (intptr_t)0 },
    { "expandEntityReferences", DontDelete|ReadOnly, (intptr_t)jsNodeIteratorExpandEntityReferences, (intptr_t)0 },
    { "referenceNode", DontDelete|ReadOnly, (intptr_t)jsNodeIteratorReferenceNode, (intptr_t)0 },
    { "pointerBeforeReferenceNode", DontDelete|ReadOnly, (intptr_t)jsNodeIteratorPointerBeforeReferenceNode, (intptr_t)0 },
    { "constructor", DontEnum|ReadOnly, (intptr_t)jsNodeIteratorConstructor, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSNodeIteratorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 255, JSNodeIteratorTableValues, 0 };
#else
    { 18, 15, JSNodeIteratorTableValues, 0 };
#endif

/* Hash table for constructor */

static const HashTableValue JSNodeIteratorConstructorTableValues[1] =
{
    { 0, 0, 0, 0 }
};

static const HashTable JSNodeIteratorConstructorTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSNodeIteratorConstructorTableValues, 0 };
#else
    { 1, 0, JSNodeIteratorConstructorTableValues, 0 };
#endif

class JSNodeIteratorConstructor : public DOMObject {
public:
    JSNodeIteratorConstructor(ExecState* exec)
        : DOMObject(JSNodeIteratorConstructor::createStructure(exec->lexicalGlobalObject()->objectPrototype()))
    {
        putDirect(exec->propertyNames().prototype, JSNodeIteratorPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    virtual const ClassInfo* classInfo() const { return &s_info; }
    static const ClassInfo s_info;

    static PassRefPtr<Structure> createStructure(JSValuePtr proto) 
    { 
        return Structure::create(proto, TypeInfo(ObjectType, ImplementsHasInstance)); 
    }
};

const ClassInfo JSNodeIteratorConstructor::s_info = { "NodeIteratorConstructor", 0, &JSNodeIteratorConstructorTable, 0 };

bool JSNodeIteratorConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSNodeIteratorConstructor, DOMObject>(exec, &JSNodeIteratorConstructorTable, this, propertyName, slot);
}

/* Hash table for prototype */

static const HashTableValue JSNodeIteratorPrototypeTableValues[4] =
{
    { "nextNode", DontDelete|Function, (intptr_t)jsNodeIteratorPrototypeFunctionNextNode, (intptr_t)0 },
    { "previousNode", DontDelete|Function, (intptr_t)jsNodeIteratorPrototypeFunctionPreviousNode, (intptr_t)0 },
    { "detach", DontDelete|Function, (intptr_t)jsNodeIteratorPrototypeFunctionDetach, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSNodeIteratorPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 3, JSNodeIteratorPrototypeTableValues, 0 };
#else
    { 8, 7, JSNodeIteratorPrototypeTableValues, 0 };
#endif

const ClassInfo JSNodeIteratorPrototype::s_info = { "NodeIteratorPrototype", 0, &JSNodeIteratorPrototypeTable, 0 };

JSObject* JSNodeIteratorPrototype::self(ExecState* exec)
{
    return getDOMPrototype<JSNodeIterator>(exec);
}

bool JSNodeIteratorPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSNodeIteratorPrototypeTable, this, propertyName, slot);
}

const ClassInfo JSNodeIterator::s_info = { "NodeIterator", 0, &JSNodeIteratorTable, 0 };

JSNodeIterator::JSNodeIterator(PassRefPtr<Structure> structure, PassRefPtr<NodeIterator> impl)
    : DOMObject(structure)
    , m_impl(impl)
{
}

JSNodeIterator::~JSNodeIterator()
{
    forgetDOMObject(*Heap::heap(this)->globalData(), m_impl.get());

}

JSObject* JSNodeIterator::createPrototype(ExecState* exec)
{
    return new (exec) JSNodeIteratorPrototype(JSNodeIteratorPrototype::createStructure(exec->lexicalGlobalObject()->objectPrototype()));
}

bool JSNodeIterator::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSNodeIterator, Base>(exec, &JSNodeIteratorTable, this, propertyName, slot);
}

JSValuePtr jsNodeIteratorRoot(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    NodeIterator* imp = static_cast<NodeIterator*>(static_cast<JSNodeIterator*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->root()));
}

JSValuePtr jsNodeIteratorWhatToShow(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    NodeIterator* imp = static_cast<NodeIterator*>(static_cast<JSNodeIterator*>(asObject(slot.slotBase()))->impl());
    return jsNumber(exec, imp->whatToShow());
}

JSValuePtr jsNodeIteratorFilter(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    NodeIterator* imp = static_cast<NodeIterator*>(static_cast<JSNodeIterator*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->filter()));
}

JSValuePtr jsNodeIteratorExpandEntityReferences(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    NodeIterator* imp = static_cast<NodeIterator*>(static_cast<JSNodeIterator*>(asObject(slot.slotBase()))->impl());
    return jsBoolean(imp->expandEntityReferences());
}

JSValuePtr jsNodeIteratorReferenceNode(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    NodeIterator* imp = static_cast<NodeIterator*>(static_cast<JSNodeIterator*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->referenceNode()));
}

JSValuePtr jsNodeIteratorPointerBeforeReferenceNode(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    NodeIterator* imp = static_cast<NodeIterator*>(static_cast<JSNodeIterator*>(asObject(slot.slotBase()))->impl());
    return jsBoolean(imp->pointerBeforeReferenceNode());
}

JSValuePtr jsNodeIteratorConstructor(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    return static_cast<JSNodeIterator*>(asObject(slot.slotBase()))->getConstructor(exec);
}
JSValuePtr JSNodeIterator::getConstructor(ExecState* exec)
{
    return getDOMConstructor<JSNodeIteratorConstructor>(exec);
}

JSValuePtr jsNodeIteratorPrototypeFunctionNextNode(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSNodeIterator::s_info))
        return throwError(exec, TypeError);
    JSNodeIterator* castedThisObj = static_cast<JSNodeIterator*>(asObject(thisValue));
    return castedThisObj->nextNode(exec, args);
}

JSValuePtr jsNodeIteratorPrototypeFunctionPreviousNode(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSNodeIterator::s_info))
        return throwError(exec, TypeError);
    JSNodeIterator* castedThisObj = static_cast<JSNodeIterator*>(asObject(thisValue));
    return castedThisObj->previousNode(exec, args);
}

JSValuePtr jsNodeIteratorPrototypeFunctionDetach(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSNodeIterator::s_info))
        return throwError(exec, TypeError);
    JSNodeIterator* castedThisObj = static_cast<JSNodeIterator*>(asObject(thisValue));
    NodeIterator* imp = static_cast<NodeIterator*>(castedThisObj->impl());

    imp->detach();
    return jsUndefined();
}

JSC::JSValuePtr toJS(JSC::ExecState* exec, NodeIterator* object)
{
    return getDOMObjectWrapper<JSNodeIterator>(exec, object);
}
NodeIterator* toNodeIterator(JSC::JSValuePtr value)
{
    return value->isObject(&JSNodeIterator::s_info) ? static_cast<JSNodeIterator*>(asObject(value))->impl() : 0;
}

}
