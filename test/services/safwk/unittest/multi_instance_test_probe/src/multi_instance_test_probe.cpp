/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "multi_instance_test_probe.h"

#include <chrono>
#include <cstdint>
#include <thread>
#include <unistd.h>

#include "errors.h"
#include "hilog/log.h"
#include "iservice_registry.h"
#include "system_ability_on_demand_event.h"

namespace OHOS {
namespace {
constexpr int32_t MULTI_INSTANCE_TEST_PROBE_SA_ID = 1498;
constexpr HiviewDFX::HiLogLabel LABEL = { LOG_CORE, 0xD001800, "MultiInstanceTestProbe" };
constexpr std::chrono::milliseconds UNLOAD_DELAY { 100 };

struct OnDemandPolicyRequest {
    int32_t policyType;
    int32_t eventId;
    std::string name;
    std::string value;
    bool persistence;
};

int32_t UpdatePolicy(int32_t saId, const OnDemandPolicyRequest& request)
{
    if ((request.policyType != static_cast<int32_t>(OnDemandPolicyType::START_POLICY) &&
        request.policyType != static_cast<int32_t>(OnDemandPolicyType::STOP_POLICY)) ||
        request.eventId < static_cast<int32_t>(OnDemandEventId::DEVICE_ONLINE) ||
        request.eventId > static_cast<int32_t>(OnDemandEventId::UNREF_EVENT) || request.name.empty()) {
        return ERR_INVALID_VALUE;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return ERR_NULL_OBJECT;
    }
    SystemAbilityOnDemandEvent event;
    event.eventId = static_cast<OnDemandEventId>(request.eventId);
    event.name = request.name;
    event.value = request.value;
    event.persistence = request.persistence;
    std::vector<SystemAbilityOnDemandEvent> events { event };
    int32_t result = samgr->UpdateOnDemandPolicy(
        saId, static_cast<OnDemandPolicyType>(request.policyType), events);
    HiviewDFX::HiLog::Info(LABEL,
        "policy update saId=%{public}d pid=%{public}d type=%{public}d event=%{public}s result=%{public}d",
        saId, static_cast<int32_t>(getpid()), request.policyType, request.name.c_str(), result);
    return result;
}

int32_t GetPolicy(int32_t saId, int32_t policyType, std::string& policy)
{
    if (policyType != static_cast<int32_t>(OnDemandPolicyType::START_POLICY) &&
        policyType != static_cast<int32_t>(OnDemandPolicyType::STOP_POLICY)) {
        return ERR_INVALID_VALUE;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        return ERR_NULL_OBJECT;
    }
    std::vector<SystemAbilityOnDemandEvent> events;
    int32_t result = samgr->GetOnDemandPolicy(saId, static_cast<OnDemandPolicyType>(policyType), events);
    if (result != ERR_OK) {
        return result;
    }
    policy.clear();
    for (const auto& event : events) {
        if (!policy.empty()) {
            policy += ";";
        }
        policy += std::to_string(static_cast<int32_t>(event.eventId)) + "|" + event.name + "|" + event.value +
            "|" + (event.persistence ? "true" : "false");
    }
    HiviewDFX::HiLog::Info(LABEL,
        "policy get saId=%{public}d pid=%{public}d type=%{public}d count=%{public}zu",
        saId, static_cast<int32_t>(getpid()), policyType, events.size());
    return ERR_OK;
}
}

REGISTER_SYSTEM_ABILITY_BY_ID(MultiInstanceTestProbe, MULTI_INSTANCE_TEST_PROBE_SA_ID, true);

MultiInstanceTestProbe::MultiInstanceTestProbe(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
    HiviewDFX::HiLog::Info(LABEL, "construct saId=%{public}d pid=%{public}d", saId, GetProbePid());
}

MultiInstanceTestProbe::~MultiInstanceTestProbe()
{
    HiviewDFX::HiLog::Info(LABEL, "destruct pid=%{public}d", GetProbePid());
}

int32_t MultiInstanceTestProbe::GetProbePid()
{
    return static_cast<int32_t>(getpid());
}

int32_t MultiInstanceTestProbe::TriggerUnload()
{
    return ScheduleUnload(false);
}

int32_t MultiInstanceTestProbe::TriggerUnloadAndCancel()
{
    return ScheduleUnload(true);
}

int32_t MultiInstanceTestProbe::TriggerRemove()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    return samgr == nullptr ? ERR_NULL_OBJECT : samgr->RemoveSystemAbility(MULTI_INSTANCE_TEST_PROBE_SA_ID);
}

int32_t MultiInstanceTestProbe::TriggerRepublish()
{
    return Publish(this) ? ERR_OK : ERR_INVALID_OPERATION;
}

int32_t MultiInstanceTestProbe::UpdateOnDemandPolicyForTest(int32_t policyType, int32_t eventId,
    const std::string& name, const std::string& value, bool persistence)
{
    const OnDemandPolicyRequest request { policyType, eventId, name, value, persistence };
    return UpdatePolicy(MULTI_INSTANCE_TEST_PROBE_SA_ID, request);
}

int32_t MultiInstanceTestProbe::GetOnDemandPolicyForTest(int32_t policyType, std::string& policy)
{
    return GetPolicy(MULTI_INSTANCE_TEST_PROBE_SA_ID, policyType, policy);
}

int32_t MultiInstanceTestProbe::ScheduleUnload(bool cancelUnload)
{
    int32_t saId = MULTI_INSTANCE_TEST_PROBE_SA_ID;
    std::thread([saId, cancelUnload]() {
        std::this_thread::sleep_for(UNLOAD_DELAY);
        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (samgr == nullptr) {
            HiviewDFX::HiLog::Error(LABEL, "get samgr failed, saId=%{public}d", saId);
            return;
        }
        int32_t unloadResult = samgr->UnloadSystemAbility(saId);
        HiviewDFX::HiLog::Info(LABEL, "self unload result=%{public}d saId=%{public}d", unloadResult, saId);
        if (cancelUnload && unloadResult == ERR_OK) {
            int32_t cancelResult = samgr->CancelUnloadSystemAbility(saId);
            HiviewDFX::HiLog::Info(LABEL, "self cancel unload result=%{public}d saId=%{public}d",
                cancelResult, saId);
        }
    }).detach();
    return ERR_OK;
}

void MultiInstanceTestProbe::OnStart()
{
    HiviewDFX::HiLog::Info(LABEL, "OnStart begin saId=%{public}d pid=%{public}d",
        MULTI_INSTANCE_TEST_PROBE_SA_ID, GetProbePid());
    bool ret = Publish(this);
    HiviewDFX::HiLog::Info(LABEL, "OnStart publish result=%{public}d saId=%{public}d pid=%{public}d",
        ret, MULTI_INSTANCE_TEST_PROBE_SA_ID, GetProbePid());
}

void MultiInstanceTestProbe::OnStop()
{
    HiviewDFX::HiLog::Info(LABEL, "OnStop saId=%{public}d pid=%{public}d",
        MULTI_INSTANCE_TEST_PROBE_SA_ID, GetProbePid());
}

void MultiInstanceTestProbe::OnDeviceLevelChanged(int32_t type, int32_t level, std::string& action)
{
    HiviewDFX::HiLog::Info(LABEL,
        "strategy saId=%{public}d pid=%{public}d type=%{public}d level=%{public}d action=%{public}s",
        MULTI_INSTANCE_TEST_PROBE_SA_ID, GetProbePid(), type, level, action.c_str());
}
}
