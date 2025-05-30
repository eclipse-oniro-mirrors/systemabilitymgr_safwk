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

#include "local_ability_manager.h"

#include <chrono>
#include <cinttypes>
#include <dlfcn.h>
#include <iostream>
#include <sys/types.h>
#include <thread>
#include <csignal>
#include <sys/time.h>
#include <securec.h>

#include "datetime_ex.h"
#include "errors.h"
#include "hitrace_meter.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "safwk_log.h"
#include "file_ex.h"
#include "string_ex.h"
#include "hisysevent_adapter.h"
#include "system_ability_ondemand_reason.h"
#include "local_ability_manager_dumper.h"
#include "timer.h"
#include "hisysevent_adapter.h"
#include "system_ability_definition.h"
#include "samgr_xcollie.h"
#ifdef SAFWK_ENABLE_RUN_ON_DEMAND_QOS
#include <sys/syscall.h>
#include <sys/resource.h>
#endif

namespace OHOS {
using std::u16string;
using std::string;
using std::vector;

namespace {
constexpr int32_t RETRY_TIMES_FOR_ONDEMAND = 10;
constexpr int32_t RETRY_TIMES_FOR_SAMGR = 50;
constexpr int32_t DEFAULT_SAID = -1;
constexpr int32_t IDLE_SA_REPORT_SIGNUM = 60;
constexpr int32_t UNUSED_RESIDENT_TIMER_INTERVAL_SECONDS = 60 * 60;
constexpr int32_t UNUSED_ONDEMAND_TIMER_INTERVAL_MSECONDS = 1000 * 60 * 1;
constexpr int32_t ONDEMAND_SA_UNUSED_TIMEOUT_LOWLIMIT = UNUSED_ONDEMAND_TIMER_INTERVAL_MSECONDS;
constexpr int32_t ONDEMAND_SA_UNUSED_TIMEOUT_UPLIMIT = 1000 * 60 * 120;
constexpr int32_t RESIDENT_SA_UNUSED_TIMEOUT = 1000 * 60 * 60;
constexpr std::chrono::milliseconds MILLISECONDS_WAITING_SAMGR_ONE_TIME(200);
constexpr std::chrono::milliseconds MILLISECONDS_WAITING_ONDEMAND_ONE_TIME(100);
constexpr int32_t TIME_S_TO_MS = 1000;
constexpr int32_t MAX_STARTSA_TIMEOUT = 65;
constexpr int32_t MAX_CHECK_TIMEOUT = 10;

constexpr int32_t MAX_SA_STARTUP_TIME = 100;
constexpr int32_t SUFFIX_LENGTH = 5; // .json length
constexpr uint32_t FFRT_DUMP_INFO_ALL = 0;
constexpr int FFRT_BUFFER_SIZE = 512 * 1024;

#ifdef SAFWK_ENABLE_RUN_ON_DEMAND_QOS
constexpr int OPEN_SO_PRIO = -20;
constexpr int NORMAL_PRIO = 0;
#endif

constexpr const char* PROFILES_DIR = "/system/profile/";
constexpr const char* PREFIX = PROFILES_DIR;
constexpr const char* SUFFIX = "_trust.json";

constexpr const char* ONDEMAND_WORKER = "SaOndemand";
constexpr const char* INIT_POOL = "SaInit";

constexpr const char* EVENT_ID = "eventId";
constexpr const char* NAME = "name";
constexpr const char* VALUE = "value";
constexpr const char* EXTRA_DATA_ID = "extraDataId";

enum {
    BOOT_START = 1,
    CORE_START = 2,
    OTHER_START = 3,
};
}

IMPLEMENT_SINGLE_INSTANCE(LocalAbilityManager);

#ifdef SAFWK_ENABLE_RUN_ON_DEMAND_QOS
static void SetThreadPrio(int priority)
{
    int tid = syscall(SYS_gettid);
    LOGI("set tid:%{public}d priority:%{public}d.", tid, priority);
    if (setpriority(PRIO_PROCESS, tid, priority) != 0) {
        LOGE("set tid:%{public}d priority:%{public}d failed.", tid, priority);
    }
}
#endif

LocalAbilityManager::LocalAbilityManager()
{
    profileParser_ = std::make_shared<ParseUtil>();
    initPool_ = std::make_unique<ThreadPool>(INIT_POOL);
}

LocalAbilityManager::~LocalAbilityManager()
{
    StopTimedQuery();
}

void LocalAbilityManager::DoStartSAProcess(const std::string& profilePath, int32_t saId)
{
    startBegin_ = GetTickCount();
    HILOGD(TAG, "SA:%{public}d", saId);
    string realProfilePath = "";
    if (!CheckAndGetProfilePath(profilePath, realProfilePath)) {
        ReportSaMainExit("DoStartSAProcess invalid path");
        HILOGE(TAG, "DoStartSAProcess invalid path");
        return;
    }
    {
        std::string traceTag = GetTraceTag(realProfilePath);
        HitraceScopedEx samgrHitrace(HITRACE_LEVEL_INFO, HITRACE_TAG_SAMGR, traceTag.c_str());
        bool ret = InitSystemAbilityProfiles(realProfilePath, saId);
        if (!ret) {
            ReportSaMainExit("InitSaProfiles no right profile");
            HILOGE(TAG, "InitSystemAbilityProfiles no right profile, will exit");
            return;
        }
        {
            SamgrXCollie samgrXCollie("safwk--CheckSamgrReady", MAX_CHECK_TIMEOUT);
            ret = CheckSystemAbilityManagerReady();
        }
        if (!ret) {
            ReportSaMainExit("CheckSamgrReady failed");
            HILOGE(TAG, "CheckSystemAbilityManagerReady failed! will exit");
            return;
        }
        ret = Run(saId);
        if (!ret) {
            ReportSaMainExit("SA Run failed");
            HILOGE(TAG, "Run failed! will exit");
            return;
        }
    }

    StartTimedQuery();
    IPCSkeleton::JoinWorkThread();
    HILOGE(TAG, "JoinWorkThread stop, will exit");
}

std::string LocalAbilityManager::GetTraceTag(const std::string& profilePath)
{
    std::vector<std::string> libPathVec;
    string traceTag = "default_proc";
    SplitStr(profilePath, "/", libPathVec);
    if ((libPathVec.size() > 0)) {
        traceTag = libPathVec[libPathVec.size() - 1];
        auto size = traceTag.length();
        if (size > SUFFIX_LENGTH) {
            return traceTag.substr(0, size - SUFFIX_LENGTH);
        }
    }
    return traceTag;
}

bool LocalAbilityManager::CheckAndGetProfilePath(const std::string& profilePath, std::string& realProfilePath)
{
    if (profilePath.length() > PATH_MAX) {
        HILOGE(TAG, "profilePath length too long!");
        return false;
    }
    char realPath[PATH_MAX] = {'\0'};
    if (realpath(profilePath.c_str(), realPath) == nullptr) {
        HILOGE(TAG, "file path does not exist!");
        return false;
    }
    realProfilePath = realPath;
    return true;
}

bool LocalAbilityManager::CheckSystemAbilityManagerReady()
{
    int32_t timeout = RETRY_TIMES_FOR_SAMGR;
    constexpr int32_t duration = std::chrono::microseconds(MILLISECONDS_WAITING_SAMGR_ONE_TIME).count();
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    while (samgrProxy == nullptr) {
        HILOGI(TAG, "%{public}s waiting for samgr...", Str16ToStr8(procName_).c_str());
        if (timeout > 0) {
            usleep(duration);
            samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        } else {
            HILOGE(TAG, "wait for samgr time out (10s)");
            return false;
        }
        timeout--;
    }
    return true;
}

bool LocalAbilityManager::InitSystemAbilityProfiles(const std::string& profilePath, int32_t saId)
{
    LOGD("InitProfiles parse sa profiles!");
    int64_t begin = GetTickCount();
    bool ret = profileParser_->ParseSaProfiles(profilePath);
    if (!ret) {
        HILOGW(TAG, "ParseSaProfiles failed!");
        return false;
    }

    procName_ = profileParser_->GetProcessName();
    auto saInfos = profileParser_->GetAllSaProfiles();
    std::string process = Str16ToStr8(procName_);
    LOGI("InitProfiles proc:%{public}s end,spend:%{public}" PRId64 "ms", process.c_str(), (GetTickCount() - begin));
    std::string path = PREFIX + process + SUFFIX;
    bool isExist = profileParser_->CheckPathExist(path);
    if (isExist) {
        CheckTrustSa(path, process, saInfos);
    }
    return InitializeSaProfiles(saId);
}

bool LocalAbilityManager::InitializeSaProfiles(int32_t saId)
{
    if (saId != DEFAULT_SAID) {
        return InitializeOnDemandSaProfile(saId);
    } else {
        return InitializeRunOnCreateSaProfiles(BOOT_START);
    }
}

void LocalAbilityManager::CheckTrustSa(const std::string& path, const std::string& process,
    const std::list<SaProfile>& saInfos)
{
    HILOGD(TAG, "CheckTrustSa start");
    std::map<std::u16string, std::set<int32_t>> trustMaps;
    bool ret = profileParser_->ParseTrustConfig(path, trustMaps);
    if (ret && !trustMaps.empty()) {
        // 1.get allowed sa set in the process
        const auto& saSets = trustMaps[Str8ToStr16(process)];
        // 2.check to-load sa in the allowed sa set, and if to-load sa not in the allowed, will remove and not load it
        for (const auto& saInfo : saInfos) {
            if (saSets.find(saInfo.saId) == saSets.end()) {
                HILOGW(TAG, "SA:%{public}d not allow to load in %{public}s", saInfo.saId, process.c_str());
                profileParser_->RemoveSaProfile(saInfo.saId);
            }
        }
    }
}

void LocalAbilityManager::ClearResource()
{
    profileParser_->ClearResource();
}

bool LocalAbilityManager::AddAbility(SystemAbility* ability)
{
    if (ability == nullptr) {
        HILOGW(TAG, "try to add null ability!");
        return false;
    }

    int32_t saId = ability->GetSystemAbilitId();
    SaProfile saProfile;
    bool ret = profileParser_->GetProfile(saId, saProfile);
    if (!ret) {
        return false;
    }
    std::unique_lock<std::shared_mutex> writeLock(localAbilityMapLock_);
    auto iter = localAbilityMap_.find(saId);
    if (iter != localAbilityMap_.end()) {
        HILOGW(TAG, "try to add existed SA:%{public}d!", saId);
        return false;
    }
    HILOGI(TAG, "set profile attr for SA:%{public}d", saId);
    ability->SetLibPath(saProfile.libPath);
    ability->SetRunOnCreate(saProfile.runOnCreate);
    ability->SetDependSa(saProfile.dependSa);
    ability->SetDependTimeout(saProfile.dependTimeout);
    ability->SetDistributed(saProfile.distributed);
    ability->SetDumpLevel(saProfile.dumpLevel);
    ability->SetCapability(saProfile.capability);
    ability->SetPermission(saProfile.permission);
    localAbilityMap_.emplace(saId, ability);
    return true;
}

bool LocalAbilityManager::RemoveAbility(int32_t systemAbilityId)
{
    if (systemAbilityId <= 0) {
        HILOGW(TAG, "invalid systemAbilityId");
        return false;
    }
    std::unique_lock<std::shared_mutex> writeLock(localAbilityMapLock_);
    (void)localAbilityMap_.erase(systemAbilityId);
    return true;
}

bool LocalAbilityManager::AddSystemAbilityListener(int32_t systemAbilityId, int32_t listenerSaId)
{
    if (!CheckInputSysAbilityId(systemAbilityId) || !CheckInputSysAbilityId(listenerSaId)) {
        HILOGW(TAG, "SA:%{public}d or listenerSA:%{public}d invalid!", systemAbilityId, listenerSaId);
        return false;
    }
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE(TAG, "failed to get samgrProxy");
        return false;
    }
    bool isNeedNotify = false;
    size_t listenerListSize = 0;
    {
        HILOGD(TAG, "SA:%{public}d, listenerSA:%{public}d", systemAbilityId, listenerSaId);
        std::lock_guard<std::mutex> autoLock(listenerLock_);
        auto& listenerList = localListenerMap_[systemAbilityId];
        auto iter = std::find_if(listenerList.begin(), listenerList.end(),
            [listenerSaId](const std::pair<int32_t, ListenerState>& listener) {
            return listener.first == listenerSaId;
        });
        if (listenerList.size() > 0) {
            sptr<IRemoteObject> object = samgrProxy->CheckSystemAbility(systemAbilityId);
            if (object != nullptr) {
                isNeedNotify = true;
            }
        }
        if (iter == listenerList.end()) {
            listenerList.push_back({listenerSaId,
                (isNeedNotify) ? ListenerState::NOTIFIED : ListenerState::INIT});
        }
        listenerListSize = listenerList.size();
        LOGI("AddSaListener SA:%{public}d,listenerSA:%{public}d,size:%{public}zu", systemAbilityId, listenerSaId,
            listenerList.size());
    }
    if (listenerListSize > 1) {
        if (isNeedNotify) {
            NotifyAbilityListener(systemAbilityId, listenerSaId, "",
                ISystemAbilityStatusChange::ON_ADD_SYSTEM_ABILITY);
        }
        return true;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(systemAbilityId, GetSystemAbilityStatusChange());
    if (ret) {
        HILOGE(TAG, "failed to subscribe SA:%{public}d, process name:%{public}s", systemAbilityId,
            Str16ToStr8(procName_).c_str());
        return false;
    }
    return true;
}

bool LocalAbilityManager::RemoveSystemAbilityListener(int32_t systemAbilityId, int32_t listenerSaId)
{
    if (!CheckInputSysAbilityId(systemAbilityId) || !CheckInputSysAbilityId(listenerSaId)) {
        HILOGW(TAG, "SA:%{public}d or listenerSA:%{public}d invalid!",
            systemAbilityId, listenerSaId);
        return false;
    }
    {
        HILOGD(TAG, "SA:%{public}d, listenerSA:%{public}d", systemAbilityId, listenerSaId);
        std::lock_guard<std::mutex> autoLock(listenerLock_);
        if (localListenerMap_.count(systemAbilityId) == 0) {
            return true;
        }
        auto& listenerList = localListenerMap_[systemAbilityId];
        auto iter = std::find_if(listenerList.begin(), listenerList.end(),
            [listenerSaId](const std::pair<int32_t, ListenerState>& listener) {
            return listener.first == listenerSaId;
        });
        if (iter != listenerList.end()) {
            listenerList.erase(iter);
        }
        HILOGI(TAG, "SA:%{public}d, size:%{public}zu", systemAbilityId,
            listenerList.size());
        if (!listenerList.empty()) {
            return true;
        }
        localListenerMap_.erase(systemAbilityId);
    }

    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE(TAG, "failed to get samgrProxy");
        return false;
    }
    int32_t ret = samgrProxy->UnSubscribeSystemAbility(systemAbilityId, GetSystemAbilityStatusChange());
    if (ret) {
        HILOGE(TAG, "failed to unsubscribe SA:%{public}d, process name:%{public}s",
            systemAbilityId, Str16ToStr8(procName_).c_str());
        return false;
    }
    return true;
}

