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

#ifndef SAFWK_TEST_SERVICES_SAFWK_UNITTEST_LISTEN_ABILITY_INCLUDE_LISTEN_ABILITY_H
#define SAFWK_TEST_SERVICES_SAFWK_UNITTEST_LISTEN_ABILITY_INCLUDE_LISTEN_ABILITY_H

#include <string>

#include "test_sa_proxy_cache_stub.h"
#include "system_ability.h"

namespace OHOS {
class TestSaProxyCache : public SystemAbility, public TestSaProxyCacheStub {
    DECLARE_SYSTEM_ABILITY(TestSaProxyCache);

public:
    TestSaProxyCache(int32_t systemAbilityId, bool runOnCreate);
    ~TestSaProxyCache();
    ErrCode GetStringFunc(
        const std::string& in_str,
        std::string& ret_str) override;

    ErrCode GetDoubleFunc(
        int32_t number,
        double& ret_number) override;

    ErrCode GetVectorFunc(
        const std::vector<bool>& in_vec,
        std::vector<int8_t>& ret_vec) override;
    uint32_t TestGetIpcSendRequestTimes() override;
    ErrCode GetSaPid(
        int32_t& pid) override;
protected:
    void OnStart() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
};
}
#endif /* SAFWK_TEST_SERVICES_SAFWK_UNITTEST_LISTEN_ABILITY_INCLUDE_LISTEN_ABILITY_H */