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
#include <csignal>
#include <sys/prctl.h>
#include <vector>

#include "errors.h"
#include "local_ability_manager.h"
#include "malloc.h"
#include "parameter.h"
#include "parse_util.h"
#include "safwk_log.h"
#include "securec.h"
#include "string_ex.h"
#include "system_ability_definition.h"

using namespace OHOS;
using std::string;

namespace {
constexpr const char* PARAM_PREFIX_U = "-u";
constexpr const char* EVENT_TYPE = "eventId";
constexpr const char* EVENT_NAME = "name";
constexpr const char* EVENT_VALUE = "value";
constexpr const char* EVENT_EXTRA_DATA_ID = "extraDataId";
using ProcessNameSetFunc = std::function<void(const string&)>;

constexpr auto DEFAULT_JSON = "/system/usr/default.json";
// The pid name can be up to 16 bytes long, including the terminating null byte.
// So need to set the max length of pid name to 15 bytes.
constexpr size_t MAX_LEN_PID_NAME = 15;
constexpr int ID_INDEX = 1;
constexpr int NAME_INDEX = 2;
constexpr int VALUE_INDEX = 3;
constexpr int EXTRA_DATA_ID_INDEX = 4;
constexpr int PROFILE_INDEX = 1;
constexpr int EVENT_INDEX = 2;
constexpr int DEFAULT_SAID = -1;
constexpr int DEFAULT_LOAD = 1;
constexpr int ONDEMAND_LOAD = 2;
constexpr int PARTEVENT_NUM = 5;
constexpr int MAX_LENGTH = 2000;
constexpr int MALLOPT_CONFIG_LEN = 2;
}

static void StartMemoryHook(const string& processName)
{
    const int paramBufLen = 128;
    const char defaultValue[paramBufLen] = { 0 };
    const char targetPrefix[] = "startup:";
    const int targetPrefixLen = 8;
    char paramValue[paramBufLen] = { 0 };
    int retParam = GetParameter("libc.hook_mode", defaultValue, paramValue, sizeof(paramValue));
    if (retParam <= 0 || strncmp(paramValue, targetPrefix, targetPrefixLen) != 0) {
        return;
    }

    if (processName.find(paramValue + targetPrefixLen) == 0) {
        const int hookSignal = 36;
        HILOGI(TAG, "raise hook signal %{public}d to %{public}s", hookSignal, processName.c_str());
        raise(hookSignal);
    }
}

static void SetProcName(const string& filePath, const ProcessNameSetFunc& setProcessName)
{
    std::vector<string> strVector;
    SplitStr(filePath, "/", strVector);
    auto vectorSize = strVector.size();
    if (vectorSize > 0) {
        auto& fileName = strVector[vectorSize - 1];
        auto dotPos = fileName.find(".");
        if (dotPos == string::npos) {
            return;
        }
        string profileName = fileName.substr(0, dotPos);
        setProcessName(profileName);
        if (dotPos > MAX_LEN_PID_NAME) {
            dotPos = MAX_LEN_PID_NAME;
        }
        profileName = fileName.substr(0, dotPos);
        int32_t ret = prctl(PR_SET_NAME, profileName.c_str());
        if (ret != 0) {
            HILOGI(TAG, "call the system API prctl failed!");
        }
        StartMemoryHook(profileName);
    }
}

// check argv size with SAID_INDEX before using the function
static int32_t ParseArgv(char *argv[], nlohmann::json& eventMap, int eventIndex)
{
    string eventStr(argv[eventIndex]);
    HILOGI(TAG, "ParseArgv extraArgv eventStr:%{public}s!", eventStr.c_str());
    int32_t saId = DEFAULT_SAID;
    if (eventStr.size() > MAX_LENGTH) {
        return DEFAULT_SAID;
    }
    std::size_t pos = eventStr.find("#");
    std::vector<string> eventVec;
    while (pos != std::string::npos) {
        std::string eventPart = eventStr.substr(0, pos);
        eventVec.push_back(eventPart);
        eventStr = eventStr.substr(pos + 1, eventStr.size() - pos - 1);
        pos = eventStr.find("#");
    }
    if (eventVec.size() != PARTEVENT_NUM) {
        HILOGE(TAG, "eventVec size is not true");
        return DEFAULT_SAID;
    }
    if (!StrToInt(eventVec[0], saId)) {
        HILOGE(TAG, "eventVec[0] StrToInt said error");
        return DEFAULT_SAID;
    }
    eventMap[EVENT_TYPE] = atoi(eventVec[ID_INDEX].c_str());
    eventMap[EVENT_NAME] = eventVec[NAME_INDEX];
    eventMap[EVENT_VALUE] = eventVec[VALUE_INDEX];
    eventMap[EVENT_EXTRA_DATA_ID] = atoi(eventVec[EXTRA_DATA_ID_INDEX].c_str());
    HILOGD(TAG, "ParseArgv extraDataId :%{public}d!", atoi(eventVec[EXTRA_DATA_ID_INDEX].c_str()));
    return saId;
}

