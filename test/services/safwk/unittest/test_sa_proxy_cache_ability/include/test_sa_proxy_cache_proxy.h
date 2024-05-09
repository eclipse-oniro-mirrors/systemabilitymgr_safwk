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

#ifndef OHOS_TESTSAPROXYCACHEPROXY_H
#define OHOS_TESTSAPROXYCACHEPROXY_H

#include "itest_sa_proxy_cache.h"
#include <iremote_proxy.h>
#include "api_cache_manager.h"

namespace OHOS {
namespace {
constexpr int64_t EXPIRE_TIME_3000MS = 3000;
constexpr int64_t EXPIRE_TIME_4000MS = 4000;
constexpr int64_t EXPIRE_TIME_5000MS = 5000;
}
class TestSaProxyCacheProxy : public IRemoteProxy<ITestSaProxyCache> {
public:
    explicit TestSaProxyCacheProxy(
        const sptr<IRemoteObject>& remote)
        : IRemoteProxy<ITestSaProxyCache>(remote)
    {
        ApiCacheManager::GetInstance().AddCacheApi(GetDescriptor(), COMMAND_GET_STRING_FUNC, EXPIRE_TIME_3000MS);
        ApiCacheManager::GetInstance().AddCacheApi(GetDescriptor(), COMMAND_GET_DOUBLE_FUNC, EXPIRE_TIME_4000MS);
        ApiCacheManager::GetInstance().AddCacheApi(GetDescriptor(), COMMAND_GET_VECTOR_FUNC, EXPIRE_TIME_5000MS);

        if (remote) {
            if (!remote->IsProxyObject()) {
                return;
            }
            deathRecipient_ = new (std::nothrow) TestSaProxyCacheRecipient(*this);
            if (deathRecipient_ == nullptr) {
                return;
            }
            if (!remote->AddDeathRecipient(deathRecipient_)) {
                return;
            }
            remote_ = remote;
        }
    }

    virtual ~TestSaProxyCacheProxy()
    {
        if (remote_ == nullptr) {
            return;
        }
        if (deathRecipient_ == nullptr) {
            return;
        }
        remote_->RemoveDeathRecipient(deathRecipient_);
        remote_ = nullptr;

        ApiCacheManager::GetInstance().DelCacheApi(GetDescriptor(), COMMAND_GET_STRING_FUNC);
        ApiCacheManager::GetInstance().DelCacheApi(GetDescriptor(), COMMAND_GET_DOUBLE_FUNC);
        ApiCacheManager::GetInstance().DelCacheApi(GetDescriptor(), COMMAND_GET_VECTOR_FUNC);
    }

    ErrCode GetStringFunc(
        const std::string& in_str,
        std::string& ret_str) override;

    ErrCode GetDoubleFunc(
        int32_t number,
        double& ret_number) override;

    ErrCode GetVectorFunc(
        const std::vector<bool>& in_vec,
        std::vector<int8_t>& ret_vec) override;

    uint32_t TestGetIpcSendRequestTimes() override;

    ErrCode GetSaPid(
        int32_t& pid) override;
protected:
    ErrCode ProxyReadVector(MessageParcel& reply, std::vector<int8_t>& ret_vec);
private:
    class TestSaProxyCacheRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit TestSaProxyCacheRecipient(TestSaProxyCacheProxy &client) : client_(client) {}
        ~TestSaProxyCacheRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            client_.OnRemoteDied(remote);
        }
    private:
        TestSaProxyCacheProxy &client_;
    };

    void OnRemoteDied(const wptr<IRemoteObject> &remoteObject)
    {
        (void)remoteObject;
        ApiCacheManager::GetInstance().ClearCache(GetDescriptor());
    }
    sptr<IRemoteObject> remote_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    static constexpr int32_t COMMAND_GET_STRING_FUNC = MIN_TRANSACTION_ID + 0;
    static constexpr int32_t COMMAND_GET_DOUBLE_FUNC = MIN_TRANSACTION_ID + 1;
    static constexpr int32_t COMMAND_GET_VECTOR_FUNC = MIN_TRANSACTION_ID + 2;
    static constexpr int32_t COMMAND_GET_SA_PID = MIN_TRANSACTION_ID + 3;

    static inline BrokerDelegator<TestSaProxyCacheProxy> delegator_;
    // only for test
    uint32_t TestSendRequestTimes = 0;
};
} // namespace OHOS
#endif // OHOS_TESTSAPROXYCACHEPROXY_H

