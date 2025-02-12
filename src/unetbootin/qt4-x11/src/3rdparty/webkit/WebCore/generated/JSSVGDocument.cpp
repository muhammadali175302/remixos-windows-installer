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


#if ENABLE(SVG)

#include "SVGElement.h"
#include "JSSVGDocument.h"

#include <wtf/GetPtr.h>

#include "Event.h"
#include "JSEvent.h"
#include "JSSVGSVGElement.h"
#include "SVGDocument.h"
#include "SVGSVGElement.h"

#include <runtime/Error.h>

using namespace JSC;

namespace WebCore {

ASSERT_CLASS_FITS_IN_CELL(JSSVGDocument)

/* Hash table */

static const HashTableValue JSSVGDocumentTableValues[2] =
{
    { "rootElement", DontDelete|ReadOnly, (intptr_t)jsSVGDocumentRootElement, (intptr_t)0 },
    { 0, 0, 0, 0 }
};

static const HashTable JSSVGDocumentTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGDocumentTableValues, 0 };
#else
    { 2, 1, JSSVGDocumentTableValues, 0 };
#endif

/* Hash table for prototype */

static const HashTableValue JSSVGDocumentPrototypeTableValues[2] =
{
    { "createEvent", DontDelete|Function, (intptr_t)jsSVGDocumentPrototypeFunctionCreateEvent, (intptr_t)1 },
    { 0, 0, 0, 0 }
};

static const HashTable JSSVGDocumentPrototypeTable =
#if ENABLE(PERFECT_HASH_SIZE)
    { 0, JSSVGDocumentPrototypeTableValues, 0 };
#else
    { 2, 1, JSSVGDocumentPrototypeTableValues, 0 };
#endif

const ClassInfo JSSVGDocumentPrototype::s_info = { "SVGDocumentPrototype", 0, &JSSVGDocumentPrototypeTable, 0 };

JSObject* JSSVGDocumentPrototype::self(ExecState* exec)
{
    return getDOMPrototype<JSSVGDocument>(exec);
}

bool JSSVGDocumentPrototype::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticFunctionSlot<JSObject>(exec, &JSSVGDocumentPrototypeTable, this, propertyName, slot);
}

const ClassInfo JSSVGDocument::s_info = { "SVGDocument", &JSDocument::s_info, &JSSVGDocumentTable, 0 };

JSSVGDocument::JSSVGDocument(PassRefPtr<Structure> structure, PassRefPtr<SVGDocument> impl)
    : JSDocument(structure, impl)
{
}

JSObject* JSSVGDocument::createPrototype(ExecState* exec)
{
    return new (exec) JSSVGDocumentPrototype(JSSVGDocumentPrototype::createStructure(JSDocumentPrototype::self(exec)));
}

bool JSSVGDocument::getOwnPropertySlot(ExecState* exec, const Identifier& propertyName, PropertySlot& slot)
{
    return getStaticValueSlot<JSSVGDocument, Base>(exec, &JSSVGDocumentTable, this, propertyName, slot);
}

JSValuePtr jsSVGDocumentRootElement(ExecState* exec, const Identifier&, const PropertySlot& slot)
{
    SVGDocument* imp = static_cast<SVGDocument*>(static_cast<JSSVGDocument*>(asObject(slot.slotBase()))->impl());
    return toJS(exec, WTF::getPtr(imp->rootElement()));
}

JSValuePtr jsSVGDocumentPrototypeFunctionCreateEvent(ExecState* exec, JSObject*, JSValuePtr thisValue, const ArgList& args)
{
    if (!thisValue->isObject(&JSSVGDocument::s_info))
        return throwError(exec, TypeError);
    JSSVGDocument* castedThisObj = static_cast<JSSVGDocument*>(asObject(thisValue));
    SVGDocument* imp = static_cast<SVGDocument*>(castedThisObj->impl());
    ExceptionCode ec = 0;
    const UString& eventType = args.at(exec, 0)->toString(exec);


    JSC::JSValuePtr result = toJS(exec, WTF::getPtr(imp->createEvent(eventType, ec)));
    setDOMException(exec, ec);
    return result;
}


}

#endif // ENABLE(SVG)
