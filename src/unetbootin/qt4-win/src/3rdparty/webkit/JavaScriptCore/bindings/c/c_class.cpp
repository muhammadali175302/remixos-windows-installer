/*
 * Copyright (C) 2003, 2006 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"

#if !PLATFORM(DARWIN) || !defined(__LP64__)

#include "c_class.h"

#include "c_instance.h"
#include "c_runtime.h"
#include "identifier.h"
#include "npruntime_impl.h"

namespace KJS { namespace Bindings {

CClass::CClass(NPClass* aClass)
{
    _isa = aClass;
}

CClass::~CClass()
{
    JSLock lock;

    deleteAllValues(_methods);
    _methods.clear();

    deleteAllValues(_fields);
    _fields.clear();
}

typedef HashMap<NPClass*, CClass*> ClassesByIsAMap;
static ClassesByIsAMap* classesByIsA = 0;

CClass* CClass::classForIsA(NPClass* isa)
{
    if (!classesByIsA)
        classesByIsA = new ClassesByIsAMap;

    CClass* aClass = classesByIsA->get(isa);
    if (!aClass) {
        aClass = new CClass(isa);
        classesByIsA->set(isa, aClass);
    }

    return aClass;
}

const char* CClass::name() const
{
    return "";
}

MethodList CClass::methodsNamed(const Identifier& identifier, Instance* instance) const
{
    MethodList methodList;

    Method* method = _methods.get(identifier.ustring().rep());
    if (method) {
        methodList.append(method);
        return methodList;
    }

    NPIdentifier ident = _NPN_GetStringIdentifier(identifier.ascii());
    const CInstance* inst = static_cast<const CInstance*>(instance);
    NPObject* obj = inst->getObject();
    if (_isa->hasMethod && _isa->hasMethod(obj, ident)){
        Method* aMethod = new CMethod(ident); // deleted in the CClass destructor
        {
            JSLock lock;
            _methods.set(identifier.ustring().rep(), aMethod);
        }
        methodList.append(aMethod);
    }
    
    return methodList;
}

Field* CClass::fieldNamed(const Identifier& identifier, Instance* instance) const
{
    Field* aField = _fields.get(identifier.ustring().rep());
    if (aField)
        return aField;
    
    NPIdentifier ident = _NPN_GetStringIdentifier(identifier.ascii());
    const CInstance* inst = static_cast<const CInstance*>(instance);
    NPObject* obj = inst->getObject();
    if (_isa->hasProperty && _isa->hasProperty(obj, ident)){
        aField = new CField(ident); // deleted in the CClass destructor
        {
            JSLock lock;
            _fields.set(identifier.ustring().rep(), aField);
        }
    }
    return aField;
}

} } // namespace KJS::Bindings

#endif
