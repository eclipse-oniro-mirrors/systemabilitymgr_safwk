/*
 * Copyright (c) 2024-2024 Huawei Device Co., Ltd.
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

#include <fstream>
#include "gtest/gtest.h"
#include "iservice_registry.h"
#include "string_ex.h"
#include "test_log.h"
#include "hisysevent_adapter.h"

#define private public
#include "local_ability_manager.h"
#include "mock_sa_realize.h"
#undef private
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace SAFWK {
namespace {
    const std::string TEST_RESOURCE_PATH = "/data/test/resource/safwk/profile/";
    const std::u16string LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN = u"ohos.localabilitymanager.accessToken";
    constexpr int SAID = 1499;
    constexpr int MUT_SAID = 9999;
    constexpr int INVALID_SAID = -1;
    constexpr int STARTCODE = 1;
    constexpr const char* EVENT_ID = "eventId";
    constexpr const char* NAME = "name";
    constexpr const char* VALUE = "value";
    constexpr const char* EXTRA_DATA_ID = "extraDataId";
}

class LocalAbilityManagerStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void LocalAbilityManagerStubTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void LocalAbilityManagerStubTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void LocalAbilityManagerStubTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void LocalAbilityManagerStubTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: OnRemoteRequest001
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, OnRemoteRequest001, TestSize.Level2)
{
    DTEST_LOG << "OnRemoteRequest001 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string deviceId = "";
    LocalAbilityManager::SystemAbilityListener *sysListener = new LocalAbilityManager::SystemAbilityListener();
    sysListener->OnAddSystemAbility(SAID, deviceId);
    int32_t result = LocalAbilityManager::GetInstance().OnRemoteRequest(0, data, reply, option);
    delete sysListener;
    EXPECT_NE(result, ERR_NONE);
    DTEST_LOG << "OnRemoteRequest001 end" << std::endl;
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: OnRemoteRequest002
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, OnRemoteRequest002, TestSize.Level2)
{
    DTEST_LOG << "OnRemoteRequest002 start" << std::endl;
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    MessageParcel reply;
    MessageOption option;
    std::string deviceId = "";
    LocalAbilityManager::SystemAbilityListener *sysListener = new LocalAbilityManager::SystemAbilityListener();
    sysListener->OnAddSystemAbility(INVALID_SAID, deviceId);
    int32_t result = LocalAbilityManager::GetInstance().OnRemoteRequest(0, data, reply, option);
    delete sysListener;
    EXPECT_NE(result, ERR_NONE);
    DTEST_LOG << "OnRemoteRequest002 end" << std::endl;
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: OnRemoteRequest003
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, OnRemoteRequest003, TestSize.Level2)
{
    DTEST_LOG << "OnRemoteRequest003 start" << std::endl;
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    MessageParcel reply;
    MessageOption option;
    std::string deviceId = "";
    LocalAbilityManager::SystemAbilityListener *sysListener = new LocalAbilityManager::SystemAbilityListener();
    sysListener->OnRemoveSystemAbility(INVALID_SAID, deviceId);
    int32_t result = LocalAbilityManager::GetInstance().OnRemoteRequest(STARTCODE, data, reply, option);
    delete sysListener;
    EXPECT_NE(result, ERR_NONE);
    DTEST_LOG << "OnRemoteRequest003 end" << std::endl;
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: OnRemoteRequest004
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, OnRemoteRequest004, TestSize.Level2)
{
    DTEST_LOG << "OnRemoteRequest004 start" << std::endl;
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    data.WriteInt32(STARTCODE);
    MessageParcel reply;
    MessageOption option;
    std::string deviceId = "";
    LocalAbilityManager::SystemAbilityListener *sysListener = new LocalAbilityManager::SystemAbilityListener();
    sysListener->OnRemoveSystemAbility(SAID, deviceId);
    int32_t result = LocalAbilityManager::GetInstance().OnRemoteRequest(STARTCODE, data, reply, option);
    delete sysListener;
    EXPECT_EQ(result, ERR_NULL_OBJECT);
    DTEST_LOG << "OnRemoteRequest004 end" << std::endl;
}

/**
 * @tc.name: OnRemoteRequest005
 * @tc.desc: OnRemoteRequest005
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, OnRemoteRequest005, TestSize.Level2)
{
    DTEST_LOG << "OnRemoteRequest005 start" << std::endl;
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    data.WriteInt32(STARTCODE);
    std::string eventStr = "test";
    data.WriteString(eventStr);
    MessageParcel reply;
    MessageOption option;
    std::string deviceId = "";
    LocalAbilityManager::SystemAbilityListener *sysListener = new LocalAbilityManager::SystemAbilityListener();
    sysListener->OnRemoveSystemAbility(SAID, deviceId);
    int32_t result = LocalAbilityManager::GetInstance().OnRemoteRequest(STARTCODE, data, reply, option);
    delete sysListener;
    EXPECT_EQ(result, ERR_NONE);
    DTEST_LOG << "OnRemoteRequest005 end" << std::endl;
}

/**
 * @tc.name: OnRemoteRequest006
 * @tc.desc: OnRemoteRequest006
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, OnRemoteRequest006, TestSize.Level2)
{
    DTEST_LOG << "OnRemoteRequest006 start" << std::endl;
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    data.WriteInt32(STARTCODE);
    std::string eventStr = "test";
    data.WriteString(eventStr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = 1;
    auto iter = LocalAbilityManager::GetInstance().memberFuncMap_.find(code);
    LocalAbilityManager::GetInstance().OnRemoteRequest(code, data, reply, option);
    EXPECT_NE(iter, LocalAbilityManager::GetInstance().memberFuncMap_.end());
    DTEST_LOG << "OnRemoteRequest006 end" << std::endl;
}

/**
 * @tc.name: OnRemoteRequest007
 * @tc.desc: OnRemoteRequest007
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, OnRemoteRequest007, TestSize.Level2)
{
    DTEST_LOG << "OnRemoteRequest007 start" << std::endl;
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    data.WriteInt32(STARTCODE);
    std::string eventStr = "test";
    data.WriteString(eventStr);
    MessageParcel reply;
    MessageOption option;
    uint32_t code = 0;
    auto iter = LocalAbilityManager::GetInstance().memberFuncMap_.find(code);
    LocalAbilityManager::GetInstance().OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(iter, LocalAbilityManager::GetInstance().memberFuncMap_.end());
    DTEST_LOG << "OnRemoteRequest007 end" << std::endl;
}

/**
 * @tc.name: StartAbilityInner001
 * @tc.desc: test StartAbilityInner with invalid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerStubTest, StartAbilityInner001, TestSize.Level2)
{
    DTEST_LOG << "StartAbilityInner001 start" << std::endl;
    MessageParcel data;
    data.WriteInt32(INVALID_SAID);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StartAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "StartAbilityInner001 end" << std::endl;
}

/**
 * @tc.name: StartAbilityInner002
 * @tc.desc: test StartAbilityInner with data.ReadString() is empty
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerStubTest, StartAbilityInner002, TestSize.Level2)
{
    DTEST_LOG << "StartAbilityInner002 start" << std::endl;
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StartAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "StartAbilityInner002 end" << std::endl;
}

/**
 * @tc.name: StartAbilityInner003
 * @tc.desc: test StartAbilityInner with data.ReadString() is not  empty
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerStubTest, StartAbilityInner003, TestSize.Level2)
{
    DTEST_LOG << "StartAbilityInner003 start" << std::endl;
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    std::string eventStr = "test";
    data.WriteString(eventStr);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StartAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
    DTEST_LOG << "StartAbilityInner003 end" << std::endl;
}

/**
 * @tc.name: StopAbilityInner001
 * @tc.desc: test StopAbilityInner with invalid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerStubTest, StopAbilityInner001, TestSize.Level2)
{
    DTEST_LOG << "StopAbilityInner001 start" << std::endl;
    MessageParcel data;
    data.WriteInt32(INVALID_SAID);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StopAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "StopAbilityInner001 end" << std::endl;
}

/**
 * @tc.name: StopAbilityInner002
 * @tc.desc: test StopAbilityInner with data.ReadString() is empty
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerStubTest, StopAbilityInner002, TestSize.Level2)
{
    DTEST_LOG << "StopAbilityInner002 start" << std::endl;
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StopAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "StopAbilityInner002 end" << std::endl;
}

/**
 * @tc.name: StopAbilityInner003
 * @tc.desc: test StopAbilityInner with data.ReadString() is empty
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerStubTest, StopAbilityInner003, TestSize.Level2)
{
    DTEST_LOG << "StopAbilityInner003 start" << std::endl;
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    std::string eventStr = "test";
    data.WriteString(eventStr);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StopAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
    DTEST_LOG << "StopAbilityInner003 end" << std::endl;
}

/**
 * @tc.name: ActiveAbilityInner001
 * @tc.desc: test ActiveAbilityInner with invalid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerStubTest, ActiveAbilityInner001, TestSize.Level2)
{
    DTEST_LOG << "ActiveAbilityInner001 start" << std::endl;
    MessageParcel data;
    data.WriteInt32(INVALID_SAID);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().ActiveAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "ActiveAbilityInner001 end" << std::endl;
}

/**
 * @tc.name: ActiveAbilityInner002
 * @tc.desc: test ActiveAbilityInner with valid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerStubTest, ActiveAbilityInner002, TestSize.Level2)
{
    DTEST_LOG << "ActiveAbilityInner002 start" << std::endl;
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().ActiveAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
    DTEST_LOG << "ActiveAbilityInner002 end" << std::endl;
}

/**
 * @tc.name: IdleAbilityInner001
 * @tc.desc: test IdleAbilityInner with invalid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerStubTest, IdleAbilityInner001, TestSize.Level2)
{
    DTEST_LOG << "IdleAbilityInner001 start" << std::endl;
    MessageParcel data;
    data.WriteInt32(INVALID_SAID);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().IdleAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "IdleAbilityInner001 end" << std::endl;
}

/**
 * @tc.name: IdleAbilityInner002
 * @tc.desc: test IdleAbilityInner with valid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerStubTest, IdleAbilityInner002, TestSize.Level2)
{
    DTEST_LOG << "IdleAbilityInner002 start" << std::endl;
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().IdleAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
    DTEST_LOG << "IdleAbilityInner002 end" << std::endl;
}

/**
 * @tc.name: OnStopAbility001
 * @tc.desc: test OnStopAbility, cover function with valid SaID
 * @tc.type: FUNC
 * @tc.require: I73XRZ
 */