void LocalAbilityManager::NotifyAbilityListener(int32_t systemAbilityId, int32_t listenerSaId,
    const std::string& deviceId, int32_t code)
{
    LOGI("NotifyListener SA:%{public}d,listenerSA:%{public}d,code:%{public}d", systemAbilityId, listenerSaId, code);
    auto ability = GetAbility(listenerSaId);
    if (ability == nullptr) {
        HILOGE(TAG, "failed to get listener SA:%{public}d", listenerSaId);
        return;
    }

    switch (code) {
        case ISystemAbilityStatusChange::ON_ADD_SYSTEM_ABILITY: {
            HILOGD(TAG, "OnAddSystemAbility, SA:%{public}d", listenerSaId);
            ability->OnAddSystemAbility(systemAbilityId, deviceId);
            break;
        }
        case ISystemAbilityStatusChange::ON_REMOVE_SYSTEM_ABILITY: {
            HILOGD(TAG, "OnRemoveSystemAbility, SA:%{public}d", listenerSaId);
            ability->OnRemoveSystemAbility(systemAbilityId, deviceId);
            break;
        }
        default:
            break;
    }
}

void LocalAbilityManager::FindAndNotifyAbilityListeners(int32_t systemAbilityId,
    const std::string& deviceId, int32_t code)
{
    HILOGD(TAG, "SA:%{public}d, code:%{public}d", systemAbilityId, code);
    int64_t begin = GetTickCount();
    std::vector<int32_t> listenerSaIdVec;
    {
        std::lock_guard<std::mutex> autoLock(listenerLock_);
        auto iter = localListenerMap_.find(systemAbilityId);
        if (iter == localListenerMap_.end()) {
            HILOGW(TAG, "SA:%{public}d not found", systemAbilityId);
            return;
        }
        if (code == ISystemAbilityStatusChange::ON_ADD_SYSTEM_ABILITY) {
            for (auto& listener : iter->second) {
                if (listener.second == ListenerState::INIT) {
                    listenerSaIdVec.push_back(listener.first);
                    listener.second = ListenerState::NOTIFIED;
                } else {
                    HILOGW(TAG, "listener SA:%{public}d has been notified add", listener.first);
                }
            }
        } else if (code == ISystemAbilityStatusChange::ON_REMOVE_SYSTEM_ABILITY) {
            for (auto& listener : iter->second) {
                listenerSaIdVec.push_back(listener.first);
                if (listener.second == ListenerState::NOTIFIED) {
                    listener.second = ListenerState::INIT;
                }
            }
        }
    }
    for (auto listenerSaId : listenerSaIdVec) {
        NotifyAbilityListener(systemAbilityId, listenerSaId, deviceId, code);
    }
    LOGI("FindNotifyListeners SA:%{public}d,size:%{public}zu,code:%{public}d,spend:%{public}" PRId64 "ms",
        systemAbilityId, listenerSaIdVec.size(), code, GetTickCount() - begin);
}

