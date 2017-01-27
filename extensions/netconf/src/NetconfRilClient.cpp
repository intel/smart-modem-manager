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

#include <string>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <uril/uril_common_v01.h>
#include <uril/uril_data_v01.h>
#include <uril/uril_nw_v01.h>
#include <uril/uril_radio_v01.h>
#include "NetconfRilClient.h"
#include <smm_core/NetregContextInterface.h>
#include <smm_core/DcContextInterface.h>
#include <smm_core/smm_log.h>
#include <smm_core/NetconfConfig.h>

void irilRspCb(uril_client_handle_t hndl, uint8_t sim_id, uint32_t msg_id,
        const void* msg, size_t msg_len, const void* client_data) {
    NetconfRilClient* thisPtr = (NetconfRilClient*) client_data;
    thisPtr->onIRilResponse(hndl, sim_id, msg_id, msg, msg_len);
}

void irilIndCb(uril_client_handle_t hndl, uint8_t sim_id,
        uint32_t msg_id) {
    NetconfRilClient* thisPtr = NetconfRilClient::getInstance();
    thisPtr->onIRilIndication(hndl, sim_id, msg_id);
}

NetconfRilClient::NetconfRilClient() {
    mEnv = SharedEnv::getInstance();
}

NetconfRilClient::~NetconfRilClient() {
}

NetconfRilClient* NetconfRilClient::sInstance = nullptr;

NetconfRilClient* NetconfRilClient::getInstance() {
    SMM_TRACE_D("NetconfRilClient::getInstance()");
    if (sInstance == nullptr) {
        SMM_TRACE_D("Creating new instance");
        sInstance = new NetconfRilClient();
    }
    return sInstance;
}

bool NetconfRilClient::init() {
    SMM_TRACE_D("NetconfRilClient::init()");
    int ret = uril_client_init(&mHandle, irilIndCb, irilRspCb);

    if (ret >= 0) {
        uril_radio_set_radio_mode_req_msg_v01 msg;
        msg.radio_mode = IRIL_CORE_RADIO_MODE_ONLINE_V01;

        ret = IRIL_CORE_CLIENT_EXECUTE_COMMAND_ASYNC(mHandle, 0,
                IRIL_CORE_RADIO_SET_RADIO_MODE_REQ_V01, &msg, nullptr, this);
    }
    return ret >= 0;
}

void NetconfRilClient::onIRilResponse(uril_client_handle_t hndl,
        uint8_t sim_id, uint32_t msg_id, const void* msg, size_t msg_len) {
    SMM_TRACE_D("NetconfRilClient::onIRilResponse(%d, %d, %d)", hndl, sim_id,
            msg_id);

    switch (msg_id) {
    case IRIL_CORE_DATA_SETUP_DATA_CALL_RESP_V01: {
        uril_data_setup_data_call_resp_msg_v01* resp =
                (uril_data_setup_data_call_resp_msg_v01*) msg;
        this->onDataCallSetup(sim_id, resp);
    }
        return;
    case IRIL_CORE_DATA_DEACTIVATE_DATA_CALL_RESP_V01: {
        uril_data_deactivate_data_call_resp_msg_v01* resp =
                (uril_data_deactivate_data_call_resp_msg_v01*) msg;
        this->onDataCallReleased(sim_id, resp);
    }
        return;
    case IRIL_CORE_DATA_SET_INITIAL_ATTACH_PARAMS_RESP_V01: {
        uril_data_set_initial_params_resp_msg_v01* resp =
                (uril_data_set_initial_params_resp_msg_v01*) msg;
        this->onInitialAttachSet(sim_id, resp);
    }
        return;
    case IRIL_CORE_NW_GET_REGISTRATION_STATUS_RESP_V01: {
        uril_nw_get_registration_status_resp_msg_v01* resp =
                (uril_nw_get_registration_status_resp_msg_v01*) msg;
        this->onNwStateChanged(sim_id, resp);
    }
        return;
    case IRIL_CORE_DATA_GET_DATA_CALL_LIST_RESP_V01: {
        uril_data_get_data_call_list_resp_msg_v01* resp =
                (uril_data_get_data_call_list_resp_msg_v01*) msg;
        this->onDataCallListChanged(sim_id, resp);
    }
        return;
    case IRIL_CORE_RADIO_SET_RADIO_MODE_RESP_V01: {
        uril_radio_set_radio_mode_resp_msg_v01* resp =
                (uril_radio_set_radio_mode_resp_msg_v01*) msg;
        this->onRadioModeSet(sim_id, resp);
    }
        return;
    case IRIL_CORE_NW_SELECTION_RESP_V01: {
        uril_nw_selection_resp_msg_v01* resp =
                (uril_nw_selection_resp_msg_v01*) msg;
        if (resp->response.result
                == uril_result_t_v01::IRIL_CORE_RESULT_SUCCESS_V01) {
            SMM_TRACE_D("auto attach request successful");
        } else {
            SMM_TRACE_E("auto attach request failed");
        }
    }
        return;
    default:
        SMM_TRACE_E("Unexpected msg id (%d)", msg_id);
        return;
    }
    return;
}

