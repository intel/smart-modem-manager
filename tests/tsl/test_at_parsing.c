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
#include "smm_core.h"
#include "../test.h"

static char sent_command_to_modem[1024];
static char sent_response_to_host[1024];
static smmcore_ctx_t g_ctx;
static bool if_up;
static char interface_name[256];

inline void cleanup()
{
    memset(sent_command_to_modem, 0, sizeof(sent_command_to_modem));
    memset(sent_response_to_host, 0, sizeof(sent_response_to_host));
    memset(interface_name, 0, sizeof(interface_name));
    if_up = false;
}

static void fake_send_command(const char *cmd, size_t cmd_len)
{
    SMM_TEST_TRACE_D("SMM -> Modem: %s", cmd);
    strncpy(sent_command_to_modem, cmd, cmd_len);
}

static void fake_send_response(const char *rsp, size_t rsp_len)
{
    SMM_TEST_TRACE_D("SMM -> Host: %s", rsp);
    strncpy(sent_response_to_host, rsp, rsp_len);
}

static bool fake_bringup_interface(const char *ipType, const char *ipAddr,
                                   const char *interface)
{
    SMM_TEST_TRACE_D("Interface bringup (%s, %s, %s)", ipType, ipAddr, interface);
    if_up = true;
    strncpy(interface_name, interface, sizeof(interface_name));
    return true;
}

static bool fake_bringdown_interface(const char *interface)
{
    SMM_TEST_TRACE_D("Interface bringdown (%s)", interface);
    if_up = false;
    strncpy(interface_name, interface, sizeof(interface_name));
    return true;
}

static int test_at_ati()
{
    const char *cmd = "ATI";

    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strlen(sent_command_to_modem) > 0) {
        SMM_TEST_TRACE_E("No direct command should be sent to the modem");
        return TEST_RESULT_FAIL;
    }
    if (strlen(sent_response_to_host) > 0) {
        SMM_TEST_TRACE_E("No direct response should be sent to the host");
        return TEST_RESULT_FAIL;
    }
    return TEST_RESULT_OK;
}

static int test_at_spio()
{
    const char *cmd = "AT^SPIO=1";

    if (false == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strstr(sent_response_to_host, "+CME ERROR") == NULL) {
        SMM_TEST_TRACE_E("CME ERROR should be sent to the host");
        return TEST_RESULT_FAIL;
    }
    return TEST_RESULT_OK;
}

static int test_at_scid()
{
    const char *cmd = "AT^SCID";

    if (false == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strcasecmp("at+ccid", sent_command_to_modem) < 0) {
        SMM_TEST_TRACE_E("at+ccid should be sent to the modem");
        return TEST_RESULT_FAIL;
    }
    if (strlen(sent_response_to_host) > 0) {
        SMM_TEST_TRACE_E("No direct response should be sent to the host");
        return TEST_RESULT_FAIL;
    }
    const char *modem_rsp = "+CCID: 89011702272000505427\r\nOK";
    if (!smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strstr(sent_response_to_host, "^SCID: 89011702272000505427") == NULL) {
        SMM_TEST_TRACE_E("^SCID should be sent back to the host");
        return TEST_RESULT_FAIL;
    }
    return TEST_RESULT_OK;
}

static int test_at_scpin()
{
    const char *cmd = "AT^SCPIN=1,4,1,0";

    if (false == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strstr(sent_response_to_host, "OK") == NULL) {
        SMM_TEST_TRACE_E("OK should be sent to the host");
        return TEST_RESULT_FAIL;
    }
    return TEST_RESULT_OK;
}

static int test_at_ssio()
{
    const char *cmd = "AT^SSIO=4,1";

    if (false == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strstr(sent_response_to_host, "OK") == NULL) {
        SMM_TEST_TRACE_E("OK should be sent to the host");
        return TEST_RESULT_FAIL;
    }
    return TEST_RESULT_OK;
}

static int test_at_sradc()
{
    const char *cmd = "AT^SRADC=0";

    if (false == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strstr(sent_response_to_host, "OK") == NULL) {
        SMM_TEST_TRACE_E("OK should be sent to the host");
        return TEST_RESULT_FAIL;
    }
    if (strstr(sent_response_to_host, "^SRADC: 0,1,875") == NULL) {
        SMM_TEST_TRACE_E("^SRADC: 0,1,875 should be sent to the host");
        return TEST_RESULT_FAIL;
    }
    return TEST_RESULT_OK;
}