bool LocalAbilityManager::OnStartAbility(int32_t systemAbilityId)
{
    HILOGD(TAG, "try to start SA:%{public}d", systemAbilityId);
    auto ability = GetAbility(systemAbilityId);
    if (ability == nullptr) {
        return false;
    }
    ability->Start();
    return true;
}

bool LocalAbilityManager::OnStopAbility(int32_t systemAbilityId)
{
    HILOGD(TAG, "try to stop SA:%{public}d", systemAbilityId);
    auto ability = GetAbility(systemAbilityId);
    if (ability == nullptr) {
        return false;
    }
    ability->Stop();
    return true;
}

SystemAbility* LocalAbilityManager::GetAbility(int32_t systemAbilityId)
{
    std::shared_lock<std::shared_mutex> readLock(localAbilityMapLock_);
    auto it = localAbilityMap_.find(systemAbilityId);
    if (it == localAbilityMap_.end()) {
        HILOGW(TAG, "SA:%{public}d not register", systemAbilityId);
        return nullptr;
    }

    return it->second;
}

bool LocalAbilityManager::GetRunningStatus(int32_t systemAbilityId)
{
    auto ability = GetAbility(systemAbilityId);
    if (ability == nullptr) {
        return false;
    }

    return ability->GetRunningStatus();
}

