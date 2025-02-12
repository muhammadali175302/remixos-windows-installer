/*
 * Copyright (C) 2007 Apple Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "RenderThemeSafari.h"

#ifdef USE_SAFARI_THEME

#include "CSSValueKeywords.h"
#include "Document.h"
#include "Element.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "HTMLInputElement.h"
#include "RenderSlider.h"
#include "RenderView.h"
#include "RetainPtr.h"
#include "cssstyleselector.h"
#include <CoreGraphics/CoreGraphics.h>
 
using std::min;

// FIXME: The platform-independent code in this class should be factored out and merged with RenderThemeMac. 
 
namespace WebCore {

using namespace SafariTheme;

enum {
    topMargin,
    rightMargin,
    bottomMargin,
    leftMargin
};

enum {
    topPadding,
    rightPadding,
    bottomPadding,
    leftPadding
};

RenderTheme* theme()
{
    static RenderThemeSafari safariTheme;
    return &safariTheme;
}

static paintThemePartPtr paintThemePart;

ThemeControlState RenderThemeSafari::determineState(RenderObject* o) const
{
    ThemeControlState result = 0;
    if (isEnabled(o) && !isReadOnlyControl(o))
        result |= SafariTheme::EnabledState;
    if (isPressed(o))
        result |= SafariTheme::PressedState;
    if (isChecked(o))
        result |= SafariTheme::CheckedState;
    if (isIndeterminate(o))
        result |= SafariTheme::IndeterminateCheckedState;
    if (isFocused(o))
        result |= SafariTheme::FocusedState;
    return result;
}

static NSControlSize controlSizeFromRect(const IntRect& rect, const IntSize sizes[])
{
    if (sizes[NSRegularControlSize].height() == rect.height())
        return NSRegularControlSize;
    else if (sizes[NSMiniControlSize].height() == rect.height())
        return NSMiniControlSize;
    
    return NSSmallControlSize;
}

RenderThemeSafari::RenderThemeSafari()
    : m_themeDLL(0)
{
    m_themeDLL = ::LoadLibrary(SAFARITHEMEDLL);
    if (m_themeDLL) {
        paintThemePart = (paintThemePartPtr)GetProcAddress(m_themeDLL, "paintThemePart");
    }
}

RenderThemeSafari::~RenderThemeSafari()
{
    if (!m_themeDLL)
        return;

    // we don't need to close the themes here because uxtheme should do that for us
    // anyway (and we could crash if uxtheme has done cleanup already)

    ::FreeLibrary(m_themeDLL);
}

Color RenderThemeSafari::platformActiveSelectionBackgroundColor() const
{
    return Color(181, 213, 255);
}

Color RenderThemeSafari::platformInactiveSelectionBackgroundColor() const
{
    return Color(212, 212, 212);
}

Color RenderThemeSafari::activeListBoxSelectionBackgroundColor() const
{
    // FIXME: This should probably just be a darker version of the platformActiveSelectionBackgroundColor
    return Color(56, 117, 215);
}

static float systemFontSizeForControlSize(NSControlSize controlSize)
{
    static float sizes[] = { 13.0f, 11.0f, 9.0f };
    
    return sizes[controlSize];
}

void RenderThemeSafari::systemFont(int propId, FontDescription& fontDescription) const
{
    static FontDescription systemFont;
    static FontDescription smallSystemFont;
    static FontDescription menuFont;
    static FontDescription labelFont;
    static FontDescription miniControlFont;
    static FontDescription smallControlFont;
    static FontDescription controlFont;

    FontDescription* cachedDesc;
    float fontSize = 0;
    switch (propId) {
        case CSS_VAL_SMALL_CAPTION:
            cachedDesc = &smallSystemFont;
            if (!smallSystemFont.isAbsoluteSize())
                fontSize = systemFontSizeForControlSize(NSSmallControlSize);
            break;
        case CSS_VAL_MENU:
            cachedDesc = &menuFont;
            if (!menuFont.isAbsoluteSize())
                fontSize = systemFontSizeForControlSize(NSRegularControlSize);
            break;
        case CSS_VAL_STATUS_BAR:
            cachedDesc = &labelFont;
            if (!labelFont.isAbsoluteSize())
                fontSize = 10.0f;
            break;
        case CSS_VAL__WEBKIT_MINI_CONTROL:
            cachedDesc = &miniControlFont;
            if (!miniControlFont.isAbsoluteSize())
                fontSize = systemFontSizeForControlSize(NSMiniControlSize);
            break;
        case CSS_VAL__WEBKIT_SMALL_CONTROL:
            cachedDesc = &smallControlFont;
            if (!smallControlFont.isAbsoluteSize())
                fontSize = systemFontSizeForControlSize(NSSmallControlSize);
            break;
        case CSS_VAL__WEBKIT_CONTROL:
            cachedDesc = &controlFont;
            if (!controlFont.isAbsoluteSize())
                fontSize = systemFontSizeForControlSize(NSRegularControlSize);
            break;
        default:
            cachedDesc = &systemFont;
            if (!systemFont.isAbsoluteSize())
                fontSize = 13.0f;
    }

    if (fontSize) {
        cachedDesc->setIsAbsoluteSize(true);
        cachedDesc->setGenericFamily(FontDescription::NoFamily);
        cachedDesc->firstFamily().setFamily("Lucida Grande");
        cachedDesc->setSpecifiedSize(fontSize);
        cachedDesc->setBold(false);
        cachedDesc->setItalic(false);
    }
    fontDescription = *cachedDesc;
}

bool RenderThemeSafari::isControlStyled(const RenderStyle* style, const BorderData& border,
                                     const BackgroundLayer& background, const Color& backgroundColor) const
{
    if (style->appearance() == TextFieldAppearance || style->appearance() == TextAreaAppearance || style->appearance() == ListboxAppearance)
        return style->border() != border;
    return RenderTheme::isControlStyled(style, border, background, backgroundColor);
}

void RenderThemeSafari::adjustRepaintRect(const RenderObject* o, IntRect& r)
{
    NSControlSize controlSize = controlSizeForFont(o->style());

    switch (o->style()->appearance()) {
        case CheckboxAppearance: {
            // We inflate the rect as needed to account for padding included in the cell to accommodate the checkbox
            // shadow" and the check.  We don't consider this part of the bounds of the control in WebKit.
            r = inflateRect(r, checkboxSizes()[controlSize], checkboxMargins(controlSize));
            break;
        }
        case RadioAppearance: {
            // We inflate the rect as needed to account for padding included in the cell to accommodate the checkbox
            // shadow" and the check.  We don't consider this part of the bounds of the control in WebKit.
            r = inflateRect(r, radioSizes()[controlSize], radioMargins(controlSize));
            break;
        }
        case PushButtonAppearance:
        case ButtonAppearance: {
            // We inflate the rect as needed to account for padding included in the cell to accommodate the checkbox
            // shadow" and the check.  We don't consider this part of the bounds of the control in WebKit.
            if (r.height() <= buttonSizes()[NSRegularControlSize].height())
                r = inflateRect(r, buttonSizes()[controlSize], buttonMargins(controlSize));
            break;
        }
        case MenulistAppearance: {
            r = inflateRect(r, popupButtonSizes()[controlSize], popupButtonMargins(controlSize));
            break;
        }
        default:
            break;
    }
}

IntRect RenderThemeSafari::inflateRect(const IntRect& r, const IntSize& size, const int* margins) const
{
    // Only do the inflation if the available width/height are too small.  Otherwise try to
    // fit the glow/check space into the available box's width/height.
    int widthDelta = r.width() - (size.width() + margins[leftMargin] + margins[rightMargin]);
    int heightDelta = r.height() - (size.height() + margins[topMargin] + margins[bottomMargin]);
    IntRect result(r);
    if (widthDelta < 0) {
        result.setX(result.x() - margins[leftMargin]);
        result.setWidth(result.width() - widthDelta);
    }
    if (heightDelta < 0) {
        result.setY(result.y() - margins[topMargin]);
        result.setHeight(result.height() - heightDelta);
    }
    return result;
}

short RenderThemeSafari::baselinePosition(const RenderObject* o) const
{
    if (o->style()->appearance() == CheckboxAppearance || o->style()->appearance() == RadioAppearance)
        return o->marginTop() + o->height() - 2; // The baseline is 2px up from the bottom of the checkbox/radio in AppKit.
    return RenderTheme::baselinePosition(o);
}

bool RenderThemeSafari::controlSupportsTints(const RenderObject* o) const
{
    if (!isEnabled(o))
        return false;

    // Checkboxes only have tint when checked.
    if (o->style()->appearance() == CheckboxAppearance)
        return isChecked(o);

    // For now assume other controls have tint if enabled.
    return true;
}

NSControlSize RenderThemeSafari::controlSizeForFont(RenderStyle* style) const
{
    int fontSize = style->fontSize();
    if (fontSize >= 16)
        return NSRegularControlSize;
    if (fontSize >= 11)
        return NSSmallControlSize;
    return NSMiniControlSize;
}
/*
void RenderThemeSafari::setControlSize(NSCell* cell, const IntSize* sizes, const IntSize& minSize)
{
    NSControlSize size;
    if (minSize.width() >= sizes[NSRegularControlSize].width() &&
        minSize.height() >= sizes[NSRegularControlSize].height())
        size = NSRegularControlSize;
    else if (minSize.width() >= sizes[NSSmallControlSize].width() &&
             minSize.height() >= sizes[NSSmallControlSize].height())
        size = NSSmallControlSize;
    else
        size = NSMiniControlSize;
    if (size != [cell controlSize]) // Only update if we have to, since AppKit does work even if the size is the same.
        [cell setControlSize:size];
}
*/
IntSize RenderThemeSafari::sizeForFont(RenderStyle* style, const IntSize* sizes) const
{
    return sizes[controlSizeForFont(style)];
}

