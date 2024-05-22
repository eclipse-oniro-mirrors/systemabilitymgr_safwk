/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "local_ability_manager_stub.h"

#include <cstdint>
#include <utility>
#include <cinttypes>

#include "errors.h"
#include "hilog/log_cpp.h"
#include "if_local_ability_manager.h"
#include "ipc_types.h"
#include "message_option.h"
#include "message_parcel.h"
#include "parse_util.h"
#include "safwk_log.h"
#include "datetime_ex.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"
#include "accesstoken_kit.h"

using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace {
const std::string PERMISSION_EXT_TRANSACTION = "ohos.permission.ACCESS_EXT_SYSTEM_ABILITY";
const std::string PERMISSION_MANAGE = "ohos.permission.MANAGE_SYSTEM_ABILITY";
}

LocalAbilityManagerStub::LocalAbilityManagerStub()
{
    memberFuncMap_[static_cast<uint32_t>(SafwkInterfaceCode::START_ABILITY_TRANSACTION)] =
        &LocalAbilityManagerStub::StartAbilityInner;
    memberFuncMap_[static_cast<uint32_t>(SafwkInterfaceCode::STOP_ABILITY_TRANSACTION)] =
        &LocalAbilityManagerStub::StopAbilityInner;
    memberFuncMap_[static_cast<uint32_t>(SafwkInterfaceCode::ACTIVE_ABILITY_TRANSACTION)] =
        &LocalAbilityManagerStub::ActiveAbilityInner;
    memberFuncMap_[static_cast<uint32_t>(SafwkInterfaceCode::IDLE_ABILITY_TRANSACTION)] =
        &LocalAbilityManagerStub::IdleAbilityInner;
    memberFuncMap_[static_cast<uint32_t>(SafwkInterfaceCode::SEND_STRATEGY_TO_SA_TRANSACTION)] =
        &LocalAbilityManagerStub::SendStrategyToSAInner;
    memberFuncMap_[static_cast<uint32_t>(SafwkInterfaceCode::IPC_STAT_CMD_TRANSACTION)] =
        &LocalAbilityManagerStub::IpcStatCmdProcInner;
    memberFuncMap_[static_cast<uint32_t>(SafwkInterfaceCode::FFRT_DUMPER_TRANSACTION)] =
        &LocalAbilityManagerStub::FfrtDumperProcInner;
    memberFuncMap_[static_cast<uint32_t>(SafwkInterfaceCode::SYSTEM_ABILITY_EXT_TRANSACTION)] =
        &LocalAbilityManagerStub::SystemAbilityExtProcInner;
}

bool LocalAbilityManagerStub::CheckPermission(uint32_t code)
{
    uint32_t accessToken = IPCSkeleton::GetCallingTokenID();
    std::string permissionToCheck;

    if (code == static_cast<uint32_t>(SafwkInterfaceCode::SYSTEM_ABILITY_EXT_TRANSACTION)) {
        permissionToCheck = PERMISSION_EXT_TRANSACTION;
    } else {
        permissionToCheck = PERMISSION_MANAGE;
    }

    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(accessToken, permissionToCheck);
    return ret == static_cast<int32_t>(Security::AccessToken::PermissionState::PERMISSION_GRANTED);
}

