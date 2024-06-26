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

#include "system_ability.h"

#include <cinttypes>

#include "datetime_ex.h"
#include "errors.h"
#include "hitrace_meter.h"
#include "if_system_ability_manager.h"
#include "hisysevent_adapter.h"
#include "iservice_registry.h"
#include "local_ability_manager.h"
#include "nlohmann/json.hpp"
#include "safwk_log.h"
#include "string_ex.h"
#include "samgr_xcollie.h"

namespace OHOS {

SystemAbility::SystemAbility(bool runOnCreate)
{
    isRunning_ = false;
    abilityState_ = SystemAbilityState::NOT_LOADED;
    isRunOnCreate_ = runOnCreate;
    isDistributed_ = false;
    dumpLevel_ = 0;
    // timeout for waiting dependency ready, which configed in json, with DEFAULT_DEPENDENCY_TIMEOUT(6s) by default
    dependTimeout_ = DEFAULT_DEPENDENCY_TIMEOUT;
    capability_ = u"";
}

SystemAbility::SystemAbility(int32_t systemAbilityId, bool runOnCreate) : SystemAbility(runOnCreate)
{
    saId_ = systemAbilityId;
}

SystemAbility::~SystemAbility()
{
    HILOGI(TAG, "SA:%{public}d destroyed", saId_);
}

bool SystemAbility::MakeAndRegisterAbility(SystemAbility* systemAbility)
{
    HILOGD(TAG, "registering system ability...");
    return LocalAbilityManager::GetInstance().AddAbility(systemAbility);
}

bool SystemAbility::AddSystemAbilityListener(int32_t systemAbilityId)
{
    HILOGD(TAG, "SA:%{public}d, listenerSA:%{public}d", systemAbilityId, saId_);
    return LocalAbilityManager::GetInstance().AddSystemAbilityListener(systemAbilityId, saId_);
}

bool SystemAbility::RemoveSystemAbilityListener(int32_t systemAbilityId)
{
    HILOGD(TAG, "SA:%{public}d, listenerSA:%{public}d", systemAbilityId, saId_);
    return LocalAbilityManager::GetInstance().RemoveSystemAbilityListener(systemAbilityId, saId_);
}

bool SystemAbility::Publish(sptr<IRemoteObject> systemAbility)
{
    if (systemAbility == nullptr) {
        HILOGE(TAG, "systemAbility is nullptr");
        return false;
    }
    LOGD("Publish SA:%{public}d", saId_);
    // Avoid automatic destruction of system ability caused by failure of publishing ability
    publishObj_ = systemAbility;
    int64_t begin = GetTickCount();
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE(TAG, "failed to get samgrProxy");
        return false;
    }

    ISystemAbilityManager::SAExtraProp saExtra(GetDistributed(), GetDumpLevel(), capability_, permission_);
    std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
    int32_t result = samgrProxy->AddSystemAbility(saId_, publishObj_, saExtra);
    KHILOGI(TAG, "SA:%{public}d result:%{public}d,spend:%{public}" PRId64 "ms",
        saId_, result, (GetTickCount() - begin));
    if (result == ERR_OK) {
        abilityState_ = SystemAbilityState::ACTIVE;
        return true;
    }
    return false;
}

bool SystemAbility::CancelIdle()
{
    {
        std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
        if (abilityState_ != SystemAbilityState::IDLE) {
            return true;
        }
    }
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE(TAG, "failed to get samgrProxy");
        return false;
    }
    int32_t result = samgrProxy->CancelUnloadSystemAbility(saId_);
    return result == ERR_OK;
}

void SystemAbility::StopAbility(int32_t systemAbilityId)
{
    HILOGD(TAG, "SA:%{public}d", systemAbilityId);

    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE(TAG, "failed to get samgrProxy");
        return;
    }
    int64_t begin = GetTickCount();
    int32_t ret = samgrProxy->RemoveSystemAbility(systemAbilityId);
    KHILOGI(TAG, "%{public}s to rm SA:%{public}d, spend:%{public}" PRId64 " ms",
        (ret == ERR_OK) ? "success" : "failed", systemAbilityId, (GetTickCount() - begin));
}

