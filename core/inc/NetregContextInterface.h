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

#ifndef NETREG_CONTEXT_INTERFACE_H_
#define NETREG_CONTEXT_INTERFACE_H_

class NetregContextInterface {
public:
    enum RegState {
        NOT_REGISTERED = 0,               // ME is not currently searching a new operator to
                                          // register to (default)
        REGISTERED_HOME_NETWORK = 1,      // Registered, home network
        UNREGISTERED_SEARCHING = 2,       // Not registered, but ME is currently searching a new
                                          // operator to register
        REGISTRATION_DENIED = 3,          // Registration denied
        UNKNOWN = 4,
        REGISTERED_ROAMING = 5,           // Registered, in roaming
        REGISTERED_SMS_ONLY_HOME_NETWORK = 6, // registered for "SMS only" ,home network (applicable
                                              // only when <AcT> indicates E-UTRAN)
        REGISTERED_SMS_ONLY_ROAMING = 7,  // registered for "SMS only", roaming (applicable only
                                          // when <AcT> indicates E-UTRAN)
        ATTACHED_EMERGENCY_ONLY = 8       // attached for emergency bearer services only (see NOTE
                                          // 2) (not applicable)
    };

    enum Rat {
        NONE = 0,
        GSM = 1,
        UMTS = 2,
        LTE = 3,
        TDSCDMA = 4
    };

    NetregContextInterface()
    {
    }
    virtual ~NetregContextInterface() {}

    virtual RegState getRegState() = 0;
    virtual int getMnc() = 0;
    virtual int getMcc() = 0;
    virtual Rat getRat() = 0;
    virtual int getLac() = 0;
    virtual int getCellId() = 0;
    virtual bool isPsAttached() = 0;
};
#endif
