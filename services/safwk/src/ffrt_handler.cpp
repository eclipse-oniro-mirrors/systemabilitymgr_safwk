
/*
* Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ffrt_handler.h"

#include <limits>

#include "safwk_log.h"

namespace OHOS {
using namespace ffrt;
namespace {
    constexpr uint64_t CONVERSION_FACTOR = 1000; // ms to us
}

FFRTHandler::FFRTHandler(const std::string& name)
{
    queue_ = std::make_shared<queue>(name.c_str());
}

bool FFRTHandler::PostTask(std::function<void()> func, const std::string& name, uint64_t delayTime)
{
    if (delayTime > std::numeric_limits<uint64_t>::max() / CONVERSION_FACTOR) {
        LOGE("invalid delay time");
        return false;
    }
    std::unique_lock<std::shared_mutex> lock(mutex_);
    task_handle handler = queue_->submit_h(func, task_attr().delay(delayTime * CONVERSION_FACTOR));
    if (handler == nullptr) {
        LOGE("FFRTHandler post task failed");
        return false;
    }
    taskMap_[name] = std::move(handler);
    return true;
}

void FFRTHandler::RemoveTask(const std::string& name)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    auto item = taskMap_.find(name);
    if (item == taskMap_.end()) {
        LOGW("rm task %{public}s NF", name.c_str());
        return;
    }
    if (item->second != nullptr) {
        auto ret = queue_->cancel(item->second);
        if (ret != 0) {
            LOGE("cancel task failed, error code %{public}d", ret);
        }
    }
    taskMap_.erase(name);
}
} // namespace OHOS