HWTEST_F(LocalAbilityManagerStubTest, OnStopAbility001, TestSize.Level2)
{
    DTEST_LOG << "OnStopAbility001 start" << std::endl;
    int32_t systemAbilityId = 1;
    bool ret = LocalAbilityManager::GetInstance().OnStopAbility(systemAbilityId);
    EXPECT_FALSE(ret);
    DTEST_LOG << "OnStopAbility001 end" << std::endl;
}

/**
 * @tc.name: OnStopAbility002
 * @tc.desc: test OnStopAbility with said is in abilityMap_
 * @tc.type: FUNC
 * @tc.require: I7G7DL
 */
HWTEST_F(LocalAbilityManagerStubTest, OnStopAbility002, TestSize.Level2)
{
    DTEST_LOG << "OnStopAbility002 start" << std::endl;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    mockSa->abilityState_ = SystemAbilityState::NOT_LOADED;
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    bool ret = LocalAbilityManager::GetInstance().OnStopAbility(SAID);
    delete mockSa;
    EXPECT_TRUE(ret);
    DTEST_LOG << "OnStopAbility002 end" << std::endl;
}

/**
 * @tc.name: ActiveAbility001
 * @tc.desc: test ActiveAbility, cover function with valid SaID
 * @tc.type: FUNC
 * @tc.require: I73XRZ
 */
