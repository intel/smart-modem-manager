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

#ifndef _SMM_MESSAGE_H_
#define _SMM_MESSAGE_H_

#include <string.h>
#include "OutputWriterInterface.h"

class SmmMessage {
protected:
    const char* mMessage;

public:

    SmmMessage(const char* msg);
    virtual ~SmmMessage()
    {
    }

    virtual bool execute(OutputWriterInterface* output, int argc, const char** argv) = 0;

    const char* getMessage()
    {
        return mMessage;
    }

    inline bool operator==(const SmmMessage &other)
    {
        return strcmp(this->mMessage, other.mMessage) == 0;
    }

    inline bool operator!=(const SmmMessage &other)
    {
        return !(*this == other);
    }

    bool errorIfArgsCountLessThan(int expected, int argc, OutputWriterInterface* output);

    virtual bool onError(SmmMessage* errorMsg, OutputWriterInterface* output)
    {
        return false;
    }
    virtual bool onSuccess(SmmMessage* successMsg, OutputWriterInterface* output)
    {
        return false;
    }
    virtual bool onAbort(SmmMessage* successMsg, OutputWriterInterface* output)
    {
        return false;
    }
};
#endif