IntSize RenderThemeSafari::sizeForSystemFont(RenderStyle* style, const IntSize* sizes) const
{
    return sizes[controlSizeForSystemFont(style)];
}

void RenderThemeSafari::setSizeFromFont(RenderStyle* style, const IntSize* sizes) const
{
    // FIXME: Check is flawed, since it doesn't take min-width/max-width into account.
    IntSize size = sizeForFont(style, sizes);
    if (style->width().isIntrinsicOrAuto() && size.width() > 0)
        style->setWidth(Length(size.width(), Fixed));
    if (style->height().isAuto() && size.height() > 0)
        style->setHeight(Length(size.height(), Fixed));
}

void RenderThemeSafari::setFontFromControlSize(CSSStyleSelector* selector, RenderStyle* style, NSControlSize controlSize) const
{
    FontDescription fontDescription;
    fontDescription.setIsAbsoluteSize(true);
    fontDescription.setGenericFamily(FontDescription::SerifFamily);

    float fontSize = systemFontSizeForControlSize(controlSize);
    fontDescription.firstFamily().setFamily("Lucida Grande");
    fontDescription.setComputedSize(fontSize);
    fontDescription.setSpecifiedSize(fontSize);

    // Reset line height
    style->setLineHeight(RenderStyle::initialLineHeight());

    if (style->setFontDescription(fontDescription))
        style->font().update();
}

