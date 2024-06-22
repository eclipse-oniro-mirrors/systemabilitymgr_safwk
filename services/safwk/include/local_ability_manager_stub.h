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


#ifndef LOCAL_ABILITY_MANAGER_STUB_H
#define LOCAL_ABILITY_MANAGER_STUB_H

#include <map>

#include "ipc_object_stub.h"
#include "refbase.h"
#include "if_local_ability_manager.h"
#include "nlohmann/json.hpp"

namespace OHOS {
class LocalAbilityManagerStub : public IRemoteStub<ILocalAbilityManager> {
public:
    LocalAbilityManagerStub();
    ~LocalAbilityManagerStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;

protected:
    static bool CheckInputSysAbilityId(int32_t systemAbilityId);

private:
    static int32_t LocalStartAbility(LocalAbilityManagerStub* stub, MessageParcel& data, MessageParcel& reply)
    {
        return stub->StartAbilityInner(data, reply);
    }
    static int32_t LocalStopAbility(LocalAbilityManagerStub* stub, MessageParcel& data, MessageParcel& reply)
    {
        return stub->StopAbilityInner(data, reply);
    }
    static int32_t LocalActiveAbility(LocalAbilityManagerStub* stub, MessageParcel& data, MessageParcel& reply)
    {
        return stub->ActiveAbilityInner(data, reply);
    }
    static int32_t LocalIdleAbility(LocalAbilityManagerStub* stub, MessageParcel& data, MessageParcel& reply)
    {
        return stub->IdleAbilityInner(data, reply);
    }
    static int32_t LocalSendStrategyToSA(LocalAbilityManagerStub* stub, MessageParcel& data, MessageParcel& reply)
    {
        return stub->SendStrategyToSAInner(data, reply);
    }
    static int32_t LocalIpcStatCmdProc(LocalAbilityManagerStub* stub, MessageParcel& data, MessageParcel& reply)
    {
        return stub->IpcStatCmdProc(data, reply);
    }
    static int32_t LocalFfrtDumperProc(LocalAbilityManagerStub* stub, MessageParcel& data, MessageParcel& reply)
    {
        return stub->FfrtDumperProc(data, reply);
    }
    static int32_t LocalSystemAbilityExtProc(LocalAbilityManagerStub* stub, MessageParcel& data, MessageParcel& reply)
    {
        return stub->SystemAbilityExtProcInner(data, reply);
    }
    int32_t StartAbilityInner(MessageParcel& data, MessageParcel& reply);
    int32_t StopAbilityInner(MessageParcel& data, MessageParcel& reply);
    int32_t ActiveAbilityInner(MessageParcel& data, MessageParcel& reply);
    int32_t IdleAbilityInner(MessageParcel& data, MessageParcel& reply);
    int32_t SendStrategyToSAInner(MessageParcel& data, MessageParcel& reply);
    int32_t IpcStatCmdProcInner(MessageParcel& data, MessageParcel& reply);
    int32_t FfrtDumperProcInner(MessageParcel& data, MessageParcel& reply);
    int32_t SystemAbilityExtProcInner(MessageParcel& data, MessageParcel& reply);
    static bool CanRequest();
    static bool EnforceInterceToken(MessageParcel& data);
    static bool CheckPermission(uint32_t code);

    using LocalAbilityManagerStubFunc =
        int32_t (*)(LocalAbilityManagerStub* stub, MessageParcel& data, MessageParcel& reply);
    std::map<uint32_t, LocalAbilityManagerStubFunc> memberFuncMap_;
};
}
#endif
