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

#ifndef ATSAMPLE_AT_COMMAND_PARSER_H_
#define ATSAMPLE_AT_COMMAND_PARSER_H_

#include <smm_core/AtCommand.h>
#include <smm_core/Netconf.h>
#include <smm_core/OutputWriterInterface.h>
#include <smm_core/AtCommandParser.h>
#include <smm_core/SharedEnvInterface.h>

class AtsampleAtCommandParser : public AtCommandParser {
private:
    SharedEnvInterface* mEnv = nullptr;

public:
    AtsampleAtCommandParser(SharedEnvInterface* env);
    virtual ~AtsampleAtCommandParser();

    class SCPINCommand : public AtCommand {
    private:
        SharedEnvInterface* mEnv = nullptr;

    public:
        SCPINCommand(SharedEnvInterface* env);
        virtual ~SCPINCommand()
        {
        }
        bool executeAsTest(OutputWriterInterface* output);
        bool executeAsQuery(OutputWriterInterface* output);
        bool executeAsWrite(OutputWriterInterface* output, int argc, const char** argv);
    };

    class CGMMCommand : public AtCommand {
    private:
        SharedEnvInterface* mEnv = nullptr;

    public:
        CGMMCommand(SharedEnvInterface* env);
        virtual ~CGMMCommand()
        {
        }
        bool executeAsTest(OutputWriterInterface* output);
        bool executeAsQuery(OutputWriterInterface* output);
        bool executeAsWrite(OutputWriterInterface* output, int argc, const char** argv);
    };

    class SSIOCommand : public AtCommand {
    private:
        SharedEnvInterface* mEnv = nullptr;

    public:
        SSIOCommand(SharedEnvInterface* env);
        virtual ~SSIOCommand()
        {
        }
        bool executeAsTest(OutputWriterInterface* output);
        bool executeAsQuery(OutputWriterInterface* output);
        bool executeAsWrite(OutputWriterInterface* output, int argc, const char** argv);
    };

    class SPIOCommand : public AtCommand {
    private:
        SharedEnvInterface* mEnv = nullptr;

    public:
        SPIOCommand(SharedEnvInterface* env);
        virtual ~SPIOCommand()
        {
        }
        bool executeAsTest(OutputWriterInterface* output);
        bool executeAsQuery(OutputWriterInterface* output);
        bool executeAsWrite(OutputWriterInterface* output, int argc, const char** argv);
    };

    class SCIDCommand : public AtCommand {
    private:
        SharedEnvInterface* mEnv = nullptr;

    public:
        SCIDCommand(SharedEnvInterface* env);
        virtual ~SCIDCommand()
        {
        }
        bool executeAsTest(OutputWriterInterface* output);
        bool executeAsQuery(OutputWriterInterface* output);
        bool executeAsWrite(OutputWriterInterface* output, int argc, const char** argv);

    protected:
        bool onSuccess(SmmMessage* successMsg, OutputWriterInterface* output);
    };

    class SRADCCommand : public AtCommand {
    private:
        SharedEnvInterface* mEnv = nullptr;

    public:
        SRADCCommand(SharedEnvInterface* env);
        virtual ~SRADCCommand()
        {
        }
        bool executeAsTest(OutputWriterInterface* output);
        bool executeAsQuery(OutputWriterInterface* output);
        bool executeAsWrite(OutputWriterInterface* output, int argc, const char** argv);
    };

    class SMONICommand : public AtCommand {
    private:
        SharedEnvInterface* mEnv = nullptr;

    public:
        SMONICommand(SharedEnvInterface* env);
        virtual ~SMONICommand()
        {
        }
        bool executeAsTest(OutputWriterInterface* output);
        bool executeAsQuery(OutputWriterInterface* output);
        bool executeAsWrite(OutputWriterInterface* output, int argc, const char** argv);
    };

    class SWWANCommand : public AtCommand {
    private:
        SharedEnvInterface* mEnv = nullptr;

    public:
        SWWANCommand(SharedEnvInterface* env);
        virtual ~SWWANCommand()
        {
        }
        bool executeAsTest(OutputWriterInterface* output);
        bool executeAsQuery(OutputWriterInterface* output);
        bool executeAsWrite(OutputWriterInterface* output, int argc, const char** argv);
    };
};
#endif
