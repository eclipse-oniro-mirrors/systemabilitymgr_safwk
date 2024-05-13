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

#ifndef SAFWK_TEST_SERVICES_SAFWK_UNITTEST_LISTEN_ABILITY_INCLUDE_LISTEN_ABILITY_PROXY_H
#define SAFWK_TEST_SERVICES_SAFWK_UNITTEST_LISTEN_ABILITY_INCLUDE_LISTEN_ABILITY_PROXY_H

#include "ilisten_ability.h"
#include <iremote_proxy.h>
#include "api_cache_manager.h"

namespace OHOS {
class ListenAbilityProxy : public IRemoteProxy<IListenAbility> {
public:
    explicit ListenAbilityProxy(
        const sptr<IRemoteObject>& remote)
        : IRemoteProxy<IListenAbility>(remote)
    {
        ApiCacheManager::GetInstance().AddCacheApi(GetDescriptor(), ADD_VOLUME, 0);
        if (remote) {
            if (!remote->IsProxyObject()) {
                return;
            }
            deathRecipient_ = new (std::nothrow) ListenDeathRecipient(*this);
            if (deathRecipient_ == nullptr) {
                return;
            }
            if (!remote->AddDeathRecipient(deathRecipient_)) {
                return;
            }
            remote_ = remote;
        }
    }

    virtual ~ListenAbilityProxy()
    {
        if (remote_ == nullptr) {
            return;
        }
        if (deathRecipient_ == nullptr) {
            return;
        }
        remote_->RemoveDeathRecipient(deathRecipient_);
        remote_ = nullptr;
        ApiCacheManager::GetInstance().DelCacheApi(GetDescriptor(), ADD_VOLUME);
    }

    ErrCode AddVolume(
        int32_t volume) override;

    ErrCode TestSaCallSa(
        int32_t input, double& output) override;

    ErrCode TestGetIpcSendRequestTimes(
        int32_t& times) override;

    ErrCode TestClearSa1493Proxy_() override;
private:
    class ListenDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit ListenDeathRecipient(ListenAbilityProxy &client) : client_(client) {}
        ~ListenDeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            client_.OnRemoteDied(remote);
        }
    private:
        ListenAbilityProxy &client_;
    };

    void OnRemoteDied(const wptr<IRemoteObject> &remoteObject);
    static constexpr int32_t ADD_VOLUME = MIN_TRANSACTION_ID + 0;
    static constexpr int32_t COMMAND_TEST_SA_CALL_SA = MIN_TRANSACTION_ID + 1;
    static constexpr int32_t COMMAND_TEST_GET_IPC_TIMES = MIN_TRANSACTION_ID + 2;
    static constexpr int32_t COMMAND_TEST_ClEAR_SA_1493_PROXY = MIN_TRANSACTION_ID + 3;
    static inline BrokerDelegator<ListenAbilityProxy> delegator_;
    sptr<IRemoteObject> remote_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
};
} // namespace OHOS
#endif // SAFWK_TEST_SERVICES_SAFWK_UNITTEST_LISTEN_ABILITY_INCLUDE_LISTEN_ABILITY_PROXY_H