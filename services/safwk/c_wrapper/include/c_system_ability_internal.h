/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#ifndef SAFWK_C_SYSTEM_ABILITY_INTERNAL_H
#define SAFWK_C_SYSTEM_ABILITY_INTERNAL_H

#include <system_ability.h>
#include <c_system_ability.h>
class CSystemAbilityInnerService;
struct CSystemAbilityInner {
    CSystemAbilityInner(const void *userData);
    ~CSystemAbilityInner();
    const void *GetUserData()
    {
        return userData_;
    };
    void SetUserData(const void *userData)
    {
        userData_ = userData;
    };
    CSystemAbilityInnerService *cSystemabilityInnerservice_;
private:
    const void *userData_;
};

class CSystemAbilityInnerService : public OHOS::SystemAbility {
public:
    CSystemAbilityInnerService(CSystemAbility *cSystemability, OnStartCb onStart, OnStopCb onStop,
        int32_t systemAbilityId, bool runOnCreate = false);
    ~CSystemAbilityInnerService();

    DECLARE_SYSTEM_ABILITY(CSystemAbilityInnerService);
    void OnStart() override;
    void OnStop() override;
    void SetCSystemAbility(CSystemAbility *cSystemability)
    {
        cSystemability_ = cSystemability;
    };
private:
    CSystemAbility *cSystemability_;
    OnStartCb onStartcallback_;
    OnStopCb onStopcallback_;
};
#endif /* SAFWK_C_SYSTEM_ABILITY_H */