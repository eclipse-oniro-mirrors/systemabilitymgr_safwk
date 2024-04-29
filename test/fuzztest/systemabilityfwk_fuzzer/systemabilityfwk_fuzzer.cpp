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

#include "local_ability_manager.h"
#include "sa_mock_permission.h"

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
}

uint32_t ConvertToUint32(const uint8_t* ptr)
{
    if (ptr == nullptr) {
        return 0;
    }
    return (ptr[ZERO_NUM] << SHIFT_FIRST) | (ptr[FIRST_NUM] << SHIFT_SECOND) |
        (ptr[SECOND_NUM] << SHIFT_THIRD) | (ptr[THIRD_NUM]);
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
    return 0;
}

