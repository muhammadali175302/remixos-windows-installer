/**
 *
 * Copyright (C)  2004  Zack Rusin <zack@kde.org>
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef CSSComputedStyleDeclaration_h
#define CSSComputedStyleDeclaration_h

#include "CSSStyleDeclaration.h"
#include "Node.h"

namespace WebCore {

class CSSMutableStyleDeclaration;
class CSSProperty;
class RenderObject;
class RenderStyle;

enum EUpdateLayout { DoNotUpdateLayout = false, UpdateLayout = true };

class CSSComputedStyleDeclaration : public CSSStyleDeclaration {
public:
    CSSComputedStyleDeclaration(PassRefPtr<Node>);
    virtual ~CSSComputedStyleDeclaration();

    virtual String cssText() const;

    virtual unsigned length() const;
    virtual String item(unsigned index) const;

    virtual PassRefPtr<CSSValue> getPropertyCSSValue(int propertyID) const;
    virtual String getPropertyValue(int propertyID) const;
    virtual bool getPropertyPriority(int propertyID) const;
    virtual int getPropertyShorthand(int propertyID) const { return -1; }
    virtual bool isPropertyImplicit(int propertyID) const { return true; }

    virtual PassRefPtr<CSSMutableStyleDeclaration> copy() const;
    virtual PassRefPtr<CSSMutableStyleDeclaration> makeMutable();

    PassRefPtr<CSSValue> getPropertyCSSValue(int propertyID, EUpdateLayout) const;

    PassRefPtr<CSSMutableStyleDeclaration> copyInheritableProperties() const;

    static void removeComputedInheritablePropertiesFrom(CSSMutableStyleDeclaration*);

private:
    virtual void setCssText(const String&, ExceptionCode&);

    virtual String removeProperty(int propertyID, ExceptionCode&);
    virtual void setProperty(int propertyId, const String& value, bool important, ExceptionCode&);

    RefPtr<Node> m_node;
};

PassRefPtr<CSSComputedStyleDeclaration> computedStyle(Node*);

} // namespace WebCore

#endif // CSSComputedStyleDeclaration_h
