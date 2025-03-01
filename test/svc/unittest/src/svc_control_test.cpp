/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "svc_control_test.h"

#include <string>

#include "ipc_skeleton.h"

namespace OHOS {
SvcControlTest::SvcControlTest()
{
}

SvcControlTest::~SvcControlTest()
{
}

SvcCmd SvcControlTest::ParseCmd(char *argv[], int32_t cnt, bool &isEnd)
{
    struct {
        std::string op;
        SvcCmd cmd;
        bool isEnd;
    } options[] = {
        { "test",   static_cast<SvcCmd>(TEST_CMD),  false },
        { "help",   SvcCmd::HELP_CMD,               true  },
    };
    SvcCmd cmd = SvcCmd::ARGS_INVALID;
    for (auto &op: options) {
        if (op.op == argv[cnt]) {
            cmd = op.cmd;
            isEnd = op.isEnd;
            break;
        }
    }
    return cmd;
}
}

int main(int argc, char *argv[])
{
    auto &svc = OHOS::SvcControlTest::GetInstance();
    return svc.Main(argc, argv, STDOUT_FILENO);
}
