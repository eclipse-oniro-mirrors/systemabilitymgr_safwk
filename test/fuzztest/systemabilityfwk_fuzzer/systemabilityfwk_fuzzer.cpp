/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "systemabilityfwk_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#define private public
#include "string_ex.h"
#include "local_ability_manager.h"
#include "sa_mock_permission.h"
#include "mock_sa_realize.h"
#include "securec.h"

namespace OHOS {
namespace Samgr {
namespace {
constexpr size_t THRESHOLD = 10;
constexpr uint8_t MAX_CALL_TRANSACTION = 16;
constexpr int32_t OFFSET = 4;
constexpr int32_t USLEEP_NUM = 200000;
constexpr int32_t SHIFT_FIRST = 24;
constexpr int32_t SHIFT_SECOND = 16;
constexpr int32_t SHIFT_THIRD = 8;
constexpr int32_t ZERO_NUM = 0;
constexpr int32_t FIRST_NUM = 1;
constexpr int32_t SECOND_NUM = 2;
constexpr int32_t THIRD_NUM = 3;
const std::u16string LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN = u"ohos.localabilitymanager.accessToken";
const uint8_t *g_baseFuzzData = nullptr;
size_t g_baseFuzzSize = 0;
size_t g_baseFuzzPos;
}

template <class T> T GetData()
{
    T object{};
    size_t objectSize = sizeof(object);
    if (g_baseFuzzData == nullptr || objectSize > g_baseFuzzSize - g_baseFuzzPos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_baseFuzzData + g_baseFuzzPos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_baseFuzzPos += objectSize;
    return object;
}

std::string BuildStringFromData(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return "";
    }
    std::string strVal(reinterpret_cast<const char *>(data), size);
    return strVal;
}

uint32_t ConvertToUint32(const uint8_t* ptr)
{
    if (ptr == nullptr) {
        return 0;
    }
    return (ptr[ZERO_NUM] << SHIFT_FIRST) | (ptr[FIRST_NUM] << SHIFT_SECOND) |
        (ptr[SECOND_NUM] << SHIFT_THIRD) | (ptr[THIRD_NUM]);
}

void FuzzLocalAbilityManager(const uint8_t* rawData, size_t size)
{
    SaMockPermission::MockPermission();
    g_baseFuzzData = rawData;
    g_baseFuzzSize = size;
    g_baseFuzzPos = 0;
    int32_t systemAbilityId = GetData<int32_t>();
    int32_t listenSaId = GetData<int32_t>();
    int32_t dependSaId = GetData<int32_t>();
    std::vector<int32_t> dependSas = {dependSaId};
    std::string strVal = BuildStringFromData(rawData, size);
    std::string profilePath = strVal;
    std::string procName = strVal;
    std::string eventStr = strVal;
    SaProfile saProfile = {Str8ToStr16(procName), systemAbilityId};
    std::list<SaProfile> saInfos = {saProfile};

    MockSaRealize *ability = new MockSaRealize(systemAbilityId, false);
    LocalAbilityManager::GetInstance().AddAbility(ability);
    LocalAbilityManager::GetInstance().DoStartSAProcess(profilePath, systemAbilityId);
    LocalAbilityManager::GetInstance().GetTraceTag(profilePath);
    LocalAbilityManager::GetInstance().InitializeSaProfiles(systemAbilityId);
    LocalAbilityManager::GetInstance().CheckTrustSa(profilePath, procName, saInfos);
    LocalAbilityManager::GetInstance().RemoveAbility(systemAbilityId);
    LocalAbilityManager::GetInstance().GetRunningStatus(systemAbilityId);

    LocalAbilityManager::GetInstance().AddSystemAbilityListener(systemAbilityId, listenSaId);
    LocalAbilityManager::GetInstance().RemoveSystemAbilityListener(systemAbilityId, listenSaId);
    LocalAbilityManager::GetInstance().CheckDependencyStatus(dependSas);
    LocalAbilityManager::GetInstance().StartSystemAbilityTask(ability);
    LocalAbilityManager::GetInstance().CheckSystemAbilityManagerReady();
    LocalAbilityManager::GetInstance().InitSystemAbilityProfiles(profilePath, systemAbilityId);
    LocalAbilityManager::GetInstance().ClearResource();
    LocalAbilityManager::GetInstance().StartOndemandSystemAbility(systemAbilityId);
    LocalAbilityManager::GetInstance().StopOndemandSystemAbility(systemAbilityId);

    LocalAbilityManager::GetInstance().GetStartReason(systemAbilityId);
    LocalAbilityManager::GetInstance().GetStopReason(systemAbilityId);
    LocalAbilityManager::GetInstance().JsonToOnDemandReason(nullptr);
    LocalAbilityManager::GetInstance().SetStartReason(systemAbilityId, nullptr);
    LocalAbilityManager::GetInstance().SetStopReason(systemAbilityId, nullptr);
    LocalAbilityManager::GetInstance().OnStartAbility(systemAbilityId);
    LocalAbilityManager::GetInstance().OnStopAbility(systemAbilityId);
    LocalAbilityManager::GetInstance().StartAbility(systemAbilityId, eventStr);
    LocalAbilityManager::GetInstance().StopAbility(systemAbilityId, eventStr);
    LocalAbilityManager::GetInstance().InitializeOnDemandSaProfile(systemAbilityId);
    LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    LocalAbilityManager::GetInstance().StartDependSaTask(ability);
    LocalAbilityManager::GetInstance().RegisterOnDemandSystemAbility(systemAbilityId);
    LocalAbilityManager::GetInstance().NeedRegisterOnDemand(saProfile, systemAbilityId);
    LocalAbilityManager::GetInstance().Run(systemAbilityId);
    LocalAbilityManager::GetInstance().AddLocalAbilityManager();
}

void FuzzIpcStatCmdProc(const uint8_t* rawData, size_t size)
{
    SaMockPermission::MockPermission();
    g_baseFuzzData = rawData;
    g_baseFuzzSize = size;
    g_baseFuzzPos = 0;
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    int32_t fd = GetData<int32_t>();
    data.WriteFileDescriptor(fd);
    int32_t cmd = GetData<int32_t>();
    data.WriteInt32(cmd);
    MessageParcel reply;
    MessageOption option;
    LocalAbilityManager::GetInstance().OnRemoteRequest(static_cast<uint32_t>(
        SafwkInterfaceCode::IPC_STAT_CMD_TRANSACTION), data, reply, option);
}

void FuzzSystemAbilityFwk(const uint8_t* rawData, size_t size)
{
    SaMockPermission::MockPermission();
    uint32_t code = ConvertToUint32(rawData);
    rawData = rawData + OFFSET;
    size = size - OFFSET;
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    data.WriteBuffer(rawData, size);
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;
    LocalAbilityManager::GetInstance().OnRemoteRequest(code % MAX_CALL_TRANSACTION, data, reply, option);
    usleep(USLEEP_NUM);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::Samgr::THRESHOLD) {
        return 0;
    }

    OHOS::Samgr::FuzzSystemAbilityFwk(data, size);
    OHOS::Samgr::FuzzIpcStatCmdProc(data, size);
    OHOS::Samgr::FuzzLocalAbilityManager(data, size);
    return 0;
}

