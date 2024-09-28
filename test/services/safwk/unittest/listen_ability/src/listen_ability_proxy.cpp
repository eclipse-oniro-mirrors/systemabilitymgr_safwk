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

#include "listen_ability_proxy.h"
#include "safwk_log.h"

namespace OHOS {
int32_t ListenAbilityProxy::AddVolume(
    int32_t volume)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteInt32(volume);
    int32_t result;

    bool hitCache = ApiCacheManager::GetInstance().PreSendRequest(GetDescriptor(), ADD_VOLUME, data, reply);
    if (hitCache == true) {
        result = reply.ReadInt32();
        return result;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return ERR_INVALID_DATA;
    }
    remote->SendRequest(ADD_VOLUME, data, reply, option);
    ApiCacheManager::GetInstance().PostSendRequest(GetDescriptor(), ADD_VOLUME, data, reply);
    result = reply.ReadInt32();
    return result;
}

ErrCode ListenAbilityProxy::TestSaCallSa(int32_t input, double& output)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInt32(input)) {
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return ERR_INVALID_DATA;
    }

    int32_t result = remote->SendRequest(COMMAND_TEST_SA_CALL_SA, data, reply, option);
    if (FAILED(result)) {
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        return errCode;
    }
    output = reply.ReadDouble();

    return ERR_OK;
}

ErrCode ListenAbilityProxy::TestGetIpcSendRequestTimes(int32_t& times)
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

    int32_t result = remote->SendRequest(COMMAND_TEST_GET_IPC_TIMES, data, reply, option);
    if (FAILED(result)) {
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        return errCode;
    }
    times = reply.ReadInt32();

    return ERR_OK;
}

ErrCode ListenAbilityProxy::TestClearSa1493Proxy_()
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

    int32_t result = remote->SendRequest(COMMAND_TEST_ClEAR_SA_1493_PROXY, data, reply, option);
    if (FAILED(result)) {
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        return errCode;
    }
    return ERR_OK;
}

void ListenAbilityProxy::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    (void)remote;
    ApiCacheManager::GetInstance().ClearCache(GetDescriptor());
    return;
}

} // namespace OHOS