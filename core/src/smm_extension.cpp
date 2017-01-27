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
#include "smm_core.h"
#include "smm_log.h"
#include "SmmFactory.h"
#include "MessageParser.h"
#include "OutputWriterInterface.h"
#include "smm_extension.h"
#include "SharedEnv.h"

bool registerExtensionFactory(smm_extension_factory_t* factory, const char* extension_name)
{
    bool ret = true;
    SMM_TRACE_D("registering extension: %s", extension_name);

    if (factory != nullptr
        && factory->set_shared_env != nullptr
        && factory->make_cmd_parser != nullptr
        && factory->make_rsp_parser != nullptr
        && factory->on_destroy != nullptr) {
        SharedEnv* env = SharedEnv::getNewInstanceFor(extension_name);

        factory->set_shared_env(env);
        factory->context = env;
        MessageParser* cmdParser = factory->make_cmd_parser();
        MessageParser* rspParser = factory->make_rsp_parser();

        if (cmdParser != nullptr && rspParser != nullptr) {
            SMM_TRACE_D("%s created", cmdParser->getName());
            env->setExtensionCommandParser(cmdParser);
            SmmFactory::getInstance()->makeRootCommandParser()->addChildParser(cmdParser);

            SMM_TRACE_D("%s created", rspParser->getName());
            env->setExtensionResponseParser(rspParser);
            SmmFactory::getInstance()->makeRootResponseParser()->addChildParser(rspParser);
        } else {
            SMM_TRACE_E("parsers provided by the extension are invalid");
            ret = false;
        }
    } else {
        SMM_TRACE_E("factory is invalid");
        ret = false;
    }
    return ret;
}

bool deregisterExtensionFactory(smm_extension_factory_t* factory, const char* extension_name)
{
    bool ret = false;
    SMM_TRACE_D("deregistering extension: %s", extension_name);
    if (factory != nullptr && factory->context != nullptr && factory->on_destroy) {
        factory->on_destroy();
        SharedEnv* env = static_cast<SharedEnv*>(factory->context);
        if (env != nullptr) {
            SmmFactory::getInstance()->makeRootResponseParser()->removeChildParser(
                env->getExtensionResponseParser());
            SmmFactory::getInstance()->makeRootCommandParser()->removeChildParser(
                env->getExtensionCommandParser());
            delete env;
        } else {
            SMM_TRACE_E("context is wrong");
            ret = false;
        }
    } else {
        SMM_TRACE_E("factory is invalid");
        ret = false;
    }
    return ret;
}