NSControlSize RenderThemeSafari::controlSizeForSystemFont(RenderStyle* style) const
{
    int fontSize = style->fontSize();
    if (fontSize >= 13)
        return NSRegularControlSize;
    if (fontSize >= 11)
        return NSSmallControlSize;
    return NSMiniControlSize;
}

bool RenderThemeSafari::paintCheckbox(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
{
    NSControlSize controlSize = controlSizeForFont(o->style());

    IntRect inflatedRect = inflateRect(r, checkboxSizes()[controlSize], checkboxMargins(controlSize));  
    paintThemePart(CheckboxPart, paintInfo.context->platformContext(), inflatedRect, controlSize, determineState(o));

    return false;
}

const IntSize* RenderThemeSafari::checkboxSizes() const
{
    static const IntSize sizes[3] = { IntSize(14, 14), IntSize(12, 12), IntSize(10, 10) };
    return sizes;
}

const int* RenderThemeSafari::checkboxMargins(NSControlSize controlSize) const
{
    static const int margins[3][4] =
    {
        { 2, 2, 2, 2 },
        { 2, 2, 2, 1 },
        { 1, 0, 0, 0 },
    };
    return margins[controlSize];
}

void RenderThemeSafari::setCheckboxSize(RenderStyle* style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;

    // Use the font size to determine the intrinsic width of the control.
    setSizeFromFont(style, checkboxSizes());
}

bool RenderThemeSafari::paintRadio(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
{
    NSControlSize controlSize = controlSizeForFont(o->style());
 
    IntRect inflatedRect = inflateRect(r, radioSizes()[controlSize], radioMargins(controlSize));    
    paintThemePart(RadioButtonPart, paintInfo.context->platformContext(), inflatedRect, controlSize, determineState(o));

    return false;
}

const IntSize* RenderThemeSafari::radioSizes() const
{
    static const IntSize sizes[3] = { IntSize(14, 15), IntSize(12, 13), IntSize(10, 10) };
    return sizes;
}

const int* RenderThemeSafari::radioMargins(NSControlSize controlSize) const
{
    static const int margins[3][4] =
    {
        { 1, 2, 2, 2 },
        { 0, 1, 2, 1 },
        { 0, 0, 1, 0 },
     };
    return margins[controlSize];
}

void RenderThemeSafari::setRadioSize(RenderStyle* style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;

    // Use the font size to determine the intrinsic width of the control.
    setSizeFromFont(style, radioSizes());
}

void RenderThemeSafari::setButtonPaddingFromControlSize(RenderStyle* style, NSControlSize size) const
{
    // Just use 8px.  AppKit wants to use 11px for mini buttons, but that padding is just too large
    // for real-world Web sites (creating a huge necessary minimum width for buttons whose space is
    // by definition constrained, since we select mini only for small cramped environments.
    // This also guarantees the HTML4 <button> will match our rendering by default, since we're using a consistent
    // padding.
    const int padding = 8;
    style->setPaddingLeft(Length(padding, Fixed));
    style->setPaddingRight(Length(padding, Fixed));
    style->setPaddingTop(Length(0, Fixed));
    style->setPaddingBottom(Length(0, Fixed));
}

void RenderThemeSafari::adjustButtonStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    // There are three appearance constants for buttons.
    // (1) Push-button is the constant for the default Aqua system button.  Push buttons will not scale vertically and will not allow
    // custom fonts or colors.  <input>s use this constant.  This button will allow custom colors and font weights/variants but won't
    // scale vertically.
    // (2) square-button is the constant for the square button.  This button will allow custom fonts and colors and will scale vertically.
    // (3) Button is the constant that means "pick the best button as appropriate."  <button>s use this constant.  This button will
    // also scale vertically and allow custom fonts and colors.  It will attempt to use Aqua if possible and will make this determination
    // solely on the rectangle of the control.

    // Determine our control size based off our font.
    NSControlSize controlSize = controlSizeForFont(style);

    if (style->appearance() == PushButtonAppearance) {
        // Ditch the border.
        style->resetBorder();

        // Height is locked to auto.
        style->setHeight(Length(Auto));

        // White-space is locked to pre
        style->setWhiteSpace(PRE);

        // Set the button's vertical size.
        setButtonSize(style);

        // Add in the padding that we'd like to use.
        setButtonPaddingFromControlSize(style, controlSize);

        // Our font is locked to the appropriate system font size for the control.  To clarify, we first use the CSS-specified font to figure out
        // a reasonable control size, but once that control size is determined, we throw that font away and use the appropriate
        // system font for the control size instead.
        setFontFromControlSize(selector, style, controlSize);
    } else {
        // Set a min-height so that we can't get smaller than the mini button.
        style->setMinHeight(Length(15, Fixed));

        // Reset the top and bottom borders.
        style->resetBorderTop();
        style->resetBorderBottom();
    }
}

const IntSize* RenderThemeSafari::buttonSizes() const
{
    static const IntSize sizes[3] = { IntSize(0, 21), IntSize(0, 18), IntSize(0, 15) };
    return sizes;
}

const int* RenderThemeSafari::buttonMargins(NSControlSize controlSize) const
{
    static const int margins[3][4] =
    {
        { 4, 6, 7, 6 },
        { 4, 5, 6, 5 },
        { 0, 1, 1, 1 },
    };
    return margins[controlSize];
}

void RenderThemeSafari::setButtonSize(RenderStyle* style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;

    // Use the font size to determine the intrinsic width of the control.
    setSizeFromFont(style, buttonSizes());
}

bool RenderThemeSafari::paintButton(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
{
    // We inflate the rect as needed to account for padding included in the cell to accommodate the button
    // shadow.  We don't consider this part of the bounds of the control in WebKit.

    NSControlSize controlSize = controlSizeFromRect(r, buttonSizes());
    IntRect inflatedRect = r;

    ThemePart part;
    if (r.height() <= buttonSizes()[NSRegularControlSize].height()) {
        // Push button
        part = PushButtonPart;

        IntSize size = buttonSizes()[controlSize];
        size.setWidth(r.width());

        // Center the button within the available space.
        if (inflatedRect.height() > size.height()) {
            inflatedRect.setY(inflatedRect.y() + (inflatedRect.height() - size.height()) / 2);
            inflatedRect.setHeight(size.height());
        }

        // Now inflate it to account for the shadow.
        inflatedRect = inflateRect(inflatedRect, size, buttonMargins(controlSize));
    } else
        part = SquareButtonPart;

    paintThemePart(part, paintInfo.context->platformContext(), inflatedRect, controlSize, determineState(o));
    return false;
}

bool RenderThemeSafari::paintTextField(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
{
    paintThemePart(TextFieldPart, paintInfo.context->platformContext(), r, (NSControlSize)0, determineState(o) & ~FocusedState);
    return false;
}

void RenderThemeSafari::adjustTextFieldStyle(CSSStyleSelector*, RenderStyle*, Element*) const
{
}

bool RenderThemeSafari::paintTextArea(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
{
    paintThemePart(TextAreaPart, paintInfo.context->platformContext(), r, (NSControlSize)0, determineState(o) & ~FocusedState);
    return false;
}

void RenderThemeSafari::adjustTextAreaStyle(CSSStyleSelector*, RenderStyle*, Element*) const
{
}

const int* RenderThemeSafari::popupButtonMargins(NSControlSize size) const
{
    static const int margins[3][4] =
    {
        { 2, 3, 3, 3 },
        { 1, 3, 3, 3 },
        { 0, 1, 0, 1 }
    };
    return margins[size];
}

const IntSize* RenderThemeSafari::popupButtonSizes() const
{
    static const IntSize sizes[3] = { IntSize(0, 21), IntSize(0, 18), IntSize(0, 15) };
    return sizes;
}

const int* RenderThemeSafari::popupButtonPadding(NSControlSize size) const
{
    static const int padding[3][4] =
    {
        { 2, 26, 3, 8 },
        { 2, 23, 3, 8 },
        { 2, 22, 3, 10 }
    };
    return padding[size];
}

bool RenderThemeSafari::paintMenuList(RenderObject* o, const RenderObject::PaintInfo& info, const IntRect& r)
{
    NSControlSize controlSize = controlSizeFromRect(r, popupButtonSizes());
    IntRect inflatedRect = r;
    IntSize size = popupButtonSizes()[controlSize];
    size.setWidth(r.width());

    // Now inflate it to account for the shadow.
    if (r.width() >= minimumMenuListSize(o->style()))
        inflatedRect = inflateRect(inflatedRect, size, popupButtonMargins(controlSize));

    paintThemePart(DropDownButtonPart, info.context->platformContext(), inflatedRect, controlSize, determineState(o));

    return false;
}

const float baseFontSize = 11.0f;
const float baseArrowHeight = 5.0f;
const float baseArrowWidth = 7.0f;
const int arrowPaddingLeft = 5;
const int arrowPaddingRight = 5;
const int paddingBeforeSeparator = 4;
const int baseBorderRadius = 5;
const int styledPopupPaddingLeft = 8;
const int styledPopupPaddingTop = 1;
const int styledPopupPaddingBottom = 2;

static void TopGradientInterpolate(void* info, const CGFloat* inData, CGFloat* outData)
{
    static float dark[4] = { 1.0f, 1.0f, 1.0f, 0.4f };
    static float light[4] = { 1.0f, 1.0f, 1.0f, 0.15f };
    float a = inData[0];
    int i = 0;
    for (i = 0; i < 4; i++)
        outData[i] = (1.0f - a) * dark[i] + a * light[i];
}

static void BottomGradientInterpolate(void* info, const CGFloat* inData, CGFloat* outData)
{
    static float dark[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
    static float light[4] = { 1.0f, 1.0f, 1.0f, 0.3f };
    float a = inData[0];
    int i = 0;
    for (i = 0; i < 4; i++)
        outData[i] = (1.0f - a) * dark[i] + a * light[i];
}

static void MainGradientInterpolate(void* info, const CGFloat* inData, CGFloat* outData)
{
    static float dark[4] = { 0.0f, 0.0f, 0.0f, 0.15f };
    static float light[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float a = inData[0];
    int i = 0;
    for (i = 0; i < 4; i++)
        outData[i] = (1.0f - a) * dark[i] + a * light[i];
}

static void TrackGradientInterpolate(void* info, const CGFloat* inData, CGFloat* outData)
{
    static float dark[4] = { 0.0f, 0.0f, 0.0f, 0.678f };
    static float light[4] = { 0.0f, 0.0f, 0.0f, 0.13f };
    float a = inData[0];
    int i = 0;
    for (i = 0; i < 4; i++)
        outData[i] = (1.0f - a) * dark[i] + a * light[i];
}

void RenderThemeSafari::paintMenuListButtonGradients(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
{
    CGContextRef context = paintInfo.context->platformContext();

    paintInfo.context->save();

    int radius = o->style()->borderTopLeftRadius().width();

    RetainPtr<CGColorSpaceRef> cspace(AdoptCF, CGColorSpaceCreateDeviceRGB());

    FloatRect topGradient(r.x(), r.y(), r.width(), r.height() / 2.0f);
    struct CGFunctionCallbacks topCallbacks = { 0, TopGradientInterpolate, NULL };
    RetainPtr<CGFunctionRef> topFunction(AdoptCF, CGFunctionCreate(NULL, 1, NULL, 4, NULL, &topCallbacks));
    RetainPtr<CGShadingRef> topShading(AdoptCF, CGShadingCreateAxial(cspace.get(), CGPointMake(topGradient.x(), topGradient.y()), CGPointMake(topGradient.x(), topGradient.bottom()), topFunction.get(), false, false));

    FloatRect bottomGradient(r.x() + radius, r.y() + r.height() / 2.0f, r.width() - 2.0f * radius, r.height() / 2.0f);
    struct CGFunctionCallbacks bottomCallbacks = { 0, BottomGradientInterpolate, NULL };
    RetainPtr<CGFunctionRef> bottomFunction(AdoptCF, CGFunctionCreate(NULL, 1, NULL, 4, NULL, &bottomCallbacks));
    RetainPtr<CGShadingRef> bottomShading(AdoptCF, CGShadingCreateAxial(cspace.get(), CGPointMake(bottomGradient.x(),  bottomGradient.y()), CGPointMake(bottomGradient.x(), bottomGradient.bottom()), bottomFunction.get(), false, false));

    struct CGFunctionCallbacks mainCallbacks = { 0, MainGradientInterpolate, NULL };
    RetainPtr<CGFunctionRef> mainFunction(AdoptCF, CGFunctionCreate(NULL, 1, NULL, 4, NULL, &mainCallbacks));
    RetainPtr<CGShadingRef> mainShading(AdoptCF, CGShadingCreateAxial(cspace.get(), CGPointMake(r.x(),  r.y()), CGPointMake(r.x(), r.bottom()), mainFunction.get(), false, false));

    RetainPtr<CGShadingRef> leftShading(AdoptCF, CGShadingCreateAxial(cspace.get(), CGPointMake(r.x(),  r.y()), CGPointMake(r.x() + radius, r.y()), mainFunction.get(), false, false));

    RetainPtr<CGShadingRef> rightShading(AdoptCF, CGShadingCreateAxial(cspace.get(), CGPointMake(r.right(),  r.y()), CGPointMake(r.right() - radius, r.y()), mainFunction.get(), false, false));
    paintInfo.context->save();
    CGContextClipToRect(context, r);
    paintInfo.context->addRoundedRectClip(r,
        o->style()->borderTopLeftRadius(), o->style()->borderTopRightRadius(),
        o->style()->borderBottomLeftRadius(), o->style()->borderBottomRightRadius());
    CGContextDrawShading(context, mainShading.get());
    paintInfo.context->restore();

    paintInfo.context->save();
    CGContextClipToRect(context, topGradient);
    paintInfo.context->addRoundedRectClip(enclosingIntRect(topGradient),
        o->style()->borderTopLeftRadius(), o->style()->borderTopRightRadius(),
        IntSize(), IntSize());
    CGContextDrawShading(context, topShading.get());
    paintInfo.context->restore();

    paintInfo.context->save();
    CGContextClipToRect(context, bottomGradient);
    paintInfo.context->addRoundedRectClip(enclosingIntRect(bottomGradient),
        IntSize(), IntSize(),
        o->style()->borderBottomLeftRadius(), o->style()->borderBottomRightRadius());
    CGContextDrawShading(context, bottomShading.get());
    paintInfo.context->restore();

    paintInfo.context->save();
    CGContextClipToRect(context, r);
    paintInfo.context->addRoundedRectClip(r,
        o->style()->borderTopLeftRadius(), o->style()->borderTopRightRadius(),
        o->style()->borderBottomLeftRadius(), o->style()->borderBottomRightRadius());
    CGContextDrawShading(context, leftShading.get());
    CGContextDrawShading(context, rightShading.get());
    paintInfo.context->restore();

    paintInfo.context->restore();
}

bool RenderThemeSafari::paintMenuListButton(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
{
    paintInfo.context->save();

    IntRect bounds = IntRect(r.x() + o->style()->borderLeftWidth(),
                             r.y() + o->style()->borderTopWidth(),
                             r.width() - o->style()->borderLeftWidth() - o->style()->borderRightWidth(),
                             r.height() - o->style()->borderTopWidth() - o->style()->borderBottomWidth());
    // Draw the gradients to give the styled popup menu a button appearance
    paintMenuListButtonGradients(o, paintInfo, bounds);
    
    // Since we actually know the size of the control here, we restrict the font scale to make sure the arrow will fit vertically in the bounds
    float fontScale = min(o->style()->fontSize() / baseFontSize, bounds.height() / baseArrowHeight);
    float centerY = bounds.y() + bounds.height() / 2.0f;
    float arrowHeight = baseArrowHeight * fontScale;
    float arrowWidth = baseArrowWidth * fontScale;
    float leftEdge = bounds.right() - arrowPaddingRight - arrowWidth;

    if (bounds.width() < arrowWidth + arrowPaddingLeft)
        return false;

    paintInfo.context->setFillColor(o->style()->color());
    paintInfo.context->setStrokeColor(NoStroke);

    FloatPoint arrow[3];
    arrow[0] = FloatPoint(leftEdge, centerY - arrowHeight / 2.0f);
    arrow[1] = FloatPoint(leftEdge + arrowWidth, centerY - arrowHeight / 2.0f);
    arrow[2] = FloatPoint(leftEdge + arrowWidth / 2.0f, centerY + arrowHeight / 2.0f);

    // Draw the arrow
    paintInfo.context->drawConvexPolygon(3, arrow, true);

    Color leftSeparatorColor(0, 0, 0, 40);
    Color rightSeparatorColor(255, 255, 255, 40);
    
    // FIXME: Should the separator thickness and space be scaled up by fontScale?
    int separatorSpace = 2;
    int leftEdgeOfSeparator = static_cast<int>(leftEdge - arrowPaddingLeft); // FIXME: Round?

    // Draw the separator to the left of the arrows
    paintInfo.context->setStrokeThickness(1.0f);
    paintInfo.context->setStrokeStyle(SolidStroke);
    paintInfo.context->setStrokeColor(leftSeparatorColor);
    paintInfo.context->drawLine(IntPoint(leftEdgeOfSeparator, bounds.y()),
                                IntPoint(leftEdgeOfSeparator, bounds.bottom()));

    paintInfo.context->setStrokeColor(rightSeparatorColor);
    paintInfo.context->drawLine(IntPoint(leftEdgeOfSeparator + separatorSpace, bounds.y()),
                                IntPoint(leftEdgeOfSeparator + separatorSpace, bounds.bottom()));

    paintInfo.context->restore();
    return false;
}

void RenderThemeSafari::adjustMenuListStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    NSControlSize controlSize = controlSizeForFont(style);

    style->resetBorder();
    style->resetPadding();
    
    // Height is locked to auto.
    style->setHeight(Length(Auto));

    // White-space is locked to pre
    style->setWhiteSpace(PRE);

    // Set the foreground color to black or gray when we have the aqua look.
    // Cast to RGB32 is to work around a compiler bug.
    style->setColor(e->isEnabled() ? static_cast<RGBA32>(Color::black) : Color::darkGray);

    // Set the button's vertical size.
    setButtonSize(style);

    // Our font is locked to the appropriate system font size for the control.  To clarify, we first use the CSS-specified font to figure out
    // a reasonable control size, but once that control size is determined, we throw that font away and use the appropriate
    // system font for the control size instead.
    setFontFromControlSize(selector, style, controlSize);
}

int RenderThemeSafari::popupInternalPaddingLeft(RenderStyle* style) const
{
    if (style->appearance() == MenulistAppearance)
        return popupButtonPadding(controlSizeForFont(style))[leftPadding];
    if (style->appearance() == MenulistButtonAppearance)
        return styledPopupPaddingLeft;
    return 0;
}

int RenderThemeSafari::popupInternalPaddingRight(RenderStyle* style) const
{
    if (style->appearance() == MenulistAppearance)
        return popupButtonPadding(controlSizeForFont(style))[rightPadding];
    if (style->appearance() == MenulistButtonAppearance) {
        float fontScale = style->fontSize() / baseFontSize;
        float arrowWidth = baseArrowWidth * fontScale;
        return static_cast<int>(ceilf(arrowWidth + arrowPaddingLeft + arrowPaddingRight + paddingBeforeSeparator));
    }
    return 0;
}

int RenderThemeSafari::popupInternalPaddingTop(RenderStyle* style) const
{
    if (style->appearance() == MenulistAppearance)
        return popupButtonPadding(controlSizeForFont(style))[topPadding];
    if (style->appearance() == MenulistButtonAppearance)
        return styledPopupPaddingTop;
    return 0;
}

int RenderThemeSafari::popupInternalPaddingBottom(RenderStyle* style) const
{
    if (style->appearance() == MenulistAppearance)
        return popupButtonPadding(controlSizeForFont(style))[bottomPadding];
    if (style->appearance() == MenulistButtonAppearance)
        return styledPopupPaddingBottom;
    return 0;
}

void RenderThemeSafari::adjustMenuListButtonStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    float fontScale = style->fontSize() / baseFontSize;
    
    style->resetPadding();
    style->setBorderRadius(IntSize(int(baseBorderRadius + fontScale - 1), int(baseBorderRadius + fontScale - 1))); // FIXME: Round up?

    const int minHeight = 15;
    style->setMinHeight(Length(minHeight, Fixed));
    
    style->setLineHeight(RenderStyle::initialLineHeight());
}

const IntSize* RenderThemeSafari::menuListSizes() const
{
    static const IntSize sizes[3] = { IntSize(9, 0), IntSize(5, 0), IntSize(0, 0) };
    return sizes;
}

int RenderThemeSafari::minimumMenuListSize(RenderStyle* style) const
{
    return sizeForSystemFont(style, menuListSizes()).width();
}

const int trackWidth = 5;
const int trackRadius = 2;

bool RenderThemeSafari::paintSliderTrack(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
{
    IntRect bounds = r;

    if (o->style()->appearance() ==  SliderHorizontalAppearance) {
        bounds.setHeight(trackWidth);
        bounds.setY(r.y() + r.height() / 2 - trackWidth / 2);
    } else if (o->style()->appearance() == SliderVerticalAppearance) {
        bounds.setWidth(trackWidth);
        bounds.setX(r.x() + r.width() / 2 - trackWidth / 2);
    }

    CGContextRef context = paintInfo.context->platformContext();
    RetainPtr<CGColorSpaceRef> cspace(AdoptCF, CGColorSpaceCreateDeviceRGB());

    paintInfo.context->save();
    CGContextClipToRect(context, bounds);

    struct CGFunctionCallbacks mainCallbacks = { 0, TrackGradientInterpolate, NULL };
    RetainPtr<CGFunctionRef> mainFunction(AdoptCF, CGFunctionCreate(NULL, 1, NULL, 4, NULL, &mainCallbacks));
    RetainPtr<CGShadingRef> mainShading;
    if (o->style()->appearance() == SliderVerticalAppearance)
        mainShading.adoptCF(CGShadingCreateAxial(cspace.get(), CGPointMake(bounds.x(),  bounds.bottom()), CGPointMake(bounds.right(), bounds.bottom()), mainFunction.get(), false, false));
    else
        mainShading.adoptCF(CGShadingCreateAxial(cspace.get(), CGPointMake(bounds.x(),  bounds.y()), CGPointMake(bounds.x(), bounds.bottom()), mainFunction.get(), false, false));

    IntSize radius(trackRadius, trackRadius);
    paintInfo.context->addRoundedRectClip(bounds,
        radius, radius,
        radius, radius);
    CGContextDrawShading(context, mainShading.get());
    paintInfo.context->restore();
    
    return false;
}

void RenderThemeSafari::adjustSliderThumbStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const 
{ 
    style->setBoxShadow(0); 
} 

const float verticalSliderHeightPadding = 0.1f;

bool RenderThemeSafari::paintSliderThumb(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
{
    ASSERT(o->parent()->isSlider());

    bool pressed = static_cast<RenderSlider*>(o->parent())->inDragMode();
    ThemeControlState state = determineState(o->parent());
    state &= ~SafariTheme::PressedState;
    if (pressed)
        state |= SafariTheme::PressedState;

    paintThemePart(SliderThumbPart, paintInfo.context->platformContext(), r, NSSmallControlSize, state);
    return false;
}

const int sliderThumbWidth = 15;
const int sliderThumbHeight = 15;

void RenderThemeSafari::adjustSliderThumbSize(RenderObject* o) const
{
    if (o->style()->appearance() == SliderThumbHorizontalAppearance || o->style()->appearance() == SliderThumbVerticalAppearance) {
        o->style()->setWidth(Length(sliderThumbWidth, Fixed));
        o->style()->setHeight(Length(sliderThumbHeight, Fixed));
    }
}

bool RenderThemeSafari::paintSearchField(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect& r)
{
    paintThemePart(SearchFieldPart, paintInfo.context->platformContext(), r, controlSizeFromRect(r, searchFieldSizes()), determineState(o));
    return false;
}

const IntSize* RenderThemeSafari::searchFieldSizes() const
{
    static const IntSize sizes[3] = { IntSize(0, 22), IntSize(0, 19), IntSize(0, 15) };
    return sizes;
}

void RenderThemeSafari::setSearchFieldSize(RenderStyle* style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;
    
    // Use the font size to determine the intrinsic width of the control.
    setSizeFromFont(style, searchFieldSizes());
}

void RenderThemeSafari::adjustSearchFieldStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    // Override border.
    style->resetBorder();
    const short borderWidth = 2;
    style->setBorderLeftWidth(borderWidth);
    style->setBorderLeftStyle(INSET);
    style->setBorderRightWidth(borderWidth);
    style->setBorderRightStyle(INSET);
    style->setBorderBottomWidth(borderWidth);
    style->setBorderBottomStyle(INSET);
    style->setBorderTopWidth(borderWidth);
    style->setBorderTopStyle(INSET);    
    
    // Override height.
    style->setHeight(Length(Auto));
    setSearchFieldSize(style);
    
    // Override padding size to match AppKit text positioning.
    const int padding = 1;
    style->setPaddingLeft(Length(padding, Fixed));
    style->setPaddingRight(Length(padding, Fixed));
    style->setPaddingTop(Length(padding, Fixed));
    style->setPaddingBottom(Length(padding, Fixed));
    
    NSControlSize controlSize = controlSizeForFont(style);
    setFontFromControlSize(selector, style, controlSize);
}

bool RenderThemeSafari::paintSearchFieldCancelButton(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect&)
{
    Node* input = o->node()->shadowAncestorNode();
    ASSERT(input);
    RenderObject* renderer = input->renderer();
    ASSERT(renderer);

    IntRect searchRect = renderer->absoluteBoundingBoxRect();

    paintThemePart(SearchFieldCancelButtonPart, paintInfo.context->platformContext(), searchRect, controlSizeFromRect(searchRect, searchFieldSizes()), determineState(o));
    return false;
}

const IntSize* RenderThemeSafari::cancelButtonSizes() const
{
    static const IntSize sizes[3] = { IntSize(16, 13), IntSize(13, 11), IntSize(13, 9) };
    return sizes;
}

void RenderThemeSafari::adjustSearchFieldCancelButtonStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    IntSize size = sizeForSystemFont(style, cancelButtonSizes());
    style->setWidth(Length(size.width(), Fixed));
    style->setHeight(Length(size.height(), Fixed));
}

const IntSize* RenderThemeSafari::resultsButtonSizes() const
{
    static const IntSize sizes[3] = { IntSize(19, 13), IntSize(17, 11), IntSize(17, 9) };
    return sizes;
}

const int emptyResultsOffset = 9;
void RenderThemeSafari::adjustSearchFieldDecorationStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    IntSize size = sizeForSystemFont(style, resultsButtonSizes());
    style->setWidth(Length(size.width() - emptyResultsOffset, Fixed));
    style->setHeight(Length(size.height(), Fixed));
}

bool RenderThemeSafari::paintSearchFieldDecoration(RenderObject*, const RenderObject::PaintInfo&, const IntRect&)
{
    return false;
}

void RenderThemeSafari::adjustSearchFieldResultsDecorationStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    IntSize size = sizeForSystemFont(style, resultsButtonSizes());
    style->setWidth(Length(size.width(), Fixed));
    style->setHeight(Length(size.height(), Fixed));
}

bool RenderThemeSafari::paintSearchFieldResultsDecoration(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect&)
{
    Node* input = o->node()->shadowAncestorNode();
    ASSERT(input);
    RenderObject* renderer = input->renderer();
    ASSERT(renderer);

    IntRect searchRect = renderer->absoluteBoundingBoxRect();

    paintThemePart(SearchFieldResultsDecorationPart, paintInfo.context->platformContext(), searchRect, controlSizeFromRect(searchRect, searchFieldSizes()), determineState(o));
    return false;
}

const int resultsArrowWidth = 5;
void RenderThemeSafari::adjustSearchFieldResultsButtonStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const
{
    IntSize size = sizeForSystemFont(style, resultsButtonSizes());
    style->setWidth(Length(size.width() + resultsArrowWidth, Fixed));
    style->setHeight(Length(size.height(), Fixed));
}

bool RenderThemeSafari::paintSearchFieldResultsButton(RenderObject* o, const RenderObject::PaintInfo& paintInfo, const IntRect&)
{
    Node* input = o->node()->shadowAncestorNode();
    ASSERT(input);
    RenderObject* renderer = input->renderer();
    ASSERT(renderer);

    IntRect searchRect = renderer->absoluteBoundingBoxRect();

    paintThemePart(SearchFieldResultsButtonPart, paintInfo.context->platformContext(), searchRect, controlSizeFromRect(searchRect, searchFieldSizes()), determineState(o));
    return false;
}

} // namespace WebCore

#endif // defined(USE_SAFARI_THEME)
