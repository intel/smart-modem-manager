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
#include "smm_log.h"
#include "DcContext.h"
#include "DcContextInterface.h"

DcContext::DcContext()
{
}

DcContext::DcContext(DcContext &ctx)
{
    mCid = ctx.mCid;
    mApn = ctx.mApn;
    mPdpType = ctx.mPdpType;
    mPdpAddr = ctx.mPdpAddr;
    mDns1 = ctx.mDns1;
    mDns2 = ctx.mDns2;
    mDComp = ctx.mDComp;
    mHComp = ctx.mHComp;
    mAuthMode = ctx.mAuthMode;
    mAuthName = ctx.mAuthName;
    mAuthPwd = ctx.mAuthPwd;
}

DcContext::~DcContext()
{
}

void DcContext::setActivated(bool activated)
{
    SMM_TRACE_D("DcContext::setActivated(%s)", activated ? "true" : "false");
    mActivated = activated;
}

bool DcContext::isActivated() const
{
    return mActivated;
}

void DcContext::setCid(int cid)
{
    SMM_TRACE_D("DcContext::setCid(%d)", cid);
    mCid = cid;
}

int DcContext::getCid() const
{
    return mCid;
}

void DcContext::setApn(std::string& apn)
{
    SMM_TRACE_D("DcContext::setApn(%s)", apn.c_str());
    mApn = apn;
}

const char* DcContext::getApn() const
{
    return mApn.c_str();
}

void DcContext::setPdpType(std::string& pdpType)
{
    SMM_TRACE_D("DcContext::setPdpType(%s)", pdpType.c_str());
    mPdpType = pdpType;
}

const char* DcContext::getPdpType() const
{
    SMM_TRACE_D("DcContext::getPdpType(): %s", mPdpType.c_str());
    return mPdpType.c_str();
}

void DcContext::setPdpAddr(std::string& pdpAddr)
{
    SMM_TRACE_D("DcContext::setPdpAddr(%s)", pdpAddr.c_str());
    mPdpAddr = pdpAddr;
}

const char* DcContext::getPdpAddr() const
{
    return mPdpAddr.c_str();
}

void DcContext::setDns1(std::string& dns1)
{
    SMM_TRACE_D("DcContext::setDns1(%s)", dns1.c_str());
    mDns1 = dns1;
}

const char* DcContext::getDns1() const
{
    return mDns1.c_str();
}

void DcContext::setDns2(std::string& dns2)
{
    SMM_TRACE_D("DcContext::setDns2(%s)", dns2.c_str());
    mDns2 = dns2;
}

const char* DcContext::getDns2() const
{
    return mDns2.c_str();
}

void DcContext::setDComp(int dComp)
{
    SMM_TRACE_D("DcContext::setDComp(%d)", dComp);
    mDComp = dComp;
}

int DcContext::getDComp() const
{
    return mDComp;
}

void DcContext::setHComp(int hComp)
{
    SMM_TRACE_D("DcContext::setHComp(%d)", hComp);
    mHComp = hComp;
}

int DcContext::getHComp() const
{
    return mHComp;
}

void DcContext::setAuthMode(int authMode)
{
    SMM_TRACE_D("DcContext::setAuthMode(%d)", authMode);
    mAuthMode = authMode;
}

int DcContext::getAuthMode() const
{
    return mAuthMode;
}

void DcContext::setAuthName(std::string& authName)
{
    SMM_TRACE_D("DcContext::setAuthName(%s)", authName.c_str());
    mAuthName = authName;
}

const char* DcContext::getAuthName() const
{
    return mAuthName.c_str();
}

void DcContext::setAuthPwd(std::string& authPwd)
{
    SMM_TRACE_D("DcContext::setAuthPwd((%s)", authPwd.c_str());
    mAuthPwd = authPwd;
}

const char* DcContext::getAuthPwd() const
{
    return mAuthPwd.c_str();
}
