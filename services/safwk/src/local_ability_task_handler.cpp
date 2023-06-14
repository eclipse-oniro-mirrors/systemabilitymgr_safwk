/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "local_ability_task_handler.h"

namespace OHOS {
namespace {
const std::string ONDEMAND_POOL = "SaOndemand";
const int32_t MAX_TASK_NUMBER = 20;
}

IMPLEMENT_SINGLE_INSTANCE(LocalAbilityTaskHandler);

LocalAbilityTaskHandler::~LocalAbilityTaskHandler()
{
    if (ondemandPool_ != nullptr) {
        ondemandPool_->Stop();
    }
}

void LocalAbilityTaskHandler::Init(bool isOndemand)
{
    if (isOndemand) {
        ondemandPool_ = std::make_unique<ThreadPool>(ONDEMAND_POOL);
        ondemandPool_->Start(std::thread::hardware_concurrency());
        ondemandPool_->SetMaxTaskNum(MAX_TASK_NUMBER);
    }
}

void LocalAbilityTaskHandler::PostTask(std::function<void()> task)
{
    if (ondemandPool_ != nullptr) {
        ondemandPool_->AddTask(task);
    } else {
        std::thread thread(task);
        thread.detach();
    }
}
}