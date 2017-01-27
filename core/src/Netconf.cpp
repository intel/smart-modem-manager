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
#include <vector>
#include <list>
#include "Netconf.h"
#include "DcContext.h"
#include "NetregContext.h"
#include "smm_log.h"
#include "NetconfConfig.h"

Netconf::Netconf(NetconfPersistence& persist,
        NwRouteController& routeController) {
    mPersist = &persist;
    mRouteController = &routeController;

    // always have at least one DC context, even undefined
    DcContext* dc = new DcContext();
    dc->setCid(-1);
    mDcContexts.push_back(dc);
}

Netconf::Netconf(Netconf &netconf) {
    mPersist = netconf.mPersist;
    mRouteController = netconf.mRouteController;
    mDcContexts = netconf.mDcContexts;
    mNetregContext = netconf.mNetregContext;
    mPendingMessages = netconf.mPendingMessages;
}

Netconf::~Netconf() {
}

DcContext* Netconf::getDcContext() {
    DcContext* ret = nullptr;

    if (mDcContexts.size() > 0) {
        ret = mDcContexts[0];
    } else {
        SMM_TRACE_E("returning null DcContext");
    }
    return ret;
}

std::vector<DcContext*>* Netconf::getDcMultiContexts() {
    return &mDcContexts;
}

NetregContext* Netconf::getNetregContext() {
    return &mNetregContext;
}

NetconfPersistence* Netconf::getPersistence() {
    return mPersist;
}

SmmMessage* Netconf::dequeuePendingMessage() {
    SmmMessage* ret = nullptr;

    if (!mPendingMessages.empty()) {
        ret = mPendingMessages.back();
        assert(ret != nullptr);
        SMM_TRACE_D("Netconf::dequeuePendingMessage(): %s removed",
                ret->getMessage());
        mPendingMessages.pop_back();
    } else {
        SMM_TRACE_D("Netconf::dequeuePendingMessage(): no message");
    }

    return ret;
}

void Netconf::enqueuePendingMessage(SmmMessage* msg) {
    SMM_TRACE_D("Netconf::enqueuePendingMessage(%s)", msg->getMessage());
    mPendingMessages.push_back(msg);
    if (mPendingMessages.size() > 1) {
        SMM_TRACE_E("Suspicious situation: %zu message waiting",
                mPendingMessages.size());
    }
}

bool Netconf::onDataCallEstablished(int cid, const char* internalIfName,
        const char* dns1, const char* dns2) {
    SMM_TRACE_D("Netconf::onDataCallEstablished(%d, %s, %s)", cid, dns1, dns2);

    mRouteController->enableTethering(
        NETWORK_INTERFACE_EXTERNAL_NAME, internalIfName,
        dns1, dns2);

    return true;
}

bool Netconf::onDataCallTerminated(int cid, const char* internalIfName) {
    SMM_TRACE_D("Netconf::onDataCallTerminated(%d)", cid);

    mRouteController->disableTethering(NETWORK_INTERFACE_EXTERNAL_NAME,
            internalIfName);

    return true;
}