void LocalAbilityManager::StartOndemandSystemAbility(int32_t systemAbilityId)
{
    pthread_setname_np(pthread_self(), ONDEMAND_WORKER);
    LOGD("StartOndemandSa LoadSaLib SA:%{public}d library", systemAbilityId);
    int64_t begin = GetTickCount();
    bool isExist = profileParser_->LoadSaLib(systemAbilityId);
    LOGI("StartOndemandSa LoadSaLib SA:%{public}d,spend:%{public}" PRId64 "ms",
        systemAbilityId, (GetTickCount() - begin));
    if (isExist) {
        int32_t timeout = RETRY_TIMES_FOR_ONDEMAND;
        constexpr int32_t duration = std::chrono::microseconds(MILLISECONDS_WAITING_ONDEMAND_ONE_TIME).count();
        {
            auto it = localAbilityMap_.begin();
            {
                std::shared_lock<std::shared_mutex> readLock(localAbilityMapLock_);
                it = localAbilityMap_.find(systemAbilityId);
            }
            while (it == localAbilityMap_.end()) {
                HILOGI(TAG, "waiting for SA:%{public}d...", systemAbilityId);
                if (timeout > 0) {
                    usleep(duration);
                    std::shared_lock<std::shared_mutex> readLock(localAbilityMapLock_);
                    it = localAbilityMap_.find(systemAbilityId);
                } else {
                    HILOGE(TAG, "waiting for SA:%{public}d time out (1s)", systemAbilityId);
                    return;
                }
                timeout--;
            }
        }

        if (!OnStartAbility(systemAbilityId)) {
            HILOGE(TAG, "failed to start SA:%{public}d", systemAbilityId);
        }
    } else {
        HILOGW(TAG, "SA:%{public}d not found", systemAbilityId);
    }
}

bool LocalAbilityManager::StartAbility(int32_t systemAbilityId, const std::string& eventStr)
{
    LOGI("StartSa recv start SA:%{public}d req", systemAbilityId);
    nlohmann::json startReason = ParseUtil::StringToJsonObj(eventStr);
    SetStartReason(systemAbilityId, startReason);
    auto task = [this, systemAbilityId] {this->StartOndemandSystemAbility(systemAbilityId);};
    std::thread thread(task);
    thread.detach();
    return true;
}

void LocalAbilityManager::StopOndemandSystemAbility(int32_t systemAbilityId)
{
    pthread_setname_np(pthread_self(), ONDEMAND_WORKER);
    if (!OnStopAbility(systemAbilityId)) {
        HILOGE(TAG, "failed to stop SA:%{public}d", systemAbilityId);
    }
}

bool LocalAbilityManager::StopAbility(int32_t systemAbilityId, const std::string& eventStr)
{
    LOGI("StopSa recv stop SA:%{public}d req", systemAbilityId);
    nlohmann::json stopReason = ParseUtil::StringToJsonObj(eventStr);
    SetStopReason(systemAbilityId, stopReason);
    auto task = [this, systemAbilityId] {this->StopOndemandSystemAbility(systemAbilityId);};
    std::thread thread(task);
    thread.detach();
    return true;
}

bool LocalAbilityManager::ActiveAbility(int32_t systemAbilityId,
    const nlohmann::json& activeReason)
{
    LOGD("ActiveSa:%{public}d", systemAbilityId);
    auto ability = GetAbility(systemAbilityId);
    if (ability == nullptr) {
        return false;
    }
    SystemAbilityOnDemandReason onDemandActiveReason = JsonToOnDemandReason(activeReason);
    ability->Active(onDemandActiveReason);
    return true;
}

bool LocalAbilityManager::IdleAbility(int32_t systemAbilityId,
    const nlohmann::json& idleReason, int32_t& delayTime)
{
    HILOGD(TAG, "idle SA:%{public}d", systemAbilityId);
    auto ability = GetAbility(systemAbilityId);
    if (ability == nullptr) {
        return false;
    }
    SystemAbilityOnDemandReason onDemandIdleReason = JsonToOnDemandReason(idleReason);
    ability->Idle(onDemandIdleReason, delayTime);
    return true;
}

SystemAbilityOnDemandReason LocalAbilityManager::JsonToOnDemandReason(const nlohmann::json& reasonJson)
{
    SystemAbilityOnDemandReason onDemandStartReason;
    if (reasonJson.contains(EVENT_ID) && reasonJson[EVENT_ID].is_number()) {
        onDemandStartReason.SetId(reasonJson[EVENT_ID]);
    }
    if (reasonJson.contains(NAME) && reasonJson[NAME].is_string()) {
        onDemandStartReason.SetName(reasonJson[NAME]);
    }
    if (reasonJson.contains(VALUE) && reasonJson[VALUE].is_string()) {
        onDemandStartReason.SetValue(reasonJson[VALUE]);
    }
    if (reasonJson.contains(EXTRA_DATA_ID) && reasonJson[EXTRA_DATA_ID].is_number()) {
        onDemandStartReason.SetExtraDataId(reasonJson[EXTRA_DATA_ID]);
    }
    return onDemandStartReason;
}

bool LocalAbilityManager::InitializeOnDemandSaProfile(int32_t saId)
{
    int64_t begin = GetTickCount();
    LOGD("InitOnDemandSa LoadSaLib SA:%{public}d", saId);
#ifdef SAFWK_ENABLE_RUN_ON_DEMAND_QOS
    SetThreadPrio(OPEN_SO_PRIO);
#endif
    bool result = profileParser_->LoadSaLib(saId);
#ifdef SAFWK_ENABLE_RUN_ON_DEMAND_QOS
    SetThreadPrio(NORMAL_PRIO);
#endif
    LOGI("InitOnDemandSa LoadSaLib SA:%{public}d finished,spend:%{public}"
        PRId64 "ms", saId, (GetTickCount() - begin));
    if (!result) {
        LOGW("InitOnDemandSa LoadSaLib fail,SA:{public}%d", saId);
        return false;
    }
    SaProfile saProfile;
    bool ret = profileParser_->GetProfile(saId, saProfile);
    if (ret) {
        return InitializeSaProfilesInnerLocked(saProfile);
    }
    return false;
}

