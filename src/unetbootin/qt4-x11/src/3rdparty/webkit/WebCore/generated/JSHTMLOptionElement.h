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

#ifndef JSHTMLOptionElement_h
#define JSHTMLOptionElement_h

#include "JSHTMLElement.h"
#include "HTMLOptionElement.h"
namespace WebCore {

class HTMLOptionElement;

class JSHTMLOptionElement : public JSHTMLElement {
    typedef JSHTMLElement Base;
public:
    JSHTMLOptionElement(PassRefPtr<JSC::Structure>, PassRefPtr<HTMLOptionElement>);
    static JSC::JSObject* createPrototype(JSC::ExecState*);
    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertySlot&);
    virtual void put(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::JSValuePtr, JSC::PutPropertySlot&);
    virtual const JSC::ClassInfo* classInfo() const { return &s_info; }
    static const JSC::ClassInfo s_info;

    static PassRefPtr<JSC::Structure> createStructure(JSC::JSValuePtr prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType));
    }

    static JSC::JSValuePtr getConstructor(JSC::ExecState*);
    HTMLOptionElement* impl() const
    {
        return static_cast<HTMLOptionElement*>(Base::impl());
    }
};

HTMLOptionElement* toHTMLOptionElement(JSC::JSValuePtr);

class JSHTMLOptionElementPrototype : public JSC::JSObject {
public:
    static JSC::JSObject* self(JSC::ExecState*);
    virtual const JSC::ClassInfo* classInfo() const { return &s_info; }
    static const JSC::ClassInfo s_info;
    JSHTMLOptionElementPrototype(PassRefPtr<JSC::Structure> structure) : JSC::JSObject(structure) { }
};

// Attributes

JSC::JSValuePtr jsHTMLOptionElementForm(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
JSC::JSValuePtr jsHTMLOptionElementDefaultSelected(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSHTMLOptionElementDefaultSelected(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsHTMLOptionElementText(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSHTMLOptionElementText(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsHTMLOptionElementIndex(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
JSC::JSValuePtr jsHTMLOptionElementDisabled(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSHTMLOptionElementDisabled(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsHTMLOptionElementLabel(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSHTMLOptionElementLabel(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsHTMLOptionElementSelected(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSHTMLOptionElementSelected(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsHTMLOptionElementValue(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSHTMLOptionElementValue(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsHTMLOptionElementConstructor(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);

} // namespace WebCore

#endif
