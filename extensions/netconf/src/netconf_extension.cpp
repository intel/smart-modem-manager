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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <smm_core/smm_log.h>
#include <smm_core/AtCommandParser.h>
#include <smm_core/smm_extension.h>
#include <smm_core/SharedEnv.h>
#include "NetconfAtCommandParser.h"
#include "NetconfAtResponseParser.h"
#include "NetconfRilClient.h"

static MessageParser* sCmdParser = nullptr;
static MessageParser* sRspParser = nullptr;

MessageParser* makeNetconfCommandParser()
{
    SMM_TRACE_D("makeNetconfCommandParser()");
    return sCmdParser;
}

MessageParser* makeNetconfResponseParser()
{
    SMM_TRACE_D("makeNetconfResponseParser()");
    return sRspParser;
}

void setSharedEnv(SharedEnvInterface* env)
{
    SMM_TRACE_D("setSharedEnv(): ignoring the env");
}

void onExtensionDestroy()
{
    SMM_TRACE_D("onExtensionDestroy()");
    delete sCmdParser;
    delete sRspParser;
}

static smm_extension_factory_t factory = {
    setSharedEnv,
    makeNetconfCommandParser,
    makeNetconfResponseParser,
    onExtensionDestroy
};

__attribute__((constructor))
void netconf_ctor()
{
    SMM_TRACE_D("netconf_ctor()");
    if (sCmdParser == nullptr) {
        sCmdParser = new NetconfAtCommandParser();
    }
    if (sRspParser == nullptr) {
        sRspParser = new NetconfAtResponseParser();
    }
    SMM_TRACE_D("Initializing connection with RIL Core...");
    if (!NetconfRilClient::getInstance()->init()) {
        SMM_TRACE_E("Connection to RIL Core failed, not going to register extension");
    } else {
        if (registerExtensionFactory(&factory, "Netconf")) {
            SMM_TRACE_D("Extension registration success");
            SMM_TRACE_D("Disabling IMS auto registration");
            SharedEnv::getInstance()->getOutputWriter()->writeToDevice("AT+XICFG=0,1,50,0\r\n"); // disable IMS autologin
        } else {
            SMM_TRACE_E("Extension registration failure");
        }
    }
}

__attribute__((destructor))
void netconf_dtor()
{
    onExtensionDestroy();
}