bool LocalAbilityManager::InitializeSaProfilesInnerLocked(const SaProfile& saProfile)
{
    std::unique_lock<std::shared_mutex> readLock(localAbilityMapLock_);
    auto iterProfile = localAbilityMap_.find(saProfile.saId);
    if (iterProfile == localAbilityMap_.end()) {
        HILOGW(TAG, "SA:%{public}d not found", saProfile.saId);
        return false;
    }
    auto systemAbility = iterProfile->second;
    if (systemAbility == nullptr) {
        HILOGW(TAG, "SA:%{public}d is null", saProfile.saId);
        return false;
    }
    if (saProfile.bootPhase > OTHER_START) {
        HILOGW(TAG, "invalid boot phase: %{public}d", saProfile.bootPhase);
        return false;
    }
    auto& saList = abilityPhaseMap_[saProfile.bootPhase];
    saList.emplace_back(systemAbility);
    return true;
}

vector<int32_t> LocalAbilityManager::CheckDependencyStatus(const vector<int32_t>& dependSa)
{
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGW(TAG, "failed to get samgrProxy");
        return dependSa;
    }

    vector<int32_t> checkSaStatusResult;
    for (const auto& saId : dependSa) {
        if (CheckInputSysAbilityId(saId)) {
            sptr<IRemoteObject> saObject = samgrProxy->CheckSystemAbility(saId);
            if (saObject == nullptr) {
                checkSaStatusResult.emplace_back(saId);
            }
        } else {
            HILOGW(TAG, "dependency's SA:%{public}d is invalid", saId);
        }
    }
    return checkSaStatusResult;
}

void LocalAbilityManager::StartDependSaTask(SystemAbility* ability)
{
    if (ability == nullptr) {
        HILOGE(TAG, "ability is null");
        return;
    }
    int64_t start = GetTickCount();
    int64_t dependTimeout = ability->GetDependTimeout();
    size_t lastSize = CheckDependencyStatus(ability->GetDependSa()).size();
    HILOGI(TAG, "SA:%{public}d's depend timeout:%{public}" PRId64 " ms,depend size:%{public}zu",
        ability->GetSystemAbilitId(), dependTimeout, lastSize);
    while (lastSize > 0) {
        int64_t end = GetTickCount();
        int64_t duration = ((end >= start) ? (end - start) : (INT64_MAX - end + start));
        if (duration < dependTimeout) {
            usleep(CHECK_DEPENDENT_SA_PERIOD);
        } else {
            break;
        }
        vector<int32_t> temp = CheckDependencyStatus(ability->GetDependSa());
        size_t curSize = temp.size();
        if (curSize != lastSize) {
            HILOGI(TAG, "SA:%{public}d's depend left:%{public}zu", ability->GetSystemAbilitId(), curSize);
        }
        lastSize = curSize;
    }
    vector<int32_t> unpreparedDeps = CheckDependencyStatus(ability->GetDependSa());
    if (unpreparedDeps.empty()) {
        HILOGI(TAG, "SA:%{public}d's depend all start", ability->GetSystemAbilitId());
        ability->Start();
    } else {
        for (const auto& unpreparedDep : unpreparedDeps) {
            HILOGI(TAG, "%{public}d's dependency:%{public}d not started in %{public}d ms",
                ability->GetSystemAbilitId(), unpreparedDep, ability->GetDependTimeout());
        }
    }
}

void LocalAbilityManager::StartSystemAbilityTask(SystemAbility* ability)
{
    if (ability != nullptr) {
        SamgrXCollie samgrXCollie("StartSaTimeout_" + ToString(ability->GetSystemAbilitId()), MAX_STARTSA_TIMEOUT);
        HILOGD(TAG, "StartSystemAbility is called for SA:%{public}d", ability->GetSystemAbilitId());
        if (ability->GetDependSa().empty()) {
            ability->Start();
        } else {
            StartDependSaTask(ability);
        }
        KHILOGI(TAG, "%{public}s SA:%{public}d init finished, %{public}" PRId64 " ms",
            Str16ToStr8(procName_).c_str(), ability->GetSystemAbilitId(), (GetTickCount() - startBegin_));
    }

    std::lock_guard<std::mutex> lock(startPhaseLock_);
    if (startTaskNum_ > 0) {
        --startTaskNum_;
    }
    startPhaseCV_.notify_one();
}

void LocalAbilityManager::RegisterOnDemandSystemAbility(int32_t saId)
{
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGI(TAG, "failed to get samgrProxy");
        return;
    }

    auto& saProfileList = profileParser_->GetAllSaProfiles();
    for (const auto& saProfile : saProfileList) {
        if (NeedRegisterOnDemand(saProfile, saId)) {
            HILOGD(TAG, "register ondemand SA:%{public}d to samgr", saProfile.saId);
            int32_t ret = samgrProxy->AddOnDemandSystemAbilityInfo(saProfile.saId, procName_);
            if (ret != ERR_OK) {
                HILOGI(TAG, "failed to add ability info for on-demand SA:%{public}d", saProfile.saId);
            }
        }
    }
}

// on default load, not-run-on-create SA will register to OnDemandSystemAbility
// on demand load, other SA will register to OnDemandSystemAbility, although some are runOnCreate
bool LocalAbilityManager::NeedRegisterOnDemand(const SaProfile& saProfile, int32_t saId)
{
    return (saId == DEFAULT_SAID && !saProfile.runOnCreate) ||
        (saId != DEFAULT_SAID && saProfile.saId != saId);
}

void LocalAbilityManager::StartPhaseTasks(const std::list<SystemAbility*>& systemAbilityList)
{
    if (systemAbilityList.empty()) {
        return;
    }

    for (auto systemAbility : systemAbilityList) {
        if (systemAbility != nullptr) {
            HILOGD(TAG, "add phase task for SA:%{public}d", systemAbility->GetSystemAbilitId());
            std::lock_guard<std::mutex> autoLock(startPhaseLock_);
            ++startTaskNum_;
            auto task = [this, systemAbility] {this->StartSystemAbilityTask(systemAbility);};
            initPool_->AddTask(task);
        }
    }
}

