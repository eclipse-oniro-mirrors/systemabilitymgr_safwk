/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "parse_util.h"
#include "system_ability_ondemand_reason.h"

namespace OHOS {
OnDemandReasonExtraData::OnDemandReasonExtraData(int32_t code, const std::string& data,
    const std::map<std::string, std::string>& want)
{
    code_ = code;
    data_ = data;
    want_ = want;
}

std::string OnDemandReasonExtraData::GetData() const
{
    return data_;
}

int32_t OnDemandReasonExtraData::GetCode() const
{
    return code_;
}

const std::map<std::string, std::string>& OnDemandReasonExtraData::GetWant() const
{
    return want_;
}

bool OnDemandReasonExtraData::Marshalling(Parcel& parcel) const
{
    if (!parcel.WriteInt32(code_)) {
        return false;
    }
    if (!parcel.WriteString(data_)) {
        return false;
    }
    nlohmann::json payload;
    for (auto it = want_.begin(); it != want_.end(); ++it) {
        payload[it->first] = it->second;
    }
    if (!parcel.WriteString(payload.dump())) {
        return false;
    }
    return true;
}

OnDemandReasonExtraData *OnDemandReasonExtraData::Unmarshalling(Parcel& parcel)
{
    int32_t code = 0;
    if (!parcel.ReadInt32(code)) {
        return nullptr;
    }
    std::string data;
    if (!parcel.ReadString(data)) {
        return nullptr;
    }
    std::map<std::string, std::string> want;
    nlohmann::json payload = ParseUtil::StringToJsonObj(parcel.ReadString());
    for (nlohmann::json::iterator it = payload.begin(); it != payload.end(); ++it) {
        if (it.value().is_string()) {
            want[it.key()] = it.value();
        }
    }
    OnDemandReasonExtraData* extraData = new OnDemandReasonExtraData(code, data, want);
    return extraData;
}

SystemAbilityOnDemandReason::SystemAbilityOnDemandReason(OnDemandReasonId reasonId, const std::string& reasonName,
    const std::string& reasonValue, int64_t extraDataId)
{
    reasonId_ = reasonId;
    reasonName_ = reasonName;
    reasonValue_ = reasonValue;
    extraDataId_ = extraDataId;
}

void SystemAbilityOnDemandReason::SetId(OnDemandReasonId reasonId)
{
    reasonId_ = reasonId;
}

OnDemandReasonId SystemAbilityOnDemandReason::GetId() const
{
    return reasonId_;
}

void SystemAbilityOnDemandReason::SetName(const std::string& reasonName)
{
    reasonName_ = reasonName;
}

std::string SystemAbilityOnDemandReason::GetName() const
{
    return reasonName_;
}

void SystemAbilityOnDemandReason::SetValue(const std::string& reasonValue)
{
    reasonValue_ = reasonValue;
}

std::string SystemAbilityOnDemandReason::GetValue() const
{
    return reasonValue_;
}

void SystemAbilityOnDemandReason::SetExtraDataId(int64_t extraDataId)
{
    extraDataId_ = extraDataId;
}

int64_t SystemAbilityOnDemandReason::GetExtraDataId() const
{
    return extraDataId_;
}

bool SystemAbilityOnDemandReason::HasExtraData() const
{
    return reasonId_ == OnDemandReasonId::COMMON_EVENT;
}

void SystemAbilityOnDemandReason::SetExtraData(OnDemandReasonExtraData& extraData)
{
    extraData_ = extraData;
}

const OnDemandReasonExtraData& SystemAbilityOnDemandReason::GetExtraData() const
{
    return extraData_;
}
}