static bool CheckSaId(int32_t saId)
{
    return (saId >= FIRST_SYS_ABILITY_ID) && (saId <= LAST_SYS_ABILITY_ID);
}

static int DoStartSAProcess(int argc, char *argv[], int32_t saId)
{
    auto setProcessName = [argc, argv](const string& name) -> void {
        char *endCh = strchr(argv[argc - 1], 0);
        if (endCh == nullptr) {
            HILOGW(TAG, "argv is invalid");
            return;
        }
        uintptr_t start = reinterpret_cast<uintptr_t>(argv[0]);
        uintptr_t end = reinterpret_cast<uintptr_t>(endCh);
        uintptr_t argvSize = end - start;

        if (memset_s(argv[0], argvSize, 0, argvSize) != EOK) {
            HILOGW(TAG, "failed to clear argv:%{public}s", strerror(errno));
            return;
        }
        if (strcpy_s(argv[0], argvSize, name.c_str()) != EOK) {
            HILOGW(TAG, "failed to set process name:%{public}s", strerror(errno));
            return;
        }
        KHILOGI(TAG, "Set process name to %{public}s", argv[0]);
    };
    // Load default system abilities related shared libraries from specific format profile
    // when this process starts.
    string profilePath(DEFAULT_JSON);
    if (argc > DEFAULT_LOAD) {
        string filePath(argv[PROFILE_INDEX]);
        if (filePath.empty() || filePath.find(".json") == string::npos) {
            HILOGE(TAG, "profile file path is invalid!");
            return 0;
        }
        SetProcName(filePath, setProcessName);
        profilePath = std::move(filePath);
    }
    LocalAbilityManager::GetInstance().DoStartSAProcess(profilePath, saId);
    return 0;
}

static void InitMallopt(int argc, char *argv[], int& ondemandLoad, int& eventIndex)
{
#ifdef CONFIG_USE_JEMALLOC_DFX_INTF
    std::vector<std::string> malloptStrList;
#endif
    for (int i = 0; i < argc - 1; ++i) {
        if (PARAM_PREFIX_M.compare(argv[i]) == 0) {
#ifdef CONFIG_USE_JEMALLOC_DFX_INTF
            malloptStrList.emplace_back(argv[i + 1]);
#endif
            ondemandLoad += MALLOPT_CONFIG_LEN;
            eventIndex += MALLOPT_CONFIG_LEN;
        }
    }
#ifdef CONFIG_USE_JEMALLOC_DFX_INTF
    for (size_t i = 0; i < malloptStrList.size(); i++) {
        std::vector<std::string> malloptItem;
        SplitStr(malloptStrList[i], MALLOPT_CONFIG_SEPARATOR, malloptItem);
        if (malloptItem.size() != MALLOPT_CONFIG_LEN) {
            HILOGE(TAG, "mallopt config string : %{public}s is invalid", malloptStrList[i].c_str());
            continue;
        }
        int key = atoi(malloptItem[0].c_str());
        int value = atoi(malloptItem[1].c_str());

        int err = mallopt(key, value);
        if (err != 1) {
            HILOGE(TAG, "mallopt failed, malloptStr : %{public}s, result : %{public}d", malloptStrList[i].c_str(), err);
        }
    }
#endif
}

int main(int argc, char *argv[])
{
    HILOGI(TAG, "enter SAFWK main, proc:%{public}d", getpid());
    // find update list
    bool checkOnDemand = true;
    int ondemandLoad = ONDEMAND_LOAD;
    int eventIndex = EVENT_INDEX;
    InitMallopt(argc, argv, ondemandLoad, eventIndex);
    string updateList;
    for (int i = 0; i < argc - 1; ++i) {
        if (PARAM_PREFIX_U == nullptr) {
            if (i == EVENT_INDEX) {
                checkOnDemand = false;
            }
            updateList = argv[i + 1];
            break;
        }
    }

    if (!updateList.empty()) {
        LocalAbilityManager::GetInstance().SetUpdateList(updateList);
    }

    // Load ondemand system abilities related shared libraries from specific json-format profile
    // when this process starts.
    int32_t saId = DEFAULT_SAID;
    if (checkOnDemand && argc > ondemandLoad) {
        nlohmann::json eventMap;
        if (eventIndex >= argc) {
            HILOGE(TAG, "sa services path config error!");
            return 0;
        }
        saId = ParseArgv(argv, eventMap, eventIndex);
        if (!CheckSaId(saId)) {
            HILOGE(TAG, "saId is invalid!");
            return 0;
        }
        LocalAbilityManager::GetInstance().SetStartReason(saId, eventMap);
    }
    
    DoStartSAProcess(argc, argv, saId);
    return 0;
}
