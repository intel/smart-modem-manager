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

#ifndef DC_CONTEXT_INTERFACE_H_
#define DC_CONTEXT_INTERFACE_H_

#define INVALID_IPV4_ADDR       "0.0.0.0"
#define IP_TYPE_IPV4            "IP"
#define IP_TYPE_IPV4V6          "IPV4V6"
#define IP_TYPE_IPV6            "IPV6"
#define AUTH_NAME_PWD_LEN       64

class DcContextInterface {
public:
    DcContextInterface() {}
    virtual ~DcContextInterface() {}

    enum AuthType {
        AUTH_MODE_DISABLED = 0,    // Authentication protocol not used, default
        AUTH_MODE_PAP = 1,         // Password Authentication Protocol
        AUTH_MODE_CHAP = 2         // Challenge Handshake Authentication Protocol
    };

    virtual bool isActivated() const = 0;
    virtual int getCid() const = 0;
    virtual const char* getApn() const = 0;
    virtual const char* getPdpType() const = 0;
    virtual const char* getPdpAddr() const = 0;
    virtual const char* getDns1() const = 0;
    virtual const char* getDns2() const = 0;
    virtual int getDComp() const = 0;
    virtual int getHComp() const = 0;
    virtual int getAuthMode() const = 0;
    virtual const char* getAuthName() const  = 0;
    virtual const char* getAuthPwd() const  = 0;
};
#endif
