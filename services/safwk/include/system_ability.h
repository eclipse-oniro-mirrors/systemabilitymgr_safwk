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

#ifndef SYSTEM_ABILITY_H
#define SYSTEM_ABILITY_H

#include <string>
#include <unordered_map>
#include <vector>

#include "iremote_object.h"
#include "refbase.h"
#include "system_ability_ondemand_reason.h"

class CSystemAbilityInnerService;
namespace OHOS {
#define REGISTER_SYSTEM_ABILITY_BY_ID(abilityClassName, systemAbilityId, runOnCreate) \
    const bool abilityClassName##_##RegisterResult = \
    SystemAbility::MakeAndRegisterAbility(new abilityClassName(systemAbilityId, runOnCreate));

#define DECLEAR_SYSTEM_ABILITY(className) \
public: \
virtual std::string GetClassName() override { \
return #className; \
}

#define DECLEAR_BASE_SYSTEM_ABILITY(className) \
public: \
virtual std::string GetClassName() = 0;

#define DECLARE_SYSTEM_ABILITY(className) \
public: \
virtual std::string GetClassName() override { \
return #className; \
}

#define DECLARE_BASE_SYSTEM_ABILITY(className) \
public: \
virtual std::string GetClassName() = 0;


#define DECLARE_SYSTEM_ABILITY(className) \
public: \
virtual std::string GetClassName() override { \
return #className; \
}

#define DECLARE_BASE_SYSTEM_ABILITY(className) \
public: \
virtual std::string GetClassName() = 0;

enum class SystemAbilityState {
    NOT_LOADED = 0,
    ACTIVE,
    IDLE,
};

class SystemAbility {
    DECLARE_BASE_SYSTEM_ABILITY(SystemAbility);

public:
    static bool MakeAndRegisterAbility(SystemAbility* systemAbility);
    bool AddSystemAbilityListener(int32_t systemAbilityId);
    bool RemoveSystemAbilityListener(int32_t systemAbilityId);

protected:
    virtual void OnDump();
    virtual void OnStart();
    virtual void OnStart(const SystemAbilityOnDemandReason& startReason);
    virtual int32_t OnIdle(const SystemAbilityOnDemandReason& idleReason);
    virtual void OnActive(const SystemAbilityOnDemandReason& activeReason);
    virtual void OnStop();
    virtual void OnStop(const SystemAbilityOnDemandReason& stopReason);
    virtual void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId);
    virtual void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId);

    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId);
    bool Publish(sptr<IRemoteObject> systemAbility);
    bool CancelIdle();
    void StopAbility(int32_t systemAbilityId);

    SystemAbility(bool runOnCreate = false);
    SystemAbility(int32_t systemAbilityId, bool runOnCreate = false);
    virtual ~SystemAbility();
    SystemAbilityState GetAbilityState();

private:
    void Start();
    void Idle(const SystemAbilityOnDemandReason& idleReason, int32_t& delayTime);
    void Active(const SystemAbilityOnDemandReason& activeReason);
    void Stop();
    void SADump();
    int32_t GetSystemAbilitId() const;
    void SetLibPath(const std::string& libPath);
    const std::string& GetLibPath() const;
    void SetDependSa(const std::vector<int32_t>& dependSa);
    const std::vector<int32_t>& GetDependSa() const;
    void SetRunOnCreate(bool isRunOnCreate);
    bool IsRunOnCreate() const;
    void SetDistributed(bool isDistributed);
    bool GetDistributed() const;
    void SetDumpLevel(unsigned int dumpLevel);
    unsigned int GetDumpLevel() const;
    void SetDependTimeout(int dependTimeout);
    int GetDependTimeout() const;
    bool GetRunningStatus() const;
    void SetCapability(const std::u16string& capability);
    const std::u16string& GetCapability() const;
    void SetPermission(const std::u16string& defPerm);
    void GetOnDemandReasonExtraData(SystemAbilityOnDemandReason& onDemandStartReason);

    friend class LocalAbilityManager;
    friend class ::CSystemAbilityInnerService;
    
private:
    int32_t saId_ = 0;
    std::string libPath_;
    std::vector<int32_t> dependSa_;
    bool isRunOnCreate_;
    bool isDistributed_;
    unsigned int dumpLevel_;
    int dependTimeout_;
    bool isRunning_;
    SystemAbilityState abilityState_;
    std::u16string capability_;
    sptr<IRemoteObject> publishObj_;
    std::u16string permission_;
    std::recursive_mutex abilityLock;
};
}

#endif
