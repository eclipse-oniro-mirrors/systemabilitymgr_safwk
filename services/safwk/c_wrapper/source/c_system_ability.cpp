/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#include "c_system_ability_internal.h"
#include "c_system_ability.h"
#include "local_ability_manager.h"
#include "safwk_log.h"

using namespace OHOS;
namespace {
const std::string TAG = "SystemAbility";
}
CSystemAbilityInner::CSystemAbilityInner(const void *userData)
    : userData_(userData)
{
}

CSystemAbilityInner::~CSystemAbilityInner()
{
}

CSystemAbilityInnerService::CSystemAbilityInnerService(CSystemAbility *cSystemability,
    OnStartCb onStart, OnStopCb onStop, int32_t systemAbilityId, bool runOnCreate)
    : SystemAbility(systemAbilityId, runOnCreate), cSystemability_(cSystemability),
    onStartcallback_(onStart), onStopcallback_(onStop)
{
}

CSystemAbilityInnerService::~CSystemAbilityInnerService()
{
}

void CSystemAbilityInnerService::OnStart()
{
    if (onStartcallback_ == nullptr || cSystemability_ == nullptr) {
        HILOGE(TAG, "OnStart onStopcallback_ or cSystemability_ is null");
        return;
    }
    onStartcallback_(cSystemability_);
    std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
    abilityState_ = SystemAbilityState::ACTIVE;
}

void CSystemAbilityInnerService::OnStop()
{
    if (onStopcallback_ == nullptr || cSystemability_ == nullptr) {
        HILOGE(TAG, "OnStop onStopcallback_ or cSystemability_ is null");
        return;
    }
    onStopcallback_(cSystemability_);
}

CSystemAbility *CreateSystemAbility(int32_t saId, bool runOncreate, OnStartCb onStart,
    OnStopCb onStop)
{
    if (onStart == nullptr || onStop == nullptr) {
        HILOGE(TAG, "CreateSystemAbility onStart or onStop is null");
        return nullptr;
    }
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    if (cSystemability == nullptr) {
        HILOGE(TAG, "new CSystemAbilityInner failed");
        return nullptr;
    }
    cSystemability->cSystemabilityInnerservice_ =
        new (std::nothrow) CSystemAbilityInnerService(cSystemability, onStart, onStop, saId, runOncreate);
    if (cSystemability->cSystemabilityInnerservice_ == nullptr) {
        HILOGE(TAG, "new CSystemAbilityInnerService failed");
        delete cSystemability;
        return nullptr;
    }
    return cSystemability;
}

const void *RemoteObjectGetUserData(CSystemAbility *cSystemability)
{
    if (cSystemability == nullptr) {
        HILOGE(TAG, "RemoteObjectGetUserData unexpected cSystemability");
        return nullptr;
    }
    return cSystemability->GetUserData();
}

bool MakeAndRegisterAbility(CSystemAbility* cSystemability, const void *userData)
{
    if (cSystemability == nullptr || userData == nullptr) {
        HILOGE(TAG, "MakeAndRegisterAbility cSystemability or userData is nullptr");
        return false;
    }
    cSystemability->SetUserData(userData);
    auto SystemAbility = cSystemability->cSystemabilityInnerservice_;
    if (SystemAbility == nullptr) {
        HILOGE(TAG, "MakeAndRegisterAbility unexpected SystemAbility");
        return false;
    }
    return LocalAbilityManager::GetInstance().AddAbility(SystemAbility);
}

void DeleteNative(CSystemAbility* cSystemability)
{
    if (cSystemability == nullptr) {
        return;
    }
    delete(cSystemability);
}