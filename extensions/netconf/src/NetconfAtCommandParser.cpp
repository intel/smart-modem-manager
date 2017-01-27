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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <smm_core/smm_log.h>
#include <smm_core/smm_utils.h>
#include <smm_core/AtCommandParser.h>
#include <smm_core/NetconfConfig.h>
#include "NetconfAtCommandParser.h"
#include "NetconfAtResponseParser.h"
#include "NetconfRilClient.h"
#include <smm_core/SharedEnv.h>


NetconfAtCommandParser::NetconfAtCommandParser()
    : AtCommandParser()
{
    registerMessage(new ATZCommand());
    registerMessage(new CGMMCommand());
    registerMessage(new CGDCONTCommand());
    registerMessage(new CGPADDRCommand());
    registerMessage(new CGCONTRDPCommand());
    registerMessage(new XGAUTHCommand());
    registerMessage(new CGACTCommand());
}

NetconfAtCommandParser::~NetconfAtCommandParser()
{
}

NetconfAtCommandParser::ATZCommand::ATZCommand()
    : AtCommand("ATZ")
{
}

bool NetconfAtCommandParser::ATZCommand::executeAsTest(OutputWriterInterface* output)
{
    return onSuccess(this, output);
}

bool NetconfAtCommandParser::ATZCommand::executeAsQuery(OutputWriterInterface* output)
{
    return onSuccess(this, output);
}

bool NetconfAtCommandParser::ATZCommand::executeAsWrite(OutputWriterInterface* output,
                                                        int argc,
                                                        const char** argv)
{
    return onSuccess(this, output);
}

NetconfAtCommandParser::CGMMCommand::CGMMCommand()
    : AtCommand("AT+CGMM")
{
}

bool NetconfAtCommandParser::CGMMCommand::executeAsTest(OutputWriterInterface* output)
{
    return onSuccess(this, output);
}

bool NetconfAtCommandParser::CGMMCommand::executeAsQuery(OutputWriterInterface* output)
{
    // this command is not yet supported, replying OK to fake the behavior.
    const char* rsp = "\r\nXMM7272 with Smart Modem Manager\r\n\r\nOK\r\n";
    output->writeToHost(rsp);
    // we have consumed this command
    return true;
}

bool NetconfAtCommandParser::CGMMCommand::executeAsWrite(OutputWriterInterface* output,
                                                         int argc,
                                                         const char** argv)
{
    return onError(this, output);
}

NetconfAtCommandParser::CGPADDRCommand::CGPADDRCommand()
    : AtCommand("AT+CGPADDR")
{
}

bool NetconfAtCommandParser::CGPADDRCommand::executeAsTest(OutputWriterInterface* output)
{
    SharedEnv* env = SharedEnv::getInstance();
    if (env->getDcContext() != nullptr && env->getDcContext()->isActivated()) {
        output->writeToHost("\r\n+CGPADDR: %d\r\n\r\nOK\r\n", env->getDcContext()->getCid());
    } else {
        output->writeToHost("\r\n+CGPADDR: \r\n\r\nOK\r\n");
    }
    return true;
}

bool NetconfAtCommandParser::CGPADDRCommand::executeAsQuery(OutputWriterInterface* output)
{
    return onError(this, output);
}

bool NetconfAtCommandParser::CGPADDRCommand::executeAsWrite(OutputWriterInterface* output,
                                                            int argc,
                                                            const char** argv)
{
    if (argc < 1) {
        return executeAsTest(output);
    }
    int cid = strtoul(argv[0], nullptr, 0);

    SMM_TRACE_D("CGPADDR { cid = %d}", cid);

    SharedEnv* env = SharedEnv::getInstance();

    if (env->getDcContext() != nullptr && env->getDcContext()->isActivated()
            && cid == env->getDcContext()->getCid()) {
        /*
         * +CGPADDR: <cid>[,<PDP_addr_1>[,<PDP_addr_2>]]
         */
        output->writeToHost("\r\n+CGPADDR: %d,\"%s\"\r\n\r\nOK\r\n",
                            cid,
                            env->getDcContext()->getPdpAddr());
    } else {
        SMM_TRACE_E("Unknown cid %d", cid);
        return onError(this, output);
    }
    return true;
}

NetconfAtCommandParser::CGCONTRDPCommand::CGCONTRDPCommand()
    : AtCommand("AT+CGCONTRDP")
{
}

bool NetconfAtCommandParser::CGCONTRDPCommand::executeAsTest(OutputWriterInterface* output)
{
    SharedEnv* env = SharedEnv::getInstance();

    if (env->getDcContext() != nullptr && env->getDcContext()->isActivated()) {
        output->writeToHost("\r\n+CGCONTRDP: %d\r\n\r\nOK\r\n",
                            env->getDcContext()->getCid());
    } else {
        output->writeToHost("\r\n+CGCONTRDP: \r\n\r\nOK\r\n");
    }
    return true;
}

