/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Simon Hausmann (hausmann@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
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
 */

#include "config.h"
#include "core/html/HTMLBodyElement.h"

#include "CSSValueKeywords.h"
#include "HTMLNames.h"
#include "bindings/v8/ScriptEventListener.h"
#include "core/css/CSSImageValue.h"
#include "core/css/CSSParser.h"
#include "core/css/StylePropertySet.h"
#include "core/dom/Attribute.h"
#include "core/events/ThreadLocalEventNames.h"
#include "core/html/HTMLFrameElementBase.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/frame/Frame.h"
#include "core/frame/FrameView.h"

namespace WebCore {

using namespace HTMLNames;

HTMLBodyElement::HTMLBodyElement(const QualifiedName& tagName, Document& document)
    : HTMLElement(tagName, document)
{
    ASSERT(hasTagName(bodyTag));
    ScriptWrappable::init(this);
}

PassRefPtr<HTMLBodyElement> HTMLBodyElement::create(Document& document)
{
    return adoptRef(new HTMLBodyElement(bodyTag, document));
}

PassRefPtr<HTMLBodyElement> HTMLBodyElement::create(const QualifiedName& tagName, Document& document)
{
    return adoptRef(new HTMLBodyElement(tagName, document));
}

HTMLBodyElement::~HTMLBodyElement()
{
}

bool HTMLBodyElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == backgroundAttr || name == marginwidthAttr || name == leftmarginAttr || name == marginheightAttr || name == topmarginAttr || name == bgcolorAttr || name == textAttr || name == bgpropertiesAttr)
        return true;
    return HTMLElement::isPresentationAttribute(name);
}

void HTMLBodyElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == backgroundAttr) {
        String url = stripLeadingAndTrailingHTMLSpaces(value);
        if (!url.isEmpty()) {
            RefPtr<CSSImageValue> imageValue = CSSImageValue::create(document().completeURL(url).string());
            imageValue->setInitiator(localName());
            style->setProperty(CSSProperty(CSSPropertyBackgroundImage, imageValue.release()));
        }
    } else if (name == marginwidthAttr || name == leftmarginAttr) {
        addHTMLLengthToStyle(style, CSSPropertyMarginRight, value);
        addHTMLLengthToStyle(style, CSSPropertyMarginLeft, value);
    } else if (name == marginheightAttr || name == topmarginAttr) {
        addHTMLLengthToStyle(style, CSSPropertyMarginBottom, value);
        addHTMLLengthToStyle(style, CSSPropertyMarginTop, value);
    } else if (name == bgcolorAttr) {
        addHTMLColorToStyle(style, CSSPropertyBackgroundColor, value);
    } else if (name == textAttr) {
        addHTMLColorToStyle(style, CSSPropertyColor, value);
    } else if (name == bgpropertiesAttr) {
        if (equalIgnoringCase(value, "fixed"))
           addPropertyToPresentationAttributeStyle(style, CSSPropertyBackgroundAttachment, CSSValueFixed);
    } else
        HTMLElement::collectStyleForPresentationAttribute(name, value, style);
}

void HTMLBodyElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == vlinkAttr || name == alinkAttr || name == linkAttr) {
        if (value.isNull()) {
            if (name == linkAttr)
                document().textLinkColors().resetLinkColor();
            else if (name == vlinkAttr)
                document().textLinkColors().resetVisitedLinkColor();
            else
                document().textLinkColors().resetActiveLinkColor();
        } else {
            RGBA32 color;
            if (CSSParser::parseColor(color, value, !document().inQuirksMode())) {
                if (name == linkAttr)
                    document().textLinkColors().setLinkColor(color);
                else if (name == vlinkAttr)
                    document().textLinkColors().setVisitedLinkColor(color);
                else
                    document().textLinkColors().setActiveLinkColor(color);
            }
        }

        setNeedsStyleRecalc();
    } else if (name == onloadAttr)
        document().setWindowAttributeEventListener(EventTypeNames::load, createAttributeEventListener(document().frame(), name, value));
    else if (name == onbeforeunloadAttr)
        document().setWindowAttributeEventListener(EventTypeNames::beforeunload, createAttributeEventListener(document().frame(), name, value));
    else if (name == onunloadAttr)
        document().setWindowAttributeEventListener(EventTypeNames::unload, createAttributeEventListener(document().frame(), name, value));
    else if (name == onpagehideAttr)
        document().setWindowAttributeEventListener(EventTypeNames::pagehide, createAttributeEventListener(document().frame(), name, value));
    else if (name == onpageshowAttr)
        document().setWindowAttributeEventListener(EventTypeNames::pageshow, createAttributeEventListener(document().frame(), name, value));
    else if (name == onpopstateAttr)
        document().setWindowAttributeEventListener(EventTypeNames::popstate, createAttributeEventListener(document().frame(), name, value));
    else if (name == onblurAttr)
        document().setWindowAttributeEventListener(EventTypeNames::blur, createAttributeEventListener(document().frame(), name, value));
    else if (name == onerrorAttr)
        document().setWindowAttributeEventListener(EventTypeNames::error, createAttributeEventListener(document().frame(), name, value));
    else if (name == onfocusAttr)
        document().setWindowAttributeEventListener(EventTypeNames::focus, createAttributeEventListener(document().frame(), name, value));
#if ENABLE(ORIENTATION_EVENTS)
    else if (name == onorientationchangeAttr)
        document().setWindowAttributeEventListener(EventTypeNames::orientationchange, createAttributeEventListener(document().frame(), name, value));
#endif
    else if (name == onhashchangeAttr)
        document().setWindowAttributeEventListener(EventTypeNames::hashchange, createAttributeEventListener(document().frame(), name, value));
    else if (name == onmessageAttr)
        document().setWindowAttributeEventListener(EventTypeNames::message, createAttributeEventListener(document().frame(), name, value));
    else if (name == onresizeAttr)
        document().setWindowAttributeEventListener(EventTypeNames::resize, createAttributeEventListener(document().frame(), name, value));
    else if (name == onscrollAttr)
        document().setWindowAttributeEventListener(EventTypeNames::scroll, createAttributeEventListener(document().frame(), name, value));
    else if (name == onselectionchangeAttr)
        document().setAttributeEventListener(EventTypeNames::selectionchange, createAttributeEventListener(document().frame(), name, value));
    else if (name == onstorageAttr)
        document().setWindowAttributeEventListener(EventTypeNames::storage, createAttributeEventListener(document().frame(), name, value));
    else if (name == ononlineAttr)
        document().setWindowAttributeEventListener(EventTypeNames::online, createAttributeEventListener(document().frame(), name, value));
    else if (name == onofflineAttr)
        document().setWindowAttributeEventListener(EventTypeNames::offline, createAttributeEventListener(document().frame(), name, value));
    else
        HTMLElement::parseAttribute(name, value);
}

Node::InsertionNotificationRequest HTMLBodyElement::insertedInto(ContainerNode* insertionPoint)
{
    HTMLElement::insertedInto(insertionPoint);
    if (insertionPoint->inDocument()) {
        // FIXME: It's surprising this is web compatible since it means a marginwidth
        // and marginheight attribute can magically appear on the <body> of all documents
        // embedded through <iframe> or <frame>.
        Element* ownerElement = document().ownerElement();
        if (ownerElement && ownerElement->isFrameElementBase()) {
            HTMLFrameElementBase* ownerFrameElement = toHTMLFrameElementBase(ownerElement);
            int marginWidth = ownerFrameElement->marginWidth();
            if (marginWidth != -1)
                setAttribute(marginwidthAttr, String::number(marginWidth));
            int marginHeight = ownerFrameElement->marginHeight();
            if (marginHeight != -1)
                setAttribute(marginheightAttr, String::number(marginHeight));
        }
    }
    return InsertionDone;
}

bool HTMLBodyElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == backgroundAttr || HTMLElement::isURLAttribute(attribute);
}

