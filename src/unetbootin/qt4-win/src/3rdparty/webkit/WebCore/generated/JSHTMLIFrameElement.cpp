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

#include "JSHTMLIFrameElement.h"

#include <wtf/GetPtr.h>

#include "Document.h"
#include "HTMLIFrameElement.h"
#include "JSDocument.h"
#include "PlatformString.h"
#include "kjs_dom.h"
#include "kjs_window.h"

using namespace KJS;

namespace WebCore {

/* Hash table */

static const HashEntry JSHTMLIFrameElementTableEntries[] =
{
    { "height", JSHTMLIFrameElement::HeightAttrNum, DontDelete, 0, &JSHTMLIFrameElementTableEntries[13] },
    { "longDesc", JSHTMLIFrameElement::LongDescAttrNum, DontDelete, 0, 0 },
    { "constructor", JSHTMLIFrameElement::ConstructorAttrNum, DontDelete|DontEnum|ReadOnly, 0, 0 },
    { "name", JSHTMLIFrameElement::NameAttrNum, DontDelete, 0, 0 },
    { "width", JSHTMLIFrameElement::WidthAttrNum, DontDelete, 0, 0 },
    { "marginWidth", JSHTMLIFrameElement::MarginWidthAttrNum, DontDelete, 0, &JSHTMLIFrameElementTableEntries[14] },
    { "contentDocument", JSHTMLIFrameElement::ContentDocumentAttrNum, DontDelete|ReadOnly, 0, 0 },
    { "frameBorder", JSHTMLIFrameElement::FrameBorderAttrNum, DontDelete, 0, 0 },
    { "align", JSHTMLIFrameElement::AlignAttrNum, DontDelete, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0 },
    { "contentWindow", JSHTMLIFrameElement::ContentWindowAttrNum, DontDelete|ReadOnly, 0, 0 },
    { "marginHeight", JSHTMLIFrameElement::MarginHeightAttrNum, DontDelete, 0, &JSHTMLIFrameElementTableEntries[15] },
    { "scrolling", JSHTMLIFrameElement::ScrollingAttrNum, DontDelete, 0, 0 },
    { "src", JSHTMLIFrameElement::SrcAttrNum, DontDelete, 0, 0 }
};

static const HashTable JSHTMLIFrameElementTable = 
{
    2, 16, JSHTMLIFrameElementTableEntries, 13
};

/* Hash table for constructor */

static const HashEntry JSHTMLIFrameElementConstructorTableEntries[] =
{
    { 0, 0, 0, 0, 0 }
};

static const HashTable JSHTMLIFrameElementConstructorTable = 
{
    2, 1, JSHTMLIFrameElementConstructorTableEntries, 1
};

class JSHTMLIFrameElementConstructor : public DOMObject {
public:
    JSHTMLIFrameElementConstructor(ExecState* exec)
    {
        setPrototype(exec->lexicalInterpreter()->builtinObjectPrototype());
        putDirect(exec->propertyNames().prototype, JSHTMLIFrameElementPrototype::self(exec), None);
    }
    virtual bool getOwnPropertySlot(ExecState*, const Identifier&, PropertySlot&);
    JSValue* getValueProperty(ExecState*, int token) const;
    virtual const ClassInfo* classInfo() const { return &info; }
    static const ClassInfo info;

    virtual bool implementsHasInstance() const { return true; }
};

const ClassInfo JSHTMLIFrameElementConstructor::info = { "HTMLIFrameElementConstructor", 0, &JSHTMLIFrameElementConstructorTable, 0 };

bool JSHTMLIFrameElementConstructor::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLIFrameElementConstructor, DOMObject>(exec, &JSHTMLIFrameElementConstructorTable, this, propertyName, slot);
}

JSValue* JSHTMLIFrameElementConstructor::getValueProperty(ExecState*, int token) const
{
    // The token is the numeric value of its associated constant
    return jsNumber(token);
}

/* Hash table for prototype */

static const HashEntry JSHTMLIFrameElementPrototypeTableEntries[] =
{
    { 0, 0, 0, 0, 0 }
};