void SystemAbility::GetOnDemandReasonExtraData(SystemAbilityOnDemandReason& onDemandStartReason)
{
    if (!onDemandStartReason.HasExtraData()) {
        return;
    }
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE(TAG, "failed to get samgrProxy");
        return;
    }
    LOGI("get extra id: %{public}d", static_cast<int32_t>(onDemandStartReason.GetExtraDataId()));
    MessageParcel extraDataParcel;
    int32_t ret = samgrProxy->GetOnDemandReasonExtraData(onDemandStartReason.GetExtraDataId(), extraDataParcel);
    if (ret != ERR_OK) {
        HILOGE(TAG, "get extra data failed");
        return;
    }
    auto extraData = extraDataParcel.ReadParcelable<OnDemandReasonExtraData>();
    if (extraData == nullptr) {
        HILOGE(TAG, "read extra data failed");
        return;
    }
    onDemandStartReason.SetExtraData(*extraData);
    HILOGD(TAG, "get extra data: %{public}d, %{public}s", onDemandStartReason.GetExtraData().GetCode(),
        onDemandStartReason.GetExtraData().GetData().c_str());
    delete extraData;
}

void SystemAbility::Start()
{
    // Ensure that the lifecycle is sequentially called by SAMGR
    HILOGD(TAG, "starting system ability...");
    {
        std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
        if (abilityState_ != SystemAbilityState::NOT_LOADED) {
            return;
        }
    }
    LOGD("Start OnStart-SA:%{public}d", saId_);
    int64_t begin = GetTickCount();
    HITRACE_METER_NAME(HITRACE_TAG_SAMGR, ToString(saId_) + "_OnStart");
    nlohmann::json startReason = LocalAbilityManager::GetInstance().GetStartReason(saId_);
    SystemAbilityOnDemandReason onDemandStartReason =
        LocalAbilityManager::GetInstance().JsonToOnDemandReason(startReason);
    GetOnDemandReasonExtraData(onDemandStartReason);

    OnStart(onDemandStartReason);

    std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
    isRunning_ = true;
    int64_t duration = GetTickCount() - begin;
    KHILOGI(TAG, "OnStart-SA:%{public}d finished, spend:%{public}" PRId64 " ms",
        saId_, duration);
    ReportSaLoadDuration(saId_, SA_LOAD_ON_START, duration);
}

void SystemAbility::Idle(const SystemAbilityOnDemandReason& idleReason,
    int32_t& delayTime)
{
    {
        std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
        if (abilityState_ != SystemAbilityState::ACTIVE) {
            HILOGI(TAG, "cannot idle SA:%{public}d", saId_);
            return;
        }
    }
    LOGD("Idle SA::%{public}d", saId_);
    int64_t begin = GetTickCount();
    delayTime = OnIdle(idleReason);
    std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
    if (delayTime == 0) {
        abilityState_ = SystemAbilityState::IDLE;
    }
    LOGI("Idle SA:%{public}d end,spend:%{public}" PRId64 "ms",
        saId_, (GetTickCount() - begin));
}

void SystemAbility::Active(const SystemAbilityOnDemandReason& activeReason)
{
    {
        std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
        if (abilityState_ != SystemAbilityState::IDLE) {
            HILOGI(TAG, "cannot active SA:%{public}d", saId_);
            return;
        }
    }
    LOGD("Active SA:%{public}d", saId_);
    int64_t begin = GetTickCount();
    OnActive(activeReason);
    std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
    abilityState_ = SystemAbilityState::ACTIVE;
    LOGI("Active SA:%{public}d end,spend:%{public}" PRId64 "ms",
        saId_, (GetTickCount() - begin));
}

void SystemAbility::Stop()
{
    HILOGD(TAG, "stopping system ability...");
    {
        std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
        if (abilityState_ == SystemAbilityState::NOT_LOADED) {
            return;
        }
    }
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE(TAG, "failed to get samgrProxy");
        return;
    }
    int64_t begin = GetTickCount();
    int32_t ret = samgrProxy->RemoveSystemAbility(saId_);
    KHILOGI(TAG, "%{public}s to rm SA:%{public}d,spend:%{public}" PRId64 "ms",
        (ret == ERR_OK) ? "suc" : "fail", saId_, (GetTickCount() - begin));

    begin = GetTickCount();
    nlohmann::json stopReason = LocalAbilityManager::GetInstance().GetStopReason(saId_);
    SystemAbilityOnDemandReason onDemandStopReason =
        LocalAbilityManager::GetInstance().JsonToOnDemandReason(stopReason);

    {
        SamgrXCollie samgrXCollie("safwk--onStop_" + ToString(saId_));
        OnStop(onDemandStopReason);
    }

    std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
    abilityState_ = SystemAbilityState::NOT_LOADED;
    isRunning_ = false;
    int64_t duration = GetTickCount() - begin;
    KHILOGI(TAG, "OnStop-SA:%{public}d finished,spend:%{public}" PRId64 "ms",
        saId_, duration);
    ReportSaUnLoadDuration(saId_, SA_UNLOAD_ON_STOP, duration);
}

