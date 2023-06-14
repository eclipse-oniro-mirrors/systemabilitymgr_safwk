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

#ifndef LOCAL_ABILITY_TASK_HANLDER_H
#define LOCAL_ABILITY_TASK_HANLDER_H

#include <shared_mutex>
#include "single_instance.h"
#include "thread_pool.h"

namespace OHOS {
class LocalAbilityTaskHandler {
    DECLARE_SINGLE_INSTANCE_BASE(LocalAbilityTaskHandler);

public:
    void PostTask(std::function<void()> task);
    void Init(bool isOndemand);

private:
    LocalAbilityTaskHandler() = default;
    ~LocalAbilityTaskHandler();

    std::unique_ptr<ThreadPool> ondemandPool_;
};
}
#endif