static int test_at_smoni()
{
    const char *cmd = "AT^SMONI";

    if (false == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strcasecmp("at+xcops=0,", sent_command_to_modem) < 0) {
        SMM_TEST_TRACE_E("at+xcops=0 should be sent to the modem");
        return TEST_RESULT_FAIL;
    }
    const char *modem_rsp = "+XCOPS: 0,123987\r\nOK";
    if (!smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should be consumed");
        return TEST_RESULT_FAIL;
    }
    cleanup();
    if (strcasecmp("at+creg?,", sent_command_to_modem) < 0) {
        SMM_TEST_TRACE_E("at+creg? should be sent to the modem");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "+CREG: 2,1,\"8B42\",\"A193511\",7\r\nOK";
    if (!smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strstr(sent_response_to_host, "^SMONI:") == NULL) {
        SMM_TEST_TRACE_E("^SMONI: should be sent to the host");
        return TEST_RESULT_FAIL;
    }
    return TEST_RESULT_OK;
}

static int test_at_swwan()
{
    const char *cmd = "AT^SWWAN=1,1";

    if (false == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strcasecmp("at+cgact=1,1", sent_command_to_modem) < 0) {
        SMM_TEST_TRACE_E("at+cgact=1,1 should be sent to the modem");
        return TEST_RESULT_FAIL;
    }
    if (strlen(sent_response_to_host) > 0) {
        SMM_TEST_TRACE_E("No direct response should be sent to the host");
        return TEST_RESULT_FAIL;
    }
    const char *modem_rsp = "OK";
    if (!smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should be consumed");
        return TEST_RESULT_FAIL;
    }
    if (strstr(sent_response_to_host, "OK") == NULL) {
        SMM_TEST_TRACE_E("OK should be sent back to the host");
        return TEST_RESULT_FAIL;
    }
    return TEST_RESULT_OK;
}

static int test_netreg()
{
    const char *cmd = "AT+CREG=2";

    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    const char *modem_rsp = "OK";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    cmd = "AT+CCREG=2";
    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "OK";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    cmd = "AT+CMGF=1";
    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "OK";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    cmd = "AT+CNMI=2,1";
    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "OK";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    cmd = "AT+CGDCONT=1,\"IP\",\"operator01.com.attz\"";
    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "OK";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    cmd = "AT+CGDCONT?";
    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "+CGDCONT: 1,\"IP\",\"operator01.com.attz\",\"0.0.0.0\",0,0\r\nOK";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    cmd = "AT+CREG?";
    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "+CREG: 2,1,\"8B42\",\"A193511\",7\r\nOK";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    cmd = "AT+CGREG?";
    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "+CGREG: 2,1,\"8B42\",\"A193511\",7\r\nOK";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    cmd = "AT+CGCONTRDP";
    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "+CGCONTRDP: 1,5,operator01.com.attz,10.66.115.164\r\nOK";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    cmd = "AT+CPLS=1";
    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "OK";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    cmd = "AT+CGATT?";
    if (true == smmcore_cmd_process(cmd, strlen(cmd))) {
        SMM_TEST_TRACE_E("The command should not be consumed");
        return TEST_RESULT_FAIL;
    }
    modem_rsp = "+CGATT: 1\r\n";
    if (smmcore_rsp_process(modem_rsp, strlen(modem_rsp))) {
        SMM_TEST_TRACE_E("Modem response should not be consumed");
        return TEST_RESULT_FAIL;
    }

    return TEST_RESULT_OK;
}

int test_at_init()
{
    g_ctx.send_cmd = fake_send_command;
    g_ctx.send_rsp = fake_send_response;
    g_ctx.bringup_nw_interface = fake_bringup_interface;
    g_ctx.bringdown_nw_interface = fake_bringdown_interface;
    g_ctx.test_mode = true;
    smmcore_init(&g_ctx);
    return TEST_RESULT_OK;
}

int test_at_run()
{
    int ret = TEST_RESULT_OK;

    cleanup();
    if ((ret = test_netreg()) != TEST_RESULT_OK) {
        SMM_TEST_TRACE_D("test_netreg FAILED");
        return ret;
    }
    cleanup();
    if ((ret = test_at_ati()) != TEST_RESULT_OK) {
        SMM_TEST_TRACE_D("test_at_ati FAILED");
        return ret;
    }
    cleanup();
    if ((ret = test_at_spio()) != TEST_RESULT_OK) {
        SMM_TEST_TRACE_D("test_at_spio FAILED");
        return ret;
    }
    cleanup();
    if ((ret = test_at_scid()) != TEST_RESULT_OK) {
        SMM_TEST_TRACE_D("test_at_scid FAILED");
        return ret;
    }
    cleanup();
    if ((ret = test_at_scpin()) != TEST_RESULT_OK) {
        SMM_TEST_TRACE_D("test_at_scpin FAILED");
        return ret;
    }
    cleanup();
    if ((ret = test_at_ssio()) != TEST_RESULT_OK) {
        SMM_TEST_TRACE_D("test_at_ssio FAILED");
        return ret;
    }
    cleanup();
    if ((ret = test_at_sradc()) != TEST_RESULT_OK) {
        SMM_TEST_TRACE_D("test_at_sradc FAILED");
        return ret;
    }
    cleanup();
    if ((ret = test_at_smoni()) != TEST_RESULT_OK) {
        SMM_TEST_TRACE_D("test_at_smoni FAILED");
        return ret;
    }
    cleanup();
    if ((ret = test_at_swwan()) != TEST_RESULT_OK) {
        SMM_TEST_TRACE_D("test_at_swwan FAILED");
        return ret;
    }
    return ret;
}

void test_at_teardown()
{
    smmcore_dispose();
}