int32_t LocalAbilityManagerStub::OnRemoteRequest(uint32_t code,
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    HILOGD(TAG, "code:%{public}u, flags:%{public}d", code, option.GetFlags());
    if (!EnforceInterceToken(data)) {
        HILOGW(TAG, "check interface token failed!");
        return ERR_PERMISSION_DENIED;
    }

    if (CheckPermission(code) == false) {
        HILOGW(TAG, "check permission failed! code:%{public}u, callingPid:%{public}d, callingTokenId:%{public}u",
            code, IPCSkeleton::GetCallingPid(), IPCSkeleton::GetCallingTokenID());
        return ERR_PERMISSION_DENIED;
    }
    HILOGD(TAG, "check permission success!");

    auto iter = memberFuncMap_.find(code);
    if (iter != memberFuncMap_.end()) {
        auto memberFunc = iter->second;
        if (memberFunc != nullptr) {
            return (this->*memberFunc)(data, reply);
        }
    }
    HILOGW(TAG, "unknown request code!");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t LocalAbilityManagerStub::StartAbilityInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t saId = -1;
    bool ret = data.ReadInt32(saId);
    if (!ret) {
        HILOGW(TAG, "read saId failed!");
        return ERR_NULL_OBJECT;
    }
    if (!CheckInputSysAbilityId(saId)) {
        HILOGW(TAG, "check SA:%{public}d id failed!", saId);
        return ERR_NULL_OBJECT;
    }
    int64_t begin = GetTickCount();
    std::string eventStr = data.ReadString();
    if (eventStr.empty()) {
        HILOGW(TAG, "LocalAbilityManagerStub::StartAbilityInner read eventStr failed!");
        return ERR_NULL_OBJECT;
    }
    bool result = StartAbility(saId, eventStr);
    LOGI("StartSaInner %{public}s to start SA:%{public}d,eventLen:%{public}zu,spend:%{public}" PRId64 "ms",
        result ? "suc" : "fail", saId, eventStr.length(), (GetTickCount() - begin));
    return ERR_NONE;
}

int32_t LocalAbilityManagerStub::StopAbilityInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t saId = -1;
    bool ret = data.ReadInt32(saId);
    if (!ret) {
        return ERR_NULL_OBJECT;
    }
    if (!CheckInputSysAbilityId(saId)) {
        HILOGW(TAG, "read saId failed!");
        return ERR_NULL_OBJECT;
    }
    int64_t begin = GetTickCount();
    std::string eventStr = data.ReadString();
    if (eventStr.empty()) {
        HILOGW(TAG, "StopAbilityInner read eventStr failed!");
        return ERR_NULL_OBJECT;
    }
    bool result = StopAbility(saId, eventStr);
    LOGI("StopSaInner %{public}s to stop SA:%{public}d,eventLen:%{public}zu,spend:%{public}" PRId64 "ms",
        result ? "suc" : "fail", saId, eventStr.length(), (GetTickCount() - begin));
    return ERR_NONE;
}

int32_t LocalAbilityManagerStub::ActiveAbilityInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t saId = -1;
    bool ret = data.ReadInt32(saId);
    if (!ret) {
        return ERR_NULL_OBJECT;
    }
    if (!CheckInputSysAbilityId(saId)) {
        HILOGW(TAG, "read saId failed!");
        return ERR_NULL_OBJECT;
    }
    int64_t begin = GetTickCount();
    nlohmann::json activeReason = ParseUtil::StringToJsonObj(data.ReadString());
    bool result = ActiveAbility(saId, activeReason);
    if (!reply.WriteBool(result)) {
        HILOGW(TAG, "ActiveAbilityInner Write result failed!");
        return ERR_NULL_OBJECT;
    }
    LOGI("ActiveSaInner %{public}s to Active SA:%{public}d,spend:%{public}" PRId64 "ms",
        result ? "suc" : "fail", saId, (GetTickCount() - begin));
    return ERR_NONE;
}

int32_t LocalAbilityManagerStub::IdleAbilityInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t saId = -1;
    bool ret = data.ReadInt32(saId);
    if (!ret) {
        return ERR_NULL_OBJECT;
    }
    if (!CheckInputSysAbilityId(saId)) {
        HILOGW(TAG, "read saId failed!");
        return ERR_NULL_OBJECT;
    }
    int64_t begin = GetTickCount();
    nlohmann::json idleReason = ParseUtil::StringToJsonObj(data.ReadString());
    int32_t delayTime = 0;
    bool result = IdleAbility(saId, idleReason, delayTime);
    if (!reply.WriteBool(result)) {
        HILOGW(TAG, "ActiveAbilityInner Write result failed!");
        return ERR_NULL_OBJECT;
    }
    if (!reply.WriteInt32(delayTime)) {
        HILOGW(TAG, "ActiveAbilityInner Write delayTime failed!");
        return ERR_NULL_OBJECT;
    }
    LOGI("IdleSaInner %{public}s to Idle SA:%{public}d,delayTime:%{public}d,spend:%{public}" PRId64 "ms",
        result ? "suc" : "fail", saId, delayTime, (GetTickCount() - begin));
    return ERR_NONE;
}

