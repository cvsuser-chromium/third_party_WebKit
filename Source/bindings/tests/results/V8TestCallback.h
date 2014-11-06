/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

// This file has been auto-generated by code_generator_v8.pm. DO NOT MODIFY!

#ifndef V8TestCallback_h
#define V8TestCallback_h

#include "bindings/tests/idls/TestCallback.h"
#include "bindings/v8/ActiveDOMCallback.h"
#include "bindings/v8/DOMWrapperWorld.h"
#include "bindings/v8/ScopedPersistent.h"

namespace WebCore {

class ExecutionContext;

class V8TestCallback : public TestCallback, public ActiveDOMCallback {
public:
    static PassRefPtr<V8TestCallback> create(v8::Handle<v8::Value> jsValue, ExecutionContext* context)
    {
        ASSERT(jsValue->IsObject());
        ASSERT(context);
        return adoptRef(new V8TestCallback(v8::Handle<v8::Object>::Cast(jsValue), context));
    }

    virtual ~V8TestCallback();

    virtual bool callbackWithNoArg();
    virtual bool callbackWithTestInterfaceEmptyArg(TestInterfaceEmpty* class1Arg);
    virtual bool callbackWithTestInterfaceEmptyArg(TestInterfaceEmpty* class2Arg, const String& strArg);
    virtual bool customCallback(TestInterfaceEmpty* testInterfaceEmptyArg, TestInterfaceEmpty* testInterfaceEmptyArg);
    virtual bool callbackWithBooleanArg(bool boolArg);
    virtual bool callbackWithSequenceArg(const Vector<RefPtr<TestInterfaceEmpty> >& sequenceArg);
    virtual bool callbackWithFloatArg(float floatArg);
    virtual bool callbackWithThisArg(ScriptValue thisValue, int arg);

    virtual ExecutionContext* executionContext() const { return ContextLifecycleObserver::executionContext(); }

private:
    V8TestCallback(v8::Handle<v8::Object>, ExecutionContext*);

    ScopedPersistent<v8::Object> m_callback;
    RefPtr<DOMWrapperWorld> m_world;
};

}

#endif // V8TestCallback_h
