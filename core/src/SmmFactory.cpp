/*
 * Copyright (c) 2016, Intel Corporation, Intel Mobile Communication
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include "smm_core.h"
#include "SmmFactory.h"
#include "OutputWriterInterface.h"
#include "ProxyOutputWriter.h"
#include "FinalOutputWriter.h"
#include "Netconf.h"
#include "NetconfLinuxPersistence.h"
#include "NetconfPersistence.h"
#include "NetconfConfig.h"
#include "MessageParser.h"
#include "NwRouteController.h"
#include "UnixNwRouteController.h"
#include "TestNwRouteController.h"
#include "ExtensionLoader.h"

SmmFactory::SmmFactory()
{
    mLoader = new ExtensionLoader();
}

SmmFactory::~SmmFactory()
{
    if (mNetconf != nullptr) {
        delete mNetconf;
        mNetconf = nullptr;
    }
    if (mFinalOutputWriter != nullptr) {
        delete mFinalOutputWriter;
        mFinalOutputWriter = nullptr;
    }
    if (mProxyOutputWriter != nullptr) {
        delete mProxyOutputWriter;
        mProxyOutputWriter = nullptr;
    }
    if (mCommandParser != nullptr) {
        delete mCommandParser;
        mCommandParser = nullptr;
    }
    if (mResponseParser) {
        delete mResponseParser;
        mResponseParser = nullptr;
    }
    if (mLoader) {
        delete mLoader;
        mLoader = nullptr;
    }
}

SmmFactory* SmmFactory::sInstance = nullptr;

SmmFactory* SmmFactory::getInstance()
{
    if (SmmFactory::sInstance == nullptr) {
        SmmFactory::sInstance = new SmmFactory();
    }
    return sInstance;
}

FinalOutputWriter* SmmFactory::makeFinalOutputWriter(const smmcore_ctx_t* ctx)
{
    assert(ctx != nullptr);

    if (mFinalOutputWriter == nullptr) {
        mFinalOutputWriter = new FinalOutputWriter(ctx);
    }
    return mFinalOutputWriter;
}

ProxyOutputWriter* SmmFactory::makeProxyOutputWriter(MessageParser* cmdParser,
                                                     MessageParser* rspParser,
                                                     FinalOutputWriter* innerWriter)
{
    if (mProxyOutputWriter == nullptr) {
        mProxyOutputWriter = new ProxyOutputWriter(cmdParser, rspParser);
        mProxyOutputWriter->setInnerOutputWriter(innerWriter);
    }
    return mProxyOutputWriter;
}

Netconf* SmmFactory::makeNetconf(const smmcore_ctx_t* ctx)
{
    NwRouteController* routeCtrl = nullptr;
    NetconfPersistence* persist = nullptr;

    if (mNetconf == nullptr) {
        if (ctx->test_mode) {
            routeCtrl = new TestNwRouteController();
        } else {
            routeCtrl = new UnixNwRouteController();
            persist = new NetconfLinuxPersistence();
        }
        mNetconf = new Netconf(*persist, *routeCtrl);
        // TODO: dealloc of routeCtrl & persist
    }
    return mNetconf;
}

CompositeMessageParser* SmmFactory::makeRootCommandParser()
{
    if (mCommandParser == nullptr) {
        mCommandParser = new CompositeMessageParser();
    }
    return mCommandParser;
}

CompositeMessageParser* SmmFactory::makeRootResponseParser()
{
    if (mResponseParser == nullptr) {
        mResponseParser = new CompositeMessageParser();
    }
    return mResponseParser;
}

bool SmmFactory::loadExtensions()
{
    return mLoader->loadExtensionsFrom(EXTENSIONS_LOAD_PATH);
}
