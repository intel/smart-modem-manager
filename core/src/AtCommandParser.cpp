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
#include "smm_core.h"
#include "MessageParser.h"
#include "AtCommandParser.h"

AtCommandParser::AtCommandParser()
    : MessageParser()
{
}

bool AtCommandParser::onDataAvailable(OutputWriterInterface* output, const char* data, size_t len)
{
    bool ret = false;

    char type[] = {
        AT_COMMAND_UNDEFINED, '\0'
    };
    char* command = nullptr;
    char* argv[MSG_MAX_ARGS];
    int argc = 0; // argv[0] -> command, argv[1] -> type

    char tmp[MSG_BUF_SIZE];
    char* p = (char*)data;
    char* q = tmp;
    size_t qm_pos = 0; //To fix parsing syntax handling error.

    memset(tmp, 0, sizeof(tmp));
    memset(argv, 0, sizeof(argv));

    SMM_TRACE_D("AtCommandParser::onDataAvailable(%s, %zu)", data, len);

    if (len > 2) { // at+<name>
        argc = 2;
        while (p != nullptr && *p != '\0' && ((size_t)(p - data)) < len) {
            if (*p == '?' || *p == '=') {
                if (command == nullptr) {
                    command = strdup(tmp);
                    argv[0] = strdup(tmp);
                }
                memset(tmp, 0, sizeof(tmp));
                q = tmp;
                if (*p == '?') {
                    if (*type == AT_COMMAND_WRITE) { // case at+cmd=?
                        *type = AT_COMMAND_TEST;
                    } else { // case at+cmd?
                        *type = AT_COMMAND_QUERY;
                    }
                    if (qm_pos == 0) {
                        qm_pos = ((size_t)(p - data));
                    }
                }
                if (*p == '=') {
                    *type = AT_COMMAND_WRITE;
                }
                argv[1] = strdup(type);
                ++p;
                continue;
            }
            if (*type == AT_COMMAND_WRITE && *p == ',') {
                // extract all parameters  at+cmd=a1,a2,a3
                argv[argc++] = strdup(tmp);
                memset(tmp, 0, sizeof(tmp));
                q = tmp;
                ++p;
                continue;
            }
            if (*p != '\r' && *p != '\n' && *p != '"') {
                *q = *p;
            } else {
                --q;
            }
            ++p;
            ++q;
        }
        if (*type == AT_COMMAND_UNDEFINED) {
            *type = AT_COMMAND_QUERY;
            argv[1] = strdup(type);
        }
        if (command == nullptr) {
            argv[0] = strdup(tmp);
        }
        if (*type == AT_COMMAND_WRITE && strlen(tmp) > 0) {
            argv[argc++] = strdup(tmp); // copy the rest in the last arg, case at+cmd=3
        }
        if ((*type == AT_COMMAND_TEST || *type == AT_COMMAND_QUERY)
                && qm_pos != len - 2 && qm_pos != 0) {
            SMM_TRACE_E("With a non-ended question mark.");
            *type =AT_COMMAND_UNDEFINED;
            argv[1] = strdup(type);
        }
        ret = MessageParser::dispatchMessage(output, argc, (const char**)argv);
        for (int i = 0; i < argc; i++) {
            delete argv[i];
        }
    } else {
        SMM_TRACE_E("Command too short to be a valid AT command");
    }
    return ret;
}
