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
#include "CompositeMessageParser.h"

CompositeMessageParser::CompositeMessageParser()
    : MessageParser()
{
    mInnerParsers = new std::list<MessageParser*>();
}

CompositeMessageParser::~CompositeMessageParser()
{
    delete mInnerParsers;
}

void CompositeMessageParser::registerMessage(SmmMessage* msg)
{
    SMM_TRACE_E("Messages shall never be registered to CompositeMessageParser");
}

void CompositeMessageParser::addChildParser(MessageParser* parser)
{
    if (parser != nullptr) {
        SMM_TRACE_D("Adding child parser: %s", parser->getName());
        mInnerParsers->push_back(parser);
    } else {
        SMM_TRACE_E("parser cannot be null");
    }
}

void CompositeMessageParser::removeChildParser(MessageParser* parser)
{
    if (parser != nullptr) {
        SMM_TRACE_D("Removing child parser: %s", parser->getName());
        mInnerParsers->remove(parser);
    } else {
        SMM_TRACE_E("parser cannot be null");
    }
}

bool CompositeMessageParser::onDataAvailable(OutputWriterInterface* output,
                                             const char* data,
                                             size_t len)
{
    SMM_TRACE_D("CompositeMessageParser::onDataAvailable(%s)", data);

    bool ret = false;

    std::list<MessageParser*>::iterator i;

    for (i = mInnerParsers->begin(); i != mInnerParsers->end(); ++i) {
        MessageParser* p = *i;

        ret = p->onDataAvailable(output, data, len);
        if (ret) {
            SMM_TRACE_D("%s has consumed the message", p->getName());
            break;
        } else {
            SMM_TRACE_D("%s has not consumed the message", p->getName());
        }
    }
    return ret;
}

bool CompositeMessageParser::dispatchMessage(OutputWriterInterface* output,
                                             int argc,
                                             const char** argv)
{
    SMM_TRACE_E("CompositeMessageParser::dispatchMessage should never be called");
    return false;
}