HWTEST_F(LocalAbilityManagerStubTest, ActiveAbility001, TestSize.Level2)
{
    DTEST_LOG << "ActiveAbility001 start" << std::endl;
    int32_t systemAbilityId = 1;
    nlohmann::json activeReason;
    bool ret = LocalAbilityManager::GetInstance().ActiveAbility(systemAbilityId, activeReason);
    // cover StartPhaseTasks
    std::list<SystemAbility*> systemAbilityList;
    LocalAbilityManager::GetInstance().StartPhaseTasks(systemAbilityList);
    EXPECT_FALSE(ret);
    DTEST_LOG << "ActiveAbility001 end" << std::endl;
}

/**
 * @tc.name: ActiveAbility002
 * @tc.desc: test ActiveAbility with sa is existed
 * @tc.type: FUNC
 * @tc.require: I7G7DL
 */
HWTEST_F(LocalAbilityManagerStubTest, ActiveAbility002, TestSize.Level2)
{
    DTEST_LOG << "ActiveAbility002 start" << std::endl;
    nlohmann::json activeReason;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    mockSa->abilityState_ = SystemAbilityState::ACTIVE;
    bool ret = LocalAbilityManager::GetInstance().ActiveAbility(SAID, activeReason);
    delete mockSa;
    EXPECT_TRUE(ret);
    DTEST_LOG << "ActiveAbility002 end" << std::endl;
}

