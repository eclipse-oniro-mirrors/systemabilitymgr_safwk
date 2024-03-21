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

#ifndef TEST_MOCK_COMMON_TT_SA_INCLUDE_TT_PROXY_H
#define TEST_MOCK_COMMON_TT_SA_INCLUDE_TT_PROXY_H

#include <string>

#include "i_test_tt_ability.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"
#include "refbase.h"

namespace OHOS {
class ITestTtAbilityProxy : public IRemoteProxy<ITestTtAbility> {
public:
    explicit ITestTtAbilityProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<ITestTtAbility>(impl) {}
    ~ITestTtAbilityProxy() = default;

    int AddVolume(int volume) override;
private:
    static inline BrokerDelegator<ITestTtAbilityProxy> delegator_;
};
}

#endif // TEST_MOCK_COMMON_TT_SA_INCLUDE_TT_PROXY_H