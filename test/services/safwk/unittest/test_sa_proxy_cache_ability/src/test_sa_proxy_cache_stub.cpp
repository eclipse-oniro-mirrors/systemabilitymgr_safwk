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

#include "test_sa_proxy_cache_stub.h"

namespace OHOS {
int32_t TestSaProxyCacheStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    if (GetDescriptor() != data.ReadInterfaceToken()) {
        return ERR_TRANSACTION_FAILED;
    }
    switch (code) {
        case COMMAND_GET_STRING_FUNC: {
            std::string in_str = Str16ToStr8(data.ReadString16());
            std::string ret_str;
            ErrCode errCode = GetStringFunc(in_str, ret_str);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteString16(Str8ToStr16(ret_str))) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case COMMAND_GET_DOUBLE_FUNC: {
            int32_t number = data.ReadInt32();
            double ret_number;
            ErrCode errCode = GetDoubleFunc(number, ret_number);
            if (!reply.WriteInt32(errCode)) {
                return ERR_INVALID_VALUE;
            }
            if (SUCCEEDED(errCode)) {
                if (!reply.WriteDouble(ret_number)) {
                    return ERR_INVALID_DATA;
                }
            }
            return ERR_NONE;
        }
        case COMMAND_GET_VECTOR_FUNC: {
            return StubGetVecFunc(data, reply);
        }
        case COMMAND_GET_SA_PID: {
            return StubGetSaPID(reply);
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    return ERR_TRANSACTION_FAILED;
}

int32_t TestSaProxyCacheStub::StubGetVecFunc(MessageParcel& data, MessageParcel& reply)
{
    std::vector<bool> in_vec;
    int32_t in_vecSize = data.ReadInt32();
    if (in_vecSize > VECTOR_MAX_SIZE) {
        return ERR_INVALID_DATA;
    }
    for (int32_t i = 0; i < in_vecSize; ++i) {
        bool value = data.ReadInt32() == 1 ? true : false;
        in_vec.push_back(value);
    }
    std::vector<int8_t> ret_vec;
    ErrCode errCode = GetVectorFunc(in_vec, ret_vec);
    if (!reply.WriteInt32(errCode)) {
        return ERR_INVALID_VALUE;
    }
    if (SUCCEEDED(errCode)) {
        if (ret_vec.size() > static_cast<size_t>(VECTOR_MAX_SIZE)) {
            return ERR_INVALID_DATA;
        }
        reply.WriteInt32(ret_vec.size());
        for (auto it = ret_vec.begin(); it != ret_vec.end(); ++it) {
            if (!reply.WriteInt32((*it))) {
                return ERR_INVALID_DATA;
            }
        }
    }
    return ERR_NONE;
}

int32_t TestSaProxyCacheStub::StubGetSaPID(MessageParcel& reply)
{
    int32_t pid;
    ErrCode errCode = GetSaPid(pid);
    if (!reply.WriteInt32(errCode)) {
        return ERR_INVALID_VALUE;
    }
    if (SUCCEEDED(errCode)) {
        if (!reply.WriteInt32(pid)) {
            return ERR_INVALID_DATA;
        }
    }
    return ERR_NONE;
}

} // namespace OHOS
