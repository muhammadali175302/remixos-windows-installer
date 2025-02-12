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

#ifndef JSSVGPathSegCurvetoQuadraticAbs_h
#define JSSVGPathSegCurvetoQuadraticAbs_h


#if ENABLE(SVG)

#include "JSSVGPathSeg.h"

namespace WebCore {

class SVGPathSegCurvetoQuadraticAbs;

class JSSVGPathSegCurvetoQuadraticAbs : public JSSVGPathSeg {
    typedef JSSVGPathSeg Base;
public:
    JSSVGPathSegCurvetoQuadraticAbs(PassRefPtr<JSC::Structure>, PassRefPtr<SVGPathSegCurvetoQuadraticAbs>, SVGElement* context);
    static JSC::JSObject* createPrototype(JSC::ExecState*);
    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertySlot&);
    virtual void put(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::JSValuePtr, JSC::PutPropertySlot&);
    virtual const JSC::ClassInfo* classInfo() const { return &s_info; }
    static const JSC::ClassInfo s_info;

    static PassRefPtr<JSC::Structure> createStructure(JSC::JSValuePtr prototype)
    {
        return JSC::Structure::create(prototype, JSC::TypeInfo(JSC::ObjectType));
    }

};


class JSSVGPathSegCurvetoQuadraticAbsPrototype : public JSC::JSObject {
public:
    static JSC::JSObject* self(JSC::ExecState*);
    virtual const JSC::ClassInfo* classInfo() const { return &s_info; }
    static const JSC::ClassInfo s_info;
    JSSVGPathSegCurvetoQuadraticAbsPrototype(PassRefPtr<JSC::Structure> structure) : JSC::JSObject(structure) { }
};

// Attributes

JSC::JSValuePtr jsSVGPathSegCurvetoQuadraticAbsX(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSSVGPathSegCurvetoQuadraticAbsX(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsSVGPathSegCurvetoQuadraticAbsY(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSSVGPathSegCurvetoQuadraticAbsY(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsSVGPathSegCurvetoQuadraticAbsX1(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSSVGPathSegCurvetoQuadraticAbsX1(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);
JSC::JSValuePtr jsSVGPathSegCurvetoQuadraticAbsY1(JSC::ExecState*, const JSC::Identifier&, const JSC::PropertySlot&);
void setJSSVGPathSegCurvetoQuadraticAbsY1(JSC::ExecState*, JSC::JSObject*, JSC::JSValuePtr);

} // namespace WebCore

#endif // ENABLE(SVG)

#endif
