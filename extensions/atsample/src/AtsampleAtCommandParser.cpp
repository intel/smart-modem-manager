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
#include <smm_core/AtCommandParser.h>
#include "AtsampleAtCommandParser.h"
#include "AtsampleAtResponseParser.h"


AtsampleAtCommandParser::AtsampleAtCommandParser(SharedEnvInterface* env)
    : AtCommandParser()
{
    assert(env != nullptr);
    mEnv = env;

    registerMessage(new CGMMCommand(env));
    registerMessage(new SCPINCommand(env));
    registerMessage(new SSIOCommand(env));
    registerMessage(new SPIOCommand(env));
    registerMessage(new SCIDCommand(env));
    registerMessage(new SRADCCommand(env));
    registerMessage(new SMONICommand(env));
    registerMessage(new SWWANCommand(env));
}

AtsampleAtCommandParser::~AtsampleAtCommandParser()
{
}

AtsampleAtCommandParser::SCPINCommand::SCPINCommand(SharedEnvInterface* env)
    : AtCommand("AT^SCPIN")
{
    assert(env != nullptr);
    mEnv = env;
}

bool AtsampleAtCommandParser::SCPINCommand::executeAsTest(OutputWriterInterface* output)
{
    return onError(this, output);
}

bool AtsampleAtCommandParser::SCPINCommand::executeAsQuery(OutputWriterInterface* output)
{
    return onError(this, output);
}

bool AtsampleAtCommandParser::SCPINCommand::executeAsWrite(OutputWriterInterface* output,
                                                        int argc,
                                                        const char** argv)
{
    // AT^SCPIN=<mode>,<pin_id>,<direction>[,<startValue>]
    int mode = -1;
    int pinId = -1;
    int direction = -1;
    int startValue = -1;

    if (errorIfArgsCountLessThan(3, argc, output)) {
        return true;
    }

    mode = strtoul(argv[0], nullptr, 0);
    pinId = strtoul(argv[1], nullptr, 0);
    direction = strtoul(argv[2], nullptr, 0);

    if (argc >= 4) {
        startValue = strtoul(argv[3], nullptr, 0);
    }
    SMM_TRACE_D("SCPINCommand { mode = %d, pinId = %d, direction = %d, startValue = %d }",
                mode, pinId, direction, startValue);

    // this command is not yet supported, replying OK to fake the behavior.
    return onSuccess(this, output);
}

AtsampleAtCommandParser::CGMMCommand::CGMMCommand(SharedEnvInterface* env)
    : AtCommand("AT+CGMM")
{
    assert(env != nullptr);
    mEnv = env;
}

bool AtsampleAtCommandParser::CGMMCommand::executeAsTest(OutputWriterInterface* output)
{
    return onSuccess(this, output);
}

bool AtsampleAtCommandParser::CGMMCommand::executeAsQuery(OutputWriterInterface* output)
{
    // this command is not yet supported, replying OK to fake the behavior.
    const char* rsp = "\r\nXMM7272 with Smart Modem Manager (Atsample)\r\n\r\nOK\r\n";
    output->writeToHost(rsp);
    // we have consumed this command
    return true;
}

bool AtsampleAtCommandParser::CGMMCommand::executeAsWrite(OutputWriterInterface* output,
                                                       int argc,
                                                       const char** argv)
{
    return onError(this, output);
}

AtsampleAtCommandParser::SSIOCommand::SSIOCommand(SharedEnvInterface* env)
    : AtCommand("AT^SSIO")
{
    assert(env != nullptr);
    mEnv = env;
}

bool AtsampleAtCommandParser::SSIOCommand::executeAsTest(OutputWriterInterface* output)
{
    return onError(this, output);
}

bool AtsampleAtCommandParser::SSIOCommand::executeAsQuery(OutputWriterInterface* output)
{
    return onError(this, output);
}

