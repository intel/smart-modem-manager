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

#ifndef _NETCONF_RIL_CLIENT_H_
#define _NETCONF_RIL_CLIENT_H_

#include <stdint.h>
#include <stddef.h>

#include <uril/uril_common_v01.h>
#include <uril/uril_data_v01.h>
#include <uril/uril_nw_v01.h>
#include <uril/uril_radio_v01.h>
#include <uril/uril_client.h>
#include <smm_core/DcContextInterface.h>
#include <smm_core/NetregContextInterface.h>
#include <smm_core/OutputWriterInterface.h>
#include <smm_core/SmmMessage.h>
#include <smm_core/MessageParser.h>
#include <smm_core/SharedEnvInterface.h>
#include <smm_core/SharedEnv.h>

class NetconfRilClient {
private:
    uril_client_handle_t mHandle = 0;
    SharedEnv* mEnv = nullptr;
    static NetconfRilClient* sInstance;

    NetconfRilClient();

    void onDataCallSetup(uint8_t sim_id,
            uril_data_setup_data_call_resp_msg_v01* resp);
    void onRadioModeSet(uint8_t sim_id,
            uril_radio_set_radio_mode_resp_msg_v01* resp);
    void onDataCallReleased(uint8_t sim_id,
            uril_data_deactivate_data_call_resp_msg_v01* resp);
    void onSimStateChanged();
    void onInitialAttachSet(uint8_t sim_id, uril_data_set_initial_params_resp_msg_v01* resp);
    void onNwStateChanged(uint8_t sim_id,
            uril_nw_get_registration_status_resp_msg_v01* resp);
    void onDataCallListChanged(uint8_t sim_id,
            uril_data_get_data_call_list_resp_msg_v01* resp);
    void updateDcContextFromDataCall(
            const uril_data_call_info_t_v01* dataCall);

public:
    virtual ~NetconfRilClient();

    bool init();

    void onIRilResponse(uril_client_handle_t hndl, uint8_t sim_id,
            uint32_t msg_id, const void* msg, size_t msg_len);
    void onIRilIndication(uril_client_handle_t hndl, uint8_t sim_id,
            uint32_t msg_id);
    bool releaseDataCall(const DcContextInterface* dcContext);
    bool setupDataCall(const DcContextInterface* dcContext,
            NetregContextInterface::Rat rat);

    static NetconfRilClient* getInstance();
};
#endif
