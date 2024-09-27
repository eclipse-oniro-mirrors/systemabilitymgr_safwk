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

#include "gtest/gtest.h"
#include "test_log.h"

#define private public
#include "system_ability_ondemand_reason.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
    const std::string DATA = "data";
    const std::string REASON_NAME = "name";
    const std::string REASON_VALUE = "value";
    constexpr int32_t CODE = 1;
    constexpr int64_t EXTRA_DATA_ID = 1;

}
class SystemAbilityOndemandReasonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SystemAbilityOndemandReasonTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SystemAbilityOndemandReasonTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SystemAbilityOndemandReasonTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SystemAbilityOndemandReasonTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/**
 * @tc.name: OnDemandReasonExtraDataGetData001
 * @tc.desc: test OnDemandReasonExtraDataGetData, get OnDemandReasonExtraData's data_
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, OnDemandReasonExtraDataGetData001, TestSize.Level2)
{
    DTEST_LOG << "OnDemandReasonExtraDataGetData001 start" << std::endl;
    OnDemandReasonExtraData onDemandReasonExtraData;
    onDemandReasonExtraData.data_ = DATA;
    string ret = onDemandReasonExtraData.GetData();
    EXPECT_EQ(ret, DATA);
    DTEST_LOG << "OnDemandReasonExtraDataGetData001 end" << std::endl;
}

/**
 * @tc.name: OnDemandReasonExtraDataGetCode001
 * @tc.desc: test OnDemandReasonExtraDataGetCode, get OnDemandReasonExtraData's code_
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, OnDemandReasonExtraDataGetCode001, TestSize.Level2)
{
    DTEST_LOG << "OnDemandReasonExtraDataGetCode001 start" << std::endl;
    OnDemandReasonExtraData onDemandReasonExtraData;
    onDemandReasonExtraData.code_ = CODE;
    int32_t ret = onDemandReasonExtraData.GetCode();
    EXPECT_EQ(ret, CODE);
    DTEST_LOG << "OnDemandReasonExtraDataGetCode001 end" << std::endl;
}

/**
 * @tc.name: OnDemandReasonExtraDataGetWant001
 * @tc.desc: test OnDemandReasonExtraDataGetWant, get OnDemandReasonExtraData's want
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, OnDemandReasonExtraDataGetWant001, TestSize.Level2)
{
    DTEST_LOG << "OnDemandReasonExtraDataGetWant001 start" << std::endl;
    OnDemandReasonExtraData onDemandReasonExtraData;
    onDemandReasonExtraData.want_["test"] = "test";
    map testwant = onDemandReasonExtraData.GetWant();
    EXPECT_FALSE(testwant.empty());
    DTEST_LOG << "OnDemandReasonExtraDataGetWant001 end" << std::endl;
}

/**
 * @tc.name: Marshalling001
 * @tc.desc: test Marshalling with empty parcel
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, Marshalling001, TestSize.Level2)
{
    DTEST_LOG << "Marshalling001 start" << std::endl;
    OnDemandReasonExtraData onDemandReasonExtraData;
    Parcel parcel;
    bool ret = onDemandReasonExtraData.Marshalling(parcel);
    EXPECT_TRUE(ret);
    DTEST_LOG << "Marshalling001 end" << std::endl;
}

/**
 * @tc.name: Unmarshalling001
 * @tc.desc: test Unmarshalling with empty parcel
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, Unmarshalling001, TestSize.Level2)
{
    DTEST_LOG << "Unmarshalling001 start" << std::endl;
    OnDemandReasonExtraData onDemandReasonExtraData;
    Parcel parcel;
    OnDemandReasonExtraData* ret = onDemandReasonExtraData.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    DTEST_LOG << "Unmarshalling001 end" << std::endl;
}

/**
 * @tc.name: Unmarshalling002
 * @tc.desc: test Unmarshalling
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, Unmarshalling002, TestSize.Level2)
{
    DTEST_LOG << "Unmarshalling002 start" << std::endl;
    OnDemandReasonExtraData onDemandReasonExtraData;
    Parcel parcel;
    int32_t code = 0;
    parcel.WriteInt32(code);
    OnDemandReasonExtraData* ret = onDemandReasonExtraData.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    DTEST_LOG << "Unmarshalling002 end" << std::endl;
}

/**
 * @tc.name: Unmarshalling003
 * @tc.desc: test Unmarshalling
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, Unmarshalling003, TestSize.Level2)
{
    DTEST_LOG << "Unmarshalling003 start" << std::endl;
    OnDemandReasonExtraData onDemandReasonExtraData;
    Parcel parcel;
    int32_t code = 0;
    parcel.WriteInt32(code);
    std::string data = "test";
    parcel.WriteString(data);
    OnDemandReasonExtraData* ret = onDemandReasonExtraData.Unmarshalling(parcel);
    EXPECT_NE(ret, nullptr);
    DTEST_LOG << "Unmarshalling003 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityOnDemandReasonSetName001
 * @tc.desc: test SystemAbilityOnDemandReasonSetName, set SystemAbilityOnDemandReason's reasonName_
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, SystemAbilityOnDemandReasonSetName001, TestSize.Level2)
{
    DTEST_LOG << "SystemAbilityOnDemandReasonSetName001 start" << std::endl;
    SystemAbilityOnDemandReason systemAbilityOnDemandReason;
    systemAbilityOnDemandReason.SetName(REASON_NAME);
    EXPECT_EQ(systemAbilityOnDemandReason.reasonName_, REASON_NAME);
    DTEST_LOG << "SystemAbilityOnDemandReasonSetName001 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityOnDemandReasonSetValue001
 * @tc.desc: test SystemAbilityOnDemandReasonSetValue, set SystemAbilityOnDemandReason's reasonValue_
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, SystemAbilityOnDemandReasonSetValue001, TestSize.Level2)
{
    DTEST_LOG << "SystemAbilityOnDemandReasonSetValue001 start" << std::endl;
    SystemAbilityOnDemandReason systemAbilityOnDemandReason;
    systemAbilityOnDemandReason.SetValue(REASON_VALUE);
    EXPECT_EQ(systemAbilityOnDemandReason.reasonValue_, REASON_VALUE);
    DTEST_LOG << "SystemAbilityOnDemandReasonSetValue001 end" << std::endl;
}

/**
 * @tc.name: GetExtraDataId001
 * @tc.desc: test GetExtraDataId, get systemAbilityOnDemandReason's extraDataId_
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, GetExtraDataId001, TestSize.Level2)
{
    DTEST_LOG << "GetExtraDataId001 start" << std::endl;
    SystemAbilityOnDemandReason systemAbilityOnDemandReason;
    systemAbilityOnDemandReason.extraDataId_ = EXTRA_DATA_ID;
    int64_t ret = systemAbilityOnDemandReason.GetExtraDataId();
    EXPECT_EQ(ret, EXTRA_DATA_ID);
    DTEST_LOG << "GetExtraDataId001 end" << std::endl;
}

/**
 * @tc.name: SetExtraDataId001
 * @tc.desc: test SetExtraDataId, set SystemAbilityOnDemandReason's extraDataId_
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, SetExtraDataId001, TestSize.Level2)
{
    DTEST_LOG << "SetExtraDataId001 start" << std::endl;
    SystemAbilityOnDemandReason systemAbilityOnDemandReason;
    systemAbilityOnDemandReason.SetExtraDataId(EXTRA_DATA_ID);
    EXPECT_EQ(systemAbilityOnDemandReason.extraDataId_, EXTRA_DATA_ID);
    DTEST_LOG << "SetExtraDataId001 end" << std::endl;
}

/**
 * @tc.name: SetExtraData001
 * @tc.desc: test SetExtraData, set SystemAbilityOnDemandReason's extraData_
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, SetExtraData001, TestSize.Level2)
{
    DTEST_LOG << "SetExtraData001 start" << std::endl;
    SystemAbilityOnDemandReason systemAbilityOnDemandReason;
    OnDemandReasonExtraData onDemandReasonExtraData;
    onDemandReasonExtraData.data_ = DATA;
    systemAbilityOnDemandReason.SetExtraData(onDemandReasonExtraData);
    string ret = systemAbilityOnDemandReason.extraData_.GetData();
    EXPECT_EQ(ret, DATA);
    DTEST_LOG << "SetExtraData001 end" << std::endl;
}

/**
 * @tc.name: GetExtraData001
 * @tc.desc: test GetExtraData, get SystemAbilityOnDemandReason's extraData_
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, GetExtraData001, TestSize.Level2)
{
    DTEST_LOG << "GetExtraData001 start" << std::endl;
    SystemAbilityOnDemandReason systemAbilityOnDemandReason;
    OnDemandReasonExtraData onDemandReasonExtraData;
    onDemandReasonExtraData.data_ = DATA;
    systemAbilityOnDemandReason.SetExtraData(onDemandReasonExtraData);
    string ret = systemAbilityOnDemandReason.GetExtraData().data_;
    EXPECT_EQ(ret, DATA);
    DTEST_LOG << "GetExtraData001 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityOnDemandReason001
 * @tc.desc: test SystemAbilityOnDemandReason
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityOndemandReasonTest, SystemAbilityOnDemandReason001, TestSize.Level2)
{
    DTEST_LOG << "SystemAbilityOnDemandReason001 start" << std::endl;
    OnDemandReasonId reasonId = OnDemandReasonId::DEVICE_ONLINE;
    std::string reasonName = "testreasonname";
    std::string reasonValue = "testreasonvalue";
    int64_t extraDataId = 1;
    SystemAbilityOnDemandReason reason(reasonId, reasonName, reasonValue, extraDataId);
    EXPECT_EQ(reason.reasonId_, reasonId);
    EXPECT_EQ(reason.reasonName_, reasonName);
    EXPECT_EQ(reason.reasonValue_, reasonValue);
    EXPECT_EQ(reason.extraDataId_, extraDataId);
    DTEST_LOG << "SystemAbilityOnDemandReason001 end" << std::endl;
}
}