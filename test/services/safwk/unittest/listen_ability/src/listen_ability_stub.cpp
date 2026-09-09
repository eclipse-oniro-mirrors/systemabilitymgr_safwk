/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "listen_ability_stub.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>

#include "errors.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "system_ability_on_demand_event.h"

namespace OHOS {
namespace {
constexpr std::chrono::milliseconds UNLOAD_DELAY { 100 };

int32_t ScheduleListenAbilityUnload()
{
    std::thread([] {
        std::this_thread::sleep_for(UNLOAD_DELAY);
        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgr != nullptr) {
            (void)samgr->UnloadSystemAbility(DISTRIBUTED_SCHED_TEST_LISTEN_ID);
        }
    }).detach();
    return ERR_OK;
}

int32_t UpdateOnDemandPolicyForTest(int32_t saId, MessageParcel& data)
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return ERR_NULL_OBJECT;
    }
    int32_t policyType = data.ReadInt32();
    int32_t eventId = data.ReadInt32();
    std::string name = data.ReadString();
    std::string value = data.ReadString();
    if (name.empty()) {
        return ERR_INVALID_VALUE;
    }
    SystemAbilityOnDemandEvent event;
    event.eventId = static_cast<OnDemandEventId>(eventId);
    event.name = name;
    event.value = value;
    std::vector<SystemAbilityOnDemandEvent> events { event };
    return samgr->UpdateOnDemandPolicy(saId, static_cast<OnDemandPolicyType>(policyType), events);
}

int32_t GetOnDemandPolicyForTest(int32_t saId, MessageParcel& data, std::string& policy)
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return ERR_NULL_OBJECT;
    }
    int32_t policyType = data.ReadInt32();
    std::vector<SystemAbilityOnDemandEvent> events;
    int32_t result = samgr->GetOnDemandPolicy(
        saId, static_cast<OnDemandPolicyType>(policyType), events);
    if (result != ERR_OK) {
        return result;
    }
    policy.clear();
    for (const auto& event : events) {
        if (!policy.empty()) {
            policy += ";";
        }
        policy += std::to_string(static_cast<int32_t>(event.eventId)) + "|" + event.name + "|" + event.value;
    }
    return ERR_OK;
}
}

int32_t ListenAbilityStub::TriggerRemoveForTest()
{
    return ERR_INVALID_OPERATION;
}

int32_t ListenAbilityStub::TriggerRepublishForTest()
{
    return ERR_INVALID_OPERATION;
}

bool ListenAbilityStub::IsPrivateRequest(uint32_t code) const
{
    switch (code) {
        case TRIGGER_UNLOAD:
        case UPDATE_ON_DEMAND_POLICY:
        case GET_ON_DEMAND_POLICY:
        case TRIGGER_REMOVE:
        case TRIGGER_REPUBLISH:
            return true;
        default:
            return false;
    }
}

int32_t ListenAbilityStub::HandlePrivateRequest(uint32_t code, MessageParcel& data, MessageParcel& reply)
{
    switch (code) {
        case TRIGGER_UNLOAD:
            return reply.WriteInt32(ScheduleListenAbilityUnload()) ? ERR_OK : ERR_FLATTEN_OBJECT;
        case UPDATE_ON_DEMAND_POLICY:
            return reply.WriteInt32(UpdateOnDemandPolicyForTest(
                DISTRIBUTED_SCHED_TEST_LISTEN_ID, data)) ? ERR_OK : ERR_FLATTEN_OBJECT;
        case GET_ON_DEMAND_POLICY: {
            std::string policy;
            int32_t result = GetOnDemandPolicyForTest(DISTRIBUTED_SCHED_TEST_LISTEN_ID, data, policy);
            return (reply.WriteInt32(result) && reply.WriteString(policy)) ? ERR_OK : ERR_FLATTEN_OBJECT;
        }
        case TRIGGER_REMOVE:
            return reply.WriteInt32(TriggerRemoveForTest()) ? ERR_OK : ERR_FLATTEN_OBJECT;
        case TRIGGER_REPUBLISH:
            return reply.WriteInt32(TriggerRepublishForTest()) ? ERR_OK : ERR_FLATTEN_OBJECT;
        default:
            return ERR_TRANSACTION_FAILED;
    }
}

int32_t ListenAbilityStub::HandleInterfaceRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    switch (code) {
        case ADD_VOLUME: {
            int32_t volume = data.ReadInt32();
            bool ret = reply.WriteInt32(AddVolume(volume));
            return (ret ? ERR_OK : ERR_FLATTEN_OBJECT);
        }
        case COMMAND_TEST_SA_CALL_SA: {
            return StubTestSaCallSa(data, reply);
        }
        case COMMAND_TEST_GET_IPC_TIMES: {
            int32_t times;
            ErrCode errCode = TestGetIpcSendRequestTimes(times);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteInt32(times)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case COMMAND_TEST_ClEAR_SA_1493_PROXY: {
            ErrCode errCode = TestClearSa1493Proxy_();
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int32_t ListenAbilityStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    if (IsPrivateRequest(code)) {
        return HandlePrivateRequest(code, data, reply);
    }
    if (GetDescriptor() != data.ReadInterfaceToken()) {
        return ERR_TRANSACTION_FAILED;
    }
    return HandleInterfaceRequest(code, data, reply, option);
}

int32_t ListenAbilityStub::StubTestSaCallSa(MessageParcel &data, MessageParcel &reply)
{
    int32_t number = data.ReadInt32();
    double retNumber;
    ErrCode errCode = TestSaCallSa(number, retNumber);
    if (!reply.WriteInt32(errCode)) {
        return ERR_INVALID_VALUE;
    }
    if (SUCCEEDED(errCode)) {
        if (!reply.WriteDouble(retNumber)) {
            return ERR_INVALID_DATA;
        }
    }
    return ERR_NONE;
}

} // namespace OHOS