/**
 * @tc.name: IdleAbility001
 * @tc.desc: test IdleAbility, cover function
 * @tc.type: FUNC
 * @tc.require: I73XRZ
 */
HWTEST_F(LocalAbilityManagerStubTest, IdleAbility001, TestSize.Level2)
{
    DTEST_LOG << "IdleAbility001 start" << std::endl;
    int32_t systemAbilityId = 1;
    nlohmann::json activeReason;
    int32_t delayTime = 0;
    bool ret = LocalAbilityManager::GetInstance().IdleAbility(systemAbilityId, activeReason, delayTime);
    // cover WaitForTasks
    LocalAbilityManager::GetInstance().WaitForTasks();
    // cover FindAndStartPhaseTasks
    LocalAbilityManager::GetInstance().FindAndStartPhaseTasks(systemAbilityId);
    EXPECT_FALSE(ret);
    DTEST_LOG << "IdleAbility001 end" << std::endl;
}

/**
 * @tc.name: IdleAbility002
 * @tc.desc: test IdleAbility with sa is existed
 * @tc.type: FUNC
 * @tc.require: I7G7DL
 */
HWTEST_F(LocalAbilityManagerStubTest, IdleAbility002, TestSize.Level2)
{
    DTEST_LOG << "IdleAbility002 start" << std::endl;
    nlohmann::json idleReason;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    mockSa->abilityState_ = SystemAbilityState::IDLE;
    int delayTime = 0;
    bool ret = LocalAbilityManager::GetInstance().IdleAbility(SAID, idleReason, delayTime);
    delete mockSa;
    EXPECT_TRUE(ret);
    DTEST_LOG << "IdleAbility002 end" << std::endl;
}

/**
 * @tc.name: JsonToOnDemandReason001
 * @tc.desc: test JsonToOnDemandReason, with assignments
 * @tc.type: FUNC
 * @tc.require: I73XRZ
 */
HWTEST_F(LocalAbilityManagerStubTest, JsonToOnDemandReason001, TestSize.Level2)
{
    DTEST_LOG << "JsonToOnDemandReason001 start" << std::endl;
    SystemAbilityOnDemandReason onDemandStartReason;
    nlohmann::json reasonJson;
    reasonJson[EVENT_ID] = 1;
    reasonJson[NAME] = "test";
    reasonJson[VALUE] = "test";
    reasonJson[EXTRA_DATA_ID] = 1;
    onDemandStartReason.SetId(reasonJson[EVENT_ID]);
    onDemandStartReason.SetName(reasonJson[NAME]);
    onDemandStartReason.SetValue(reasonJson[VALUE]);
    onDemandStartReason.SetExtraDataId(reasonJson[EXTRA_DATA_ID]);
    SystemAbilityOnDemandReason ret = LocalAbilityManager::GetInstance().JsonToOnDemandReason(reasonJson);
    EXPECT_EQ(ret.extraDataId_, onDemandStartReason.extraDataId_);
    DTEST_LOG << "JsonToOnDemandReason001 end" << std::endl;
}

/**
 * @tc.name: FfrtDumperProc001
 * @tc.desc: test FfrtDumperProc
 * @tc.type: FUNC
 * @tc.require: I7G7DL
 */
HWTEST_F(LocalAbilityManagerStubTest, FfrtDumperProc001, TestSize.Level2)
{
    DTEST_LOG << "FfrtDumperProc001 start" << std::endl;
    std::string ffrtDumperInfo;
    bool result = LocalAbilityManager::GetInstance().FfrtDumperProc(ffrtDumperInfo);
    EXPECT_TRUE(result);
    DTEST_LOG << "FfrtDumperProc001 end" << std::endl;
}

