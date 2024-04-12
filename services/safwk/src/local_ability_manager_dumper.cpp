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
#include "local_ability_manager_dumper.h"

#include "vector"
#include "unistd.h"
#include "string_ex.h"
#include "ipc_payload_statistics.h"

namespace OHOS {
const std::string DUMP_SUCCESS = " success\n";
const std::string DUMP_FAIL = " fail\n";

bool LocalAbilityManagerDumper::StartIpcStatistics(std::string& result)
{
    result = std::string("StartIpcStatistics pid:") + std::to_string(getpid());
    bool ret = IPCPayloadStatistics::StartStatistics();
    result += ret ? DUMP_SUCCESS : DUMP_FAIL;
    return ret;
}

bool LocalAbilityManagerDumper::StopIpcStatistics(std::string& result)
{
    result = std::string("StopIpcStatistics pid:") + std::to_string(getpid());
    bool ret = IPCPayloadStatistics::StopStatistics();
    result += ret ? DUMP_SUCCESS : DUMP_FAIL;
    return ret;
}

bool LocalAbilityManagerDumper::GetIpcStatistics(std::string& result)
{
    result += "********************************GlobalStatisticsInfo********************************";
    result += "\nCurrentPid:";
    result += std::to_string(getpid());
    result += "\nTotalCount:";
    result += std::to_string(IPCPayloadStatistics::GetTotalCount());
    result += "\nTotalTimeCost:";
    result += std::to_string(IPCPayloadStatistics::GetTotalCost());
    std::vector<int32_t> pids;
    pids = IPCPayloadStatistics::GetPids();
    for (unsigned int i = 0; i < pids.size(); i++) {
        result += "\n--------------------------------ProcessStatisticsInfo-------------------------------";
        result += "\nCallingPid:";
        result += std::to_string(pids[i]);
        result += "\nCallingPidTotalCount:";
        result += std::to_string(IPCPayloadStatistics::GetCount(pids[i]));
        result += "\nCallingPidTotalTimeCost:";
        result += std::to_string(IPCPayloadStatistics::GetCost(pids[i]));
        std::vector<IPCInterfaceInfo> intfs;
        intfs = IPCPayloadStatistics::GetDescriptorCodes(pids[i]);
        for (unsigned int j = 0; j < intfs.size(); j++) {
            result += "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~InterfaceStatisticsInfo~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
            result += "\nDescriptorCode:";
            result += Str16ToStr8(intfs[j].desc) + std::string("_") + std::to_string(intfs[j].code);
            result += "\nDescriptorCodeCount:";
            result += std::to_string(
                IPCPayloadStatistics::GetDescriptorCodeCount(pids[i], intfs[j].desc, intfs[j].code));
            result += "\nDescriptorCodeTimeCost:";
            result += "\nTotal:";
            result += std::to_string(
                IPCPayloadStatistics::GetDescriptorCodeCost(pids[i], intfs[j].desc, intfs[j].code).totalCost);
            result += " | Max:";
            result += std::to_string(
                IPCPayloadStatistics::GetDescriptorCodeCost(pids[i], intfs[j].desc, intfs[j].code).maxCost);
            result += " | Min:";
            result += std::to_string(
                IPCPayloadStatistics::GetDescriptorCodeCost(pids[i], intfs[j].desc, intfs[j].code).minCost);
            result += " | Avg:";
            result += std::to_string(
                IPCPayloadStatistics::GetDescriptorCodeCost(pids[i], intfs[j].desc, intfs[j].code).averCost);
            result += "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
        }
        result += "\n------------------------------------------------------------------------------------";
    }
    result += "\n************************************************************************************\n";
    return true;
}
} // namespace OHOS