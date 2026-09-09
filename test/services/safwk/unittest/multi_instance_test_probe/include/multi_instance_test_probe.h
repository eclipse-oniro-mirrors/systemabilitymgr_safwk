/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef TEST_MOCK_COMMON_MULTI_INSTANCE_TEST_PROBE_INCLUDE_MULTI_INSTANCE_TEST_PROBE_H
#define TEST_MOCK_COMMON_MULTI_INSTANCE_TEST_PROBE_INCLUDE_MULTI_INSTANCE_TEST_PROBE_H

#include "multi_instance_test_probe_stub.h"
#include "system_ability.h"

namespace OHOS {
class MultiInstanceTestProbe : public SystemAbility, public MultiInstanceTestProbeStub {
    DECLARE_SYSTEM_ABILITY(MultiInstanceTestProbe);

public:
    MultiInstanceTestProbe(int32_t saId, bool runOnCreate);
    ~MultiInstanceTestProbe() override;

    int32_t GetProbePid() override;
    int32_t TriggerUnload() override;
    int32_t TriggerUnloadAndCancel() override;
    int32_t TriggerRemove() override;
    int32_t TriggerRepublish() override;
    int32_t UpdateOnDemandPolicyForTest(int32_t policyType, int32_t eventId,
        const std::string& name, const std::string& value, bool persistence) override;
    int32_t GetOnDemandPolicyForTest(int32_t policyType, std::string& policy) override;

protected:
    void OnStart() override;
    void OnStop() override;
    void OnDeviceLevelChanged(int32_t type, int32_t level, std::string& action) override;

private:
    int32_t ScheduleUnload(bool cancelUnload);
};

}

#endif // TEST_MOCK_COMMON_MULTI_INSTANCE_TEST_PROBE_INCLUDE_MULTI_INSTANCE_TEST_PROBE_H
