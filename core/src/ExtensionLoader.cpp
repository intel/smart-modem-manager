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
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <dlfcn.h>
#include <list>
#include "smm_core.h"
#include "smm_log.h"
#include "ExtensionLoader.h"

ExtensionLoader::ExtensionLoader()
{
    mHandles = new std::list<void*>();
}

ExtensionLoader::~ExtensionLoader()
{
    unloadAllExtensions();
    if (mHandles != nullptr) {
        delete mHandles;
        mHandles = nullptr;
    }
}

bool ExtensionLoader::loadExtensionsFrom(const char* directory)
{
    bool ret = true;

    SMM_TRACE_D("Loading extension from directory: %s", directory);

    DIR* d = opendir(directory);
    char libPath[PATH_MAX];

    if (d != nullptr) {
        struct dirent* entry = nullptr;

        while ((entry = readdir(d)) != nullptr) {
            char* dot = strrchr(entry->d_name, '.');
            bool has_libsmm = (strstr(entry->d_name, "libsmm") != nullptr);
            bool is_libsmm_core = (strstr(entry->d_name, "libsmm_core.so") != nullptr);
            if (dot != nullptr && strcmp(dot, ".so") == 0 && has_libsmm && !is_libsmm_core) {
                snprintf(libPath, sizeof(libPath), "%s/%s", directory, entry->d_name);
                loadExtensionFrom(libPath);
            }
        }
        closedir(d);
    } else {
        SMM_TRACE_E("Failed opening directory: %s", strerror(errno));
    }

    return ret;
}

bool ExtensionLoader::loadExtensionFrom(const char* file)
{
    SMM_TRACE_D("Loading extension from file: %s", file);

    if (file != nullptr) {
        void* h = dlopen(file, RTLD_NOW);
        if (h == nullptr) {
            SMM_TRACE_E("dlopen() failed: %s", dlerror());
        } else {
            SMM_TRACE_D("dlopen() successful");
            mHandles->push_back(h);
            return true;
        }
    }
    return false;
}

void ExtensionLoader::unloadAllExtensions()
{
    SMM_TRACE_D("Unloading all extensions");

    if (mHandles != nullptr) {
        std::list<void*>::iterator i;

        for (i = mHandles->begin(); i != mHandles->end(); ++i) {
            void* h = *i;

            if (h != nullptr) {
                if (dlclose(h) < 0) {
                    SMM_TRACE_E("dlclose() failed: %s", dlerror());
                }
            }
        }
    }
}
