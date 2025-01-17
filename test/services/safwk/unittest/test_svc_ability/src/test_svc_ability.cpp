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
#include "test_svc_ability.h"

#include "hilog/log.h"
#include "iremote_object.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "test_sa_proxy_cache_proxy.h"

using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0xD001800, "TestSvcAbility"};
}

REGISTER_SYSTEM_ABILITY_BY_ID(TestSvcAbility, DISTRIBUTED_SCHED_TEST_TT_ID, true);

TestSvcAbility::TestSvcAbility(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
    HiLog::Info(LABEL, "TestSvcAbility()");
}

TestSvcAbility::~TestSvcAbility()
{
    HiLog::Info(LABEL, "~TestSvcAbility()");
}

void TestSvcAbility::OnStart()
{
    HiLog::Info(LABEL, "OnStart()");
    bool res = Publish(this);
    if (!res) {
        HiLog::Error(LABEL, "publish failed!");
        return;
    }
}

int32_t TestSvcAbility::OnSvcCmd(int32_t fd, const std::vector<std::u16string>& args)
{
    int32_t svcResult = -1;
    std::string info = "";
    if (args.size() != 1) {
        std::string error = "wrong parameter size, available size = 1\n" + info;
        if (!SaveStringToFd(fd, error)) {
            HiLog::Info(LABEL, "TestSvcAbility save string to fd failed.");
        }
        return svcResult;
    }

    std::vector<std::string> vecArgs;
    std::transform(args.begin(), args.end(), std::back_inserter(vecArgs), [](const std::u16string& arg) {
        return Str16ToStr8(arg);
    });

    std::string cmd = vecArgs[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    HiLog::Info(LABEL, "svc command is svc test %{public}s.", cmd.c_str());
    if (cmd == "help" || cmd == "enable" || cmd == "disable") {
        info = "svc command is svc test " + cmd + "\n";
        svcResult = 0;
    } else {
        info = "svc command test failed, unknown cmd\n";
        svcResult = -1;
    }

    if (!SaveStringToFd(fd, info)) {
        HiLog::Info(LABEL, "TestSvcAbility save string to fd failed.");
    }
    return svcResult;
}
}