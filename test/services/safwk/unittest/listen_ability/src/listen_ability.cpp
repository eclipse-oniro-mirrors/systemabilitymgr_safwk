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
#include "listen_ability.h"

#include "hilog/log.h"
#include "iremote_object.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "test_sa_proxy_cache_proxy.h"

using namespace OHOS::HiviewDFX;

namespace OHOS {
namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0xD001800, "ListenAbility"};
}

REGISTER_SYSTEM_ABILITY_BY_ID(ListenAbility, DISTRIBUTED_SCHED_TEST_LISTEN_ID, true);

ListenAbility::ListenAbility(int32_t saId, bool runOnCreate) : SystemAbility(saId, runOnCreate)
{
    HiLog::Info(LABEL, "ListenAbility()");
}

ListenAbility::~ListenAbility()
{
    HiLog::Info(LABEL, "~ListenAbility()");
}

int32_t ListenAbility::AddVolume(int32_t volume)
{
    HiLog::Info(LABEL, "AddVolume volume = %d.", volume);
    return (volume + 1);
}

ErrCode ListenAbility::TestSaCallSa(int32_t input, double& output)
{
    if (TestSa1493Proxy_ == nullptr)  {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(DISTRIBUTED_SCHED_TEST_TT_ID);
        TestSa1493Proxy_ = iface_cast<ITestSaProxyCache>(remoteObject);
    }
    if (TestSa1493Proxy_ == nullptr) {
        HiLog::Info(LABEL, "TestSa1493Proxy_ is nullptr");
        return ERR_TIMED_OUT;
    }

    auto ret = TestSa1493Proxy_->GetDoubleFunc(input, output);
    HiLog::Info(LABEL, "ret :%{public}d Listen Testsacall input:%{public}d, output:%{public}lf", ret, input, output);
    return ret;
}

ErrCode ListenAbility::TestGetIpcSendRequestTimes(int32_t& times)
{
    if (TestSa1493Proxy_ == nullptr) {
        HiLog::Info(LABEL, "TestClearSa1493Proxy_ is nullptr");
        return ERR_NULL_OBJECT;
    }
    times = TestSa1493Proxy_->TestGetIpcSendRequestTimes();
    return ERR_OK;
}

ErrCode ListenAbility::TestClearSa1493Proxy_()
{
    TestSa1493Proxy_.clear();
    return ERR_OK;
}

void ListenAbility::OnStart()
{
    HiLog::Info(LABEL, "OnStart()");
    bool res = Publish(this);
    if (!res) {
        HiLog::Error(LABEL, "publish failed!");
        return;
    }
    HiLog::Info(LABEL, "%{public}s called:AddAbilityListener begin!", __func__);
    AddSystemAbilityListener(DISTRIBUTED_SCHED_SA_ID);
    AddSystemAbilityListener(SOFTBUS_SERVER_SA_ID);
    AddSystemAbilityListener(RES_SCHED_SYS_ABILITY_ID);
    AddSystemAbilityListener(BACKGROUND_TASK_MANAGER_SERVICE_ID);
    HiLog::Info(LABEL, "%{public}s called:AddAbilityListener end!", __func__);
}

void ListenAbility::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    HiLog::Info(LABEL, "OnAddSystemAbility systemAbilityId:%{public}d added!", systemAbilityId);
}

void ListenAbility::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    HiLog::Info(LABEL, "OnRemoveSystemAbility systemAbilityId:%{public}d removed!", systemAbilityId);
}
}
