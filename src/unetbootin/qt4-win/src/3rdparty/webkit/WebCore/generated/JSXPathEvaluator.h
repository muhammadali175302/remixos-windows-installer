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

#ifndef JSXPathEvaluator_H
#define JSXPathEvaluator_H


#if ENABLE(XPATH)

#include "kjs_binding.h"

namespace WebCore {

class XPathEvaluator;

class JSXPathEvaluator : public KJS::DOMObject {
public:
    JSXPathEvaluator(KJS::ExecState*, XPathEvaluator*);
    virtual ~JSXPathEvaluator();
    virtual bool getOwnPropertySlot(KJS::ExecState*, const KJS::Identifier&, KJS::PropertySlot&);
    KJS::JSValue* getValueProperty(KJS::ExecState*, int token) const;
    virtual const KJS::ClassInfo* classInfo() const { return &info; }
    static const KJS::ClassInfo info;

    static KJS::JSValue* getConstructor(KJS::ExecState*);
    enum {
        // The Constructor Attribute
        ConstructorAttrNum, 

        // Functions
        CreateExpressionFuncNum, CreateNSResolverFuncNum, EvaluateFuncNum
    };
    XPathEvaluator* impl() const { return m_impl.get(); }
private:
    RefPtr<XPathEvaluator> m_impl;
};

KJS::JSValue* toJS(KJS::ExecState*, XPathEvaluator*);
XPathEvaluator* toXPathEvaluator(KJS::JSValue*);

class JSXPathEvaluatorPrototype : public KJS::JSObject {
public:
    static KJS::JSObject* self(KJS::ExecState* exec);
    virtual const KJS::ClassInfo* classInfo() const { return &info; }
    static const KJS::ClassInfo info;
    bool getOwnPropertySlot(KJS::ExecState*, const KJS::Identifier&, KJS::PropertySlot&);
    JSXPathEvaluatorPrototype(KJS::ExecState* exec)
        : KJS::JSObject(exec->lexicalInterpreter()->builtinObjectPrototype()) { }
};

class JSXPathEvaluatorPrototypeFunction : public KJS::InternalFunctionImp {
public:
    JSXPathEvaluatorPrototypeFunction(KJS::ExecState* exec, int i, int len, const KJS::Identifier& name)
        : KJS::InternalFunctionImp(static_cast<KJS::FunctionPrototype*>(exec->lexicalInterpreter()->builtinFunctionPrototype()), name)
        , id(i)
    {
        put(exec, exec->propertyNames().length, KJS::jsNumber(len), KJS::DontDelete|KJS::ReadOnly|KJS::DontEnum);
    }
    virtual KJS::JSValue* callAsFunction(KJS::ExecState*, KJS::JSObject*, const KJS::List&);

private:
    int id;
};

} // namespace WebCore

#endif // ENABLE(XPATH)

#endif