/**
 * @tc.name: SendStrategyToSA001
 * @tc.desc: test SendStrategyToSA001, cover function with valid SaID
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SendStrategyToSA001, TestSize.Level2)
{
    DTEST_LOG << "SendStrategyToSA001 start" << std::endl;
    int32_t systemAbilityId = 1;
    std::string action = "";
    bool ret = LocalAbilityManager::GetInstance().SendStrategyToSA(1, systemAbilityId, 1, action);
    EXPECT_FALSE(ret);
    DTEST_LOG << "SendStrategyToSA001 end" << std::endl;
}

/**
 * @tc.name: SendStrategyToSA002
 * @tc.desc: test SendStrategyToSA with said is in abilityMap_
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SendStrategyToSA002, TestSize.Level2)
{
    DTEST_LOG << "SendStrategyToSA002 start" << std::endl;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    mockSa->abilityState_ = SystemAbilityState::NOT_LOADED;
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    std::string action = "";
    bool ret = LocalAbilityManager::GetInstance().SendStrategyToSA(1, SAID, 1, action);
    delete mockSa;
    EXPECT_TRUE(ret);
    DTEST_LOG << "SendStrategyToSA002 end" << std::endl;
}

/**
 * @tc.name: IpcStatCmdProc001
 * @tc.desc: test IpcStatCmdProc001, cover function with valid fd and valid cmd
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, IpcStatCmdProc001, TestSize.Level2)
{
    DTEST_LOG << "IpcStatCmdProc001 start" << std::endl;
    int32_t fd = 1;
    int32_t cmd = 0;
    bool ret = LocalAbilityManager::GetInstance().IpcStatCmdProc(fd, cmd);
    EXPECT_TRUE(ret);
    DTEST_LOG << "IpcStatCmdProc001 end" << std::endl;
}

/**
 * @tc.name: IpcStatCmdProc002
 * @tc.desc: test IpcStatCmdProc002, cover function with valid fd and invalid cmd
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, IpcStatCmdProc002, TestSize.Level2)
{
    DTEST_LOG << "IpcStatCmdProc002 start" << std::endl;
    int32_t fd = 1;
    int32_t cmd = -1;
    bool ret = LocalAbilityManager::GetInstance().IpcStatCmdProc(fd, cmd);
    EXPECT_FALSE(ret);
    DTEST_LOG << "IpcStatCmdProc002 end" << std::endl;
}

/**
 * @tc.name: IpcStatCmdProc003
 * @tc.desc: test IpcStatCmdProc003, cover function with invalid fd and valid cmd
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, IpcStatCmdProc003, TestSize.Level2)
{
    DTEST_LOG << "IpcStatCmdProc003 start" << std::endl;
    int32_t fd = -1;
    int32_t cmd = 0;
    bool ret = LocalAbilityManager::GetInstance().IpcStatCmdProc(fd, cmd);
    EXPECT_FALSE(ret);
    DTEST_LOG << "IpcStatCmdProc003 end" << std::endl;
}

/**
 * @tc.name: IpcStatCmdProc004
 * @tc.desc: test IpcStatCmdProc004, cover function with invalid fd and invalid cmd
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, IpcStatCmdProc004, TestSize.Level2)
{
    DTEST_LOG << "IpcStatCmdProc004 start" << std::endl;
    int32_t fd = -1;
    int32_t cmd = -1;
    bool ret = LocalAbilityManager::GetInstance().IpcStatCmdProc(fd, cmd);
    EXPECT_FALSE(ret);
    DTEST_LOG << "IpcStatCmdProc004 end" << std::endl;
}

/**
 * @tc.name: CheckPermission001
 * @tc.desc: test CheckPermission001
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, CheckPermission001, TestSize.Level2)
{
    DTEST_LOG << "CheckPermission001 start" << std::endl;
    uint32_t code = static_cast<uint32_t>(SafwkInterfaceCode::START_ABILITY_TRANSACTION);
    bool result = LocalAbilityManager::GetInstance().CheckPermission(code);
    EXPECT_TRUE(result);
    DTEST_LOG << "CheckPermission001 end" << std::endl;
}

/**
 * @tc.name: CheckPermission002
 * @tc.desc: test CheckPermission002
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, CheckPermission002, TestSize.Level2)
{
    DTEST_LOG << "CheckPermission002 start" << std::endl;
    uint32_t code = static_cast<uint32_t>(SafwkInterfaceCode::SYSTEM_ABILITY_EXT_TRANSACTION);
    bool result = LocalAbilityManager::GetInstance().CheckPermission(code);
    EXPECT_TRUE(result);
    DTEST_LOG << "CheckPermission002 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityExtProc001
 * @tc.desc: test SystemAbilityExtProc001
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SystemAbilityExtProc001, TestSize.Level2)
{
    DTEST_LOG << "SystemAbilityExtProc001 start" << std::endl;
    std::string extension = "ext1";
    SystemAbilityExtensionPara callback;
    bool isAsync = false;
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProc(extension, INVALID_SAID, &callback, isAsync);
    EXPECT_FALSE(ret == ERR_NONE);
    DTEST_LOG << "SystemAbilityExtProc001 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityExtProc002
 * @tc.desc: test SystemAbilityExtProc002
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SystemAbilityExtProc002, TestSize.Level2)
{
    DTEST_LOG << "SystemAbilityExtProc002 start" << std::endl;
    std::string extension = "ext1";
    SystemAbilityExtensionPara callback;
    bool isAsync = false;

    std::string deviceId = "";
    MockSaRealize *sysAby = new MockSaRealize(MUT_SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[MUT_SAID] = sysAby;
    LocalAbilityManager::GetInstance().NotifyAbilityListener(SAID, MUT_SAID, deviceId, STARTCODE);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = sysAby;
    LocalAbilityManager::GetInstance().NotifyAbilityListener(SAID, MUT_SAID, deviceId, STARTCODE);
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProc(extension, SAID, &callback, isAsync);
    EXPECT_TRUE(ret == ERR_NONE);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete sysAby;
    DTEST_LOG << "SystemAbilityExtProc002 end" << std::endl;
}

/**
 * @tc.name: SendStrategyToSAInner001
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SendStrategyToSAInner001, TestSize.Level2)
{
    DTEST_LOG << "SendStrategyToSAInner001 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().SendStrategyToSAInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "SendStrategyToSAInner001 end" << std::endl;
}

/**
 * @tc.name: SendStrategyToSAInner002
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SendStrategyToSAInner002, TestSize.Level2)
{
    DTEST_LOG << "SendStrategyToSAInner002 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int type = 1;
    data.WriteInt32(type);
    int32_t ret = LocalAbilityManager::GetInstance().SendStrategyToSAInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "SendStrategyToSAInner002 end" << std::endl;
}

/**
 * @tc.name: SendStrategyToSAInner003
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SendStrategyToSAInner003, TestSize.Level2)
{
    DTEST_LOG << "SendStrategyToSAInner003 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int type = 1;
    data.WriteInt32(type);
    int32_t saId = -1;
    data.WriteInt32(saId);
    int32_t ret = LocalAbilityManager::GetInstance().SendStrategyToSAInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "SendStrategyToSAInner003 end" << std::endl;
}

/**
 * @tc.name: SendStrategyToSAInner004
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SendStrategyToSAInner004, TestSize.Level2)
{
    DTEST_LOG << "SendStrategyToSAInner004 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int type = 1;
    data.WriteInt32(type);
    int32_t saId = 1;
    data.WriteInt32(saId);
    int32_t ret = LocalAbilityManager::GetInstance().SendStrategyToSAInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "SendStrategyToSAInner004 end" << std::endl;
}

/**
 * @tc.name: SendStrategyToSAInner005
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SendStrategyToSAInner005, TestSize.Level2)
{
    DTEST_LOG << "SendStrategyToSAInner005 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int type = 1;
    data.WriteInt32(type);
    int32_t saId = 1;
    data.WriteInt32(saId);
    int level = 1;
    data.WriteInt32(level);
    int32_t ret = LocalAbilityManager::GetInstance().SendStrategyToSAInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "SendStrategyToSAInner005 end" << std::endl;
}

/**
 * @tc.name: SendStrategyToSAInner006
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SendStrategyToSAInner006, TestSize.Level2)
{
    DTEST_LOG << "SendStrategyToSAInner006 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int type = 1;
    data.WriteInt32(type);
    int32_t saId = 1;
    data.WriteInt32(saId);
    int level = 1;
    data.WriteInt32(level);
    std::string action = "";
    data.WriteString(action);
    int32_t ret = LocalAbilityManager::GetInstance().SendStrategyToSAInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "SendStrategyToSAInner006 end" << std::endl;
}

/**
 * @tc.name: IpcStatCmdProcInner001
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, IpcStatCmdProcInner001, TestSize.Level2)
{
    DTEST_LOG << "IpcStatCmdProcInner001 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().IpcStatCmdProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "IpcStatCmdProcInner001 end" << std::endl;
}

/**
 * @tc.name: IpcStatCmdProcInner002
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, IpcStatCmdProcInner002, TestSize.Level2)
{
    DTEST_LOG << "IpcStatCmdProcInner002 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t fd = 1;
    data.WriteFileDescriptor(fd);
    int32_t ret = LocalAbilityManager::GetInstance().IpcStatCmdProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "IpcStatCmdProcInner002 end" << std::endl;
}

/**
 * @tc.name: IpcStatCmdProcInner003
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, IpcStatCmdProcInner003, TestSize.Level2)
{
    DTEST_LOG << "IpcStatCmdProcInner003 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t fd = 1;
    int cmd = -1;
    data.WriteInt32(cmd);
    data.WriteFileDescriptor(fd);
    int32_t ret = LocalAbilityManager::GetInstance().IpcStatCmdProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "IpcStatCmdProcInner003 end" << std::endl;
}

/**
 * @tc.name: FfrtStatCmdProcInner001
 * @tc.desc: test FfrtStatCmdProcInner.
 * @tc.type: FUNC
 * @tc.require: IBMM2R
 */
