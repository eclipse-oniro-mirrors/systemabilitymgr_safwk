/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "test_audio_ability_stub.h"
#include "errors.h"

using namespace std;
using namespace OHOS::HiviewDFX;

namespace OHOS {
int32_t TestAudioAbilityStub::OnRemoteRequest(uint32_t code,
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    HiLog::Info(label_, "TestAudioAbilityStub::OnReceived, code = %{public}d, flags= %{public}d",
        code, option.GetFlags());

    switch (code) {
        case ADD_VOLUME: {
            return reply.WriteInt32(AddVolume(data.ReadInt32()));
        }

        case REDUCE_VOLUME: {
            return reply.WriteInt32(ReduceVolume(data.ReadInt32()));
        }

        case TEST_RPCINT32: {
            return reply.WriteInt32(TestRpcInt32(data.ReadInt32()));
        }

        case TEST_RPCUINT32: {
            return reply.WriteUint32(TestRpcUInt32(data.ReadUint32()));
        }

        case TEST_RPCINT64: {
            return reply.WriteInt64(TestRpcInt64(data.ReadInt64()));
        }

        case TEST_RPCUINT64: {
            return reply.WriteUint64(TestRpcUInt64(data.ReadUint64()));
        }

        case TEST_RPCFLOAT: {
            return reply.WriteFloat(TestRpcFloat(data.ReadFloat()));
        }

        case TEST_RPCDOUBLE: {
            return reply.WriteDouble(TestRpcDouble(data.ReadDouble()));
        }

        case TEST_RPCSTRING16: {
            u16string strTmp = TestRpcString16(data.ReadString16());
            return reply.WriteString16(strTmp.c_str());
        }

        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
}
}
