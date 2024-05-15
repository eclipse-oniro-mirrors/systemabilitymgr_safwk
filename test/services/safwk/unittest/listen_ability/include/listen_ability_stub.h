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

#ifndef SAFWK_TEST_SERVICES_SAFWK_UNITTEST_LISTEN_ABILITY_INCLUDE_LISTEN_ABILITY_STUB_H
#define SAFWK_TEST_SERVICES_SAFWK_UNITTEST_LISTEN_ABILITY_INCLUDE_LISTEN_ABILITY_STUB_H

#include "ilisten_ability.h"
#include <iremote_stub.h>

namespace OHOS {
class ListenAbilityStub : public IRemoteStub<IListenAbility> {
public:
    int32_t OnRemoteRequest(
        uint32_t code,
        MessageParcel& data,
        MessageParcel& reply,
        MessageOption& option) override;

protected:
    int32_t StubTestSaCallSa(MessageParcel &data, MessageParcel &reply);
private:
    static constexpr int32_t ADD_VOLUME = MIN_TRANSACTION_ID + 0;
    static constexpr int32_t COMMAND_TEST_SA_CALL_SA = MIN_TRANSACTION_ID + 1;
    static constexpr int32_t COMMAND_TEST_GET_IPC_TIMES = MIN_TRANSACTION_ID + 2;
    static constexpr int32_t COMMAND_TEST_ClEAR_SA_1493_PROXY = MIN_TRANSACTION_ID + 3;
};
} // namespace OHOS
#endif // SAFWK_TEST_SERVICES_SAFWK_UNITTEST_LISTEN_ABILITY_INCLUDE_LISTEN_ABILITY_STUB_H