bool AtsampleAtCommandParser::SSIOCommand::executeAsWrite(OutputWriterInterface* output,
                                                       int argc,
                                                       const char** argv)
{
    // AT^SSIO=<io_id>,<value>
    int ioId = -1;
    int value = -1;

    if (errorIfArgsCountLessThan(2, argc, output)) {
        return true;
    }
    ioId = strtoul(argv[0], nullptr, 0);
    value = strtoul(argv[1], nullptr, 0);

    SMM_TRACE_D("SSIOCommand { ioId = %d, value = %d }",
                ioId, value);

    // this command is not yet supported, replying OK to fake the behavior.
    return onSuccess(this, output);
}

AtsampleAtCommandParser::SPIOCommand::SPIOCommand(SharedEnvInterface* env)
    : AtCommand("AT^SPIO")
{
    assert(env != nullptr);
    mEnv = env;
}

bool AtsampleAtCommandParser::SPIOCommand::executeAsTest(OutputWriterInterface* output)
{
    return onError(this, output);
}

bool AtsampleAtCommandParser::SPIOCommand::executeAsQuery(OutputWriterInterface* output)
{
    return onError(this, output);
}

bool AtsampleAtCommandParser::SPIOCommand::executeAsWrite(OutputWriterInterface* output,
                                                       int argc,
                                                       const char** argv)
{
    // AT^SPIO=<mode>
    int mode = -1;

    if (errorIfArgsCountLessThan(1, argc, output)) {
        return true;
    }
    mode = strtoul(argv[0], nullptr, 0);

    SMM_TRACE_D("SPIOCommand { mode = %d }", mode);

    return onSuccess(this, output);
}

AtsampleAtCommandParser::SCIDCommand::SCIDCommand(SharedEnvInterface* env)
    : AtCommand("AT^SCID")
{
    assert(env != nullptr);
    mEnv = env;
}

bool AtsampleAtCommandParser::SCIDCommand::executeAsTest(OutputWriterInterface* output)
{
    return onSuccess(this, output);
}

bool AtsampleAtCommandParser::SCIDCommand::executeAsQuery(OutputWriterInterface* output)
{
    // AT^SCID
    SMM_TRACE_D("SCIDCommand { }");

    mEnv->enqueuePendingMessage(this);

    // translate into AT+CCID
    const char* cmd = "AT+CCID\r\n";
    output->writeToDevice(cmd);
    // we have consumed this command
    return true;
}

bool AtsampleAtCommandParser::SCIDCommand::executeAsWrite(OutputWriterInterface* output,
                                                       int argc,
                                                       const char** argv)
{
    return onError(this, output);
}

bool AtsampleAtCommandParser::SCIDCommand::onSuccess(SmmMessage* successMsg,
                                                  OutputWriterInterface* output)
{
    SMM_TRACE_D("SCIDCommand::onSuccess(%s)", successMsg->getMessage());

    if (strcmp(successMsg->getMessage(), "+CCID") == 0) {
        AtsampleAtResponseParser::CCIDResponse* ccidRsp =
            dynamic_cast<AtsampleAtResponseParser::CCIDResponse*>(successMsg);
        if (ccidRsp != nullptr) {
            const char* ccid = ccidRsp->getCcid();
            SMM_TRACE_D("SCID = %s", ccid);
            output->writeToHost("^SCID: %s\r\nOK\r\n", ccid);
            return true;
        }
    }
    return false;
}

AtsampleAtCommandParser::SRADCCommand::SRADCCommand(SharedEnvInterface* env)
    : AtCommand("AT^SRADC")
{
    assert(env != nullptr);
    mEnv = env;
}

bool AtsampleAtCommandParser::SRADCCommand::executeAsTest(OutputWriterInterface* output)
{
    return onError(this, output);
}

bool AtsampleAtCommandParser::SRADCCommand::executeAsQuery(OutputWriterInterface* output)
{
    return onError(this, output);
}

bool AtsampleAtCommandParser::SRADCCommand::executeAsWrite(OutputWriterInterface* output,
                                                        int argc,
                                                        const char** argv)
{
    // AT^SRADC=<ch>
    int ch = -1;

    if (errorIfArgsCountLessThan(1, argc, output)) {
        return true;
    }

    ch = strtoul(argv[0], nullptr, 0);

    SMM_TRACE_D("SRADCCommand { ch = %d }", ch);

    // this command is not yet supported, replying OK to fake the behavior.
    output->writeToHost("\r\n^SRADC: %d,1,875\r\n\r\nOK\r\n", ch);
    // we have consumed this command
    return true;
}

