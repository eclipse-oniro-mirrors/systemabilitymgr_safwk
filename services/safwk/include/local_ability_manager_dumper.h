/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef LOCAL_ABILITY_MANAGER_DUMPER_H
#define LOCAL_ABILITY_MANAGER_DUMPER_H

#include <string>
#include "if_local_ability_manager.h"
#include "ffrt_handler.h"

namespace OHOS {
class LocalAbilityManagerDumper {
public:
    LocalAbilityManagerDumper();
    ~LocalAbilityManagerDumper();

    static bool StartIpcStatistics(std::string& result);
    static bool StopIpcStatistics(std::string& result);
    static bool GetIpcStatistics(std::string& result);
    static bool CollectFfrtStatistics(int32_t cmd, std::string& result);
private:
    static bool StartFfrtStatistics(std::string& result);
    static bool StopFfrtStatistics(std::string& result);
    static bool GetFfrtStatistics(std::string& result);
    static void FfrtStatisticsParser(std::string& result);
    static void ClearFfrtStatisticsBufferLocked();
    static void ClearFfrtStatistics();
    static std::shared_ptr<FFRTHandler> handler_;
};
}

#endif