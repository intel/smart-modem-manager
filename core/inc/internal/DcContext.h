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

#ifndef _DC_CONTEXT_H_
#define _DC_CONTEXT_H_

#include <string>
#include "DcContextInterface.h"

class DcContext : public DcContextInterface {
private:
    int mCid = -1;
    std::string mApn;
    std::string mPdpType;
    std::string mPdpAddr;
    std::string mDns1;
    std::string mDns2;
    int mDComp = 0;
    int mHComp = 0;
    int mAuthMode = AuthType::AUTH_MODE_DISABLED;
    std::string mAuthName;
    std::string mAuthPwd;
    bool mActivated = false;

public:
    DcContext();
    DcContext(DcContext &ctx);
    virtual ~DcContext();
    virtual void setActivated(bool activated);
    virtual bool isActivated() const;
    virtual void setCid(int cid);
    virtual int getCid() const;
    virtual void setApn(std::string& apn);
    virtual const char* getApn() const;
    virtual void setPdpType(std::string& pdpType);
    virtual const char* getPdpType() const;
    virtual void setPdpAddr(std::string& pdpAddr);
    virtual const char* getPdpAddr() const;
    virtual void setDns1(std::string& dns1);
    virtual const char* getDns1() const;
    virtual void setDns2(std::string& dns2);
    virtual const char* getDns2() const;
    virtual void setDComp(int dComp);
    virtual int getDComp() const;
    virtual void setHComp(int hComp);
    virtual int getHComp() const;
    virtual void setAuthMode(int authMode);
    virtual int getAuthMode() const;
    virtual void setAuthName(std::string& authName);
    virtual const char* getAuthName() const;
    virtual void setAuthPwd(std::string& authPwd);
    virtual const char* getAuthPwd() const;
};
#endif