bool HTMLBodyElement::supportsFocus() const
{
    // This override is needed because the inherited method bails if the parent is editable.
    // The <body> should be focusable even if <html> is editable.
    return rendererIsEditable() || HTMLElement::supportsFocus();
}

static int adjustForZoom(int value, Document* document)
{
    Frame* frame = document->frame();
    float zoomFactor = frame->pageZoomFactor();
    if (zoomFactor == 1)
        return value;
    // Needed because of truncation (rather than rounding) when scaling up.
    if (zoomFactor > 1)
        value++;
    return static_cast<int>(value / zoomFactor);
}

int HTMLBodyElement::scrollLeft()
{
    Document& document = this->document();

    // FIXME: There are cases where body.scrollLeft is allowed to return
    // non-zero values in both quirks and strict mode. It happens when
    // <body> has an overflow that is not the Frame overflow.
    // http://dev.w3.org/csswg/cssom-view/#dom-element-scrollleft
    if (!document.inQuirksMode())
        UseCounter::countDeprecation(&document, UseCounter::ScrollLeftBodyNotQuirksMode);

    document.updateLayoutIgnorePendingStylesheets();
    FrameView* view = document.view();
    return view ? adjustForZoom(view->scrollX(), &document) : 0;
}

void HTMLBodyElement::setScrollLeft(int scrollLeft)
{
    Document& document = this->document();

    if (!document.inQuirksMode())
        UseCounter::countDeprecation(&document, UseCounter::ScrollLeftBodyNotQuirksMode);

    document.updateLayoutIgnorePendingStylesheets();
    Frame* frame = document.frame();
    if (!frame)
        return;
    FrameView* view = frame->view();
    if (!view)
        return;
    view->setScrollPosition(IntPoint(static_cast<int>(scrollLeft * frame->pageZoomFactor()), view->scrollY()));
}

int HTMLBodyElement::scrollTop()
{
    Document& document = this->document();

    // FIXME: There are cases where body.scrollTop is allowed to return
    // non-zero values in both quirks and strict mode. It happens when
    // body has a overflow that is not the Frame overflow.
    // http://dev.w3.org/csswg/cssom-view/#dom-element-scrolltop
    if (!document.inQuirksMode())
        UseCounter::countDeprecation(&document, UseCounter::ScrollTopBodyNotQuirksMode);

    document.updateLayoutIgnorePendingStylesheets();
    FrameView* view = document.view();
    return view ? adjustForZoom(view->scrollY(), &document) : 0;
}

void HTMLBodyElement::setScrollTop(int scrollTop)
{
    Document& document = this->document();

    if (!document.inQuirksMode())
        UseCounter::countDeprecation(&document, UseCounter::ScrollTopBodyNotQuirksMode);

    document.updateLayoutIgnorePendingStylesheets();
    Frame* frame = document.frame();
    if (!frame)
        return;
    FrameView* view = frame->view();
    if (!view)
        return;
    view->setScrollPosition(IntPoint(view->scrollX(), static_cast<int>(scrollTop * frame->pageZoomFactor())));
}

int HTMLBodyElement::scrollHeight()
{
    // Update the document's layout.
    Document& document = this->document();
    document.updateLayoutIgnorePendingStylesheets();
    FrameView* view = document.view();
    return view ? adjustForZoom(view->contentsHeight(), &document) : 0;
}

int HTMLBodyElement::scrollWidth()
{
    // Update the document's layout.
    Document& document = this->document();
    document.updateLayoutIgnorePendingStylesheets();
    FrameView* view = document.view();
    return view ? adjustForZoom(view->contentsWidth(), &document) : 0;
}

void HTMLBodyElement::addSubresourceAttributeURLs(ListHashSet<KURL>& urls) const
{
    HTMLElement::addSubresourceAttributeURLs(urls);

    addSubresourceURL(urls, document().completeURL(getAttribute(backgroundAttr)));
}

} // namespace WebCore