bool NetconfAtCommandParser::CGCONTRDPCommand::executeAsQuery(OutputWriterInterface* output)
{
    return executeAsTest(output);
}

bool NetconfAtCommandParser::CGCONTRDPCommand::executeAsWrite(OutputWriterInterface* output,
                                                              int argc,
                                                              const char** argv)
{
    if (errorIfArgsCountLessThan(1, argc, output)) {
        return true;
    }
    int cid = strtoul(argv[0], nullptr, 0);

    SMM_TRACE_D("CGCONTRDPCommand { cid = %d}", cid);

    SharedEnv* env = SharedEnv::getInstance();

    if (env->getDcContext() != nullptr && env->getDcContext()->isActivated()
            && cid == env->getDcContext()->getCid()) {
        /*
         * +CGCONTRDP: <cid>, <bearer_id>, <apn> [,<source_addr and subnet_mask>[, <gw_addr>
         * [, <DNS_prim_addr>[, <DNS_sec_addr>[, <PCSCF_prim_addr>
         * [, <P-CSCF_sec_addr>[,<IM_CN_Signalling_Flag>]]]]]]]
         */
        output->writeToHost("\r\n+CGCONTRDP: %d,0,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\r\n\r\nOK\r\n",
                            cid,
                            env->getDcContext()->getApn(),
                            env->getDcContext()->getPdpAddr(),
                            env->getDcContext()->getPdpAddr(),
                            env->getDcContext()->getDns1(),
                            env->getDcContext()->getDns2());
    } else {
        SMM_TRACE_E("Unknown cid %d", cid);
        return onError(this, output);
    }
    return true;
}

NetconfAtCommandParser::CGDCONTCommand::CGDCONTCommand()
    : AtCommand("AT+CGDCONT")
{
}

bool NetconfAtCommandParser::CGDCONTCommand::executeAsTest(OutputWriterInterface* output)
{
    // let the modem reply
    return false;
}

bool NetconfAtCommandParser::CGDCONTCommand::executeAsQuery(OutputWriterInterface* output)
{
    SharedEnv* env = SharedEnv::getInstance();

    DcContext* dcContext = static_cast<DcContext*>(env->getDcContext());
    assert(dcContext != nullptr);

    if (!env->getNetconf()->getPersistence()->loadDcContext(dcContext)) {
        SMM_TRACE_E("Failed loading APN conf, using default conf");
    } else {
        SMM_TRACE_D("APN conf loaded");
    }

    if (dcContext->getCid() != -1) {
        output->writeToHost("\r\n+CGDCONT: %d,\"%s\",\"%s\",\"%s\",%d,%d\r\n\r\nOK\r\n",
                            dcContext->getCid(),
                            dcContext->getPdpType(),
                            dcContext->getApn(),
                            dcContext->getPdpAddr(),
                            dcContext->getDComp(),
                            dcContext->getHComp());
    } else {
        output->writeToHost("\r\n+CGDCONT:\r\n\r\nOK\r\n");
    }

    // we have consumed this command
    return true;
}