AtsampleAtCommandParser::SMONICommand::SMONICommand(SharedEnvInterface* env)
    : AtCommand("AT^SMONI")
{
    assert(env != nullptr);
    mEnv = env;
}

bool AtsampleAtCommandParser::SMONICommand::executeAsTest(OutputWriterInterface* output)
{
    return onSuccess(this, output);
}

bool AtsampleAtCommandParser::SMONICommand::executeAsQuery(OutputWriterInterface* output)
{
    // AT^SMONI
    SMM_TRACE_D("SMONICommand { }");

    NetregContextInterface* nr = mEnv->getNetregContext();
    assert(nr != nullptr);
    DcContextInterface* dc = mEnv->getDcContext();

    switch (nr->getRat()) {
        case NetregContext::Rat::GSM:
            output->writeToHost("\r\n^SMONI: %s,0,0,%d,%d,%d,%x,0,0,0,0,G,0,0,0\r\n\r\nOK\r\n",
                                "2G",
                                nr->getMcc(),
                                nr->getMnc(),
                                nr->getLac(),
                                nr->getCellId());
            break;
        case NetregContext::Rat::UMTS:
            output->writeToHost("\r\n^SMONI: %s,0,0,0,0,%d,%d,%d,%x,0,0,0,0,0,0,0,0,0,0\r\n\r\nOK\r\n",
                                "3G",
                                nr->getMcc(),
                                nr->getMnc(),
                                nr->getLac(),
                                nr->getCellId());
            break;
        case NetregContext::Rat::LTE:
            output->writeToHost("\r\n^SMONI: %s,0,0,0,0,FDD,%d,%d,%x,0000000,0,0,0,0,%s\r\n\r\nOK\r\n",
                                "4G",
                                nr->getMcc(),
                                nr->getMnc(),
                                nr->getCellId(),
                                dc != nullptr && dc->isActivated() ? "CONN" : "NOCONN");
            break;
        case NetregContext::Rat::NONE:
        default:
            output->writeToHost("\r\n^SMONI: 3G,SEARCH,SEARCH\r\n\r\nOK\r\n");
            break;
    }

    // camping 2G
    // ^SMONI: 2G,71,-61,262,02,0143,83BA,33,33,3,6,G,NOCONN

    // camping 3G
    // ^SMONI: 3G,10564,296,-7.5,-79,262,02,0143,00228FF,-92,-78,NOCONN

    // camping 4G
    // ^SMONI: 4G,6300,20,10,10,FDD,262,02,BF75,0345103,350,33,-94,-7,NOCONN

    // we have consumed this command
    return true;
}

bool AtsampleAtCommandParser::SMONICommand::executeAsWrite(OutputWriterInterface* output,
                                                        int argc,
                                                        const char** argv)
{
    return this->onError(this, output);
}

AtsampleAtCommandParser::SWWANCommand::SWWANCommand(SharedEnvInterface* env)
    : AtCommand("AT^SWWAN")
{
    assert(env != nullptr);
    mEnv = env;
}

bool AtsampleAtCommandParser::SWWANCommand::executeAsTest(OutputWriterInterface* output)
{
    output->writeToDevice("AT+CGACT=?");
    return true;
}

bool AtsampleAtCommandParser::SWWANCommand::executeAsQuery(OutputWriterInterface* output)
{
    output->writeToDevice("AT+CGACT?");
    return true;
}

bool AtsampleAtCommandParser::SWWANCommand::executeAsWrite(OutputWriterInterface* output,
                                                        int argc,
                                                        const char** argv)
{
    // AT^SWWAN=<state>,<cid>

    int cid = -1;
    int state = -1;

    if (errorIfArgsCountLessThan(2, argc, output)) {
        return true;
    }

    state = strtoul(argv[0], nullptr, 0);
    cid = strtoul(argv[1], nullptr, 0);

    SMM_TRACE_D("SWWANCommand { state = %d, cid = %d }", state, cid);

    output->writeToDevice("AT+CGACT=%d,%d\r\n", state, cid);
    return true;
}
