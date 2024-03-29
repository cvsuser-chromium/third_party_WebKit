/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2012 Motorola Mobility Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DOMURL_h
#define DOMURL_h

#include "bindings/v8/ScriptWrappable.h"
#include "core/dom/DOMURLUtils.h"
#include "weborigin/KURL.h"
#include "wtf/Forward.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace WebCore {

class Blob;
class ExceptionState;
class ExecutionContext;
class URLRegistrable;

class DOMURL FINAL : public ScriptWrappable, public DOMURLUtils, public RefCounted<DOMURL> {

public:
    static PassRefPtr<DOMURL> create(const String& url, ExceptionState& es)
    {
        return adoptRef(new DOMURL(url, blankURL(), es));
    }
    static PassRefPtr<DOMURL> create(const String& url, const String& base, ExceptionState& es)
    {
        return adoptRef(new DOMURL(url, KURL(KURL(), base), es));
    }
    static PassRefPtr<DOMURL> create(const String& url, PassRefPtr<DOMURL> base, ExceptionState& es)
    {
        ASSERT(base);
        return adoptRef(new DOMURL(url, base->m_url, es));
    }

    static String createObjectURL(ExecutionContext*, Blob*);
    static void revokeObjectURL(ExecutionContext*, const String&);

    static String createPublicURL(ExecutionContext*, URLRegistrable*);

    virtual KURL url() const OVERRIDE { return m_url; }
    virtual void setURL(const KURL& url) OVERRIDE { m_url = url; }

    virtual String input() const OVERRIDE { return m_input; }
    virtual void setInput(const String&) OVERRIDE;

private:
    DOMURL(const String& url, const KURL& base, ExceptionState&);

    KURL m_url;
    String m_input;
};

} // namespace WebCore

#endif // DOMURL_h