bool NetconfAtCommandParser::CGDCONTCommand::executeAsWrite(OutputWriterInterface* output,
                                                            int argc,
                                                            const char** argv)
{
    // "AT+CGDCONT=[<cid>[,<PDP_type>[,<APN>[,<PDP_addr>[,<d_comp>[,<h_comp>>]]]]]"
    int cid = -1;
    int dComp = 0;
    int hComp = 0;
    const char* pdpAddr = INVALID_IPV4_ADDR;
    const char* rsp = "\r\nOK\r\n";

    if (errorIfArgsCountLessThan(3, argc, output)) {
        return true;
    }

    cid = strtoul(argv[0], nullptr, 0);
    std::string pdpType(argv[1]);
    std::string apn(argv[2]);

    if (strcmp(pdpType.c_str(), IP_TYPE_IPV4) != 0
        && strcmp(pdpType.c_str(), IP_TYPE_IPV4V6) != 0
        && strcmp(pdpType.c_str(), IP_TYPE_IPV6) != 0) {
        SMM_TRACE_E("Invalid PDP type");
        return this->onError(this, output);
    }

    if (argc >= 4) {
        pdpAddr = argv[3];
        if (!isValidIpV4Address(pdpAddr) && !isValidIpV6Address(pdpAddr)) {
            SMM_TRACE_E("Invalid IP addr");
            return this->onError(this, output);
        }
    }
    if (argc >= 5) {
        dComp = strtoul(argv[4], nullptr, 0);
    }
    if (argc >= 6) {
        hComp = strtoul(argv[5], nullptr, 0);
    }
    SMM_TRACE_D(
        "CGDCONTCommand { cid=%d, pdpType=%s, apn=%s, pdpAddr=%s, dComp=%d, hComp=%d }",
        cid,
        pdpType.c_str(),
        apn.c_str(),
        pdpAddr,
        dComp,
        hComp);

    SharedEnv* env = SharedEnv::getInstance();

    DcContext* dcContext = env->getNetconf()->getDcContext();
    assert(dcContext != nullptr);

    SMM_TRACE_D("Loading APN conf");
    if (!env->getNetconf()->getPersistence()->loadDcContext(dcContext)) {
        SMM_TRACE_E("Failed loading APN conf, using default conf");
    } else {
        SMM_TRACE_D("APN conf loaded");
    }

    dcContext->setCid(cid);
    dcContext->setPdpType(pdpType);
    dcContext->setApn(apn);
    // IP addr should be allocated by the network
    /*if (argc >= 4) {
     *  dcContext->setPdpAddr(pdpAddr);
     *  }
     *  if (argc >= 5) {
     *          dcContext->setDComp(dComp);
     *  }
     *  if (argc >= 6) {
     *          dcContext->setHComp(hComp);
     *  }*/
    if (argc >= 4) {
        SMM_TRACE_E("Ignoring PDP addr, DComp and HComp");
    }

    if (!env->getNetconf()->getPersistence()->storeDcContext(dcContext)) {
        SMM_TRACE_E("Couldnt persist CGDCONT data");
        return this->onError(this, output);
    } else {
        SMM_TRACE_D("CGDCONT data persisted");
    }
    output->writeToHost(rsp);
    // we have consumed this command
    return true;
}

NetconfAtCommandParser::XGAUTHCommand::XGAUTHCommand()
    : AtCommand("AT+XGAUTH")
{
}

bool NetconfAtCommandParser::XGAUTHCommand::executeAsTest(OutputWriterInterface* output)
{
    // let the modem reply
    return false;
}

bool NetconfAtCommandParser::XGAUTHCommand::executeAsQuery(OutputWriterInterface* output)
{
    SharedEnv* env = SharedEnv::getInstance();

    DcContext* dcContext = env->getNetconf()->getDcContext();
    assert(dcContext != nullptr);

    if (!env->getNetconf()->getPersistence()->loadDcContext(dcContext)) {
        SMM_TRACE_E("Failed loading APN conf, using default conf");
    } else {
        SMM_TRACE_D("APN conf loaded");
    }

    if (dcContext->getCid() != -1) {
        output->writeToHost("\r\n+XGAUTH: %d,%d,\"%s\",\"%s\"\r\n\r\nOK\r\n",
                            dcContext->getCid(),
                            dcContext->getAuthMode(),
                            dcContext->getAuthName(),
                            dcContext->getAuthPwd());
    } else {
        output->writeToHost("\r\n+XGAUTH:\r\n\r\nOK\r\n");
    }

    // we have consumed this command
    return true;
}

bool NetconfAtCommandParser::XGAUTHCommand::executeAsWrite(OutputWriterInterface* output,
                                                            int argc,
                                                            const char** argv)
{
    // "AT+XGAUTH=<cid>,<auth>,<name>,<pwd>"
    int cid = -1;
    int authMode = DcContextInterface::AuthType::AUTH_MODE_DISABLED;
    const char* rsp = "\r\nOK\r\n";

    if (errorIfArgsCountLessThan(4, argc, output)) {
        return true;
    }

    if (authMode < DcContextInterface::AuthType::AUTH_MODE_DISABLED
        || authMode > DcContextInterface::AuthType::AUTH_MODE_CHAP) {
        SMM_TRACE_E("Invalid XGAUTH Authentication Mode");
        return this->onError(this, output);
    }

    if ((strlen(argv[2]) > AUTH_NAME_PWD_LEN)
        || (strlen(argv[3]) > AUTH_NAME_PWD_LEN)) {
        SMM_TRACE_E("Too Long XGAUTH Name and/or Password Parameter");
        return this->onError(this, output);
    }

    cid = strtoul(argv[0], nullptr, 0);
    authMode = strtoul(argv[1], nullptr, 0);
    std::string authName(argv[2]);
    std::string authPwd(argv[3]);


    SMM_TRACE_D(
        "XGAUTHCommand { cid=%d, auth=%d, name=%s, pwd=%s }",
        cid,
        authMode,
        authName.c_str(),
        authPwd.c_str());

    SharedEnv* env = SharedEnv::getInstance();

    DcContext* dcContext = env->getNetconf()->getDcContext();
    assert(dcContext != nullptr);

    if (!env->getNetconf()->getPersistence()->loadDcContext(dcContext)) {
        SMM_TRACE_E("Failed loading APN conf, using default conf");
    } else {
        SMM_TRACE_D("APN conf loaded");
    }

    if (dcContext->getCid() == -1) {
        SMM_TRACE_E("Define PDP Context by using AT+CGDCONT before trying XGAUTH");
        return this->onError(this, output);
    }

    if (dcContext->getCid() != cid) {
        SMM_TRACE_E("Wrong XGAUTH cid parameter, cannot set Autentication for non existing cid");
        return this->onError(this, output);
    }

    dcContext->setAuthMode(authMode);
    dcContext->setAuthName(authName);
    dcContext->setAuthPwd(authPwd);

    if (!env->getNetconf()->getPersistence()->storeDcContext(dcContext)) {
        SMM_TRACE_E("Couldnt persist XGAUTH data");
        return this->onError(this, output);
    } else {
        SMM_TRACE_D("XGAUTH data persisted");
    }

    output->writeToHost(rsp);
    // we have consumed this command
    return true;
}

