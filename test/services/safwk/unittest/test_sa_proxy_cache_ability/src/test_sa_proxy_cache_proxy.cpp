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

#include "test_sa_proxy_cache_proxy.h"
#include "safwk_log.h"

namespace OHOS {
ErrCode TestSaProxyCacheProxy::GetStringFunc(
    const std::string& in_str,
    std::string& ret_str)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteString16(Str8ToStr16(in_str))) {
        return ERR_INVALID_DATA;
    }

    bool hitCache = ApiCacheManager::GetInstance().PreSendRequest(GetDescriptor(),
        COMMAND_GET_STRING_FUNC, data, reply);
    if (hitCache == true) {
        ErrCode errCode = reply.ReadInt32();
        if (FAILED(errCode)) {
            return errCode;
        }
        ret_str = Str16ToStr8(reply.ReadString16());
        return ERR_OK;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return ERR_INVALID_DATA;
    }

    TestSendRequestTimes++;
    int32_t result = remote->SendRequest(COMMAND_GET_STRING_FUNC, data, reply, option);
    if (FAILED(result)) {
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        return errCode;
    }

    ApiCacheManager::GetInstance().PostSendRequest(GetDescriptor(), COMMAND_GET_STRING_FUNC, data, reply);
    ret_str = Str16ToStr8(reply.ReadString16());
    return ERR_OK;
}

ErrCode TestSaProxyCacheProxy::GetDoubleFunc(
    int32_t number,
    double& ret_number)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInt32(number)) {
        return ERR_INVALID_DATA;
    }

    HILOGD(TAG, "testsaproxycache");
    bool hitCache = ApiCacheManager::GetInstance().PreSendRequest(GetDescriptor(),
        COMMAND_GET_DOUBLE_FUNC, data, reply);
    if (hitCache == true) {
        HILOGD(TAG, "hit");
        ErrCode errCode = reply.ReadInt32();
        if (FAILED(errCode)) {
            return errCode;
        }
        ret_number = reply.ReadDouble();
        return ERR_OK;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return ERR_INVALID_DATA;
    }

    TestSendRequestTimes++;
    int32_t result = remote->SendRequest(COMMAND_GET_DOUBLE_FUNC, data, reply, option);
    if (FAILED(result)) {
        HILOGD(TAG, "result err");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HILOGD(TAG, "errCode failed");
        return errCode;
    }

    ApiCacheManager::GetInstance().PostSendRequest(GetDescriptor(), COMMAND_GET_DOUBLE_FUNC, data, reply);
    ret_number = reply.ReadDouble();
        HILOGD(TAG, "ret_number %lf", ret_number);
    return ERR_OK;
}

ErrCode TestSaProxyCacheProxy::ProxyReadVector(MessageParcel& reply, std::vector<int8_t>& ret_vec)
{
    int32_t ret_vecSize = reply.ReadInt32();
    if (ret_vecSize > VECTOR_MAX_SIZE) {
        return ERR_INVALID_DATA;
    }

    for (int32_t i = 0; i < ret_vecSize; ++i) {
        int8_t value = (int8_t)reply.ReadInt32();
        ret_vec.push_back(value);
    }
    return ERR_OK;
}

ErrCode TestSaProxyCacheProxy::GetVectorFunc(const std::vector<bool>& in_vec, std::vector<int8_t>& ret_vec)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_INVALID_VALUE;
    }

    if (in_vec.size() > static_cast<size_t>(VECTOR_MAX_SIZE)) {
        return ERR_INVALID_DATA;
    }
    data.WriteInt32(in_vec.size());
    for (auto it = in_vec.begin(); it != in_vec.end(); ++it) {
        if (!data.WriteInt32((*it) ? 1 : 0)) {
            return ERR_INVALID_DATA;
        }
    }

    bool hitCache = ApiCacheManager::GetInstance().PreSendRequest(GetDescriptor(),
        COMMAND_GET_VECTOR_FUNC, data, reply);
    if (hitCache == true) {
        ErrCode errCode = reply.ReadInt32();
        if (FAILED(errCode)) {
            return errCode;
        }
        return ProxyReadVector(reply, ret_vec);
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return ERR_INVALID_DATA;
    }

    TestSendRequestTimes++;
    int32_t result = remote->SendRequest(COMMAND_GET_VECTOR_FUNC, data, reply, option);
    if (FAILED(result)) {
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        return errCode;
    }

    ApiCacheManager::GetInstance().PostSendRequest(GetDescriptor(), COMMAND_GET_VECTOR_FUNC, data, reply);
    return ProxyReadVector(reply, ret_vec);
}

uint32_t TestSaProxyCacheProxy::TestGetIpcSendRequestTimes()
{
    return TestSendRequestTimes;
}

ErrCode TestSaProxyCacheProxy::GetSaPid(
    int32_t& pid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return ERR_INVALID_DATA;
    }

    int32_t result = remote->SendRequest(COMMAND_GET_SA_PID, data, reply, option);
    if (FAILED(result)) {
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        return errCode;
    }

    pid = reply.ReadInt32();
    return ERR_OK;
}

} // namespace OHOS
