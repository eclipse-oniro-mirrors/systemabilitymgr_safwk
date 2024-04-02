// Copyright (C) 2023 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "system_ability_wrapper.h"

#include <memory>
#include <string>
#include <utility>

#include "cxx.h"
#include "safwk_log.h"
#include "system_ability.h"
#include "system_ability_ondemand_reason.h"
#include "wrapper.rs.h"

namespace OHOS {
namespace SafwkRust {
const std::string TAG = "SafwkRustInterface";

SystemAbilityWrapper::SystemAbilityWrapper(
    rust::Box<AbilityWrapper> ability_, int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate)
{
    this->ability_ = ability_.into_raw();
}

SystemAbilityWrapper::~SystemAbilityWrapper()
{
    rust::Box<AbilityWrapper>::from_raw(this->ability_);
}

void SystemAbilityWrapper::StopAbilityWrapper(int32_t systemAbilityId)
{
    SystemAbility::StopAbility(systemAbilityId);
}

std::string SystemAbilityWrapper::GetClassName()
{
    return std::string("hello");
}

void SystemAbilityWrapper::OnDump()
{
    ability_->OnDump();
}

void SystemAbilityWrapper::OnStart()
{
    ability_->OnStart(this);
}

void SystemAbilityWrapper::OnStart(const OHOS::SystemAbilityOnDemandReason &startReason)
{
    ability_->OnStartWithReason(BuildReasonWrapper(startReason), this);
}

bool SystemAbilityWrapper::CancelIdleWrapper(SystemAbilityWrapper *systemAbilityWrapper)
{
    return systemAbilityWrapper->CancelIdle();
}

bool SystemAbilityWrapper::PublishWrapper(SystemAbilityWrapper *systemAbilityWrapper, sptr<IRemoteObject> systemAbility)
{
    return systemAbilityWrapper->Publish(systemAbility);
}

int32_t SystemAbilityWrapper::OnIdle(const OHOS::SystemAbilityOnDemandReason &idleReason)
{
    return ability_->OnIdle(BuildReasonWrapper(idleReason), this);
}

void SystemAbilityWrapper::OnActive(const OHOS::SystemAbilityOnDemandReason &activeReason)
{
    ability_->OnActive(BuildReasonWrapper(activeReason));
}

void SystemAbilityWrapper::OnStop()
{
    ability_->OnStop();
}

void SystemAbilityWrapper::OnStop(const OHOS::SystemAbilityOnDemandReason &stopReason)
{
    ability_->OnStopWithReason(BuildReasonWrapper(stopReason));
}

void SystemAbilityWrapper::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    ability_->OnAddSystemAbility(systemAbilityId, deviceId);
}

void SystemAbilityWrapper::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    ability_->OnRemoveSystemAbility(systemAbilityId, deviceId);
}

void SystemAbilityWrapper::OnDeviceLevelChanged(int32_t type, int32_t level, std::string &action)
{
    ability_->OnDeviceLevelChanged(type, level, action);
}

void StopAbility(int32_t systemAbilityId)
{
    SystemAbilityWrapper::StopAbilityWrapper(systemAbilityId);
}

bool StubPublish(SystemAbilityWrapper *systemAbilityWrapper, rust::Box<AbilityStub> ability)
{
    auto stub = sptr<RemoteServiceStub>::MakeSptr(ability.into_raw());
    if (stub == nullptr) {
        return false;
    }
    return SystemAbilityWrapper::PublishWrapper(systemAbilityWrapper, stub);
}

bool CancelIdle(SystemAbilityWrapper *systemAbilityWrapper)
{
    return SystemAbilityWrapper::CancelIdleWrapper(systemAbilityWrapper);
}

bool RegisterAbility(SystemAbilityWrapper *system_ability)
{
    HILOGD(TAG, "register_ability_");
    return SystemAbility::MakeAndRegisterAbility(static_cast<SystemAbility *>(system_ability));
}

SystemAbilityOnDemandReason BuildReasonWrapper(const OHOS::SystemAbilityOnDemandReason &reason)
{
    return SystemAbilityOnDemandReason{
        .name = reason.GetName(),
        .value = reason.GetValue(),
        .reason_id = reason.GetId(),
        .extra_data = OnDemandReasonExtraData{ .data = reason.GetExtraData().GetData(),
            .code = reason.GetExtraData().GetCode() },
        .extra_data_id = reason.GetExtraDataId(),
    };
}

std::unique_ptr<SystemAbilityWrapper> BuildSystemAbility(
    rust::Box<AbilityWrapper> ability_, int32_t systemAbilityId, bool runOnCreate)
{
    return std::make_unique<SystemAbilityWrapper>(std::move(ability_), systemAbilityId, runOnCreate);
}

RemoteServiceStub::RemoteServiceStub(AbilityStub *ability)
{
    this->inner_ = ability;
}

RemoteServiceStub::~RemoteServiceStub()
{
    auto ability = rust::Box<AbilityStub>::from_raw(this->inner_);
}

int RemoteServiceStub ::OnRemoteRequest(
    uint32_t code, OHOS::MessageParcel &data, OHOS::MessageParcel &reply, OHOS::MessageOption &option)
{
    return inner_->on_remote_request(code, data, reply);
}

int RemoteServiceStub::Dump(int fd, const std::vector<std::u16string> &args)
{
    rust::vec<rust::string> rust_v;
    for (auto arg : args) {
        rust_v.push_back(arg.data());
    }
    return inner_->dump(fd, rust_v);
}

} // namespace SafwkRust
} // namespace OHOS