void LocalAbilityManager::WaitForTasks()
{
    int64_t begin = GetTickCount();
    HILOGD(TAG, "start waiting for all tasks!");
    std::unique_lock<std::mutex> lck(startPhaseLock_);
    if (!startPhaseCV_.wait_for(lck, std::chrono::seconds(MAX_SA_STARTUP_TIME),
        [this] () { return startTaskNum_ == 0; })) {
        HILOGW(TAG, "start timeout!");
    }
    startTaskNum_ = 0;
    int64_t end = GetTickCount();
    LOGI("start tasks proc:%{public}s end,spend %{public}" PRId64 "ms",
        Str16ToStr8(procName_).c_str(), (end - begin));
}

void LocalAbilityManager::FindAndStartPhaseTasks(int32_t saId)
{
    if (saId == DEFAULT_SAID) {
        for (uint32_t bootPhase = BOOT_START; bootPhase <= OTHER_START; ++bootPhase) {
            auto iter = abilityPhaseMap_.find(bootPhase);
            if (iter != abilityPhaseMap_.end()) {
                StartPhaseTasks(iter->second);
                InitializeRunOnCreateSaProfiles(bootPhase + 1);
                WaitForTasks();
            } else {
                InitializeRunOnCreateSaProfiles(bootPhase + 1);
            }
        }
    } else {
        for (uint32_t bootPhase = BOOT_START; bootPhase <= OTHER_START; ++bootPhase) {
            auto iter = abilityPhaseMap_.find(bootPhase);
            if (iter != abilityPhaseMap_.end()) {
                StartPhaseTasks(iter->second);
                WaitForTasks();
            }
        }
    }
}

bool LocalAbilityManager::InitializeRunOnCreateSaProfiles(uint32_t bootPhase)
{
    if (bootPhase > OTHER_START) {
        return false;
    }
    int64_t begin = GetTickCount();
    LOGD("ROC_InitProfiles load phase %{public}d libraries", bootPhase);
    profileParser_->OpenSo(bootPhase);
    LOGI("ROC_InitProfiles proc:%{public}s phase:%{public}d end, spend:%{public}" PRId64 "ms",
        Str16ToStr8(procName_).c_str(), bootPhase, (GetTickCount() - begin));
    auto& saProfileList = profileParser_->GetAllSaProfiles();
    if (saProfileList.empty()) {
        HILOGW(TAG, "sa profile is empty");
        return false;
    }
    for (const auto& saProfile : saProfileList) {
        if (saProfile.bootPhase != bootPhase) {
            continue;
        }
        if (!InitializeSaProfilesInnerLocked(saProfile)) {
            HILOGW(TAG, "SA:%{public}d init fail", saProfile.saId);
            continue;
        }
    }
    return true;
}

bool LocalAbilityManager::Run(int32_t saId)
{
    HILOGD(TAG, "local ability manager is running...");
    bool addResult = AddLocalAbilityManager();
    if (!addResult) {
        HILOGE(TAG, "failed to add local abilitymanager");
        return false;
    }
    LOGD("Run succ to add proc name:%{public}s", Str16ToStr8(procName_).c_str());
    uint32_t concurrentThreads = std::thread::hardware_concurrency();
    LOGI("Run curThread is %{public}d,proc:%{public}s,SA:%{public}d",
        concurrentThreads, Str16ToStr8(procName_).c_str(), saId);
    initPool_->Start(concurrentThreads);
    initPool_->SetMaxTaskNum(MAX_TASK_NUMBER);

    RegisterOnDemandSystemAbility(saId);
    FindAndStartPhaseTasks(saId);
    initPool_->Stop();
    return true;
}

bool LocalAbilityManager::AddLocalAbilityManager()
{
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        HILOGE(TAG, "failed to get samgrProxy");
        return false;
    }

    if (localAbilityManager_ == nullptr) {
        localAbilityManager_ = this;
    }
    int32_t ret = samgrProxy->AddSystemProcess(procName_, localAbilityManager_);
    return ret == ERR_OK;
}

void LocalAbilityManager::SetStartReason(int32_t saId, const nlohmann::json& event)
{
    std::lock_guard<std::mutex> autoLock(ReasonLock_);
    saIdToStartReason_[saId] = event;
}

void LocalAbilityManager::SetStopReason(int32_t saId, const nlohmann::json& event)
{
    std::lock_guard<std::mutex> autoLock(ReasonLock_);
    saIdToStopReason_[saId] = event;
}

nlohmann::json LocalAbilityManager::GetStartReason(int32_t saId)
{
    std::lock_guard<std::mutex> autoLock(ReasonLock_);
    return saIdToStartReason_[saId];
}

nlohmann::json LocalAbilityManager::GetStopReason(int32_t saId)
{
    std::lock_guard<std::mutex> autoLock(ReasonLock_);
    return saIdToStopReason_[saId];
}

sptr<ISystemAbilityStatusChange> LocalAbilityManager::GetSystemAbilityStatusChange()
{
    std::lock_guard<std::mutex> autoLock(listenerLock_);
    if (statusChangeListener_ == nullptr) {
        statusChangeListener_ = new SystemAbilityListener();
    }
    return statusChangeListener_;
}

void LocalAbilityManager::SystemAbilityListener::OnAddSystemAbility(int32_t systemAbilityId,
    const std::string& deviceId)
{
    HILOGD(TAG, "SA:%{public}d added", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        HILOGW(TAG, "SA:%{public}d is invalid!", systemAbilityId);
        return;
    }

    GetInstance().FindAndNotifyAbilityListeners(systemAbilityId, deviceId,
        ISystemAbilityStatusChange::ON_ADD_SYSTEM_ABILITY);
}

void LocalAbilityManager::SystemAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId,
    const std::string& deviceId)
{
    HILOGD(TAG, "SA:%{public}d removed", systemAbilityId);
    if (!CheckInputSysAbilityId(systemAbilityId)) {
        HILOGW(TAG, "SA:%{public}d is invalid!", systemAbilityId);
        return;
    }

    GetInstance().FindAndNotifyAbilityListeners(systemAbilityId, deviceId,
        ISystemAbilityStatusChange::ON_REMOVE_SYSTEM_ABILITY);
}

