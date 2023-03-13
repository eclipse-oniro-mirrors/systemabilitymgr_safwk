/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "iservice_registry.h"
#include "local_ability_manager_stub.h"
#include "memory"
#include "sa_mock_permission.h"
#include "test_log.h"

#define private public
#define protected public
#include "local_ability_manager.h"
#include "mock_sa_realize.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace SAFWK {
namespace {
    constexpr int32_t SAID = 1489;
    const std::string TEST_STRING = "test";
    const std::string TEST_RESOURCE_PATH = "/data/test/resource/samgr/profile/";
    constexpr int32_t LISTENER_ID = 1488;
    constexpr int32_t MOCK_DEPEND_TIMEOUT = 1000;
}

class MockLocalAbilityManager : public LocalAbilityManagerStub {
public:
    MockLocalAbilityManager() = default;
    ~MockLocalAbilityManager() = default;

    bool StartAbility(int32_t systemAbilityId, const std::string& eventStr) override;
    bool StopAbility(int32_t systemAbilityId, const std::string& eventStr) override;
    bool ActiveAbility(int32_t systemAbilityId,
        const std::unordered_map<std::string, std::string>& activeReason) override;
    bool IdleAbility(int32_t systemAbilityId,
        const std::unordered_map<std::string, std::string>& idleReason, int32_t& delayTime) override;
};
class SystemAbilityTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SystemAbilityTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SystemAbilityTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SystemAbilityTest::SetUp()
{
    SaMockPermission::MockPermission();
    DTEST_LOG << "SetUp" << std::endl;
}

void SystemAbilityTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

bool MockLocalAbilityManager::StartAbility(int32_t systemAbilityId, const std::string& eventStr)
{
    DTEST_LOG << "said : " << systemAbilityId <<std::endl;
    return true;
}

bool MockLocalAbilityManager::StopAbility(int32_t systemAbilityId, const std::string& eventStr)
{
    DTEST_LOG << "said : " << systemAbilityId <<std::endl;
    return true;
}

bool MockLocalAbilityManager::ActiveAbility(int32_t systemAbilityId,
    const std::unordered_map<std::string, std::string>& activeReason)
{
    DTEST_LOG << "said : " << systemAbilityId <<std::endl;
    return true;
}

bool MockLocalAbilityManager::IdleAbility(int32_t systemAbilityId,
    const std::unordered_map<std::string, std::string>& idleReason, int32_t& delayTime)
{
    DTEST_LOG << "said : " << systemAbilityId <<std::endl;
    return true;
}

