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

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/sysctl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "smm_log.h"
#include "UnixNwRouteController.h"

static const char IPV4_FORWARDING_PROC_FILE[] = "/proc/sys/net/ipv4/ip_forward";
static const char IPV6_FORWARDING_PROC_FILE[] = "/proc/sys/net/ipv6/conf/all/forwarding";
static const char DNS_RESOLVE_CONF_FILE[] = "/etc/resolv.conf";

bool writeToFile(const char* filename, const char* value)
{
    int fd = open(filename, O_WRONLY);
    if (fd < 0) {
        SMM_TRACE_E("Failed to open %s: %s", filename, strerror(errno));
        return false;
    }
    const ssize_t len = strlen(value);
    if (write(fd, value, len) != len) {
        SMM_TRACE_E("Failed to write %s to %s: %s", value, filename, strerror(errno));
        close(fd);
        return false;
    } else {
        SMM_TRACE_D("Success writing to %s: %s", filename, value);
    }
    close(fd);
    return true;
}

bool UnixNwRouteController::setIpFwdEnabled(bool enabled)
{
    bool success = true;
    const char* str = enabled ? "0" : "1";
    SMM_TRACE_D("Setting IP forward enable (sysctl) = %d", enabled);
    success &= writeToFile(IPV4_FORWARDING_PROC_FILE, str);
    success &= writeToFile(IPV6_FORWARDING_PROC_FILE, str);
    return success;
}

UnixNwRouteController::UnixNwRouteController()
{
}

UnixNwRouteController::~UnixNwRouteController()
{
}

bool UnixNwRouteController::addRoute(const char* interface,
                                     const char* destination,
                                     const char* nexthop)
{
    return false;
}

bool UnixNwRouteController::removeRoute(const char* interface,
                                        const char* destination,
                                        const char* nexthop)
{
    return false;
}

bool UnixNwRouteController::enableTethering(const char* inputInterface,
                                            const char* outputInterface,
                                            const char* dns1,
                                            const char* dns2)
{
    char command[512];

    SMM_TRACE_D("UnixNwRouteController::enableTethering(%s, %s", inputInterface,
                outputInterface);

    mode_t oldmask = umask(022);
    bool ret = true; // setIpFwdEnabled(true);  set IP forward is done temporarily in init of the platform

    snprintf(command, sizeof(command),
             "iptables --table nat --append POSTROUTING --out-interface %s -j MASQUERADE",
             outputInterface);
    SMM_TRACE_D("%s", command);
    system(command);

    snprintf(command, sizeof(command),
             "iptables --append FORWARD --in-interface %s -j ACCEPT",
             inputInterface);
    SMM_TRACE_D("%s", command);
    system(command);

    char dns[256];
    snprintf(dns, sizeof(dns), "nameserver %s\nnameserver %s\n", dns1, dns2);
    if (!writeToFile(DNS_RESOLVE_CONF_FILE, dns)) {
        SMM_TRACE_E("Write to %s failed", DNS_RESOLVE_CONF_FILE);
    } else {
        SMM_TRACE_D("Write to %s succeeded", DNS_RESOLVE_CONF_FILE);
    }
    umask(oldmask);

    return ret;
}

bool UnixNwRouteController::disableTethering(const char* inputInterface,
                                             const char* outputInterface)
{
    SMM_TRACE_D("UnixNwRouteController::disableTethering(%s, %s",
                inputInterface,
                outputInterface);

    bool ret = setIpFwdEnabled(false);

    if (ret) {
        SMM_TRACE_D("iptables --flush\r\n");
        ret = system("iptables --flush\r\n");
    }
    return ret;
}
