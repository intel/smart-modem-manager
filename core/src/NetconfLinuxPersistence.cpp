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
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "NetconfPersistence.h"
#include "NetconfLinuxPersistence.h"
#include "NetconfConfig.h"
#include "smm_log.h"

bool makeDir(const char* dir)
{
    if (mkdir(dir, 0700) < 0 && errno != EEXIST) {
        return false;
    }
    return true;
}

NetconfLinuxPersistence::NetconfLinuxPersistence()
{
}

bool NetconfLinuxPersistence::storeDcContext(DcContext* dc)
{
    bool ret = false;
    int fd = -1;

    char filepath[512];
    char buff[512];

    SMM_TRACE_D("NetconfLinuxPersistence::storeDcContext()");

    if (makeDir(SMM_PERSIST_DIR)) {
        snprintf(filepath, sizeof(filepath), "%s/%s", SMM_PERSIST_DIR, "dc_ctx");

        mode_t old_mask = umask(0700);

        fd = open(filepath, O_WRONLY | O_CREAT);

        int buff_len = snprintf(buff, sizeof(buff),
                                "cid=%d apn=%s pdptype=%s auth=%d name=%s pwd=%s ",
                                dc->getCid(),
                                dc->getApn(),
                                dc->getPdpType(),
                                dc->getAuthMode(),
                                dc->getAuthName(),
                                dc->getAuthPwd());

        ssize_t count = -1;

        if (fd >= 0) {
            count = write(fd, buff, buff_len);
            if (count > 0) {
                ret = true;
            } else {
                SMM_TRACE_D("write failed");
            }
        } else {
            SMM_TRACE_D("open(%s) failed", filepath);
        }
        close(fd);
        umask(old_mask);
    } else {
        SMM_TRACE_D("mkdir(%s) failed", SMM_PERSIST_DIR);
    }
    return ret;
}

bool NetconfLinuxPersistence::loadDcContext(DcContext* dc)
{
    bool ret = false;
    int fd = -1;

    char filepath[512];
    char buff[512];

    SMM_TRACE_D("NetconfLinuxPersistence::loadDcContext()");

    snprintf(filepath, sizeof(filepath), "%s/%s", SMM_PERSIST_DIR, "dc_ctx");

    fd = open(filepath, O_RDONLY);

    if (fd >= 0) {
        ssize_t count = -1;
        count = read(fd, buff, sizeof(buff));
        if (count > 0) {
            int cid = 0;
            char apn[128];
            char pdptype[128];
            int auth = 0;
            char name[128];
            char pwd[128];

            if (sscanf(buff, "cid=%d apn=%s pdptype=%s auth=%d name=%s pwd=%s ",
                       &cid,
                       apn,
                       pdptype,
                       &auth,
                       name,
                       pwd) == 6) {
                dc->setCid(cid);
                std::string apnStr(apn);
                dc->setApn(apnStr);
                std::string pdpTypeStr(pdptype);
                dc->setPdpType(pdpTypeStr);
                dc->setAuthMode(auth);
                std::string authName(name);
                dc->setAuthName(authName);
                std::string authPwd(pwd);
                dc->setAuthPwd(authPwd);
                ret = true;
            } else if (sscanf(buff, "cid=%d apn=%s pdptype=%s auth=%d", //no auth
                    &cid,
                    apn,
                    pdptype,
                    &auth) == 4) {
                dc->setCid(cid);
                std::string apnStr(apn);
                dc->setApn(apnStr);
                std::string pdpTypeStr(pdptype);
                dc->setPdpType(pdpTypeStr);
                dc->setAuthMode(auth);
                ret = true;
            } else {
                SMM_TRACE_D("parse failed");
            }
        } else {
            SMM_TRACE_D("read failed");
        }
        close(fd);
    } else {
        SMM_TRACE_D("open(%s) failed", filepath);
    }
    return ret;
}
