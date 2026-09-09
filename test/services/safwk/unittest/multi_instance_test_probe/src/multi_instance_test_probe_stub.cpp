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

#include "multi_instance_test_probe_stub.h"

#include "errors.h"
#include "ipc_object_stub.h"
#include "ipc_types.h"
#include "message_option.h"
#include "message_parcel.h"

namespace OHOS {
int32_t MultiInstanceTestProbeStub::OnRemoteRequest(uint32_t code,
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    switch (code) {
        case GET_PID: {
            bool ret = reply.WriteInt32(GetProbePid());
            return ret ? ERR_OK : ERR_FLATTEN_OBJECT;
        }
        case TRIGGER_UNLOAD: {
            bool ret = reply.WriteInt32(TriggerUnload());
            return ret ? ERR_OK : ERR_FLATTEN_OBJECT;
        }
        case TRIGGER_UNLOAD_CANCEL: {
            bool ret = reply.WriteInt32(TriggerUnloadAndCancel());
            return ret ? ERR_OK : ERR_FLATTEN_OBJECT;
        }
        case TRIGGER_REMOVE: {
            bool ret = reply.WriteInt32(TriggerRemove());
            return ret ? ERR_OK : ERR_FLATTEN_OBJECT;
        }
        case TRIGGER_REPUBLISH: {
            bool ret = reply.WriteInt32(TriggerRepublish());
            return ret ? ERR_OK : ERR_FLATTEN_OBJECT;
        }
        case UPDATE_ON_DEMAND_POLICY: {
            int32_t policyType = data.ReadInt32();
            int32_t eventId = data.ReadInt32();
            std::string name = data.ReadString();
            std::string value = data.ReadString();
            bool persistence = data.ReadBool();
            bool ret = reply.WriteInt32(
                UpdateOnDemandPolicyForTest(policyType, eventId, name, value, persistence));
            return ret ? ERR_OK : ERR_FLATTEN_OBJECT;
        }
        case GET_ON_DEMAND_POLICY: {
            int32_t policyType = data.ReadInt32();
            std::string policy;
            int32_t result = GetOnDemandPolicyForTest(policyType, policy);
            bool ret = reply.WriteInt32(result) && reply.WriteString(policy);
            return ret ? ERR_OK : ERR_FLATTEN_OBJECT;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
}
