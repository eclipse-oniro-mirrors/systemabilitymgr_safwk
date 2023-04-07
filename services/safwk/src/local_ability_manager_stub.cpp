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

#include "errors.h"
#include "hilog/log_cpp.h"
#include "if_local_ability_manager.h"
#include "ipc_types.h"
#include "message_option.h"
#include "message_parcel.h"
#include "parse_util.h"
#include "safwk_log.h"
#include "system_ability_definition.h"

using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace {
const std::string TAG = "LocalAbilityManagerStub";
}

LocalAbilityManagerStub::LocalAbilityManagerStub()
{
    memberFuncMap_[START_ABILITY_TRANSACTION] =
        &LocalAbilityManagerStub::StartAbilityInner;
    memberFuncMap_[STOP_ABILITY_TRANSACTION] =
        &LocalAbilityManagerStub::StopAbilityInner;
    memberFuncMap_[ACTIVE_ABILITY_TRANSACTION] =
        &LocalAbilityManagerStub::ActiveAbilityInner;
    memberFuncMap_[IDLE_ABILITY_TRANSACTION] =
        &LocalAbilityManagerStub::IdleAbilityInner;
}

int32_t LocalAbilityManagerStub::OnRemoteRequest(uint32_t code,
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    HILOGI(TAG, "code:%{public}u, flags:%{public}d", code, option.GetFlags());
    if (!EnforceInterceToken(data)) {
        HILOGW(TAG, "check interface token failed!");
        return ERR_PERMISSION_DENIED;
    }
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
    int32_t saId = data.ReadInt32();
    if (!CheckInputSysAbilityId(saId)) {
        HILOGW(TAG, "read saId failed!");
        return ERR_NULL_OBJECT;
    }
    std::string eventStr = data.ReadString();
    if (eventStr.empty()) {
        HILOGW(TAG, "LocalAbilityManagerStub::StartAbilityInner read eventStr failed!");
        return ERR_NULL_OBJECT;
    }
    bool result = StartAbility(saId, eventStr);
    HILOGI(TAG, "%{public}s to start ability", result ? "success" : "failed");
    return ERR_NONE;
}

int32_t LocalAbilityManagerStub::StopAbilityInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t saId = data.ReadInt32();
    if (!CheckInputSysAbilityId(saId)) {
        HILOGW(TAG, "read saId failed!");
        return ERR_NULL_OBJECT;
    }
    std::string eventStr = data.ReadString();
    if (eventStr.empty()) {
        HILOGW(TAG, "LocalAbilityManagerStub::StopAbilityInner read eventStr failed!");
        return ERR_NULL_OBJECT;
    }
    bool result = StopAbility(saId, eventStr);
    HILOGI(TAG, "%{public}s to stop ability", result ? "success" : "failed");
    return ERR_NONE;
}

int32_t LocalAbilityManagerStub::ActiveAbilityInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t saId = data.ReadInt32();
    if (!CheckInputSysAbilityId(saId)) {
        HILOGW(TAG, "read saId failed!");
        return ERR_NULL_OBJECT;
    }
    nlohmann::json activeReason = ParseUtil::StringToJsonObj(data.ReadString());
    bool result = ActiveAbility(saId, activeReason);
    if (!reply.WriteBool(result)) {
        HILOGW(TAG, "ActiveAbilityInner Write result failed!");
        return ERR_NULL_OBJECT;
    }
    return ERR_NONE;
}

int32_t LocalAbilityManagerStub::IdleAbilityInner(MessageParcel& data, MessageParcel& reply)
{
    int32_t saId = data.ReadInt32();
    if (!CheckInputSysAbilityId(saId)) {
        HILOGW(TAG, "read saId failed!");
        return ERR_NULL_OBJECT;
    }
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