void NetconfRilClient::onIRilIndication(uril_client_handle_t hndl,
        uint8_t sim_id, uint32_t msg_id) {
    SMM_TRACE_D("NetconfRilClient::onIRilIndication(%d, %d, %d)", hndl, sim_id,
            msg_id);

    switch (msg_id) {
    case IRIL_CORE_NW_REGISTRATION_EVENT_IND_V01: {
        SMM_TRACE_D("IRIL_CORE_NW_REGISTRATION_EVENT_IND_V01");
        uint32_t msg_id = IRIL_CORE_NW_GET_REGISTRATION_STATUS_REQ_V01;
        uril_nw_get_registration_status_req_msg_v01 req;

        SMM_TRACE_D("Requesting registration status");
        IRIL_CORE_CLIENT_EXECUTE_COMMAND_ASYNC(mHandle, 0, msg_id, &req,
                nullptr, this);
    }
        break;
    case IRIL_CORE_DATA_CALL_LIST_CHANGED_EVENT_IND_V01: {
        SMM_TRACE_D("NetconfRilClient::onDataCallListChanged()");
        uint32_t msg_id = IRIL_CORE_DATA_GET_DATA_CALL_LIST_REQ_V01;
        uril_data_get_data_call_list_req_msg_v01 req;

        SMM_TRACE_D("Requesting data call list");
        IRIL_CORE_CLIENT_EXECUTE_COMMAND_ASYNC(mHandle, 0, msg_id, &req,
                nullptr, this);
    }
        break;
    }
}

void NetconfRilClient::onSimStateChanged() {
    SMM_TRACE_D("NetconfRilClient::onSimStateChanged()");
}

void NetconfRilClient::onInitialAttachSet(uint8_t sim_id, uril_data_set_initial_params_resp_msg_v01* resp) {
    SMM_TRACE_D("NetconfRilClient::onInitialAttachSet()");
    if (resp->response.result
            == uril_result_t_v01::IRIL_CORE_RESULT_SUCCESS_V01) {
        SMM_TRACE_D("Initial attach set, triggering auto nw attach");
        uint32_t msg_id = IRIL_CORE_NW_SELECTION_REQ_V01;
        uril_nw_selection_req_msg_v01 req;
        memset(&req, 0, sizeof(req));
        req.nw_selection_info.nw_selection_type =
                uril_nw_selection_type_t_v01::IRIL_CORE_NW_SELECTION_AUTOMATIC_V01;

        IRIL_CORE_CLIENT_EXECUTE_COMMAND_ASYNC(mHandle, 0, msg_id, &req,
                nullptr, this);
    } else {
        SMM_TRACE_E("Setting initial attach failed");
    }
}

void NetconfRilClient::onRadioModeSet(uint8_t sim_id,
        uril_radio_set_radio_mode_resp_msg_v01* resp) {
    SMM_TRACE_D("NetconfRilClient::onRadioModeSet()");

    if (resp->response.result
            == uril_result_t_v01::IRIL_CORE_RESULT_SUCCESS_V01) {

        DcContext* dc = static_cast<DcContext*>(mEnv->getDcContext());
        const char* apn;

        if (!mEnv->getNetconf()->getPersistence()->loadDcContext(dc)) {
            SMM_TRACE_E("Failed loading APN conf");
            apn = "";
        } else {
            SMM_TRACE_D("APN conf loaded");
            apn = dc->getApn();
        }
        SMM_TRACE_D("setting initial attach");
        uint32_t msg_id = IRIL_CORE_DATA_SET_INITIAL_ATTACH_PARAMS_REQ_V01;
            uril_data_set_initial_params_req_msg_v01 req;
            memset(&req, 0, sizeof(req));
            strcpy(req.apn, apn);

            IRIL_CORE_CLIENT_EXECUTE_COMMAND_ASYNC(mHandle, 0, msg_id, &req,
                    nullptr, this);
    } else {
        SMM_TRACE_E("Setting radio mode failed");
    }
}