HWTEST_F(LocalAbilityManagerStubTest, FfrtStatCmdProcInner001, TestSize.Level2)
{
    DTEST_LOG << "FfrtStatCmdProcInner001 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().FfrtStatCmdProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "FfrtStatCmdProcInner001 end" << std::endl;
}

/**
 * @tc.name: FfrtStatCmdProcInner002
 * @tc.desc: test FfrtStatCmdProcInner.
 * @tc.type: FUNC
 * @tc.require: IBMM2R
 */
HWTEST_F(LocalAbilityManagerStubTest, FfrtStatCmdProcInner002, TestSize.Level2)
{
    DTEST_LOG << "FfrtStatCmdProcInner002 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t fd = 1;
    data.WriteFileDescriptor(fd);
    int32_t ret = LocalAbilityManager::GetInstance().FfrtStatCmdProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "FfrtStatCmdProcInner002 end" << std::endl;
}

/**
 * @tc.name: FfrtStatCmdProcInner003
 * @tc.desc: test FfrtStatCmdProcInner.
 * @tc.type: FUNC
 * @tc.require: IBMM2R
 */
HWTEST_F(LocalAbilityManagerStubTest, FfrtStatCmdProcInner003, TestSize.Level2)
{
    DTEST_LOG << "FfrtStatCmdProcInner003 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t fd = 1;
    int cmd = -1;
    data.WriteInt32(cmd);
    data.WriteFileDescriptor(fd);
    int32_t ret = LocalAbilityManager::GetInstance().FfrtStatCmdProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
    DTEST_LOG << "FfrtStatCmdProcInner003 end" << std::endl;
}

