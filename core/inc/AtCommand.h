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

#ifndef _AT_COMMAND_H_
#define _AT_COMMAND_H_

#include <string.h>
#include "SmmMessage.h"
#include "OutputWriterInterface.h"

#define AT_COMMAND_UNDEFINED            'u'
#define AT_COMMAND_TEST                 't'
#define AT_COMMAND_WRITE                'w'
#define AT_COMMAND_QUERY                'q'

class AtCommand : public SmmMessage {
public:
    AtCommand(const char* msg);
    virtual ~AtCommand()
    {
    }
    bool execute(OutputWriterInterface* output, int argc, const char** argv);

protected:
    virtual bool executeAsTest(OutputWriterInterface* output) = 0;
    virtual bool executeAsQuery(OutputWriterInterface* output) = 0;
    virtual bool executeAsWrite(OutputWriterInterface* output, int argc, const char** argv) = 0;

    virtual bool onError(SmmMessage* errorMsg, OutputWriterInterface* output);
    virtual bool onCmeError(SmmMessage* errorMsg, const char* cmeMsg,
                            OutputWriterInterface* output);
    virtual bool onSuccess(SmmMessage* successMsg, OutputWriterInterface* output);
    virtual bool onAbort(SmmMessage* successMsg, OutputWriterInterface* output);
};
#endif
