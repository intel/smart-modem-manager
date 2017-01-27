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

#include "smm_log.h"
#include "MessageParser.h"
#include "AtResponse.h"
#include "AtResponseParser.h"
#include "SharedEnv.h"

AtResponseParser::AtResponseParser()
    : MessageParser()
{

    registerMessage(new CmeErrorResponse());
    registerMessage(new GenericErrorResponse());
    registerMessage(new GenericOkResponse());
}

bool AtResponseParser::onDataAvailable(OutputWriterInterface* output, const char* data, size_t len)
{
    bool ret = false;

    char type[] = {
        AT_RESPONSE_URC, '\0'
    };
    char* response = nullptr;
    char* argv[1024];
    int argc = 2;     // argv[0] -> response, argv[1] -> type

    char tmp[1024];
    char* p = (char*)data;
    char* q = tmp;

    memset(tmp, 0, sizeof(tmp));
    memset(argv, 0, sizeof(argv));

    SMM_TRACE_D("AtResponseParser::onDataAvailable(%s, %zu)", data, len);
    while (p != nullptr && *p != '\0' && ((size_t)(p - data)) < len) {
        if (strncmp("OK", p, strlen("OK")) == 0) {
            *type = AT_RESPONSE_OK;
            if (response == nullptr) {
                argv[0] = strdup("OK");
            }
            argv[1] = strdup(type);
            p += strlen("OK") - 1;
        } else if (strncmp("ERROR", p, strlen("ERROR")) == 0) {
            *type = AT_RESPONSE_ERROR;
            argv[0] = strdup("ERROR");
            response = argv[0];
            argv[1] = strdup(type);
            p += strlen("ERROR") - 1;
        } else if (strncmp("+CME ERROR", p, strlen("+CME ERROR")) == 0) {
            *type = AT_RESPONSE_CME_ERROR;
            argv[0] = strdup("+CME ERROR");
            response = argv[0];
            argv[1] = strdup(type);
            p += strlen("+CME ERROR") - 1;
        } else {
            if (*p == ':') {
                if (response == nullptr) {
                    response = strdup(tmp);
                    argv[0] = strdup(tmp);
                }
                memset(tmp, 0, sizeof(tmp));
                q = tmp;
                do {
                    ++p;
                } while (*p == ' ');
                continue;
            }
            if (*p == ',') {
                // extract all parameters  +rsp: a1,a2,a3
                argv[argc++] = strdup(tmp);
                memset(tmp, 0, sizeof(tmp));
                q = tmp;
                ++p;
                continue;
            }
            if (*p != '\r' && *p != '\n' && *p != '\"') {
                *q = *p;
            } else {
                --q;
            }
        }
        ++p;
        ++q;
    }
    argv[argc++] = strdup(tmp);
    argv[1] = strdup(type);
    ret = dispatchMessage(output, argc, (const char**)argv);
    for (int i = 0; i < argc; i++) {
        delete argv[i];
    }
    return ret;
}

AtResponseParser::CmeErrorResponse::CmeErrorResponse()
    : AtResponse("+CME ERROR")
{
}

bool AtResponseParser::CmeErrorResponse::execute(OutputWriterInterface* output,
                                                 int argc,
                                                 const char** argv)
{
    // +CME ERROR: desc
    const char* desc = "";

    if (argc >= 3) {
        desc = argv[2];
    }
    SMM_TRACE_D("CmeErrorResponse { desc = %s }", desc);

    SmmMessage* lastPendingCommand = SharedEnv::getInstance()->dequeuePendingMessage();
    if (lastPendingCommand != nullptr) {
        return lastPendingCommand->onError(this, output);
    }
    // for now let's not consume it
    return false;
}

AtResponseParser::GenericErrorResponse::GenericErrorResponse()
    : AtResponse("ERROR")
{
}

bool AtResponseParser::GenericErrorResponse::execute(OutputWriterInterface* output, int argc,
                                                     const char** argv)
{
    // ERROR
    SMM_TRACE_D("GenericErrorResponse { }");

    SmmMessage* lastPendingCommand = SharedEnv::getInstance()->dequeuePendingMessage();
    if (lastPendingCommand != nullptr) {
        return lastPendingCommand->onError(this, output);
    }
    // for now let's not consume it
    return false;
}

AtResponseParser::GenericOkResponse::GenericOkResponse()
    : AtResponse("OK")
{
}

bool AtResponseParser::GenericOkResponse::execute(OutputWriterInterface* output, int argc,
                                                  const char** argv)
{
    // OK
    SMM_TRACE_D("GenericOkResponse { }");

    SmmMessage* lastPendingCommand = SharedEnv::getInstance()->dequeuePendingMessage();
    if (lastPendingCommand != nullptr) {
        return lastPendingCommand->onSuccess(this, output);
    }
    return false;
}
