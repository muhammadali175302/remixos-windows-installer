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

#include "JSEntityReference.h"

#include <wtf/GetPtr.h>

#include "EntityReference.h"

using namespace KJS;

namespace WebCore {

/* Hash table */

static const HashEntry JSEntityReferenceTableEntries[] =
{
    { "constructor", JSEntityReference::ConstructorAttrNum, DontDelete|DontEnum|ReadOnly, 0, 0 }
};

static const HashTable JSEntityReferenceTable = 
{
    2, 1, JSEntityReferenceTableEntries, 1
};

/* Hash table for constructor */

static const HashEntry JSEntityReferenceConstructorTableEntries[] =
{
    { 0, 0, 0, 0, 0 }
};

static const HashTable JSEntityReferenceConstructorTable = 
{
    2, 1, JSEntityReferenceConstructorTableEntries, 1
};

class JSEntityReferenceConstructor : public DOMObject {
public:
    JSEntityReferenceConstructor(ExecState* exec)
    {
        setPrototype(exec->lexicalInterpreter()->builtinObjectPrototype());
        putDirect(exec->propertyNames().prototype, JSEntityReferencePrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    JSValue* getValueProperty(ExecState*, int token) const;
    virtual const ClassInfo* classInfo() const { return &info; }
    static const ClassInfo info;

    virtual bool implementsHasInstance() const { return true; }
};

const ClassInfo JSEntityReferenceConstructor::info = { "EntityReferenceConstructor", 0, &JSEntityReferenceConstructorTable, 0 };

bool JSEntityReferenceConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSEntityReferenceConstructor, DOMObject>(exec, &JSEntityReferenceConstructorTable, this, propertyName, slot);
}

JSValue* JSEntityReferenceConstructor::getValueProperty(ExecState*, int token) const
{
    // The token is the numeric value of its associated constant
    return jsNumber(token);
}

/* Hash table for prototype */

static const HashEntry JSEntityReferencePrototypeTableEntries[] =
{
    { 0, 0, 0, 0, 0 }
};

static const HashTable JSEntityReferencePrototypeTable = 
{
    2, 1, JSEntityReferencePrototypeTableEntries, 1
};

const ClassInfo JSEntityReferencePrototype::info = { "EntityReferencePrototype", 0, &JSEntityReferencePrototypeTable, 0 };

JSObject* JSEntityReferencePrototype::self(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSEntityReferencePrototype>(exec, "[[JSEntityReference.prototype]]");
}

const ClassInfo JSEntityReference::info = { "EntityReference", &JSNode::info, &JSEntityReferenceTable, 0 };

JSEntityReference::JSEntityReference(ExecState* exec, EntityReference* impl)
    : JSNode(exec, impl)
{
    setPrototype(JSEntityReferencePrototype::self(exec));
}

bool JSEntityReference::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSEntityReference, JSNode>(exec, &JSEntityReferenceTable, this, propertyName, slot);
}

JSValue* JSEntityReference::getValueProperty(ExecState* exec, int token) const
{
    switch (token) {
    case ConstructorAttrNum:
        return getConstructor(exec);
    }
    return 0;
}

JSValue* JSEntityReference::getConstructor(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSEntityReferenceConstructor>(exec, "[[EntityReference.constructor]]");
}

}
