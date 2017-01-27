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

#ifndef _SMM_CORE_H_
#define _SMM_CORE_H_

#include <stdbool.h>
#include <stddef.h>

#define MAX_SMM_MESSAGE_SIZE    256
#define MSG_BUF_SIZE                    128
#define MSG_MAX_ARGS                    32

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (* smmcore_cmd_sending_t)(const char* cmd, size_t cmd_len);
typedef void (* smmcore_rsp_sending_t)(const char* rsp, size_t rsp_len);
typedef bool (* smmcore_test_bringup_interface)(const char* ipType, const char* ipAddr,
                                                const char* interface);
typedef bool (* smmcore_test_bringdown_interface)(const char* interface);

typedef struct {
    smmcore_cmd_sending_t send_cmd;
    smmcore_rsp_sending_t send_rsp;
    bool test_mode;
    smmcore_test_bringup_interface bringup_nw_interface;
    smmcore_test_bringdown_interface bringdown_nw_interface;
} smmcore_ctx_t;

bool smmcore_cmd_process(const char* cmd, size_t cmd_len);
bool smmcore_rsp_process(const char* rsp, size_t rsp_len);
void smmcore_init(const smmcore_ctx_t* ctx);
void smmcore_dispose();


#ifdef __cplusplus
}
#endif
#endif // _SMM_CORE_H_