void NetconfRilClient::onNwStateChanged(uint8_t sim_id,
        uril_nw_get_registration_status_resp_msg_v01* resp) {
    SMM_TRACE_D("NetconfRilClient::onNwStateChanged()");

    if (resp->response.result
            != uril_result_t_v01::IRIL_CORE_RESULT_SUCCESS_V01) {
        SMM_TRACE_E("get nw reg status failed");
        return;
    }

    if (mEnv != nullptr) {
        int lac = 0, cellid = 0;
        char* mcc = nullptr, *mnc = nullptr;
        NetregContext::Rat rat;
        bool psattached = false;
        bool csattached = false;

        NetregContext* nr = mEnv->getNetconf()->getNetregContext();

        psattached = resp->data_registration_follows &&
            resp->data_registration.registration_state
                == uril_nw_service_t_v01::IRIL_CORE_NW_SERVICE_FULL_V01;

        csattached = resp->voice_registration_follows &&
                    resp->voice_registration.registration_state
                        == uril_nw_service_t_v01::IRIL_CORE_NW_SERVICE_FULL_V01;
        if (csattached) {
            SMM_TRACE_D("CS attached");
            switch (resp->voice_registration.rat) {
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_TD_SCDMA_V01:
                SMM_TRACE_D("rat is TDSCDMA");
                if (resp->nw_cell_identity_tdscdma_follows) {
                    cellid = resp->nw_cell_identity_tdscdma.cid;
                    lac = resp->nw_cell_identity_tdscdma.lac;
                    mcc = resp->nw_cell_identity_tdscdma.mcc;
                    mnc = resp->nw_cell_identity_tdscdma.mnc;
                }
                rat = NetregContext::Rat::TDSCDMA;
                break;
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_GSM_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_EDGE_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_GPRS_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_1xRTT_V01:
                SMM_TRACE_D("rat is GSM");
                if (resp->nw_cell_identity_gsm_follows) {
                    cellid = resp->nw_cell_identity_gsm.cid;
                    lac = resp->nw_cell_identity_gsm.lac;
                    mcc = resp->nw_cell_identity_gsm.mcc;
                    mnc = resp->nw_cell_identity_gsm.mnc;
                }
                rat = NetregContext::Rat::GSM;
                break;
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_HSPAP_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_EHRPD_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_EVDO_B_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_HSPA_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_HSUPA_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_HSDPA_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_EVDO_A_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_EVDO_0_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_IS95B_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_IS95A_V01:
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_UMTS_V01:
                SMM_TRACE_D("rat is UMTS");
                if (resp->nw_cell_identity_wcdma_follows) {
                    cellid = resp->nw_cell_identity_wcdma.cid;
                    lac = resp->nw_cell_identity_wcdma.lac;
                    mcc = resp->nw_cell_identity_wcdma.mcc;
                    mnc = resp->nw_cell_identity_wcdma.mnc;
                }
                rat = NetregContext::Rat::UMTS;
                break;
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_LTE_V01:
                SMM_TRACE_D("rat is LTE");
                if (resp->nw_cell_identity_lte_follows) {
                    cellid = resp->nw_cell_identity_lte.ci;
                    mcc = resp->nw_cell_identity_lte.mcc;
                    mnc = resp->nw_cell_identity_lte.mnc;
                }
                rat = NetregContext::Rat::LTE;
                break;
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_IWLAN_V01:
                SMM_TRACE_E("IWLAN rat is not supported");
                break;
            case uril_nw_rat_t_v01::IRIL_CORE_NW_RAT_NONE_V01:
            default:
                SMM_TRACE_E("unknown rat");
                break;
            }
            nr->setRat((NetregContext::Rat) rat);
            if (mnc != nullptr && mcc != nullptr) {
                nr->setMcc(atoi(mcc));
                nr->setMnc(atoi(mnc));
            }
            nr->setLac(lac);
            nr->setCellId(cellid);
        } else {
            SMM_TRACE_D("No CS attach");
        }
        nr->setPsAttached(psattached);
    }
}

