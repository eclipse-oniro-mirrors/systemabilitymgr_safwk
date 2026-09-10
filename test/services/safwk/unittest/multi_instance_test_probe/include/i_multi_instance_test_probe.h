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

#ifndef TEST_MOCK_COMMON_MULTI_INSTANCE_TEST_PROBE_INCLUDE_I_MULTI_INSTANCE_TEST_PROBE_H
#define TEST_MOCK_COMMON_MULTI_INSTANCE_TEST_PROBE_INCLUDE_I_MULTI_INSTANCE_TEST_PROBE_H

#include <string>

#include "iremote_broker.h"

namespace OHOS {
class IMultiInstanceTestProbe : public IRemoteBroker {
public:
    enum {
        GET_PID = 2,
        TRIGGER_UNLOAD = 3,
        TRIGGER_UNLOAD_CANCEL = 4,
        UPDATE_ON_DEMAND_POLICY = 5,
        GET_ON_DEMAND_POLICY = 6,
        TRIGGER_REMOVE = 7,
        TRIGGER_REPUBLISH = 8,
    };

    virtual int32_t GetProbePid() = 0;
    virtual int32_t TriggerUnload() = 0;
    virtual int32_t TriggerUnloadAndCancel() = 0;
    virtual int32_t TriggerRemove() = 0;
    virtual int32_t TriggerRepublish() = 0;
    virtual int32_t UpdateOnDemandPolicyForTest(int32_t policyType, int32_t eventId,
        const std::string& name, const std::string& value, bool persistence) = 0;
    virtual int32_t GetOnDemandPolicyForTest(int32_t policyType, std::string& policy) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.test.IMultiInstanceTestProbe");
};
}

#endif // TEST_MOCK_COMMON_MULTI_INSTANCE_TEST_PROBE_INCLUDE_I_MULTI_INSTANCE_TEST_PROBE_H
