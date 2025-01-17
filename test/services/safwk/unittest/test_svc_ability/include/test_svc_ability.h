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

#ifndef SAFWK_TEST_SERVICES_SAFWK_UNITTEST_SVC_ABILITY_INCLUDE_TEST_SVC_ABILITY_H
#define SAFWK_TEST_SERVICES_SAFWK_UNITTEST_SVC_ABILITY_INCLUDE_TEST_SVC_ABILITY_H

#include <string>
#include <iremote_stub.h>
#include "itest_svc_ability.h"
#include "system_ability.h"
#include "file_ex.h"

namespace OHOS {
class TestSvcAbilityStub : public IRemoteStub<ITestSvcAbility> {

};

class TestSvcAbility : public SystemAbility, public TestSvcAbilityStub {
    DECLARE_SYSTEM_ABILITY(TestSvcAbility);

public:
    TestSvcAbility(int32_t systemAbilityId, bool runOnCreate);
    ~TestSvcAbility();
protected:
    void OnStart() override;
    int32_t OnSvcCmd(int32_t fd, const std::vector<std::u16string>& args) override;
};
}
#endif /* SAFWK_TEST_SERVICES_SAFWK_UNITTEST_SVC_ABILITY_INCLUDE_TEST_SVC_ABILITY_H */
