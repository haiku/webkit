/*
 * Copyright (C) 2018-2020 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "config.h"
#include "WorkletGlobalScope.h"

#include "Frame.h"
#include "InspectorInstrumentation.h"
#include "JSWorkletGlobalScope.h"
#include "PageConsoleClient.h"
#include "SecurityOriginPolicy.h"
#include "Settings.h"
#include "WorkerEventLoop.h"
#include "WorkerMessagePortChannelProvider.h"
#include "WorkerOrWorkletThread.h"
#include "WorkerScriptLoader.h"
#include "WorkletParameters.h"
#include <JavaScriptCore/Exception.h>
#include <JavaScriptCore/JSLock.h>
#include <JavaScriptCore/ScriptCallStack.h>
#include <wtf/IsoMallocInlines.h>

namespace WebCore {
using namespace Inspector;

WTF_MAKE_ISO_ALLOCATED_IMPL(WorkletGlobalScope);

WorkletGlobalScope::WorkletGlobalScope(WorkerOrWorkletThread& thread, const WorkletParameters& parameters)
    : m_thread(&thread)
    , m_script(makeUnique<WorkletScriptController>(this))
    , m_topOrigin(SecurityOrigin::createUnique())
    , m_url(parameters.windowURL)
    , m_jsRuntimeFlags(parameters.jsRuntimeFlags)
{
    auto addResult = allWorkletGlobalScopesSet().add(this);
    ASSERT_UNUSED(addResult, addResult);

    setSecurityOriginPolicy(SecurityOriginPolicy::create(SecurityOrigin::create(this->url())));
    setContentSecurityPolicy(makeUnique<ContentSecurityPolicy>(URL { this->url() }, *this));
}

WorkletGlobalScope::WorkletGlobalScope(Document& document, Ref<JSC::VM>&& vm, ScriptSourceCode&& code)
    : m_document(makeWeakPtr(document))
    , m_script(makeUnique<WorkletScriptController>(WTFMove(vm), this))
    , m_topOrigin(SecurityOrigin::createUnique())
    , m_url(code.url())
    , m_jsRuntimeFlags(document.settings().javaScriptRuntimeFlags())
    , m_code(WTFMove(code))
{
    auto addResult = allWorkletGlobalScopesSet().add(this);
    ASSERT_UNUSED(addResult, addResult);

    ASSERT(document.page());

    setSecurityOriginPolicy(SecurityOriginPolicy::create(SecurityOrigin::create(this->url())));
    setContentSecurityPolicy(makeUnique<ContentSecurityPolicy>(URL { this->url() }, *this));
}

WorkletGlobalScope::~WorkletGlobalScope()
{
    ASSERT(!m_script);
    removeFromContextsMap();
    auto removeResult = allWorkletGlobalScopesSet().remove(this);
    ASSERT_UNUSED(removeResult, removeResult);
}

void WorkletGlobalScope::prepareForDestruction()
{
    if (!m_script)
        return;
    if (m_defaultTaskGroup)
        m_defaultTaskGroup->stopAndDiscardAllTasks();
    stopActiveDOMObjects();
    removeAllEventListeners();
    if (m_eventLoop)
        m_eventLoop->clearMicrotaskQueue();
    removeRejectedPromiseTracker();
    m_script->vm().notifyNeedTermination();
    m_script = nullptr;
}

auto WorkletGlobalScope::allWorkletGlobalScopesSet() -> WorkletGlobalScopesSet&
{
    static NeverDestroyed<WorkletGlobalScopesSet> scopes;
    return scopes;
}

EventLoopTaskGroup& WorkletGlobalScope::eventLoop()
{
    if (UNLIKELY(!m_defaultTaskGroup)) {
        m_eventLoop = WorkerEventLoop::create(*this);
        m_defaultTaskGroup = makeUnique<EventLoopTaskGroup>(*m_eventLoop);
        if (activeDOMObjectsAreStopped())
            m_defaultTaskGroup->stopAndDiscardAllTasks();
    }
    return *m_defaultTaskGroup;
}

String WorkletGlobalScope::userAgent(const URL& url) const
{
    if (!m_document)
        return "";
    return m_document->userAgent(url);
}

void WorkletGlobalScope::evaluate()
{
    if (m_code)
        m_script->evaluate(*m_code);
}

bool WorkletGlobalScope::isJSExecutionForbidden() const
{
    return !m_script || m_script->isExecutionForbidden();
}

void WorkletGlobalScope::disableEval(const String& errorMessage)
{
    m_script->disableEval(errorMessage);
}

void WorkletGlobalScope::disableWebAssembly(const String& errorMessage)
{
    m_script->disableWebAssembly(errorMessage);
}

URL WorkletGlobalScope::completeURL(const String& url, ForceUTF8) const
{
    if (url.isNull())
        return URL();
    return URL(this->url(), url);
}

void WorkletGlobalScope::logExceptionToConsole(const String& errorMessage, const String& sourceURL, int lineNumber, int columnNumber, RefPtr<ScriptCallStack>&& stack)
{
    if (!m_document || isJSExecutionForbidden())
        return;
    m_document->logExceptionToConsole(errorMessage, sourceURL, lineNumber, columnNumber, WTFMove(stack));
}

void WorkletGlobalScope::addConsoleMessage(std::unique_ptr<Inspector::ConsoleMessage>&& message)
{
    if (!m_document || isJSExecutionForbidden() || !message)
        return;
    m_document->addConsoleMessage(makeUnique<Inspector::ConsoleMessage>(message->source(), message->type(), message->level(), message->message(), 0));
}

void WorkletGlobalScope::addConsoleMessage(MessageSource source, MessageLevel level, const String& message, unsigned long requestIdentifier)
{
    if (!m_document || isJSExecutionForbidden())
        return;
    m_document->addConsoleMessage(source, level, message, requestIdentifier);
}

void WorkletGlobalScope::addMessage(MessageSource source, MessageLevel level, const String& messageText, const String& sourceURL, unsigned lineNumber, unsigned columnNumber, RefPtr<ScriptCallStack>&& callStack, JSC::JSGlobalObject*, unsigned long requestIdentifier)
{
    if (!m_document || isJSExecutionForbidden())
        return;
    m_document->addMessage(source, level, messageText, sourceURL, lineNumber, columnNumber, WTFMove(callStack), nullptr, requestIdentifier);
}

ReferrerPolicy WorkletGlobalScope::referrerPolicy() const
{
    return ReferrerPolicy::NoReferrer;
}

void WorkletGlobalScope::fetchAndInvokeScript(const URL& moduleURL, FetchRequestCredentials credentials, CompletionHandler<void(Optional<Exception>&&)>&& completionHandler)
{
    ASSERT(!isMainThread());
    m_scriptFetchJobs.append({ moduleURL, credentials, WTFMove(completionHandler) });
    processNextScriptFetchJobIfNeeded();
}

void WorkletGlobalScope::processNextScriptFetchJobIfNeeded()
{
    if (m_scriptFetchJobs.isEmpty() || m_scriptLoader)
        return;

    auto& scriptFetchJob = m_scriptFetchJobs.first();

    ResourceRequest request { scriptFetchJob.moduleURL };

    FetchOptions fetchOptions;
    fetchOptions.mode = FetchOptions::Mode::SameOrigin;
    fetchOptions.cache = FetchOptions::Cache::Default;
    fetchOptions.redirect = FetchOptions::Redirect::Follow;
    fetchOptions.destination = FetchOptions::Destination::Worker;
    fetchOptions.credentials = scriptFetchJob.credentials;

    auto contentSecurityPolicyEnforcement = shouldBypassMainWorldContentSecurityPolicy() ? ContentSecurityPolicyEnforcement::DoNotEnforce : ContentSecurityPolicyEnforcement::EnforceChildSrcDirective;

    m_scriptLoader = WorkerScriptLoader::create();
    m_scriptLoader->loadAsynchronously(*this, WTFMove(request), WTFMove(fetchOptions), contentSecurityPolicyEnforcement, ServiceWorkersMode::All, *this);
}

void WorkletGlobalScope::didReceiveResponse(unsigned long, const ResourceResponse&)
{
}

void WorkletGlobalScope::notifyFinished()
{
    auto completedJob = m_scriptFetchJobs.takeFirst();

    if (m_scriptLoader->failed()) {
        didCompleteScriptFetchJob(WTFMove(completedJob), Exception { NetworkError, makeString("Failed to fetch module, error: ", m_scriptLoader->error().localizedDescription()) });
        return;
    }

    // FIXME: This should really be run as a module script but we don't support this in workers yet.
    URL moduleURL(m_scriptLoader->responseURL());
    auto addResult = m_evaluatedModules.add(moduleURL);
    if (addResult.isNewEntry) {
        NakedPtr<JSC::Exception> exception;
        m_script->evaluate(ScriptSourceCode(m_scriptLoader->script(), WTFMove(moduleURL)), exception);
        if (exception)
            m_script->setException(exception);
    }

    didCompleteScriptFetchJob(WTFMove(completedJob), { });
}

void WorkletGlobalScope::didCompleteScriptFetchJob(ScriptFetchJob&& job, Optional<Exception> result)
{
    m_scriptLoader = nullptr;

    job.completionHandler(WTFMove(result));

    processNextScriptFetchJobIfNeeded();
}

MessagePortChannelProvider& WorkletGlobalScope::messagePortChannelProvider()
{
    if (!m_messagePortChannelProvider)
        m_messagePortChannelProvider = makeUnique<WorkerMessagePortChannelProvider>(*this);
    return *m_messagePortChannelProvider;
}

bool WorkletGlobalScope::isContextThread() const
{
    if (m_thread)
        return m_thread->thread() == &Thread::current();
    return isMainThread();
}

} // namespace WebCore
