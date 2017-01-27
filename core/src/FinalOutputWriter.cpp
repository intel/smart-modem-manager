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
#include <stdio.h>
#include <assert.h>
#include "smm_log.h"
#include "smm_core.h"
#include "OutputWriterInterface.h"
#include "FinalOutputWriter.h"

static void copy_and_replace_escape_chars_by_spaces(char* dest, const char* src, size_t len)
{
    if (src != nullptr && dest != nullptr) {
        for (size_t i = 0; i < len; i++) {
            if (src[i] == '\n' || src[i] == '\r') {
                dest[i] = ' ';
            } else {
                dest[i] = src[i];
            }
        }
    }
}

FinalOutputWriter::FinalOutputWriter(const smmcore_ctx_t* ctx)
    : OutputWriterInterface()
{
    SMM_TRACE_D("%s", __FUNCTION__);
    assert(ctx != nullptr);
    assert(ctx->send_rsp != nullptr);
    assert(ctx->send_cmd != nullptr);
    mCtx = ctx;
}

void FinalOutputWriter::writeToHost(const char* format, ...)
{
    va_list argp;
    va_start(argp, format);

    char buff[MSG_BUF_SIZE];
    vsnprintf(buff, sizeof(buff), format, argp);

    char log[MAX_SMM_MESSAGE_SIZE];
    memset(log, 0, sizeof(log));

    size_t len = strlen(buff);

    copy_and_replace_escape_chars_by_spaces(log, buff, len);

    SMM_TRACE_D("FinalOutputWriter::writeToHost(%s, %zu)", log, len);
    mCtx->send_rsp(buff, len);

    va_end(argp);
}

void FinalOutputWriter::writeToDevice(const char* format, ...)
{
    va_list argp;
    va_start(argp, format);

    char buff[MSG_BUF_SIZE];
    vsnprintf(buff, sizeof(buff), format, argp);

    char log[MAX_SMM_MESSAGE_SIZE];
    memset(log, 0, sizeof(log));

    size_t len = strlen(buff);

    copy_and_replace_escape_chars_by_spaces(log, buff, len);

    SMM_TRACE_D("FinalOutputWriter::writeToDevice(%s, %zu)", log, len);
    mCtx->send_cmd(buff, len);

    va_end(argp);
}