int32_t LocalAbilityManagerStub::SendStrategyToSAInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t type = -1;
    bool ret = data.ReadInt32(type);
    if (!ret) {
        return ERR_NULL_OBJECT;
    }
    int32_t saId = -1;
    ret = data.ReadInt32(saId);
    if (!ret) {
        return ERR_NULL_OBJECT;
    }
    if (!CheckInputSysAbilityId(saId)) {
        HILOGW(TAG, "read saId failed!");
        return ERR_NULL_OBJECT;
    }
    int32_t level = -1;
    ret = data.ReadInt32(level);
    if (!ret) {
        return ERR_NULL_OBJECT;
    }
    std::string aciton;
    ret = data.ReadString(aciton);
    if (!ret) {
        return ERR_NULL_OBJECT;
    }
    bool result = SendStrategyToSA(type, saId, level, aciton);
    if (!reply.WriteBool(result)) {
        HILOGW(TAG, "SendStrategyToSA Write result failed!");
        return ERR_NULL_OBJECT;
    }
    HILOGD(TAG, "SendStrategyToSA called %{public}s  ", result ? "success" : "failed");
    return ERR_NONE;
}

int32_t LocalAbilityManagerStub::IpcStatCmdProcInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t fd = data.ReadFileDescriptor();
    if (fd < 0) {
        return ERR_NULL_OBJECT;
    }
    int cmd = -1;
    bool ret = data.ReadInt32(cmd);
    if (!ret) {
        ::close(fd);
        return ERR_NULL_OBJECT;
    }
    bool result = IpcStatCmdProc(fd, cmd);
    if (!reply.WriteBool(result)) {
        HILOGW(TAG, "IpcStatCmdProc Write result failed!");
        ::close(fd);
        return ERR_NULL_OBJECT;
    }
    ::close(fd);
    HILOGD(TAG, "IpcStatCmdProc called %{public}s  ", result ? "success" : "failed");
    return ERR_NONE;
}

int32_t LocalAbilityManagerStub::FfrtDumperProcInner(MessageParcel& data, MessageParcel& reply)
{
    std::string ffrtDumperInfo;
    bool result = FfrtDumperProc(ffrtDumperInfo);
    HILOGI(TAG, "safwk ffrt dumper result %{public}s", result ? "succeed" : "failed");
    if (!reply.WriteString(ffrtDumperInfo)) {
        HILOGW(TAG, "FfrtDumperProc write ffrtDumperInfo failed!");
        return ERR_NULL_OBJECT;
    }
    HILOGD(TAG, "FfrtDumperProc called %{public}s ", result? "success" : "failed");
    return ERR_NONE;
}

int32_t LocalAbilityManagerStub::SystemAbilityExtProcInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t saId = -1;
    bool ret = data.ReadInt32(saId);
    if (!ret) {
        return INVALID_DATA;
    }
    if (!CheckInputSysAbilityId(saId)) {
        HILOGW(TAG, "read saId failed!");
        return INVALID_DATA;
    }
    std::string extension = data.ReadString();
    if (extension.empty()) {
        HILOGW(TAG, "LocalAbilityManagerStub::SystemAbilityExtProcInner read extension failed!");
        return INVALID_DATA;
    }

    SystemAbilityExtensionPara callback;
    callback.data_ = &data;
    callback.reply_ = &reply;

    int32_t result = SystemAbilityExtProc(extension, saId, &callback, false);
    if (result != ERR_NONE) {
        HILOGW(TAG, "SystemAbilityExtProc fail!");
        return result;
    }

    HILOGD(TAG, "SystemAbilityExtProc success ");
    return ERR_NONE;
}

bool LocalAbilityManagerStub::CheckInputSysAbilityId(int32_t systemAbilityId)
{
    return (systemAbilityId >= FIRST_SYS_ABILITY_ID) && (systemAbilityId <= LAST_SYS_ABILITY_ID);
}

bool LocalAbilityManagerStub::EnforceInterceToken(MessageParcel& data)
{
    std::u16string interfaceToken = data.ReadInterfaceToken();
    return interfaceToken == LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN;
}
}