void SystemAbility::SADump()
{
    OnDump();
}

int32_t SystemAbility::GetSystemAbilitId() const
{
    return saId_;
}

void SystemAbility::SetLibPath(const std::string& libPath)
{
    libPath_ = libPath;
}

const std::string& SystemAbility::GetLibPath() const
{
    return libPath_;
}

void SystemAbility::SetDependSa(const std::vector<std::int32_t>& dependSa)
{
    dependSa_ = dependSa;
}

const std::vector<std::int32_t>& SystemAbility::GetDependSa() const
{
    return dependSa_;
}

void SystemAbility::SetRunOnCreate(bool isRunOnCreate)
{
    isRunOnCreate_ = isRunOnCreate;
}

bool SystemAbility::IsRunOnCreate() const
{
    return isRunOnCreate_;
}

void SystemAbility::SetDistributed(bool isDistributed)
{
    isDistributed_ = isDistributed;
}

bool SystemAbility::GetDistributed() const
{
    return isDistributed_;
}

bool SystemAbility::GetRunningStatus() const
{
    return isRunning_;
}

SystemAbilityState SystemAbility::GetAbilityState()
{
    std::lock_guard<std::recursive_mutex> autoLock(abilityLock);
    return abilityState_;
}

void SystemAbility::SetDumpLevel(uint32_t dumpLevel)
{
    dumpLevel_ = dumpLevel;
}

uint32_t SystemAbility::GetDumpLevel() const
{
    return dumpLevel_;
}

void SystemAbility::SetDependTimeout(int32_t dependTimeout)
{
    if (dependTimeout >= MIN_DEPENDENCY_TIMEOUT && dependTimeout <= MAX_DEPENDENCY_TIMEOUT) {
        dependTimeout_ = dependTimeout;
    }
    LOGD("SetDependTimeout new:%{public}d,old:%{public}d", dependTimeout, dependTimeout_);
}

int32_t SystemAbility::GetDependTimeout() const
{
    return dependTimeout_;
}

// The details should be implemented by subclass
void SystemAbility::OnDump()
{
}

// The details should be implemented by subclass
void SystemAbility::OnStart()
{
}

// The details should be implemented by subclass
void SystemAbility::OnStart(const SystemAbilityOnDemandReason& startReason)
{
    OnStart();
}

int32_t SystemAbility::OnIdle(const SystemAbilityOnDemandReason& idleReason)
{
    LOGD("OnIdle SA, idle reason %{public}d, %{public}s, %{public}s",
        idleReason.GetId(), idleReason.GetName().c_str(), idleReason.GetValue().c_str());
    return 0;
}

void SystemAbility::OnActive(const SystemAbilityOnDemandReason& activeReason)
{
    LOGD("OnActive SA, active reason %{public}d, %{public}s, %{public}s",
        activeReason.GetId(), activeReason.GetName().c_str(), activeReason.GetValue().c_str());
}

// The details should be implemented by subclass
void SystemAbility::OnStop()
{
}

// The details should be implemented by subclass
void SystemAbility::OnStop(const SystemAbilityOnDemandReason& stopReason)
{
    OnStop();
}

// The details should be implemented by subclass
void SystemAbility::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
}

// The details should be implemented by subclass
void SystemAbility::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
}

sptr<IRemoteObject> SystemAbility::GetSystemAbility(int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE(TAG, "failed to get samgrProxy");
        return nullptr;
    }

    return samgrProxy->GetSystemAbility(systemAbilityId);
}

void SystemAbility::SetCapability(const std::u16string& capability)
{
    capability_ = capability;
}

const std::u16string& SystemAbility::GetCapability() const
{
    return capability_;
}

void SystemAbility::SetPermission(const std::u16string& permission)
{
    permission_ = permission;
}

// The details should be implemented by subclass
void SystemAbility::OnDeviceLevelChanged(int32_t type, int32_t level, std::string& action)
{
}

int32_t SystemAbility::OnExtension(const std::string& extension, MessageParcel& data, MessageParcel& reply)
{
    return 0;
}
}