void NetconfRilClient::updateDcContextFromDataCall(
        const uril_data_call_info_t_v01* dataCall) {
    std::vector<DcContext*>::iterator i;
    std::vector<DcContext*>* dcs = mEnv->getNetconf()->getDcMultiContexts();
    DcContext* foundDc = nullptr;
    bool wasActivated = false;

    for (i = dcs->begin(); i != dcs->end(); ++i) {
        if ((*i)->getCid() == dataCall->cid || (*i)->getCid() == -1) {
            foundDc = *i;
            break;
        }
    }
    if (foundDc == nullptr) { // insert new DC
        foundDc = new DcContext();
        dcs->push_back(foundDc);
    } else {
        wasActivated = foundDc->isActivated();
    }

    foundDc->setCid(dataCall->cid);
    foundDc->setActivated(
            dataCall->phy_link_state
                    == uril_physical_link_state_t_v01::IRIL_CORE_DATA_PHYSLINK_STATE_ACTIVE_UP_V01);
    std::string apn(dataCall->interface_name);
    foundDc->setApn(apn);
    std::string dns1("8.8.8.8");
    //std::string dns1(dataCall->dnsp_addr.addr); TODO: check the parsing of DNS1
    foundDc->setDns1(dns1);
    //std::string dns2(dataCall->dnss_addr.addr); TODO: check the parsing of DNS2
    std::string dns2("8.8.8.8");
    foundDc->setDns2(dns2);
    std::string pdpAddr(dataCall->pdp_addrs[0].addr);
    foundDc->setPdpAddr(pdpAddr);

    switch (dataCall->pdp_type) {
    case uril_data_pdp_type_t_v01::IRIL_CORE_DATA_PDP_TYPE_IP: {
        std::string pdptype(IP_TYPE_IPV4);
        foundDc->setPdpType(pdptype);
    }
        break;
    case uril_data_pdp_type_t_v01::IRIL_CORE_DATA_PDP_TYPE_IPV4V6: {
        std::string pdptype(IP_TYPE_IPV4V6);
        foundDc->setPdpType(pdptype);
    }
        break;
    case uril_data_pdp_type_t_v01::IRIL_CORE_DATA_PDP_TYPE_IPV6: {
        std::string pdptype(IP_TYPE_IPV6);
        foundDc->setPdpType(pdptype);
    }
        break;
    default:
        break;
    }
    if (wasActivated && !foundDc->isActivated()) {
        mEnv->getNetconf()->onDataCallTerminated(foundDc->getCid(),
                dataCall->interface_name);
    } else if (!wasActivated && foundDc->isActivated()) {
        mEnv->getNetconf()->onDataCallEstablished(foundDc->getCid(),
                dataCall->interface_name, foundDc->getDns1(),
                foundDc->getDns2());
    }
}

void NetconfRilClient::onDataCallListChanged(uint8_t sim_id,
        uril_data_get_data_call_list_resp_msg_v01* resp) {
    SMM_TRACE_D("NetconfRilClient::onDataCallListChanged()");

    for (uint8_t i = 0; i < resp->data_calls_len; i++) {
        this->updateDcContextFromDataCall(&(resp->data_calls[i]));
    }
}

