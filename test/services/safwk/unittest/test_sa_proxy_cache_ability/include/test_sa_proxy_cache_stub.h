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

#ifndef OHOS_TESTSAPROXYCACHESTUB_H
#define OHOS_TESTSAPROXYCACHESTUB_H

#include "itest_sa_proxy_cache.h"
#include <iremote_stub.h>

namespace OHOS {
class TestSaProxyCacheStub : public IRemoteStub<ITestSaProxyCache> {
public:
    int32_t OnRemoteRequest(
        uint32_t code,
        MessageParcel& data,
        MessageParcel& reply,
        MessageOption& option) override;

protected:
    int32_t StubGetVecFunc(MessageParcel& data, MessageParcel& reply);
    int32_t StubGetSaPID(MessageParcel& reply);
private:
    static constexpr int32_t COMMAND_GET_STRING_FUNC = MIN_TRANSACTION_ID + 0;
    static constexpr int32_t COMMAND_GET_DOUBLE_FUNC = MIN_TRANSACTION_ID + 1;
    static constexpr int32_t COMMAND_GET_VECTOR_FUNC = MIN_TRANSACTION_ID + 2;
    static constexpr int32_t COMMAND_GET_SA_PID = MIN_TRANSACTION_ID + 3;
};
} // namespace OHOS
#endif // OHOS_TESTSAPROXYCACHESTUB_H

