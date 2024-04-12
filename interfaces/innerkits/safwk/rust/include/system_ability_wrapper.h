/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef SYSTEM_ABILITY_WRAPPER_H
#define SYSTEM_ABILITY_WRAPPER_H

#include <memory>

#include "cxx.h"
#include "ipc_object_stub.h"
#include "safwk_log.h"
#include "system_ability.h"
#include "system_ability_ondemand_reason.h"

namespace OHOS {
namespace SafwkRust {
struct AbilityWrapper;
struct AbilityStub;
struct SystemAbilityOnDemandReason;
struct OnDemandReasonExtraData;

class SystemAbilityWrapper : public SystemAbility {
public:
    SystemAbilityWrapper(rust::Box<AbilityWrapper> ability, int32_t systemAbilityId, bool runOnCreate);
    ~SystemAbilityWrapper() override;

    void StopAbilityWrapper(int32_t systemAbilityId);
    bool CancelIdleWrapper();
    bool PublishWrapper(rust::Box<AbilityStub> ability);

protected:
    std::string GetClassName() override;

    void OnDump() override;

    void OnStart() override;

    void OnStart(const OHOS::SystemAbilityOnDemandReason &startReason) override;

    int32_t OnIdle(const OHOS::SystemAbilityOnDemandReason &idleReason) override;

    void OnActive(const OHOS::SystemAbilityOnDemandReason &activeReason) override;

    void OnStop() override;

    void OnStop(const OHOS::SystemAbilityOnDemandReason &stopReason) override;

    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;

    void OnDeviceLevelChanged(int32_t type, int32_t level, std::string &action) override;

private:
    AbilityWrapper *ability_;
};

void StopAbility(int32_t systemAbilityId);
bool CancelIdle(SystemAbilityWrapper *systemAbilityWrapper);
bool StubPublish(SystemAbilityWrapper *systemAbilityWrapper, rust::Box<AbilityStub> ability);
bool RegisterAbility(SystemAbilityWrapper *system_ability);

SystemAbilityOnDemandReason BuildReasonWrapper(const OHOS::SystemAbilityOnDemandReason &reason);

std::unique_ptr<SystemAbilityWrapper> BuildSystemAbility(
    rust::Box<AbilityWrapper> ability, int32_t systemAbilityId, bool runOnCreate);

struct RemoteServiceStub : public OHOS::IPCObjectStub {
public:
    explicit RemoteServiceStub(AbilityStub *stub);
    ~RemoteServiceStub();

    int OnRemoteRequest(
        uint32_t code, OHOS::MessageParcel &data, OHOS::MessageParcel &reply, OHOS::MessageOption &option) override;

    int Dump(int fd, const std::vector<std::u16string> &args) override;

private:
    AbilityStub *inner_;
};

} // namespace SafwkRust
} // namespace OHOS

#endif