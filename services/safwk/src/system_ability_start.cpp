/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <vector>

#include "local_ability_manager.h"
#include "malloc.h"
#include "parse_util.h"
#include "safwk_log.h"
#include "securec.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "hisysevent_adapter.h"

using namespace OHOS;
using std::string;

namespace {
constexpr const char* PARAM_PREFIX_M = "-m";
constexpr const char* EVENT_TYPE = "eventId";
constexpr const char* EVENT_NAME = "name";
constexpr const char* EVENT_VALUE = "value";
constexpr const char* EVENT_EXTRA_DATA_ID = "extraDataId";
#ifdef CONFIG_USE_JEMALLOC_DFX_INTF
constexpr const char* MALLOPT_CONFIG_SEPARATOR = ":";
#endif

constexpr auto DEFAULT_JSON = "/system/usr/default.json";
constexpr int ID_INDEX = 1;
constexpr int NAME_INDEX = 2;
constexpr int VALUE_INDEX = 3;
constexpr int EXTRA_DATA_ID_INDEX = 4;
constexpr int PROFILE_INDEX = 1;
constexpr int EVENT_INDEX = 2;
constexpr int DEFAULT_SAID = -1;
constexpr int DEFAULT_LOAD = 1;
constexpr int ONDEMAND_LOAD = 2;
constexpr int ARGC_LIMIT = 2;
constexpr int ARGC_MAX_COUNT = 64;
constexpr int PARTEVENT_NUM = 5;
constexpr int MAX_LENGTH = 2000;
constexpr int MALLOPT_CONFIG_LEN = 2;
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
    int extraDataId = atoi(eventVec[EXTRA_DATA_ID_INDEX].c_str());
    eventMap[EVENT_EXTRA_DATA_ID] = extraDataId;
    HILOGD(TAG, "ParseArgv extraDataId :%{public}d!", extraDataId);
    return saId;
}

static bool CheckSaId(int32_t saId)
{
    return (saId >= FIRST_SYS_ABILITY_ID) && (saId <= LAST_SYS_ABILITY_ID);
}

static int DoStartSAProcess(int argc, char *argv[], int32_t saId)
{
    // Load default system abilities related shared libraries from specific format profile
    // when this process starts.
    string profilePath(DEFAULT_JSON);
    if (argc > DEFAULT_LOAD) {
        string filePath(argv[PROFILE_INDEX]);
        if (filePath.empty() || filePath.find(".json") == string::npos) {
            HILOGE(TAG, "profile file path is invalid!");
            return 0;
        }
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
        if (strcmp(PARAM_PREFIX_M, argv[i]) == 0) {
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

extern "C" int StartSA(int argc, char *argv[])
{
    if (argc < ARGC_LIMIT || argc > ARGC_MAX_COUNT) {
        HILOGE(TAG, "argc : %{public}d is invalid", argc);
        return -1;
    }
    if (argv == nullptr) {
        HILOGE(TAG, "argv is NULL!");
        return -1;
    }
    HILOGI(TAG, "enter SAFWK StartSA, proc:%{public}d", getpid());
    int ondemandLoad = ONDEMAND_LOAD;
    int eventIndex = EVENT_INDEX;
    InitMallopt(argc, argv, ondemandLoad, eventIndex);
    // Load ondemand system abilities related shared libraries from specific json-format profile
    // when this process starts.
    int32_t saId = DEFAULT_SAID;
    if (argc > ondemandLoad) {
        nlohmann::json eventMap;
        if (eventIndex >= argc) {
            ReportSaMainExit("sa services path config error");
            HILOGE(TAG, "sa services path config error!");
            return 0;
        }
        saId = ParseArgv(argv, eventMap, eventIndex);
        if (!CheckSaId(saId)) {
            ReportSaMainExit("saId is invalid");
            HILOGE(TAG, "saId is invalid!");
            return 0;
        }
        LocalAbilityManager::GetInstance().SetStartReason(saId, eventMap);
    }
    
    DoStartSAProcess(argc, argv, saId);
    return 0;
}