/**
 * @tc.name: RemoveSystemAbilityListener001
 * @tc.desc: Check RemoveSystemAbilityListener
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SystemAbilityTest, RemoveSystemAbilityListener001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->AddSystemAbilityListener(LISTENER_ID);
    bool res = sysAby->RemoveSystemAbilityListener(LISTENER_ID);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: MakeAndRegisterAbility001
 * @tc.desc: Check MakeAndRegisterAbility
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SystemAbilityTest, MakeAndRegisterAbility001, TestSize.Level2)
{
    bool res = SystemAbility::MakeAndRegisterAbility(new MockSaRealize(SAID, false));
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: MakeAndRegisterAbility002
 * @tc.desc: Check MakeAndRegisterAbility
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SystemAbilityTest, MakeAndRegisterAbility002, TestSize.Level2)
{
    bool ret = LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(TEST_RESOURCE_PATH + "1489.xml");
    EXPECT_EQ(ret, true);
    bool res = SystemAbility::MakeAndRegisterAbility(new MockSaRealize(SAID, false));
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: Publish001
 * @tc.desc: Verify Publish when systemabitly is nullptr
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SystemAbilityTest, Publish001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    bool ret = sysAby->Publish(nullptr);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: Publish002
 * @tc.desc: Verify Publish function
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SystemAbilityTest, Publish002, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sptr<IRemoteObject> obj(new MockLocalAbilityManager());
    bool ret = sysAby->Publish(obj);
    ASSERT_TRUE(ret);
    sysAby->Stop();
    sysAby->StopAbility(-1);
    sysAby->Start();
    sysAby->Stop();
    EXPECT_EQ(sysAby->abilityState_, SystemAbilityState::NOT_LOADED);
}

/**
 * @tc.name: SetDependTimeout001
 * @tc.desc: Verify SetDependTimeout
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SystemAbilityTest, SetDependTimeout001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->SetDependTimeout(0);
    sysAby->SetDependTimeout(MOCK_DEPEND_TIMEOUT);
    EXPECT_EQ(sysAby->GetDependTimeout(), MOCK_DEPEND_TIMEOUT);
}

/**
 * @tc.name: GetSystemAbility001
 * @tc.desc: Check GetSystemAbility
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SystemAbilityTest, GetSystemAbility001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sptr<IRemoteObject> obj = sysAby->GetSystemAbility(-1);
    EXPECT_TRUE(obj == nullptr);
}

/**
 * @tc.name: CancelIdle001
 * @tc.desc: test CancelIdle with abilityState_ is not SystemAbilityState::IDLE
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, CancelIdle001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->abilityState_ = SystemAbilityState::NOT_LOADED;
    int32_t ret = sysAby->CancelIdle();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CancelIdle002
 * @tc.desc: test CancelIdle with abilityState_ is SystemAbilityState::IDLE
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, CancelIdle002, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->abilityState_ = SystemAbilityState::IDLE;
    int32_t ret = sysAby->CancelIdle();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: Start001
 * @tc.desc: test Start with abilityState_ is not SystemAbilityState::NOT_LOADED
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, Start001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->abilityState_ = SystemAbilityState::IDLE;
    sysAby->Start();
    EXPECT_FALSE(sysAby->isRunning_);
}

/**
 * @tc.name: Start002
 * @tc.desc: test Start with is SystemAbilityState::NOT_LOADED
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, Start002, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->abilityState_ = SystemAbilityState::NOT_LOADED;
    sysAby->Start();
    EXPECT_TRUE(sysAby->isRunning_);
}

/**
 * @tc.name: Idle001
 * @tc.desc: test Idle with abilityState_ is not SystemAbilityState::ACTIVE
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, Idle001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->abilityState_ = SystemAbilityState::IDLE;
    std::unordered_map<std::string, std::string> idleReason;
    int32_t delayTime = 123;
    sysAby->Idle(idleReason, delayTime);
    EXPECT_EQ(sysAby->abilityState_,  SystemAbilityState::IDLE);
}

/**
 * @tc.name: Idle002
 * @tc.desc: test Idle with abilityState_ is SystemAbilityState::ACTIVE and delayTime is 0
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, Idle002, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->abilityState_ = SystemAbilityState::ACTIVE;
    std::unordered_map<std::string, std::string> idleReason;
    idleReason[TEST_STRING] = TEST_STRING;
    int32_t noDelayTime = 0;
    sysAby->Idle(idleReason, noDelayTime);
    EXPECT_EQ(sysAby->abilityState_, SystemAbilityState::IDLE);
}

/**
 * @tc.name: Idle003
 * @tc.desc: test Idle with abilityState_ is SystemAbilityState::ACTIVE and delayTime is 0
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, Idle003, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->abilityState_ = SystemAbilityState::ACTIVE;
    std::unordered_map<std::string, std::string> idleReason;
    idleReason[TEST_STRING] = TEST_STRING;
    int32_t delayTime = 123;
    sysAby->Idle(idleReason, delayTime);
    EXPECT_EQ(sysAby->abilityState_, SystemAbilityState::IDLE);
}

/**
 * @tc.name: Active001
 * @tc.desc: test Active with abilityState_ is not SystemAbilityState::IDLE
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, Active001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->abilityState_ = SystemAbilityState::ACTIVE;
    std::unordered_map<std::string, std::string> activeReason;
    sysAby->Active(activeReason);
    EXPECT_EQ(sysAby->abilityState_, SystemAbilityState::ACTIVE);
}

/**
 * @tc.name: Active002
 * @tc.desc: test Active with abilityState_ is SystemAbilityState::IDLE
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, Active002, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->abilityState_ = SystemAbilityState::IDLE;
    std::unordered_map<std::string, std::string> activeReason;
    sysAby->Active(activeReason);
    EXPECT_EQ(sysAby->abilityState_, SystemAbilityState::ACTIVE);
}

/**
 * @tc.name: GetLibPath001
 * @tc.desc: Check GetLibPath,cover function
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, GetLibPath001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->libPath_ = TEST_RESOURCE_PATH;
    EXPECT_EQ(sysAby->libPath_, TEST_RESOURCE_PATH);
}

/**
 * @tc.name: IsRunOnCreate001
 * @tc.desc: Check IsRunOnCreate,cover function
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, IsRunOnCreate001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    sysAby->isRunOnCreate_ = true;
    EXPECT_TRUE(sysAby->isRunOnCreate_);
}

/**
 * @tc.name: OnIdle001
 * @tc.desc: Check OnIdle,cover function
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(SystemAbilityTest, OnIdle001, TestSize.Level2)
{
    std::shared_ptr<SystemAbility> sysAby = std::make_shared<MockSaRealize>(SAID, false);
    std::unordered_map<std::string, std::string> idleReason;
    int32_t ret = sysAby->OnIdle(idleReason);
    EXPECT_EQ(ret, 0);
}
}
}