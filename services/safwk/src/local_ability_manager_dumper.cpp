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
#include "ffrt_inner.h"
#include "safwk_log.h"

#include "vector"
#include "unistd.h"
#include "string_ex.h"
#include "ipc_payload_statistics.h"

namespace OHOS {
namespace {
constexpr const char* DUMP_SUCCESS = " success\n";
constexpr const char* DUMP_FAIL = " fail\n";
constexpr int32_t COLLECT_FFRT_METRIC_MAX_SIZE = 5000;
constexpr int32_t FFRT_STAT_SIZE = sizeof(ffrt_stat);
constexpr int32_t BUFFER_SIZE = FFRT_STAT_SIZE * COLLECT_FFRT_METRIC_MAX_SIZE;
constexpr int32_t DELAY_TIME = 60 * 1000;
}

std::shared_ptr<FFRTHandler> LocalAbilityManagerDumper::handler_ = nullptr;
char* LocalAbilityManagerDumper::ffrtMetricBuffer = nullptr;
bool LocalAbilityManagerDumper::collectEnable = false;
std::mutex LocalAbilityManagerDumper::ffrtMetricLock;

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

bool LocalAbilityManagerDumper::StartFfrtStatistics(std::string& result)
{
    if (collectEnable) {
        result.append("collect has been started\n");
        return false;
    }
    ClearFfrtStatisticsBufferLocked();
    ffrtMetricBuffer = new char[BUFFER_SIZE]();
    auto ret = ffrt_dump(ffrt_dump_cmd_t::DUMP_START_STAT, ffrtMetricBuffer, BUFFER_SIZE);
    if (ret != ERR_OK) {
        ClearFfrtStatisticsBufferLocked();
        result.append("collect start failed\n");
        return false;
    }
    collectEnable = true;
    result.append("collect start success\n");
    if (handler_ == nullptr) {
        handler_ = std::make_shared<FFRTHandler>("safwk_ffrtDumpHandler");
    }
    LOGI("StartFfrtStatistics PostTask delayTime:%{public}dms", DELAY_TIME);
    handler_->PostTask(ClearFfrtStatistics, "ClearFfrtStatistics", DELAY_TIME);
    return true;
}

bool LocalAbilityManagerDumper::StopFfrtStatistics(std::string& result)
{
    if (!collectEnable) {
        result.append("collect has not been started\n");
        return false;
    }
    collectEnable = false;
    auto ret = ffrt_dump(ffrt_dump_cmd_t::DUMP_STOP_STAT, ffrtMetricBuffer, BUFFER_SIZE);
    if (ret != ERR_OK) {
        ClearFfrtStatisticsBufferLocked();
        result.append("collect stop failed\n");
        return false;
    }
    result.append("collect stop success\n");
    return true;
}

bool LocalAbilityManagerDumper::GetFfrtStatistics(std::string& result)
{
    if (collectEnable) {
        result.append("collect has not been stopped\n");
        return false;
    }
    if (ffrtMetricBuffer == nullptr) {
        result.append("info not collected\n");
        return false;
    }
    FfrtStatisticsParser(result);
    ClearFfrtStatisticsBufferLocked();
    handler_ = nullptr;
    return true;
}

void LocalAbilityManagerDumper::FfrtStatisticsParser(std::string& result)
{
    ffrt_stat* currentStat = (ffrt_stat*)ffrtMetricBuffer;
    char* lastStat = ffrtMetricBuffer + BUFFER_SIZE;
    std::string taskInfo;
    uint64_t maxTime = 0;
    uint64_t minTime = std::numeric_limits<uint64_t>::max();
    uint64_t sumTime = 0;
    uint64_t avgTime = 0;
    int count = 0;
    while ((char*)currentStat < lastStat && std::strcmp(currentStat->taskName, "") != 0) {
        if (currentStat->startTime > currentStat->endTime) {
            currentStat = (ffrt_stat*)((char*)currentStat + FFRT_STAT_SIZE);
            continue;
        }
        auto duration = currentStat->endTime - currentStat->startTime;
        sumTime += duration;
        maxTime = std::max(maxTime, duration);
        minTime = std::min(minTime, duration);
        ++count;
        taskInfo.append(currentStat->taskName);
        taskInfo.append(" " + ToString(currentStat->startTime));
        taskInfo.append(" " + ToString(currentStat->endTime) + "\n");
        currentStat = (ffrt_stat*)((char*)currentStat + FFRT_STAT_SIZE);
    }
    if (count == 0) {
        minTime = 0;
    } else {
        avgTime = sumTime / count;
    }
    result.append("sumTime:" + ToString(sumTime) + " maxTime:" + ToString(maxTime));
    result.append(" minTime:" + ToString(minTime) + " avgTime:" + ToString(avgTime));
    result.append(" cntTime:" + ToString(count) + "\n");
    result.append("-------------------------------------------------------------------------------------------\n");
    result.append("taskName                                                        startTime(us)   endTime(us)\n");
    result.append("-------------------------------------------------------------------------------------------\n");
    result.append(taskInfo);
    result.append("-------------------------------------------------------------------------------------------\n");
}

void LocalAbilityManagerDumper::ClearFfrtStatisticsBufferLocked()
{
    if (ffrtMetricBuffer != nullptr) {
        delete[] ffrtMetricBuffer;
        ffrtMetricBuffer = nullptr;
        LOGI("ClearFfrtStatisticsBuffer success");
    }
    if (handler_ != nullptr) {
        handler_->RemoveTask("ClearFfrtStatistics");
    }
}

void LocalAbilityManagerDumper::ClearFfrtStatistics()
{
    LOGW("ClearFfrtStatistics start");
    std::lock_guard<std::mutex> autoLock(ffrtMetricLock);
    if (collectEnable) {
        auto ret = ffrt_dump(ffrt_dump_cmd_t::DUMP_STOP_STAT, ffrtMetricBuffer, BUFFER_SIZE);
        if (ret != ERR_OK) {
            LOGE("ClearFfrtStatistics stop ffrt_dump err:%{public}d", ret);
        }
        collectEnable = false;
    }
    ClearFfrtStatisticsBufferLocked();
}

bool LocalAbilityManagerDumper::CollectFfrtStatistics(int32_t cmd, std::string& result)
{
    std::lock_guard<std::mutex> autoLock(ffrtMetricLock);
    result.append("pid:" + ToString(getpid()) + " ");
    auto ret = false;
    switch (cmd) {
        case FFRT_STAT_CMD_START: {
            ret = StartFfrtStatistics(result);
            break;
        }
        case FFRT_STAT_CMD_STOP: {
            ret = StopFfrtStatistics(result);
            break;
        }
        case FFRT_STAT_CMD_GET: {
            ret = GetFfrtStatistics(result);
            break;
        }
        default:
            break;
    }
    return ret;
}
} // namespace OHOS