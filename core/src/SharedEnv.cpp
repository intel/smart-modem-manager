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

#include <vector>
#include <algorithm>
#include <iterator>
#include <assert.h>
#include "smm_log.h"
#include "Netconf.h"
#include "SharedEnv.h"
#include "OutputWriterInterface.h"

SharedEnv::SharedEnv(Netconf* netconf, OutputWriterInterface* output, const char* instanceName)
{
    assert(netconf != nullptr);
    assert(instanceName != nullptr);
    assert(output != nullptr);
    mNetconf = netconf;
    mOutput = output;
    mInstanceName = instanceName;
    sInstance = this;
}

SharedEnv::SharedEnv(SharedEnv &env, const char* instanceName)
{
    assert(instanceName != nullptr);
    assert(env.mNetconf != nullptr);
    mNetconf = env.mNetconf;
    mInstanceName = instanceName;
}

SharedEnv* SharedEnv::sInstance = nullptr;

SharedEnv* SharedEnv::getInstance()
{
    assert(sInstance != nullptr);
    return sInstance;
}

SharedEnv* SharedEnv::getNewInstanceFor(const char* instanceName)
{
    SMM_TRACE_D("SharedEnv::getNewInstanceFor(%s)", instanceName);
    return new SharedEnv(*getInstance(), instanceName);
}

SmmMessage* SharedEnv::dequeuePendingMessage()
{
    SMM_TRACE_D("%s: SharedEnv::dequeuePendingMessage()", mInstanceName);
    return mNetconf->dequeuePendingMessage();
}

OutputWriterInterface* SharedEnv::getOutputWriter()
{
    SMM_TRACE_D("%s: SharedEnv::getOutputWriter()", mInstanceName);
    return mOutput;
}

Netconf* SharedEnv::getNetconf()
{
    return mNetconf;
}

void SharedEnv::enqueuePendingMessage(SmmMessage* msg)
{
    SMM_TRACE_D("%s: SharedEnv::enqueuePendingMessage(%s)", mInstanceName,
                msg != nullptr ? msg->getMessage() : "null");
    mNetconf->enqueuePendingMessage(msg);
}

MessageParser* SharedEnv::getExtensionCommandParser()
{
    return mCmdParser;
}

MessageParser* SharedEnv::getExtensionResponseParser()
{
    return mRspParser;
}

void SharedEnv::setExtensionCommandParser(MessageParser* parser)
{
    mCmdParser = parser;

}
void SharedEnv::setExtensionResponseParser(MessageParser* parser)
{
    mRspParser = parser;
}

DcContextInterface* SharedEnv::getDcContext()
{
    SMM_TRACE_D("%s: SharedEnv::getDcContext()", mInstanceName);
    return mNetconf->getDcContext();
}

std::vector<DcContextInterface*> SharedEnv::getDcMultiContexts() {
    SMM_TRACE_D("%s: SharedEnv::getDcMultiContexts()", mInstanceName);
    std::vector<DcContextInterface*> ret;
    std::vector<DcContext*>* dcs = mNetconf->getDcMultiContexts();

    std::transform(ret.begin(), ret.end(),
            std::back_insert_iterator<std::vector<DcContext*>>(*dcs),
            [] (DcContextInterface* in) {
                return dynamic_cast<DcContext*>(in);
            });
    return ret;
}

NetregContextInterface* SharedEnv::getNetregContext()
{
    SMM_TRACE_D("%s: SharedEnv::getNetregContext()", mInstanceName);
    return mNetconf->getNetregContext();
}
