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

#include "JSHTMLMapElement.h"

#include <wtf/GetPtr.h>

#include "HTMLCollection.h"
#include "HTMLMapElement.h"
#include "JSHTMLCollection.h"
#include "PlatformString.h"

using namespace KJS;

namespace WebCore {

/* Hash table */

static const HashEntry JSHTMLMapElementTableEntries[] =
{
    { "areas", JSHTMLMapElement::AreasAttrNum, DontDelete|ReadOnly, 0, &JSHTMLMapElementTableEntries[3] },
    { "name", JSHTMLMapElement::NameAttrNum, DontDelete, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { "constructor", JSHTMLMapElement::ConstructorAttrNum, DontDelete|DontEnum|ReadOnly, 0, 0 }
};

static const HashTable JSHTMLMapElementTable = 
{
    2, 4, JSHTMLMapElementTableEntries, 3
};

/* Hash table for constructor */

static const HashEntry JSHTMLMapElementConstructorTableEntries[] =
{
    { 0, 0, 0, 0, 0 }
};

static const HashTable JSHTMLMapElementConstructorTable = 
{
    2, 1, JSHTMLMapElementConstructorTableEntries, 1
};

class JSHTMLMapElementConstructor : public DOMObject {
public:
    JSHTMLMapElementConstructor(ExecState* exec)
    {
        setPrototype(exec->lexicalInterpreter()->builtinObjectPrototype());
        putDirect(exec->propertyNames().prototype, JSHTMLMapElementPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    JSValue* getValueProperty(ExecState*, int token) const;
    virtual const ClassInfo* classInfo() const { return &info; }
    static const ClassInfo info;

    virtual bool implementsHasInstance() const { return true; }
};

const ClassInfo JSHTMLMapElementConstructor::info = { "HTMLMapElementConstructor", 0, &JSHTMLMapElementConstructorTable, 0 };

bool JSHTMLMapElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLMapElementConstructor, DOMObject>(exec, &JSHTMLMapElementConstructorTable, this, propertyName, slot);
}

JSValue* JSHTMLMapElementConstructor::getValueProperty(ExecState*, int token) const
{
    // The token is the numeric value of its associated constant
    return jsNumber(token);
}

/* Hash table for prototype */

static const HashEntry JSHTMLMapElementPrototypeTableEntries[] =
{
    { 0, 0, 0, 0, 0 }
};

static const HashTable JSHTMLMapElementPrototypeTable = 
{
    2, 1, JSHTMLMapElementPrototypeTableEntries, 1
};

const ClassInfo JSHTMLMapElementPrototype::info = { "HTMLMapElementPrototype", 0, &JSHTMLMapElementPrototypeTable, 0 };

JSObject* JSHTMLMapElementPrototype::self(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSHTMLMapElementPrototype>(exec, "[[JSHTMLMapElement.prototype]]");
}

const ClassInfo JSHTMLMapElement::info = { "HTMLMapElement", &JSHTMLElement::info, &JSHTMLMapElementTable, 0 };

JSHTMLMapElement::JSHTMLMapElement(ExecState* exec, HTMLMapElement* impl)
    : JSHTMLElement(exec, impl)
{
    setPrototype(JSHTMLMapElementPrototype::self(exec));
}

bool JSHTMLMapElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLMapElement, JSHTMLElement>(exec, &JSHTMLMapElementTable, this, propertyName, slot);
}

JSValue* JSHTMLMapElement::getValueProperty(ExecState* exec, int token) const
{
    switch (token) {
    case AreasAttrNum: {
        HTMLMapElement* imp = static_cast<HTMLMapElement*>(impl());

        return toJS(exec, WTF::getPtr(imp->areas()));
    }
    case NameAttrNum: {
        HTMLMapElement* imp = static_cast<HTMLMapElement*>(impl());

        return jsString(imp->name());
    }
    case ConstructorAttrNum:
        return getConstructor(exec);
    }
    return 0;
}

void JSHTMLMapElement::put(ExecState* exec, const Identifier& propertyName, JSValue* value, int attr)
{
    lookupPut<JSHTMLMapElement, JSHTMLElement>(exec, propertyName, value, attr, &JSHTMLMapElementTable, this);
}

void JSHTMLMapElement::putValueProperty(ExecState* exec, int token, JSValue* value, int /*attr*/)
{
    switch (token) {
    case NameAttrNum: {
        HTMLMapElement* imp = static_cast<HTMLMapElement*>(impl());

        imp->setName(valueToStringWithNullCheck(exec, value));
        break;
    }
    }
}

JSValue* JSHTMLMapElement::getConstructor(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSHTMLMapElementConstructor>(exec, "[[HTMLMapElement.constructor]]");
}

}
