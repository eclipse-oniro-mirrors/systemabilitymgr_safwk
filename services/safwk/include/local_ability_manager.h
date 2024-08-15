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

#ifndef LOCAL_ABILITY_MANAGER_H
#define LOCAL_ABILITY_MANAGER_H

#include <string>
#include <unordered_map>
#include <list>
#include <unistd.h>
#include <condition_variable>
#include <shared_mutex>
#include "local_ability_manager_stub.h"
#include "system_ability.h"
#include "thread_pool.h"
#include "parse_util.h"
#include "single_instance.h"
#include "system_ability_ondemand_reason.h"
#include "system_ability_status_change_stub.h"
#include "if_system_ability_manager.h"
#include "timer.h"

namespace OHOS {
// Check all dependencies's availability before the timeout period ended, [200, 60000].
const int32_t MIN_DEPENDENCY_TIMEOUT = 200;
const int32_t MAX_DEPENDENCY_TIMEOUT = 60000;
const int32_t DEFAULT_DEPENDENCY_TIMEOUT = 6000;

class LocalAbilityManager : public LocalAbilityManagerStub {
    DECLARE_SINGLE_INSTANCE_BASE(LocalAbilityManager);

public:
    bool AddAbility(SystemAbility* ability);
    bool RemoveAbility(int32_t systemAbilityId);
    SystemAbility* GetAbility(int32_t systemAbilityId);
    bool GetRunningStatus(int32_t systemAbilityId);
    bool AddSystemAbilityListener(int32_t systemAbilityId, int32_t listenerSaId);
    bool RemoveSystemAbilityListener(int32_t systemAbilityId, int32_t listenerSaId);
    std::vector<int32_t> CheckDependencyStatus(const std::vector<int32_t>& dependSas);
    void StartSystemAbilityTask(SystemAbility* sa);
    bool CheckSystemAbilityManagerReady();
    bool InitSystemAbilityProfiles(const std::string& profilePath, int32_t saId);
    void ClearResource();
    void StartOndemandSystemAbility(int32_t systemAbilityId);
    void StopOndemandSystemAbility(int32_t systemAbilityId);
    bool StartAbility(int32_t systemAbilityId, const std::string& eventStr) override;
    bool ActiveAbility(int32_t systemAbilityId,
        const nlohmann::json& activeReason) override;
    bool IdleAbility(int32_t systemAbilityId, const nlohmann::json& idleReason,
        int32_t& delayTime) override;
    bool StopAbility(int32_t systemAbilityId, const std::string& eventStr) override;
    void DoStartSAProcess(const std::string& profilePath, int32_t saId);
    void SetStartReason(int32_t systemAbilityId, const nlohmann::json& event);
    void SetStopReason(int32_t systemAbilityId, const nlohmann::json& event);
    nlohmann::json GetStartReason(int32_t systemAbilityId);
    nlohmann::json GetStopReason(int32_t systemAbilityId);
    SystemAbilityOnDemandReason JsonToOnDemandReason(const nlohmann::json& reasonJson);
    bool SendStrategyToSA(int32_t type, int32_t systemAbilityId, int32_t level, std::string& action) override;
    bool IpcStatCmdProc(int32_t fd, int32_t cmd) override;
    bool FfrtDumperProc(std::string& result) override;
    int32_t SystemAbilityExtProc(const std::string& extension, int32_t said,
        SystemAbilityExtensionPara* callback, bool isAsync = false) override;

private:
    LocalAbilityManager();
    ~LocalAbilityManager();

    bool AddLocalAbilityManager();
    void RegisterOnDemandSystemAbility(int32_t saId);
    void FindAndStartPhaseTasks(int32_t saId);
    void StartPhaseTasks(const std::list<SystemAbility*>& startTasks);
    void CheckTrustSa(const std::string& path, const std::string& process, const std::list<SaProfile>& saInfos);
    void NotifyAbilityListener(int32_t systemAbilityId, int32_t listenerSaId,
        const std::string& deviceId, int32_t code);
    void WaitForTasks();
    void StartDependSaTask(SystemAbility* ability);
    class SystemAbilityListener : public SystemAbilityStatusChangeStub {
    public:
        SystemAbilityListener(int32_t listenerSaId) : listenerSaId_(listenerSaId) {}
        void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
        void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
        int32_t GetListenerSaId() { return listenerSaId_; }
    private:
        int32_t listenerSaId_;
    };

    bool CheckAndGetProfilePath(const std::string& profilePath, std::string& realProfilePath);
    bool InitializeSaProfiles(int32_t saId);
    bool InitializeOnDemandSaProfile(int32_t saId);
    bool InitializeRunOnCreateSaProfiles(uint32_t bootPhase);
    bool InitializeSaProfilesInnerLocked(const SaProfile& saProfile);
    bool Run(int32_t saId);
    bool NeedRegisterOnDemand(const SaProfile& saProfile, int32_t saId);
    bool OnStartAbility(int32_t systemAbilityId);
    bool OnStopAbility(int32_t systemAbilityId);
    std::string GetTraceTag(const std::string& profilePath);
    bool IsResident();
    bool IsConfigUnused();
    void InitUnusedCfg();
    void StartTimedQuery();
    void IdentifyUnusedResident();
    void IdentifyUnusedOndemand();
    bool NoNeedCheckUnused(int32_t saId);
    void LimitUnusedTimeout(int32_t saId, int32_t timeout);
    bool GetSaLastRequestTime(const sptr<ISystemAbilityManager>& samgr, int32_t saId, uint64_t& lastRequestTime);

    std::map<int32_t, SystemAbility*> abilityMap_;
    std::map<uint32_t, std::list<SystemAbility*>> abilityPhaseMap_;
    std::shared_mutex abilityMapLock_;
    sptr<LocalAbilityManager> localAbilityManager_;
    std::map<int32_t, nlohmann::json> saIdToStartReason_;
    std::map<int32_t, nlohmann::json> saIdToStopReason_;
    // Max task number in pool is 20.
    const int32_t MAX_TASK_NUMBER = 20;
    // Check dependent sa status every 50 ms, it equals 50000us.
    const int32_t CHECK_DEPENDENT_SA_PERIOD = 50000;

    std::mutex listenerLock_;
    std::map<std::pair<int32_t, int32_t>, sptr<ISystemAbilityStatusChange>> listenerMap_;
    std::mutex ReasonLock_;
    std::shared_ptr<ParseUtil> profileParser_;

    std::condition_variable startPhaseCV_;
    std::mutex startPhaseLock_;
    int32_t startTaskNum_ = 0;
    std::u16string procName_;
    int64_t startBegin_ = 0;

    // Thread pool used to start system abilities in parallel.
    std::unique_ptr<ThreadPool> initPool_;
    std::unique_ptr<Utils::Timer> idleTimer_;
    // longtime-unusedtimeout map
    std::shared_mutex unusedCfgMapLock_;
    std::map<int32_t, int32_t> unusedCfgMap_;
};
}
#endif
