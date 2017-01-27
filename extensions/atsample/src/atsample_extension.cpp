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
#include <smm_core/SharedEnvInterface.h>
#include "AtsampleAtCommandParser.h"
#include "AtsampleAtResponseParser.h"

static MessageParser* sCmdParser = nullptr;
static MessageParser* sRspParser = nullptr;
static SharedEnvInterface* sEnv = nullptr;

MessageParser* makeAtsampleCommandParser()
{
    SMM_TRACE_D("makeAtsampleCommandParser()");
    return sCmdParser;
}

MessageParser* makeAtsampleResponseParser()
{
    SMM_TRACE_D("makeAtsampleResponseParser()");
    return sRspParser;
}

void setSharedEnv(SharedEnvInterface* env)
{
    SMM_TRACE_D("setSharedEnv()");
    assert(env != nullptr);
    sEnv = env;

    if (sCmdParser == nullptr) {
        sCmdParser = new AtsampleAtCommandParser(sEnv);
    }
    if (sRspParser == nullptr) {
        sRspParser = new AtsampleAtResponseParser(sEnv);
    }
}

void onExtensionDestroy()
{
    SMM_TRACE_D("onExtensionDestroy()");
    delete sCmdParser;
    delete sRspParser;
}

static smm_extension_factory_t factory = {
    setSharedEnv,
    makeAtsampleCommandParser,
    makeAtsampleResponseParser,
    onExtensionDestroy
};

__attribute__((constructor))
void atsample_ctor()
{
    SMM_TRACE_D("atsample_ctor()");

    if (registerExtensionFactory(&factory, "Atsample")) {
        SMM_TRACE_D("Extension registration success");
    } else {
        SMM_TRACE_E("Extension registration failure");
    }
}

__attribute__((destructor))
void atsample_dtor()
{
    onExtensionDestroy();
}