bool LocalAbilityManager::SendStrategyToSA(int32_t type, int32_t systemAbilityId, int32_t level, std::string& action)
{
    HILOGD(TAG, "SendStrategyTo SA:%{public}d", systemAbilityId);
    auto ability = GetAbility(systemAbilityId);
    if (ability == nullptr) {
        HILOGW(TAG, "failed to get SA:%{public}d", systemAbilityId);
        return false;
    }
    ability->OnDeviceLevelChanged(type, level, action);
    return true;
}

bool LocalAbilityManager::IpcStatCmdProc(int32_t fd, int32_t cmd)
{
    bool ret = false;
    std::string result;

    HILOGI(TAG, "IpcStatCmdProc:fd=%{public}d cmd=%{public}d request", fd, cmd);
    if (cmd < IPC_STAT_CMD_START || cmd >= IPC_STAT_CMD_MAX) {
        HILOGW(TAG, "para invalid, fd=%{public}d cmd=%{public}d", fd, cmd);
        return false;
    }

    switch (cmd) {
        case IPC_STAT_CMD_START: {
            ret = LocalAbilityManagerDumper::StartIpcStatistics(result);
            break;
        }
        case IPC_STAT_CMD_STOP: {
            ret = LocalAbilityManagerDumper::StopIpcStatistics(result);
            break;
        }
        case IPC_STAT_CMD_GET: {
            ret = LocalAbilityManagerDumper::GetIpcStatistics(result);
            break;
        }
        default:
            return false;
    }

    if (!SaveStringToFd(fd, result)) {
        HILOGW(TAG, "save to fd failed");
        return false;
    }
    return ret;
}

bool LocalAbilityManager::FfrtStatCmdProc(int32_t fd, int32_t cmd)
{
    HILOGI(TAG, "FfrtStatCmdProc:fd=%{public}d cmd=%{public}d request", fd, cmd);
    if (cmd < FFRT_STAT_CMD_START || cmd >= FFRT_STAT_CMD_MAX) {
        HILOGW(TAG, "para invalid, fd=%{public}d cmd=%{public}d", fd, cmd);
        return false;
    }
    std::string result;
    auto ret = LocalAbilityManagerDumper::CollectFfrtStatistics(cmd, result);
    if (!SaveStringToFd(fd, result)) {
        HILOGW(TAG, "save to fd failed");
        return false;
    }
    return ret;
}

typedef void (*PGetSdkName)(uint32_t cmd, char *buf, uint32_t len);

bool LocalAbilityManager::FfrtDumperProc(std::string& ffrtDumperInfo)
{
    HILOGI(TAG, "FfrtDumperPorc request");
    PGetSdkName pFFrtDumpInfo = (PGetSdkName)dlsym(RTLD_DEFAULT, "ffrt_dump");
    char* pszErr = dlerror();
    if (pszErr != NULL) {
        HILOGE(TAG, "dlsym err info: %{public}s", pszErr);
    }
    if (pFFrtDumpInfo == NULL) {
        HILOGE(TAG, "dlsym failed");
        ffrtDumperInfo.append("process " + std::to_string(getpid()) + " did not load ffrt\n");
        return false;
    }
    char* buffer = new char[FFRT_BUFFER_SIZE + 1]();
    buffer[FFRT_BUFFER_SIZE] = 0;
    (*pFFrtDumpInfo)(FFRT_DUMP_INFO_ALL, buffer, FFRT_BUFFER_SIZE);
    if (strlen(buffer) == 0) {
        HILOGE(TAG, "get samgr FfrtDumperInfo failed");
        delete[] buffer;
        return false;
    }
    ffrtDumperInfo += buffer;
    delete[] buffer;
    return true;
}

int32_t LocalAbilityManager::SystemAbilityExtProc(const std::string& extension, int32_t said,
    SystemAbilityExtensionPara* callback, bool isAsync)
{
    (void)isAsync;
    if (callback == nullptr) {
        return INVALID_DATA;
    }

    HILOGD(TAG, "SystemAbilityExtProc Extension %{public}s SA:%{public}d", extension.c_str(), said);
    auto ability = GetAbility(said);
    if (ability == nullptr) {
        return INVALID_DATA;
    }
    return ability->OnExtension(extension, *callback->data_, *callback->reply_);
}

bool LocalAbilityManager::IsResident()
{
    std::shared_lock<std::shared_mutex> readLock(localAbilityMapLock_);
    for (const auto& it : localAbilityMap_) {
        if ((it.second != nullptr) && (it.second->IsRunOnCreate())) {
            return true;
        }
    }
    return false;
}

bool LocalAbilityManager::NoNeedCheckUnused(int32_t saId)
{
    std::set<int32_t> saIdWhiteList = {};
    return saIdWhiteList.find(saId) != saIdWhiteList.end();
}

bool LocalAbilityManager::IsConfigUnused()
{
    std::shared_lock<std::shared_mutex> readLock(unusedCfgMapLock_);
    HILOGI(TAG, "unusedCfgMap_ size:%{public}zu", unusedCfgMap_.size());
    return !unusedCfgMap_.empty();
}

void LocalAbilityManager::LimitUnusedTimeout(int32_t saId, int32_t timeout)
{
    int64_t millisecTimeout = static_cast<int64_t>(timeout * TIME_S_TO_MS);
    if (millisecTimeout < ONDEMAND_SA_UNUSED_TIMEOUT_LOWLIMIT) {
        unusedCfgMap_[saId] = ONDEMAND_SA_UNUSED_TIMEOUT_LOWLIMIT;
    } else if (millisecTimeout > ONDEMAND_SA_UNUSED_TIMEOUT_UPLIMIT) {
        unusedCfgMap_[saId] = ONDEMAND_SA_UNUSED_TIMEOUT_UPLIMIT;
    } else {
        unusedCfgMap_[saId] = millisecTimeout;
    }
}

void LocalAbilityManager::InitUnusedCfg()
{
    auto saProfileList = profileParser_->GetAllSaProfiles();
    std::shared_lock<std::shared_mutex> writeLock(unusedCfgMapLock_);
    for (const auto& saProfile : saProfileList) {
        if (!saProfile.runOnCreate && saProfile.stopOnDemand.unusedTimeout != -1) {
            LimitUnusedTimeout(saProfile.saId, saProfile.stopOnDemand.unusedTimeout);
        }
    }
}