NetconfAtCommandParser::CGACTCommand::CGACTCommand()
    : AtCommand("AT+CGACT")
{
}

bool NetconfAtCommandParser::CGACTCommand::executeAsTest(OutputWriterInterface* output)
{
    // let the modem reply
    return false;
}

bool NetconfAtCommandParser::CGACTCommand::executeAsQuery(OutputWriterInterface* output)
{
    SharedEnv* env = SharedEnv::getInstance();

    DcContext* dcContext = static_cast<DcContext*>(env->getDcContext());

    if (dcContext != nullptr && dcContext->getCid() != -1) {
        output->writeToHost("\r\n+CGACT: %d,%d\r\n\r\nOK\r\n",
                            dcContext->getCid(),
                            dcContext->isActivated() ? 1 : 0);
    } else {
        output->writeToHost("\r\n+CGACT:\r\n\r\nOK\r\n");
    }

    // we have consumed this command
    return true;
}

bool NetconfAtCommandParser::CGACTCommand::executeAsWrite(OutputWriterInterface* output,
                                                          int argc,
                                                          const char** argv)
{
    // AT+CGACT=<state>,<cid>

    int cid = -1;
    int state = -1;

    if (errorIfArgsCountLessThan(2, argc, output)) {
        return true;
    }

    state = strtoul(argv[0], nullptr, 0);
    cid = strtoul(argv[1], nullptr, 0);

    SMM_TRACE_D("CGACTCommand { state = %d, cid = %d }", state, cid);

    SharedEnv* env = SharedEnv::getInstance();

    if (state == 1) {
        // no NW registration
        if (env->getNetregContext()->getRat() == NetregContext::Rat::NONE) {
            SMM_TRACE_D("No network registration, cannot activate PDP");
            return this->onError(this, output);
        }

        if (env->getDcContext() == nullptr) {
            SMM_TRACE_D("No DC context defined");
            return this->onError(this, output);
        }
        // check if a PDP is already established (case where we attached to LTE)
        if (env->getDcContext()->isActivated()) {
            SMM_TRACE_D("PDP is already established");
            return this->onSuccess(this, output);
        }
        if (NetconfRilClient::getInstance()->setupDataCall(env->getDcContext(),
                env->getNetregContext()->getRat())) {
            env->enqueuePendingMessage(this);
            SMM_TRACE_D("setupDataCall() succeeded");
        } else {
            SMM_TRACE_D("setupDataCall() failed");
            return this->onError(this, output);
        }
    } else {
        // no NW registration
        if (env->getNetregContext()->getRat() == NetregContext::Rat::NONE) {
            SMM_TRACE_D("No network registration, cannot deactivate PDP");
            return this->onError(this, output);
        }

        if (env->getDcContext() == nullptr) {
            SMM_TRACE_D("No DC context defined");
            return this->onError(this, output);
        }

        // check if a PDP is already deactivated
        if (!env->getDcContext()->isActivated()) {
            SMM_TRACE_D("PDP is already deactivated");
            return this->onSuccess(this, output);
        }
        if (NetconfRilClient::getInstance()->releaseDataCall(env->getDcContext())) {
            env->enqueuePendingMessage(this);
            SMM_TRACE_D("releaseDataCall() succeeded");
        } else {
            SMM_TRACE_D("releaseDataCall() failed");
            return this->onError(this, output);
        }
    }

    // we have consumed this command
    return true;
}