static const HashTable JSHTMLIFrameElementPrototypeTable = 
{
    2, 1, JSHTMLIFrameElementPrototypeTableEntries, 1
};

const ClassInfo JSHTMLIFrameElementPrototype::info = { "HTMLIFrameElementPrototype", 0, &JSHTMLIFrameElementPrototypeTable, 0 };

JSObject* JSHTMLIFrameElementPrototype::self(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSHTMLIFrameElementPrototype>(exec, "[[JSHTMLIFrameElement.prototype]]");
}

const ClassInfo JSHTMLIFrameElement::info = { "HTMLIFrameElement", &JSHTMLElement::info, &JSHTMLIFrameElementTable, 0 };

JSHTMLIFrameElement::JSHTMLIFrameElement(ExecState* exec, HTMLIFrameElement* impl)
    : JSHTMLElement(exec, impl)
{
    setPrototype(JSHTMLIFrameElementPrototype::self(exec));
}

bool JSHTMLIFrameElement::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSHTMLIFrameElement, JSHTMLElement>(exec, &JSHTMLIFrameElementTable, this, propertyName, slot);
}

JSValue* JSHTMLIFrameElement::getValueProperty(ExecState* exec, int token) const
{
    switch (token) {
    case AlignAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return jsString(imp->align());
    }
    case FrameBorderAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return jsString(imp->frameBorder());
    }
    case HeightAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return jsString(imp->height());
    }
    case LongDescAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return jsString(imp->longDesc());
    }
    case MarginHeightAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return jsString(imp->marginHeight());
    }
    case MarginWidthAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return jsString(imp->marginWidth());
    }
    case NameAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return jsString(imp->name());
    }
    case ScrollingAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return jsString(imp->scrolling());
    }
    case SrcAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return jsString(imp->src());
    }
    case WidthAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return jsString(imp->width());
    }
    case ContentDocumentAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return checkNodeSecurity(exec, imp->contentDocument()) ? toJS(exec, WTF::getPtr(imp->contentDocument())) : jsUndefined();
    }
    case ContentWindowAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        return toJS(exec, WTF::getPtr(imp->contentWindow()));
    }
    case ConstructorAttrNum:
        return getConstructor(exec);
    }
    return 0;
}

void JSHTMLIFrameElement::put(ExecState* exec, const Identifier& propertyName, JSValue* value, int attr)
{
    lookupPut<JSHTMLIFrameElement, JSHTMLElement>(exec, propertyName, value, attr, &JSHTMLIFrameElementTable, this);
}

void JSHTMLIFrameElement::putValueProperty(ExecState* exec, int token, JSValue* value, int /*attr*/)
{
    switch (token) {
    case AlignAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        imp->setAlign(valueToStringWithNullCheck(exec, value));
        break;
    }
    case FrameBorderAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        imp->setFrameBorder(valueToStringWithNullCheck(exec, value));
        break;
    }
    case HeightAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        imp->setHeight(valueToStringWithNullCheck(exec, value));
        break;
    }
    case LongDescAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        imp->setLongDesc(valueToStringWithNullCheck(exec, value));
        break;
    }
    case MarginHeightAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        imp->setMarginHeight(valueToStringWithNullCheck(exec, value));
        break;
    }
    case MarginWidthAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        imp->setMarginWidth(valueToStringWithNullCheck(exec, value));
        break;
    }
    case NameAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        imp->setName(valueToStringWithNullCheck(exec, value));
        break;
    }
    case ScrollingAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        imp->setScrolling(valueToStringWithNullCheck(exec, value));
        break;
    }
    case SrcAttrNum: {
        setSrc(exec, value);
        break;
    }
    case WidthAttrNum: {
        HTMLIFrameElement* imp = static_cast<HTMLIFrameElement*>(impl());

        imp->setWidth(valueToStringWithNullCheck(exec, value));
        break;
    }
    }
}

JSValue* JSHTMLIFrameElement::getConstructor(ExecState* exec)
{
    return KJS::cacheGlobalObject<JSHTMLIFrameElementConstructor>(exec, "[[HTMLIFrameElement.constructor]]");
}

}