/**
 * @tc.name: FfrtDumperProcInner001
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, FfrtDumperProcInner001, TestSize.Level2)
{
    DTEST_LOG << "FfrtDumperProcInner001 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    std::string ffrtDumperInfo = "Test Ffrt Dumper Info";
    int32_t ret = LocalAbilityManager::GetInstance().FfrtDumperProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
    DTEST_LOG << "FfrtDumperProcInner001 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityExtProcInner001
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SystemAbilityExtProcInner001, TestSize.Level2)
{
    DTEST_LOG << "SystemAbilityExtProcInner001 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProcInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "SystemAbilityExtProcInner001 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityExtProcInner002
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SystemAbilityExtProcInner002, TestSize.Level2)
{
    DTEST_LOG << "SystemAbilityExtProcInner002 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int saId = 1;
    data.WriteInt32(saId);
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProcInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "SystemAbilityExtProcInner002 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityExtProcInner003
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SystemAbilityExtProcInner003, TestSize.Level2)
{
    DTEST_LOG << "SystemAbilityExtProcInner003 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int saId = -1;
    data.WriteInt32(saId);
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProcInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "SystemAbilityExtProcInner003 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityExtProcInner004
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, SystemAbilityExtProcInner004, TestSize.Level2)
{
    DTEST_LOG << "SystemAbilityExtProcInner004 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int saId = 1;
    data.WriteInt32(saId);
    std::string extension = "TestExtension";
    data.WriteString(extension);
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProcInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "SystemAbilityExtProcInner004 end" << std::endl;
}

/**
 * @tc.name: ServiceControlCmdInner001
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, ServiceControlCmdInner001, TestSize.Level2)
{
    DTEST_LOG << "ServiceControlCmdInner001 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().ServiceControlCmdInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "ServiceControlCmdInner001 end" << std::endl;
}

/**
 * @tc.name: ServiceControlCmdInner002
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, ServiceControlCmdInner002, TestSize.Level2)
{
    DTEST_LOG << "ServiceControlCmdInner002 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t systemAbilityId = -1;
    EXPECT_EQ(data.WriteInt32(systemAbilityId), true);
    int32_t ret = LocalAbilityManager::GetInstance().ServiceControlCmdInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "ServiceControlCmdInner002 end" << std::endl;
}

/**
 * @tc.name: ServiceControlCmdInner003
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, ServiceControlCmdInner003, TestSize.Level2)
{
    DTEST_LOG << "ServiceControlCmdInner003 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t systemAbilityId = 1;
    EXPECT_EQ(data.WriteInt32(systemAbilityId), true);
    int32_t ret = LocalAbilityManager::GetInstance().ServiceControlCmdInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "ServiceControlCmdInner003 end" << std::endl;
}

/**
 * @tc.name: ServiceControlCmdInner004
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, ServiceControlCmdInner004, TestSize.Level2)
{
    DTEST_LOG << "ServiceControlCmdInner004 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t systemAbilityId = 1;
    EXPECT_EQ(data.WriteInt32(systemAbilityId), true);
    int32_t fd = -1;
    EXPECT_EQ(data.WriteInt32(fd), true);
    int32_t ret = LocalAbilityManager::GetInstance().ServiceControlCmdInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "ServiceControlCmdInner004 end" << std::endl;
}

/**
 * @tc.name: ServiceControlCmdInner005
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, ServiceControlCmdInner005, TestSize.Level2)
{
    DTEST_LOG << "ServiceControlCmdInner005 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t systemAbilityId = 1;
    EXPECT_EQ(data.WriteInt32(systemAbilityId), true);
    int32_t fd = 1;
    EXPECT_EQ(data.WriteFileDescriptor(fd), true);
    int32_t ret = LocalAbilityManager::GetInstance().ServiceControlCmdInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "ServiceControlCmdInner005 end" << std::endl;
}

/**
 * @tc.name: ServiceControlCmdInner006
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, ServiceControlCmdInner006, TestSize.Level2)
{
    DTEST_LOG << "ServiceControlCmdInner006 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    int32_t systemAbilityId = 1;
    EXPECT_EQ(data.WriteInt32(systemAbilityId), true);
    int32_t fd = 1;
    EXPECT_EQ(data.WriteFileDescriptor(fd), true);
    std::string args = "help";
    std::vector<std::u16string> u16args (1, Str8ToStr16(args));
    EXPECT_EQ(data.WriteString16Vector(u16args), true);
    int32_t ret = LocalAbilityManager::GetInstance().ServiceControlCmdInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "ServiceControlCmdInner006 end" << std::endl;
}

/**
 * @tc.name: ServiceControlCmdInner007
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, ServiceControlCmdInner007, TestSize.Level2)
{
    DTEST_LOG << "ServiceControlCmdInner007 start" << std::endl;
    MessageParcel data;
    MessageParcel reply;
    EXPECT_EQ(data.WriteInt32(SAID), true);
    int32_t fd = 1;
    EXPECT_EQ(data.WriteFileDescriptor(fd), true);
    std::string args = "help";
    std::vector<std::u16string> u16args (1, Str8ToStr16(args));
    EXPECT_EQ(data.WriteString16Vector(u16args), true);
    MockSaRealize *sysAby = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = sysAby;
    int32_t ret = LocalAbilityManager::GetInstance().ServiceControlCmdInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete sysAby;
    DTEST_LOG << "ServiceControlCmdInner007 end" << std::endl;
}

/**
 * @tc.name: ServiceControlCmd001
 * @tc.desc: test ServiceControlCmd001
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, ServiceControlCmd001, TestSize.Level2)
{
    DTEST_LOG << "ServiceControlCmd001 start" << std::endl;
    MockSaRealize *sysAby = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = sysAby;
    int32_t fd = 1;
    std::vector<std::u16string> args (1, Str8ToStr16(std::string("help")));
    int32_t ret = LocalAbilityManager::GetInstance().ServiceControlCmd(fd, SAID, args);
    EXPECT_EQ(ret, ERR_NONE);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete sysAby;
    DTEST_LOG << "ServiceControlCmd001 end" << std::endl;
}

/**
 * @tc.name: ServiceControlCmd002
 * @tc.desc: test ServiceControlCmd002
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerStubTest, ServiceControlCmd002, TestSize.Level2)
{
    DTEST_LOG << "ServiceControlCmd002 start" << std::endl;
    int32_t fd = 1;
    std::vector<std::u16string> args (1, Str8ToStr16(std::string("help")));
    int32_t ret = LocalAbilityManager::GetInstance().ServiceControlCmd(fd, SAID, args);
    EXPECT_EQ(ret, INVALID_DATA);
    DTEST_LOG << "ServiceControlCmd002 end" << std::endl;
}
} // namespace SAFWK
} // namespace OHOS