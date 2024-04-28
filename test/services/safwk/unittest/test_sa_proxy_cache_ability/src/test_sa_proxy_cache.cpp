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

#include "test_sa_proxy_cache.h"
#include <unistd.h>
#include "system_ability_definition.h"
#include "hilog/log.h"

using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0xD001800, "TestSaProxyCache"};
}

REGISTER_SYSTEM_ABILITY_BY_ID(TestSaProxyCache, DISTRIBUTED_SCHED_TEST_TT_ID, true);

TestSaProxyCache::TestSaProxyCache(
    int32_t systemAbilityId, bool runOnCreate) : SystemAbility(systemAbilityId, runOnCreate)
{
    HiLog::Info(LABEL, "TestSaProxyCache");
}

TestSaProxyCache::~TestSaProxyCache()
{
    HiLog::Info(LABEL, "~TestSaProxyCache()");
}

ErrCode TestSaProxyCache::GetStringFunc(const std::string& in_str, std::string& ret_str)
{
    std::string str = in_str;

    ret_str += str;
    reverse(str.begin(), str.end());
    ret_str += str;
    return ERR_OK;
}

ErrCode TestSaProxyCache::GetDoubleFunc(int32_t number, double& ret_number)
{
    const double pi = 3.14;
    ret_number = number * pi;
    return ERR_OK;
}

ErrCode TestSaProxyCache::GetVectorFunc(const std::vector<bool>& in_vec, std::vector<int8_t>& ret_vec)
{
    for (auto i:in_vec) {
        if (i == true) {
            ret_vec.push_back(1);
        } else {
            ret_vec.push_back(-1);
        }
    }
    return ERR_OK;
}

uint32_t TestSaProxyCache::TestGetIpcSendRequestTimes()
{
    return 0;
}

ErrCode TestSaProxyCache::GetSaPid(int32_t& pid)
{
    pid = getpid();
    return ERR_OK;
}

void TestSaProxyCache::OnStart()
{
    HiLog::Info(LABEL, "OnStart()");
    bool res = Publish(this);
    if (!res) {
        HiLog::Error(LABEL, "publish failed!");
        return;
    }
}

void TestSaProxyCache::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    HiLog::Info(LABEL, "OnAddSystemAbility systemAbilityId:%{public}d added!", systemAbilityId);
}

void TestSaProxyCache::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    HiLog::Info(LABEL, "OnRemoveSystemAbility systemAbilityId:%{public}d removed!", systemAbilityId);
}
}