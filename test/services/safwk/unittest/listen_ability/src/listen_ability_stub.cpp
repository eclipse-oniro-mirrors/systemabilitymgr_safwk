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

#include "listen_ability_stub.h"

namespace OHOS {
int32_t ListenAbilityStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    if (GetDescriptor() != data.ReadInterfaceToken()) {
        return ERR_TRANSACTION_FAILED;
    }
    switch (code) {
        case ADD_VOLUME: {
            int32_t volume = data.ReadInt32();
            bool ret = reply.WriteInt32(AddVolume(volume));
            return (ret ? ERR_OK : ERR_FLATTEN_OBJECT);
        }
        case COMMAND_TEST_SA_CALL_SA: {
            return StubTestSaCallSa(data, reply);
        }
        case COMMAND_TEST_GET_IPC_TIMES: {
            int32_t times;
            ErrCode errCode = TestGetIpcSendRequestTimes(times);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteInt32(times)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case COMMAND_TEST_ClEAR_SA_1493_PROXY: {
            ErrCode errCode = TestClearSa1493Proxy_();
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            return ERR_NONE;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return ERR_TRANSACTION_FAILED;
}

int32_t ListenAbilityStub::StubTestSaCallSa(MessageParcel &data, MessageParcel &reply)
{
    int32_t number = data.ReadInt32();
    double retNumber;
    ErrCode errCode = TestSaCallSa(number, retNumber);
    if (!reply.WriteInt32(errCode)) {
        return ERR_INVALID_VALUE;
    }
    if (SUCCEEDED(errCode)) {
        if (!reply.WriteDouble(retNumber)) {
            return ERR_INVALID_DATA;
        }
    }
    return ERR_NONE;
}

} // namespace OHOS