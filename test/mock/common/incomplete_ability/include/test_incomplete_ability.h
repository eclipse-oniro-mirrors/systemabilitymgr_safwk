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

#ifndef TEST_MOCK_COMMON_INCOMPLETE_SA_INCLUDE_INCOMPLETE_SERVICE_H
#define TEST_MOCK_COMMON_INCOMPLETE_SA_INCLUDE_INCOMPLETE_SERVICE_H

#include <string>

#include "test_incomplete_ability_stub.h"
#include "refbase.h"
#include "system_ability.h"

namespace OHOS {
class TestIncompleteAbility : public SystemAbility, public TestIncompleteAbilityStub {
    DECLARE_SYSTEM_ABILITY(TestIncompleteAbility);

public:
    TestIncompleteAbility(int32_t saId, bool runOnCreate);
    ~TestIncompleteAbility();

    int AddVolume(int volume) override;
protected:
    void OnStart() override;
    void OnStop() override;
};
}

#endif // TEST_MOCK_COMMON_INCOMPLETE_SA_INCLUDE_INCOMPLETE_SERVICE_H
