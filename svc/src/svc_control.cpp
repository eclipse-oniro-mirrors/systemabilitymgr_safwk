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
#include "svc_control.h"

#include <cstdio>
#include <string>
#include <map>
#include <functional>

#include "iservice_registry.h"
#include "if_local_ability_manager.h"
#include "string_ex.h"
#include "ipc_skeleton.h"
#include "safwk_log.h"

namespace OHOS {
SvcControl::SvcControl()
{
}

SvcControl::~SvcControl()
{
}

void SvcControl::HelpInfo(int32_t fd)
{
    dprintf(fd, "svc wifi | bluetooth | nearlink enable | disable | help\n");
}

std::string SvcControl::CmdErrorToString(SvcCmd cmd)
{
    if (cmd < SvcCmd::ARGS_CNT_NOT_ENOUGH) {
        return "exception error code";
    }
    std::unordered_map<SvcCmd, std::string> errToString = {
        { SvcCmd::ARGS_INVALID, "argument invalid" },
        { SvcCmd::ARGS_CNT_EXCEED, "argument's cnt exceed" },
        { SvcCmd::ARGS_CNT_NOT_ENOUGH, "argument's cnt not enough" },
    };
    return errToString[cmd];
}

int32_t SvcControl::Main(int32_t argc, char *argv[], int32_t outFd)
{
    HILOGI(TAG, "enter SVC main, proc:%{public}d", getpid());
    if (argv == nullptr) {
        dprintf(outFd, "argument is null\n");
        HelpInfo(outFd);
        HILOGE(TAG, "argument is null");
        return -1;
    }
    SvcCmd cmd = Parse(argc, argv);
    if (cmd == SvcCmd::HELP_CMD) {
        HelpInfo(outFd);
        return 0;
    } else if (cmd <= SvcCmd::ARGS_INVALID) {
        dprintf(outFd, "parse failed, parse status: %s\n", CmdErrorToString(cmd).c_str());
        HelpInfo(outFd);
        HILOGE(TAG, "parse failed, parse status: %{public}d", static_cast<int32_t>(cmd));
        return -1;
    }

    int32_t saId = static_cast<int32_t>(cmd);
    auto sysm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sysm == nullptr) {
        dprintf(outFd, "get object of system ability manager failed\n");
        HILOGE(TAG, "GetSystemAbilityManager failed");
        return -1;
    }
    sptr<IRemoteObject> localObj = sysm->GetLocalAbilityManagerProxy(saId);
    if (localObj == nullptr) {
        (void)sysm->LoadSystemAbility(saId, TIME_OUT);
        localObj = sysm->GetLocalAbilityManagerProxy(saId);
    }
    if (localObj == nullptr) {
        dprintf(outFd, "get object of local ability manager proxy failed\n");
        HILOGE(TAG, "GetLocalAbilityManagerProxy failed");
        return -1;
    }

    sptr<ILocalAbilityManager> localAbilityManager = iface_cast<ILocalAbilityManager>(localObj);
    std::vector<std::u16string> args;
    SetCmdArgs(argc, argv, args);

    int32_t ret = localAbilityManager->ServiceControlCmd(outFd, saId, args);
    if (ret != NO_ERROR) {
        dprintf(outFd, "set service cmd failed, ret: %d\n", ret);
        HILOGE(TAG, "set ServiceControlCmd failed, ret: %{public}d", ret);
        return -1;
    }
    dprintf(outFd, "set service cmd success\n");
    return ret;
}

SvcCmd SvcControl::Parse(int32_t argc, char *argv[])
{
    if (argc < ARG_MIN_COUNT) {
        HILOGE(TAG, "need more arguments(%{public}d), min size %{public}d", argc, ARG_MIN_COUNT);
        return SvcCmd::ARGS_CNT_NOT_ENOUGH;
    }
    if (argc > ARG_MAX_COUNT) {
        HILOGE(TAG, "too many arguments(%{public}d), limit size %{public}d", argc, ARG_MAX_COUNT);
        return SvcCmd::ARGS_CNT_EXCEED;
    }

    for (int32_t i = 0; i < argc; ++i) {
        if (argv[i] == nullptr) {
            HILOGE(TAG, "argument(%{public}d) is null", i);
            return SvcCmd::ARGS_INVALID;
        }
        size_t len = strlen(argv[i]);
        if (len == 0) {
            HILOGE(TAG, "argument(%{public}d) is empty", i);
            return SvcCmd::ARGS_INVALID;
        } else if (len > SINGLE_ARG_MAXLEN) {
            HILOGE(TAG, "too long argument(%{public}d), limit size: %{public}d", i, SINGLE_ARG_MAXLEN);
            return SvcCmd::ARGS_INVALID;
        }
    }

    constexpr int32_t cnt = 1;
    bool isEnd = false;
    SvcCmd cmd = ParseCmd(argv, cnt, isEnd);
    if (cmd == SvcCmd::ARGS_INVALID) {
        HILOGE(TAG, "the %{public}dth argument(%{public}s) is abnormal", cnt, argv[cnt]);
    } else if (isEnd && argc > cnt + 1) {
        cmd = SvcCmd::ARGS_CNT_EXCEED;
        HILOGE(TAG, "too many arguments(%{public}d), current limit size %{public}d", argc, cnt + 1);
    } else if (!isEnd && argc == cnt + 1) {
        cmd = SvcCmd::ARGS_CNT_NOT_ENOUGH;
        HILOGE(TAG, "the argument(%{public}s) need more sub-argument", argv[cnt]);
    }
    return cmd;
}

SvcCmd SvcControl::ParseCmd(char *argv[], int32_t cnt, bool &isEnd)
{
    struct {
        std::string op;
        SvcCmd cmd;
        bool isEnd;
    } options[] = {
        { "wifi",       SvcCmd::WIFI_CMD,       false },
        { "bluetooth",  SvcCmd::BLUETOOTH_CMD,  false },
        { "nearlink",   SvcCmd::NEARLINK_CMD,   false },
        { "help",       SvcCmd::HELP_CMD,       true  },
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

void SvcControl::SetCmdArgs(int32_t argc, char *argv[], std::vector<std::u16string>& args)
{
    std::string argStr;
    for (int32_t i = 0; i < ARG_MIN_COUNT; ++i) {
        argStr += argv[i];
        argStr += " ";
    }
    for (int32_t i = ARG_MIN_COUNT; i < argc; ++i) {
        args.push_back(Str8ToStr16(std::string(argv[i])));
        argStr += argv[i];
        argStr += " ";
    }
    HILOGI(TAG, "svc cmd: %{public}s, callingUid=%{public}d, callingPid=%{public}d", argStr.c_str(),
        IPCSkeleton::GetCallingUid(), IPCSkeleton::GetCallingPid());
}
}
