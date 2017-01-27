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
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "MessageParser.h"
#include "FinalOutputWriter.h"
#include "OutputWriterInterface.h"
#include "ProxyOutputWriter.h"

ProxyOutputWriter::ProxyOutputWriter(MessageParser* toHostParser, MessageParser* toDeviceParser)
    : OutputWriterInterface()
{
    assert(toHostParser != nullptr);
    assert(toDeviceParser != nullptr);

    mToHostParser = toHostParser;
    mToDeviceParser = toDeviceParser;
}

ProxyOutputWriter::~ProxyOutputWriter()
{
}

FinalOutputWriter* ProxyOutputWriter::setInnerOutputWriter(FinalOutputWriter* newInnerOutputWriter)
{
    FinalOutputWriter* ret = mOutputWriter;

    mOutputWriter = newInnerOutputWriter;

    return ret;
}

void ProxyOutputWriter::writeToHost(const char* format, ...)
{
    assert(mOutputWriter != nullptr);

    va_list argp;
    va_start(argp, format);

    char buff[MSG_BUF_SIZE];
    vsnprintf(buff, sizeof(buff), format, argp);

    /* first we re-inject the message to the parent composite parser
     * to seek for a parser that can handle it.
     */
    if (!mToHostParser->onDataAvailable(this, buff, strlen(buff))) {
        // if no parser found (i.e. all parsers returned false), forward to the host
        mOutputWriter->writeToHost(buff);
    }
    // else it is up to the parser that returned true to issue the message to the host

    va_end(argp);
}

void ProxyOutputWriter::writeToDevice(const char* format, ...)
{
    assert(mOutputWriter != nullptr);

    va_list argp;
    va_start(argp, format);

    char buff[MSG_BUF_SIZE];
    vsnprintf(buff, sizeof(buff), format, argp);

    /* first we re-inject the message to the parent composite parser
     * to seek for a parser that can handle it.
     */
    if (!mToDeviceParser->onDataAvailable(this, buff, strlen(buff))) {
        // if no parser found (i.e. all parsers returned false), forward to the device
        mOutputWriter->writeToDevice(buff);
    }
    // else it is up to the parser that returned true to issue the message to the host

    va_end(argp);
}
