/*
 * Copyright (C) 2013 Google Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/track/VTTRegion.h"

#include "bindings/v8/ExceptionState.h"
#include "bindings/v8/ExceptionStatePlaceholder.h"
#include "core/dom/ClientRect.h"
#include "core/dom/DOMTokenList.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/track/WebVTTParser.h"
#include "core/rendering/RenderInline.h"
#include "core/rendering/RenderObject.h"
#include "platform/Logging.h"
#include "wtf/MathExtras.h"
#include "wtf/text/StringBuilder.h"

namespace WebCore {

// The following values default values are defined within the WebVTT Regions Spec.
// https://dvcs.w3.org/hg/text-tracks/raw-file/default/608toVTT/region.html

// The region occupies by default 100% of the width of the video viewport.
static const float defaultWidth = 100;

// The region has, by default, 3 lines of text.
static const long defaultHeightInLines = 3;

// The region and viewport are anchored in the bottom left corner.
static const float defaultAnchorPointX = 0;
static const float defaultAnchorPointY = 100;

// The region doesn't have scrolling text, by default.
static const bool defaultScroll = false;

// Default region line-height (vh units)
static const float lineHeight = 5.33;

// Default scrolling animation time period (s).
static const float scrollTime = 0.433;

VTTRegion::VTTRegion()
    : m_id(emptyString())
    , m_width(defaultWidth)
    , m_heightInLines(defaultHeightInLines)
    , m_regionAnchor(FloatPoint(defaultAnchorPointX, defaultAnchorPointY))
    , m_viewportAnchor(FloatPoint(defaultAnchorPointX, defaultAnchorPointY))
    , m_scroll(defaultScroll)
    , m_track(0)
    , m_currentTop(0)
    , m_scrollTimer(this, &VTTRegion::scrollTimerFired)
{
}

VTTRegion::~VTTRegion()
{
}

void VTTRegion::setTrack(TextTrack* track)
{
    m_track = track;
}

void VTTRegion::setId(const String& id)
{
    m_id = id;
}

void VTTRegion::setWidth(double value, ExceptionState& es)
{
    if (std::isinf(value) || std::isnan(value)) {
        es.throwUninformativeAndGenericTypeError();
        return;
    }

    if (value < 0 || value > 100) {
        es.throwUninformativeAndGenericDOMException(IndexSizeError);
        return;
    }

    m_width = value;
}

void VTTRegion::setHeight(long value, ExceptionState& es)
{
    if (value < 0) {
        es.throwUninformativeAndGenericDOMException(IndexSizeError);
        return;
    }

    m_heightInLines = value;
}

void VTTRegion::setRegionAnchorX(double value, ExceptionState& es)
{
    if (std::isinf(value) || std::isnan(value)) {
        es.throwUninformativeAndGenericTypeError();
        return;
    }

    if (value < 0 || value > 100) {
        es.throwUninformativeAndGenericDOMException(IndexSizeError);
        return;
    }

    m_regionAnchor.setX(value);
}

void VTTRegion::setRegionAnchorY(double value, ExceptionState& es)
{
    if (std::isinf(value) || std::isnan(value)) {
        es.throwUninformativeAndGenericTypeError();
        return;
    }

    if (value < 0 || value > 100) {
        es.throwUninformativeAndGenericDOMException(IndexSizeError);
        return;
    }

    m_regionAnchor.setY(value);
}

void VTTRegion::setViewportAnchorX(double value, ExceptionState& es)
{
    if (std::isinf(value) || std::isnan(value)) {
        es.throwUninformativeAndGenericTypeError();
        return;
    }

    if (value < 0 || value > 100) {
        es.throwUninformativeAndGenericDOMException(IndexSizeError);
        return;
    }

    m_viewportAnchor.setX(value);
}

void VTTRegion::setViewportAnchorY(double value, ExceptionState& es)
{
    if (std::isinf(value) || std::isnan(value)) {
        es.throwUninformativeAndGenericTypeError();
        return;
    }

    if (value < 0 || value > 100) {
        es.throwUninformativeAndGenericDOMException(IndexSizeError);
        return;
    }

    m_viewportAnchor.setY(value);
}

const AtomicString VTTRegion::scroll() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, upScrollValueKeyword, ("up", AtomicString::ConstructFromLiteral));

    if (m_scroll)
        return upScrollValueKeyword;

    return "";
}

void VTTRegion::setScroll(const AtomicString& value, ExceptionState& es)
{
    DEFINE_STATIC_LOCAL(const AtomicString, upScrollValueKeyword, ("up", AtomicString::ConstructFromLiteral));

    if (value != emptyString() && value != upScrollValueKeyword) {
        es.throwUninformativeAndGenericDOMException(SyntaxError);
        return;
    }

    m_scroll = value == upScrollValueKeyword;
}

void VTTRegion::updateParametersFromRegion(VTTRegion* region)
{
    m_heightInLines = region->height();
    m_width = region->width();

    m_regionAnchor = FloatPoint(region->regionAnchorX(), region->regionAnchorY());
    m_viewportAnchor = FloatPoint(region->viewportAnchorX(), region->viewportAnchorY());

    setScroll(region->scroll(), ASSERT_NO_EXCEPTION);
}

void VTTRegion::setRegionSettings(const String& input)
{
    m_settings = input;
    unsigned position = 0;

    while (position < input.length()) {
        while (position < input.length() && WebVTTParser::isValidSettingDelimiter(input[position]))
            position++;

        if (position >= input.length())
            break;

        parseSetting(input, &position);
    }
}

VTTRegion::RegionSetting VTTRegion::getSettingFromString(const String& setting)
{
    DEFINE_STATIC_LOCAL(const AtomicString, idKeyword, ("id", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, heightKeyword, ("height", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, widthKeyword, ("width", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, regionAnchorKeyword, ("regionanchor", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, viewportAnchorKeyword, ("viewportanchor", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, scrollKeyword, ("scroll", AtomicString::ConstructFromLiteral));

    if (setting == idKeyword)
        return Id;
    if (setting == heightKeyword)
        return Height;
    if (setting == widthKeyword)
        return Width;
    if (setting == viewportAnchorKeyword)
        return ViewportAnchor;
    if (setting == regionAnchorKeyword)
        return RegionAnchor;
    if (setting == scrollKeyword)
        return Scroll;

    return None;
}

void VTTRegion::parseSettingValue(RegionSetting setting, const String& value)
{
    DEFINE_STATIC_LOCAL(const AtomicString, scrollUpValueKeyword, ("up", AtomicString::ConstructFromLiteral));

    bool isValidSetting;
    String numberAsString;
    int number;
    unsigned position;
    FloatPoint anchorPosition;

    switch (setting) {
    case Id:
        if (value.find("-->") == kNotFound)
            m_id = value;
        break;
    case Width:
        number = WebVTTParser::parseFloatPercentageValue(value, isValidSetting);
        if (isValidSetting)
            m_width = number;
        else
            LOG(Media, "VTTRegion::parseSettingValue, invalid Width");
        break;
    case Height:
        position = 0;

        numberAsString = WebVTTParser::collectDigits(value, &position);
        number = value.toInt(&isValidSetting);

        if (isValidSetting && number >= 0)
            m_heightInLines = number;
        else
            LOG(Media, "VTTRegion::parseSettingValue, invalid Height");
        break;
    case RegionAnchor:
        anchorPosition = WebVTTParser::parseFloatPercentageValuePair(value, ',', isValidSetting);
        if (isValidSetting)
            m_regionAnchor = anchorPosition;
        else
            LOG(Media, "VTTRegion::parseSettingValue, invalid RegionAnchor");
        break;
    case ViewportAnchor:
        anchorPosition = WebVTTParser::parseFloatPercentageValuePair(value, ',', isValidSetting);
        if (isValidSetting)
            m_viewportAnchor = anchorPosition;
        else
            LOG(Media, "VTTRegion::parseSettingValue, invalid ViewportAnchor");
        break;
    case Scroll:
        if (value == scrollUpValueKeyword)
            m_scroll = true;
        else
            LOG(Media, "VTTRegion::parseSettingValue, invalid Scroll");
        break;
    case None:
        break;
    }
}

void VTTRegion::parseSetting(const String& input, unsigned* position)
{
    String setting = WebVTTParser::collectWord(input, position);

    size_t equalOffset = setting.find('=', 1);
    if (equalOffset == kNotFound || !equalOffset || equalOffset == setting.length() - 1)
        return;

    RegionSetting name = getSettingFromString(setting.substring(0, equalOffset));
    String value = setting.substring(equalOffset + 1, setting.length() - 1);

    parseSettingValue(name, value);
}

const AtomicString& VTTRegion::textTrackCueContainerShadowPseudoId()
{
    DEFINE_STATIC_LOCAL(const AtomicString, trackRegionCueContainerPseudoId,
        ("-webkit-media-text-track-region-container", AtomicString::ConstructFromLiteral));

    return trackRegionCueContainerPseudoId;
}

const AtomicString& VTTRegion::textTrackCueContainerScrollingClass()
{
    DEFINE_STATIC_LOCAL(const AtomicString, trackRegionCueContainerScrollingClass,
        ("scrolling", AtomicString::ConstructFromLiteral));

    return trackRegionCueContainerScrollingClass;
}

const AtomicString& VTTRegion::textTrackRegionShadowPseudoId()
{
    DEFINE_STATIC_LOCAL(const AtomicString, trackRegionShadowPseudoId,
        ("-webkit-media-text-track-region", AtomicString::ConstructFromLiteral));

    return trackRegionShadowPseudoId;
}

PassRefPtr<HTMLDivElement> VTTRegion::getDisplayTree(Document& document)
{
    if (!m_regionDisplayTree) {
        m_regionDisplayTree = HTMLDivElement::create(document);
        prepareRegionDisplayTree();
    }

    return m_regionDisplayTree;
}

void VTTRegion::willRemoveTextTrackCueBox(TextTrackCueBox* box)
{
    LOG(Media, "VTTRegion::willRemoveTextTrackCueBox");
    ASSERT(m_cueContainer->contains(box));

    double boxHeight = box->getBoundingClientRect()->bottom() - box->getBoundingClientRect()->top();

    m_cueContainer->classList()->remove(textTrackCueContainerScrollingClass(), ASSERT_NO_EXCEPTION);

    m_currentTop += boxHeight;
    m_cueContainer->setInlineStyleProperty(CSSPropertyTop, m_currentTop, CSSPrimitiveValue::CSS_PX);
}


void VTTRegion::appendTextTrackCueBox(PassRefPtr<TextTrackCueBox> displayBox)
{
    ASSERT(m_cueContainer);

    if (m_cueContainer->contains(displayBox.get()))
        return;

    m_cueContainer->appendChild(displayBox);
    displayLastTextTrackCueBox();
}

void VTTRegion::displayLastTextTrackCueBox()
{
    LOG(Media, "VTTRegion::displayLastTextTrackCueBox");
    ASSERT(m_cueContainer);

    // FIXME: This should not be causing recalc styles in a loop to set the "top" css
    // property to move elements. We should just scroll the text track cues on the
    // compositor with an animation.

    if (m_scrollTimer.isActive())
        return;

    // If it's a scrolling region, add the scrolling class.
    if (isScrollingRegion())
        m_cueContainer->classList()->add(textTrackCueContainerScrollingClass(), ASSERT_NO_EXCEPTION);

    float regionBottom = m_regionDisplayTree->getBoundingClientRect()->bottom();

    // Find first cue that is not entirely displayed and scroll it upwards.
    for (size_t i = 0; i < m_cueContainer->childNodeCount() && !m_scrollTimer.isActive(); ++i) {
        float childTop = toHTMLDivElement(m_cueContainer->childNode(i))->getBoundingClientRect()->top();
        float childBottom = toHTMLDivElement(m_cueContainer->childNode(i))->getBoundingClientRect()->bottom();

        if (regionBottom >= childBottom)
            continue;

        float height = childBottom - childTop;

        m_currentTop -= std::min(height, childBottom - regionBottom);
        m_cueContainer->setInlineStyleProperty(CSSPropertyTop, m_currentTop, CSSPrimitiveValue::CSS_PX);

        startTimer();
    }
}

void VTTRegion::prepareRegionDisplayTree()
{
    ASSERT(m_regionDisplayTree);

    // 7.2 Prepare region CSS boxes

    // FIXME: Change the code below to use viewport units when
    // http://crbug/244618 is fixed.

    // Let regionWidth be the text track region width.
    // Let width be 'regionWidth vw' ('vw' is a CSS unit)
    m_regionDisplayTree->setInlineStyleProperty(CSSPropertyWidth,
        m_width, CSSPrimitiveValue::CSS_PERCENTAGE);

    // Let lineHeight be '0.0533vh' ('vh' is a CSS unit) and regionHeight be
    // the text track region height. Let height be 'lineHeight' multiplied
    // by regionHeight.
    double height = lineHeight * m_heightInLines;
    m_regionDisplayTree->setInlineStyleProperty(CSSPropertyHeight,
        height, CSSPrimitiveValue::CSS_VH);

    // Let viewportAnchorX be the x dimension of the text track region viewport
    // anchor and regionAnchorX be the x dimension of the text track region
    // anchor. Let leftOffset be regionAnchorX multiplied by width divided by
    // 100.0. Let left be leftOffset subtracted from 'viewportAnchorX vw'.
    double leftOffset = m_regionAnchor.x() * m_width / 100;
    m_regionDisplayTree->setInlineStyleProperty(CSSPropertyLeft,
        m_viewportAnchor.x() - leftOffset,
        CSSPrimitiveValue::CSS_PERCENTAGE);

    // Let viewportAnchorY be the y dimension of the text track region viewport
    // anchor and regionAnchorY be the y dimension of the text track region
    // anchor. Let topOffset be regionAnchorY multiplied by height divided by
    // 100.0. Let top be topOffset subtracted from 'viewportAnchorY vh'.
    double topOffset = m_regionAnchor.y() * height / 100;
    m_regionDisplayTree->setInlineStyleProperty(CSSPropertyTop,
        m_viewportAnchor.y() - topOffset,
        CSSPrimitiveValue::CSS_PERCENTAGE);


    // The cue container is used to wrap the cues and it is the object which is
    // gradually scrolled out as multiple cues are appended to the region.
    m_cueContainer = HTMLDivElement::create(m_regionDisplayTree->document());
    m_cueContainer->setInlineStyleProperty(CSSPropertyTop,
        0.0,
        CSSPrimitiveValue::CSS_PX);

    m_cueContainer->setPart(textTrackCueContainerShadowPseudoId());
    m_regionDisplayTree->appendChild(m_cueContainer);

    // 7.5 Every WebVTT region object is initialised with the following CSS
    m_regionDisplayTree->setPart(textTrackRegionShadowPseudoId());
}

void VTTRegion::startTimer()
{
    LOG(Media, "VTTRegion::startTimer");

    if (m_scrollTimer.isActive())
        return;

    double duration = isScrollingRegion() ? scrollTime : 0;
    m_scrollTimer.startOneShot(duration);
}

void VTTRegion::stopTimer()
{
    LOG(Media, "VTTRegion::stopTimer");

    if (m_scrollTimer.isActive())
        m_scrollTimer.stop();
}

void VTTRegion::scrollTimerFired(Timer<VTTRegion>*)
{
    LOG(Media, "VTTRegion::scrollTimerFired");

    stopTimer();
    displayLastTextTrackCueBox();
}

} // namespace WebCore
