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

#include <stdio.h>
#include <assert.h>
#include "smm_log.h"
#include "AtResponse.h"

AtResponse::AtResponse(const char* msg) : SmmMessage(msg)
{
}

bool AtResponse::execute(OutputWriterInterface* output, int argc, const char** argv)
{
    assert(argc >= 2); // 0: response name, 1: response type
    switch (argv[1][0]) {
        case AT_RESPONSE_URC:
            SMM_TRACE_D("%s : executeAsTest", this->getMessage());
            return this->executeAsUrc(output, argc - 2, &(argv[2]));
        case AT_RESPONSE_REPLY:
        case AT_RESPONSE_OK:
        case AT_RESPONSE_ERROR:
            SMM_TRACE_D("%s : executeAsReply", this->getMessage());
            return this->executeAsReply(output, argc - 2, &(argv[2]));
        case AT_RESPONSE_TEST:
            SMM_TRACE_D("%s : executeAsTestReply", this->getMessage());
            return this->executeAsTestReply(output, argc - 2, &(argv[2]));
        default:
            SMM_TRACE_D("%s : onError", this->getMessage());
            return this->onError(this, output);
    }
}

bool AtResponse::executeAsTestReply(OutputWriterInterface* output, int argc, const char** argv)
{
    return false;
}

bool AtResponse::executeAsUrc(OutputWriterInterface* output, int argc, const char** argv)
{
    return false;
}