bool NetconfRilClient::setupDataCall(const DcContextInterface* dcContext,
        NetregContextInterface::Rat rat) {
    SMM_TRACE_D("NetconfRilClient::setupDataCall()");
    int ret;
    uint32_t msg_id = IRIL_CORE_DATA_SETUP_DATA_CALL_REQ_V01;
    uril_data_setup_data_call_req_msg_v01 req;
    memset(&req, 0, sizeof(req));

    SMM_TRACE_D("NetconfRilClient::setupDataCall()");

// RAT
// TODO: for now RAT does not matter as it is not taken cared in lower layers
    switch (rat) {
    case NetregContextInterface::NONE:
        req.rat = IRIL_CORE_DATA_BEARER_TECH_TYPE_UNKNOWN_V01;
        break;
    case NetregContextInterface::GSM:
        req.rat = IRIL_CORE_DATA_BEARER_TECH_TYPE_HSPA_V01;
        break;
    case NetregContextInterface::LTE:
        req.rat = IRIL_CORE_DATA_BEARER_TECH_TYPE_LTE_V01;
        break;
    default: // TODO: All TECH TYPE MODES
        SMM_TRACE_E("Unknown RAT tech (%d)", rat);
        req.rat = IRIL_CORE_DATA_BEARER_TECH_TYPE_UNKNOWN_V01;
        break;
    }

// TODO: profile_id
    req.profile_id = 0;

// APN
    strncpy(req.apn, dcContext->getApn(), IRIL_CORE_DATA_MAX_APN_LEN_V01);
    req.apn[IRIL_CORE_DATA_MAX_APN_LEN_V01 - 1] = '\0';

// PDP
    if (!strcmp(dcContext->getPdpType(), IP_TYPE_IPV4)) {
        req.pdp_type = IRIL_CORE_DATA_PDP_TYPE_IP;
    } else if (!strcmp(dcContext->getPdpType(), IP_TYPE_IPV4V6)) {
        req.pdp_type = IRIL_CORE_DATA_PDP_TYPE_IPV4V6;
    } else if (!strcmp(dcContext->getPdpType(), IP_TYPE_IPV6)) {
        req.pdp_type = IRIL_CORE_DATA_PDP_TYPE_IPV6;
    } else {
        SMM_TRACE_E("Unknown PDP type (%s)", dcContext->getPdpType());
        return false;
    }

// TODO: auth_type, user, password
    req.auth_type = IRIL_CORE_DATA_AUTH_TYPE_NONE;

    ret = IRIL_CORE_CLIENT_EXECUTE_COMMAND_ASYNC(mHandle, 0, msg_id, &req,
            nullptr, this);
    if (ret) {
        SMM_TRACE_E("Unable to start session, ril command sync failed");
        return false;
    }

    return true;
}

void NetconfRilClient::onDataCallSetup(uint8_t sim_id,
        uril_data_setup_data_call_resp_msg_v01* resp) {
    bool result = false;

    SMM_TRACE_D("NetconfRilClient::onDataCallSetup()");

    if (resp->response.result != IRIL_CORE_RESULT_SUCCESS_V01) {
        SMM_TRACE_E("Unable to start session (%d)", resp->response.error);
    } else {
        SMM_TRACE_D("Data call setup successful");
        updateDcContextFromDataCall(&(resp->data_call));
        result = true;
    }
    SmmMessage* msg = mEnv->dequeuePendingMessage();
    if (msg != nullptr) {
        if (result) {
            msg->onSuccess(nullptr, mEnv->getOutputWriter());
        } else {
            msg->onError(nullptr, mEnv->getOutputWriter());
        }
    }
}

bool NetconfRilClient::releaseDataCall(const DcContextInterface* dcContext) {
    SMM_TRACE_D("NetconfRilClient::releaseDataCall()");

    int ret;
    uint32_t msg_id = IRIL_CORE_DATA_DEACTIVATE_DATA_CALL_REQ_V01;
    uril_data_deactivate_data_call_req_msg_v01
    req = {
        .cid = (uint8_t) dcContext->getCid(),
        .reason = IRIL_CORE_DATA_END_REASON_NONE,
    };

    SMM_TRACE_D("NetconfRilClient::releaseDataCall()");

    ret = IRIL_CORE_CLIENT_EXECUTE_COMMAND_ASYNC(mHandle, 0, msg_id, &req,
            nullptr, this);
    if (ret < 0) {
        SMM_TRACE_E("Unable to release data call, ril command sync failed");
        return false;
    } else {
        SMM_TRACE_D("Release data call request sent");
    }
    return true;
}

void NetconfRilClient::onDataCallReleased(uint8_t sim_id,
        uril_data_deactivate_data_call_resp_msg_v01 *resp) {
    bool result = false;
    SMM_TRACE_D("NetconfRilClient::onDataCallReleased()");

    if (resp->response.result == IRIL_CORE_RESULT_SUCCESS_V01) {
        SMM_TRACE_D("Data call release successful");
        result = true;
    } else {
        SMM_TRACE_E("Unable to release data call (%d)", resp->response.error);
    }
    SmmMessage* msg = mEnv->dequeuePendingMessage();
    if (msg != nullptr) {
        if (result) {
            msg->onSuccess(nullptr, mEnv->getOutputWriter());
        } else {
            msg->onError(nullptr, mEnv->getOutputWriter());
        }
    }
}