bool LocalAbilityManager::GetSaLastRequestTime(int32_t saId, uint64_t& lastRequestTime)
{
    auto ability = GetAbility(saId);
    if (ability == nullptr) {
        return false;
    }
    sptr<IRemoteObject> object = ability->GetAbilityRemoteObject();
    if (object == nullptr) {
        return false;
    }
    sptr<IPCObjectStub> saStub = reinterpret_cast<IPCObjectStub*>(object.GetRefPtr());
    if (saStub == nullptr) {
        return false;
    }
    lastRequestTime = saStub->GetLastRequestTime();
    return true;
}

void LocalAbilityManager::IdentifyUnusedResident()
{
    auto cur = std::chrono::steady_clock::now();
    uint64_t currTime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        cur.time_since_epoch()).count());

    std::shared_lock<std::shared_mutex> readLock(localAbilityMapLock_);
    for (const auto& it : localAbilityMap_) {
        int32_t saId = it.first;
        uint64_t lastRequestTime = 0;
        bool ret = GetSaLastRequestTime(saId, lastRequestTime);
        if ((ret != true) || (currTime <= lastRequestTime)) {
            continue;
        }
        uint64_t idleTime = currTime - lastRequestTime;
        uint64_t threshold = static_cast<uint64_t>(RESIDENT_SA_UNUSED_TIMEOUT);
        if (idleTime > threshold) {
            char reason[128] = {0};
            errno_t res = snprintf_s(reason, sizeof(reason), sizeof(reason) - 1,
                "saId:%d REASON:long time unused: %" PRIu64, saId, idleTime);
            if (res < 0) {
                continue;
            }
            ReportSAIdle(reason);
        }
    }
}

void LocalAbilityManager::IdentifyUnusedOndemand()
{
    auto cur = std::chrono::steady_clock::now();
    uint64_t currTime = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        cur.time_since_epoch()).count());

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        HILOGE(TAG, "failed to get samgrProxy");
        return;
    }

    std::shared_lock<std::shared_mutex> readLock(unusedCfgMapLock_);
    for (const auto& it : unusedCfgMap_) {
        int32_t saId = it.first;
        uint64_t lastRequestTime;
        bool ret = GetSaLastRequestTime(saId, lastRequestTime);
        if ((ret != true) || (currTime <= lastRequestTime)) {
            continue;
        }
        uint64_t threshold = it.second;
        uint64_t idleTime = currTime - lastRequestTime;
        HILOGD(TAG, "ondemand SA:%{public}d, idleTime:%{public}" PRIu64 ", longtime-unused threshold:%{public}" PRIu64,
            saId, idleTime, threshold);
        if (idleTime > threshold) {
            samgr->UnloadSystemAbility(saId);
            HILOGI(TAG, "ondemand SA:%{public}d, longtime:%{public}" PRIu64 "unused", saId, idleTime);
        }
    }
}

void IdentifyUnusedResidentSignalHandler(int signo)
{
    if (signo == IDLE_SA_REPORT_SIGNUM) {
        LocalAbilityManager::GetInstance().IdentifyUnusedResident();
    }
}

void LocalAbilityManager::StartResidentTimer()
{
    struct sigaction sa;
    struct sigevent sev;
    struct itimerspec ts;

    if (sigaction(IDLE_SA_REPORT_SIGNUM, nullptr, &sa) == -1) {
        HILOGE(TAG, "sigsaction get old handler failed");
        return;
    }

    if (sa.sa_handler != SIG_DFL) {
        HILOGE(TAG, "sig %{public}d has been used", IDLE_SA_REPORT_SIGNUM);
        return;
    }

    sa.sa_handler = &IdentifyUnusedResidentSignalHandler;
    sa.sa_flags = 0;
    sa.sa_flags |= SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(IDLE_SA_REPORT_SIGNUM, &sa, nullptr) == -1) {
        HILOGE(TAG, "sigsaction set new handler failed");
        return;
    }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = IDLE_SA_REPORT_SIGNUM;
    sev.sigev_value.sival_ptr = &residentTimer_;
    if (timer_create(CLOCK_REALTIME, &sev, &residentTimer_) != 0) {
        HILOGE(TAG, "timer_create failed");
        return;
    }

    ts.it_value.tv_sec = UNUSED_RESIDENT_TIMER_INTERVAL_SECONDS;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = UNUSED_RESIDENT_TIMER_INTERVAL_SECONDS;
    ts.it_interval.tv_nsec = 0;
    if (timer_settime(residentTimer_, 0, &ts, nullptr) != 0) {
        HILOGE(TAG, "timer_settime failed");
        return;
    }
}

void LocalAbilityManager::StartOnDemandTimer()
{
    int32_t timerInterval = 0;
    std::function<void()> timerCallback;
    InitUnusedCfg();
    if (IsConfigUnused()) {
        timerInterval = UNUSED_ONDEMAND_TIMER_INTERVAL_MSECONDS;
        timerCallback = std::bind(&LocalAbilityManager::IdentifyUnusedOndemand, this);
        idleTimer_ = std::make_unique<Utils::Timer>("OS_IdleSaReport", -1);
        idleTimer_->Setup();
        ondemandTimer_ = idleTimer_->Register(timerCallback, timerInterval);
        HILOGI(TAG, "StartIdleTimer timerId:%{public}u, interval:%{public}d", ondemandTimer_, timerInterval);
    }
}

void LocalAbilityManager::StartTimedQuery()
{
    {
        std::shared_lock<std::shared_mutex> readLock(localAbilityMapLock_);
        for (const auto& it : localAbilityMap_) {
            if (NoNeedCheckUnused(it.first)) {
                HILOGI(TAG, "SA:%{public}d no need check unused", it.first);
                return;
            }
        }
    }

    if (IsResident()) {
        StartResidentTimer();
    } else {
        StartOnDemandTimer();
    }
}

void LocalAbilityManager::StopTimedQuery()
{
    if (residentTimer_ != nullptr) {
        timer_delete(residentTimer_);
        residentTimer_ = nullptr;
    }

    if (idleTimer_!= nullptr) {
        idleTimer_->Unregister(ondemandTimer_);
        idleTimer_->Shutdown();
        idleTimer_ = nullptr;
    }
}

int32_t LocalAbilityManager::ServiceControlCmd(int32_t fd, int32_t systemAbilityId,
    const std::vector<std::u16string>& args)
{
    auto ability = GetAbility(systemAbilityId);
    if (ability == nullptr) {
        HILOGE(TAG, "failed to get ability");
        return INVALID_DATA;
    }
    return ability->OnSvcCmd(fd, args);
}
}
