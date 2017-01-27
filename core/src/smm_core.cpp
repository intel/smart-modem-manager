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

#include <pthread.h>
#include "smm_core.h"
#include "smm_log.h"
#include "SmmFactory.h"
#include "MessageParser.h"
#include "NetconfConfig.h"
#include "Netconf.h"
#include "SharedEnv.h"

static const smmcore_ctx_t* g_ctx;
static MessageParser* g_command_parser;
static MessageParser* g_response_parser;
static ProxyOutputWriter* g_proxy_outputwriter;
static FinalOutputWriter* g_final_outputwriter;
static Netconf* g_netconf;
static SharedEnv* g_shared_env;
static SmmFactory* g_factory;

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

bool smmcore_cmd_process(const char* cmd, size_t cmd_len)
{
    bool ret = false;

    SMM_TRACE_D("smmcore_cmd_process");

    if (cmd_len > MAX_SMM_MESSAGE_SIZE) {
        SMM_TRACE_D("Command is too long, not handling it");
        return false;
    }

    char log[MAX_SMM_MESSAGE_SIZE];
    memset(log, 0, sizeof(log));

    copy_and_replace_escape_chars_by_spaces(log, cmd, cmd_len);
    SMM_TRACE_D("Host -> SMM: %s", log);

    if (g_command_parser != nullptr) {
        ret = g_command_parser->onDataAvailable(g_proxy_outputwriter, cmd, cmd_len);
    } else {
        SMM_TRACE_E("smmcore_init() must be called first");
    }
    SMM_TRACE_E("smmcore_cmd_process() returning %d", (int)ret);
    return ret;
}

bool smmcore_rsp_process(const char* rsp, size_t rsp_len)
{
    bool ret = false;

    SMM_TRACE_D("smmcore_rsp_process");

    if (rsp_len > MAX_SMM_MESSAGE_SIZE) {
        SMM_TRACE_D("Command is too long, not handling it");
        return false;
    }

    char log[MAX_SMM_MESSAGE_SIZE];
    memset(log, 0, sizeof(log));

    copy_and_replace_escape_chars_by_spaces(log, rsp, rsp_len);
    SMM_TRACE_D("Modem -> SMM: %s", log);

    if (g_response_parser != nullptr) {
        ret = g_response_parser->onDataAvailable(g_proxy_outputwriter, rsp, rsp_len);
    } else {
        SMM_TRACE_E("smmcore_init() must be called first");
    }
    SMM_TRACE_E("smmcore_rsp_process() returning %d", (int)ret);
    return ret;
}

void smmcore_init(const smmcore_ctx_t* ctx)
{
    SMM_TRACE_D("%s", __FUNCTION__);
    g_ctx = ctx;
    g_factory = SmmFactory::getInstance();
    g_netconf = g_factory->makeNetconf(ctx);
    g_command_parser = g_factory->makeRootCommandParser();
    g_response_parser = g_factory->makeRootResponseParser();
    g_final_outputwriter = g_factory->makeFinalOutputWriter(ctx);
    g_proxy_outputwriter = g_factory->makeProxyOutputWriter(g_command_parser,
                                                            g_response_parser,
                                                            g_final_outputwriter);
    g_shared_env = new SharedEnv(g_netconf, g_proxy_outputwriter, "core");
    g_factory->loadExtensions();
}

void smmcore_dispose()
{
    SMM_TRACE_D("%s", __FUNCTION__);
    g_ctx = nullptr;
    delete g_factory;
}
