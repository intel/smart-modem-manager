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

#include <string.h>
#include <assert.h>
#include "smm_log.h"
#include "MessageParser.h"

MessageParser::MessageParser()
{
    mMessages = new std::list<SmmMessage*>();
}

MessageParser::~MessageParser()
{
    delete mMessages;
}

void MessageParser::registerMessage(SmmMessage* msg)
{
    assert(msg != nullptr);
    mMessages->push_back(msg);
}

bool MessageParser::dispatchMessage(OutputWriterInterface* output, int argc, const char** argv)
{
    assert(output != nullptr);
    SMM_TRACE_D("MessageParser::dispatchMessage(%d, ...)", argc);

    if (argc > 0) {
        std::list<SmmMessage*>::iterator i;

        for (i = mMessages->begin(); i != mMessages->end(); ++i) {
            SmmMessage* c = *i;

            if (argv[0] != nullptr && !strcasecmp(argv[0], c->getMessage())) {
                return c->execute(output, argc, argv);
            }
        }
    }
    return false;
}
