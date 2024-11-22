/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
    constexpr int VAILD_SAID = 401;
    constexpr int SAID = 1499;
    constexpr int MUT_SAID = 9999;
    constexpr int INVALID_SAID = -1;
    constexpr int STARTCODE = 1;
    constexpr uint32_t BOOTPHASE = 1;
    constexpr uint32_t OTHERPHASE = 3;
}

class LocalAbilityManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void LocalAbilityManagerTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void LocalAbilityManagerTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void LocalAbilityManagerTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void LocalAbilityManagerTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/**
 * @tc.name: CheckTrustSa001
 * @tc.desc:  CheckTrustSa with not all allow
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckTrustSa001, TestSize.Level1)
{
    DTEST_LOG << "CheckTrustSa001 start" << std::endl;
    /**
     * @tc.steps: step1. parse multi-sa profile
     * @tc.expected: step1. return true when load multi-sa profile
     */
    bool ret = LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles
        (TEST_RESOURCE_PATH + "multi_sa_profile.json");
    EXPECT_TRUE(ret);
    /**
     * @tc.steps: step2. CheckTrustSa with not all allow
     * @tc.expected: step2. load allowed sa
     */
    auto profiles = LocalAbilityManager::GetInstance().profileParser_->GetAllSaProfiles();
    auto path = TEST_RESOURCE_PATH + "test_trust_not_all_allow.json";
    auto process = "test";
    LocalAbilityManager::GetInstance().CheckTrustSa(path, process, profiles);
    profiles = LocalAbilityManager::GetInstance().profileParser_->GetAllSaProfiles();
    EXPECT_EQ(profiles.size(), 2);
    DTEST_LOG << "CheckTrustSa001 end" << std::endl;
}

/**
 * @tc.name: CheckTrustSa002
 * @tc.desc:  CheckTrustSa with all allow
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckTrustSa002, TestSize.Level1)
{
    DTEST_LOG << "CheckTrustSa002 start" << std::endl;
    LocalAbilityManager::GetInstance().profileParser_->ClearResource();
    /**
     * @tc.steps: step1. parse multi-sa profile
     * @tc.expected: step1. return true when load multi-sa profile
     */
    bool ret = LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles
        (TEST_RESOURCE_PATH + "multi_sa_profile.json");
    EXPECT_TRUE(ret);
    /**
     * @tc.steps: step2. CheckTrustSa with all allow
     * @tc.expected: step2. load all sa
     */
    auto profiles = LocalAbilityManager::GetInstance().profileParser_->GetAllSaProfiles();
    auto path = TEST_RESOURCE_PATH + "test_trust_all_allow.json";
    auto process = "test";
    LocalAbilityManager::GetInstance().CheckTrustSa(path, process, profiles);
    auto result = LocalAbilityManager::GetInstance().profileParser_->GetAllSaProfiles();
    EXPECT_EQ(result.size(), 4);
    DTEST_LOG << "CheckTrustSa002 end" << std::endl;
}

/**
 * @tc.name: DoStartSAProcess001
 * @tc.desc:  DoStartSAProcess001
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, DoStartSAProcess001, TestSize.Level2)
{
    DTEST_LOG << "DoStartSAProcess001 start" << std::endl;
    LocalAbilityManager::GetInstance().DoStartSAProcess("profile_audio.json", SAID);
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_TRUE(sm != nullptr);
    auto ability = sm->GetSystemAbility(SAID);
    EXPECT_TRUE(ability == nullptr);
    DTEST_LOG << "DoStartSAProcess001 end" << std::endl;
}

/**
 * @tc.name: DoStartSAProcess002
 * @tc.desc:  DoStartSAProcess002
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, DoStartSAProcess002, TestSize.Level2)
{
    DTEST_LOG << "DoStartSAProcess002 start" << std::endl;
    string profilePath = "/system/usr/profile_audio.json";
    LocalAbilityManager::GetInstance().DoStartSAProcess(profilePath, SAID);
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto ability = sm->GetSystemAbility(SAID);
    EXPECT_EQ(ability, nullptr);
    DTEST_LOG << "DoStartSAProcess002 end" << std::endl;
}

/**
 * @tc.name: DoStartSAProcess003
 * @tc.desc:  DoStartSAProcess, InitializeSaProfiles failed!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, DoStartSAProcess003, TestSize.Level2)
{
    DTEST_LOG << "DoStartSAProcess003 start" << std::endl;
    string profilePath = "/system/usr/profile_audio.json";
    int32_t invalidSaid = -2;
    LocalAbilityManager::GetInstance().DoStartSAProcess(profilePath, invalidSaid);
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto ability = sm->GetSystemAbility(invalidSaid);
    EXPECT_EQ(ability, nullptr);
    DTEST_LOG << "DoStartSAProcess003 end" << std::endl;
}

/**
 * @tc.name: GetTraceTag001
 * @tc.desc:  GetTraceTag
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetTraceTag001, TestSize.Level1)
{
    DTEST_LOG << "GetTraceTag001 start" << std::endl;
    string profilePath = "/system/usr/profile_audio.json";
    string traceTag = LocalAbilityManager::GetInstance().GetTraceTag(profilePath);
    EXPECT_EQ(traceTag, "profile_audio");
    DTEST_LOG << "GetTraceTag001 end" << std::endl;
}

/**
 * @tc.name: GetTraceTag002
 * @tc.desc:  GetTraceTag
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetTraceTag002, TestSize.Level1)
{
    DTEST_LOG << "GetTraceTag002 start" << std::endl;
    string profilePath = "";
    string traceTag = LocalAbilityManager::GetInstance().GetTraceTag(profilePath);
    EXPECT_EQ(traceTag, "default_proc");
    DTEST_LOG << "GetTraceTag002 end" << std::endl;
}

/**
 * @tc.name: GetTraceTag003
 * @tc.desc:  GetTraceTag
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetTraceTag003, TestSize.Level1)
{
    DTEST_LOG << "GetTraceTag003 start" << std::endl;
    string profilePath = "/system/usr/test";
    string traceTag = LocalAbilityManager::GetInstance().GetTraceTag(profilePath);
    EXPECT_EQ(traceTag, "test");
    DTEST_LOG << "GetTraceTag003 end" << std::endl;
}

/**
 * @tc.name: CheckAndGetProfilePath001
 * @tc.desc:  CheckAndGetProfilePath
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckAndGetProfilePath001, TestSize.Level3)
{
    DTEST_LOG << "CheckAndGetProfilePath001 start" << std::endl;
    string profilePath = "/system/usr/profile_audio.json";
    string realProfilePath = "";
    bool res = LocalAbilityManager::GetInstance().CheckAndGetProfilePath(profilePath, realProfilePath);
    EXPECT_TRUE(res);
    DTEST_LOG << "CheckAndGetProfilePath001 end" << std::endl;
}

/**
 * @tc.name: CheckAndGetProfilePath002
 * @tc.desc:  CheckAndGetProfilePath, Doc dir is not matched!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckAndGetProfilePath002, TestSize.Level1)
{
    DTEST_LOG << "CheckAndGetProfilePath002 start" << std::endl;
    string profilePath = TEST_RESOURCE_PATH + "test_trust_all_allow.json";
    string realProfilePath = "";
    bool res = LocalAbilityManager::GetInstance().CheckAndGetProfilePath(profilePath, realProfilePath);
    EXPECT_FALSE(res);
    DTEST_LOG << "CheckAndGetProfilePath002 end" << std::endl;
}

/**
 * @tc.name: CheckAndGetProfilePath003
 * @tc.desc: test CheckAndGetProfilePath, with invalid profilepath length
 * @tc.type: FUNC
 * @tc.require: I73XRZ
 */
HWTEST_F(LocalAbilityManagerTest, CheckAndGetProfilePath003, TestSize.Level2)
{
    DTEST_LOG << "CheckAndGetProfilePath003 start" << std::endl;
    string profilePath;
    for (; profilePath.size() <= PATH_MAX;) {
        profilePath = profilePath + TEST_RESOURCE_PATH;
    }
    string realProfilePath = "";
    bool res = LocalAbilityManager::GetInstance().CheckAndGetProfilePath(profilePath, realProfilePath);
    // cover ClearResource
    LocalAbilityManager::GetInstance().ClearResource();
    EXPECT_FALSE(res);
    DTEST_LOG << "CheckAndGetProfilePath003 end" << std::endl;
}

/**
 * @tc.name: CheckSystemAbilityManagerReady001
 * @tc.desc:  CheckSystemAbilityManagerReady, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckSystemAbilityManagerReady001, TestSize.Level3)
{
    DTEST_LOG << "CheckSystemAbilityManagerReady001 start" << std::endl;
    string profilePath = TEST_RESOURCE_PATH + "test_trust_all_allow.json";
    string realProfilePath = "";
    bool res = LocalAbilityManager::GetInstance().CheckSystemAbilityManagerReady();
    EXPECT_TRUE(res);
    DTEST_LOG << "CheckSystemAbilityManagerReady001 end" << std::endl;
}

/**
 * @tc.name: InitSystemAbilityProfiles001
 * @tc.desc:  InitSystemAbilityProfiles, ParseSaProfiles failed!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitSystemAbilityProfiles001, TestSize.Level1)
{
    DTEST_LOG << "InitSystemAbilityProfiles001 start" << std::endl;
    string profilePath = "";
    bool res = LocalAbilityManager::GetInstance().InitSystemAbilityProfiles(profilePath, SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "InitSystemAbilityProfiles001 end" << std::endl;
}

/**
 * @tc.name: InitSystemAbilityProfiles002
 * @tc.desc:  InitSystemAbilityProfiles, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitSystemAbilityProfiles002, TestSize.Level3)
{
    DTEST_LOG << "InitSystemAbilityProfiles002 start" << std::endl;
    string profilePath = "/system/usr/profile_audio.json";
    bool res = LocalAbilityManager::GetInstance().InitSystemAbilityProfiles(profilePath, SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "InitSystemAbilityProfiles002 end" << std::endl;
}

/**
 * @tc.name: InitSystemAbilityProfiles003
 * @tc.desc:  InitSystemAbilityProfiles, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitSystemAbilityProfiles003, TestSize.Level3)
{
    DTEST_LOG << "InitSystemAbilityProfiles003 start" << std::endl;
    string profilePath = "/system/usr/profile_audio.json";
    int32_t defaultId = -1;
    bool res = LocalAbilityManager::GetInstance().InitSystemAbilityProfiles(profilePath, defaultId);
    EXPECT_TRUE(res);
    DTEST_LOG << "InitSystemAbilityProfiles003 end" << std::endl;
}

/**
 * @tc.name: AddAbility001
 * @tc.desc: AddAbility, try to add null ability!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddAbility001, TestSize.Level3)
{
    DTEST_LOG << "AddAbility001 start" << std::endl;
    string path = "";
    string process = "process";
    std::list<SaProfile> saInfos;
    LocalAbilityManager::GetInstance().CheckTrustSa(path, process, saInfos);
    bool res = LocalAbilityManager::GetInstance().AddAbility(nullptr);
    EXPECT_FALSE(res);
    DTEST_LOG << "AddAbility001 end" << std::endl;
}

/**
 * @tc.name: AddAbility002
 * @tc.desc: AddAbility, try to add existed ability
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddAbility002, TestSize.Level3)
{
    DTEST_LOG << "AddAbility002 start" << std::endl;
    MockSaRealize *sysAby = new MockSaRealize(MUT_SAID, false);
    bool ret = LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles
        (TEST_RESOURCE_PATH + "multi_sa_profile.json");
    EXPECT_TRUE(ret);
    LocalAbilityManager::GetInstance().abilityMap_[MUT_SAID] = sysAby;
    bool res = LocalAbilityManager::GetInstance().AddAbility(sysAby);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete sysAby;
    EXPECT_FALSE(res);
    DTEST_LOG << "AddAbility002 end" << std::endl;
}

/**
 * @tc.name: RemoveAbility001
 * @tc.desc: RemoveAbility, invalid systemAbilityId
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveAbility001, TestSize.Level1)
{
    DTEST_LOG << "RemoveAbility001 start" << std::endl;
    bool res = LocalAbilityManager::GetInstance().RemoveAbility(INVALID_SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "RemoveAbility001 end" << std::endl;
}

/**
 * @tc.name: RemoveAbility002
 * @tc.desc: RemoveAbility, invalid systemAbilityId
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveAbility002, TestSize.Level3)
{
    DTEST_LOG << "RemoveAbility002 start" << std::endl;
    bool res = LocalAbilityManager::GetInstance().RemoveAbility(SAID);
    EXPECT_TRUE(res);
    DTEST_LOG << "RemoveAbility002 end" << std::endl;
}

/**
 * @tc.name: AddSystemAbilityListener001
 * @tc.desc: AddSystemAbilityListener, SA or listenerSA invalid
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddSystemAbilityListener001, TestSize.Level1)
{
    DTEST_LOG << "AddSystemAbilityListener001 start" << std::endl;
    bool res = LocalAbilityManager::GetInstance().AddSystemAbilityListener(SAID, INVALID_SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "AddSystemAbilityListener001 end" << std::endl;
}

/**
 * @tc.name: AddSystemAbilityListener002
 * @tc.desc: AddSystemAbilityListener, SA or listenerSA invalid
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddSystemAbilityListener002, TestSize.Level1)
{
    DTEST_LOG << "AddSystemAbilityListener002 start" << std::endl;
    bool res = LocalAbilityManager::GetInstance().AddSystemAbilityListener(INVALID_SAID, SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "AddSystemAbilityListener002 end" << std::endl;
}

/**
 * @tc.name: AddSystemAbilityListener003
 * @tc.desc: AddSystemAbilityListener, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddSystemAbilityListener003, TestSize.Level1)
{
    DTEST_LOG << "AddSystemAbilityListener003 start" << std::endl;
    LocalAbilityManager::GetInstance().listenerMap_[SAID].push_back({MUT_SAID, ListenerState::INIT});
    bool res = LocalAbilityManager::GetInstance().AddSystemAbilityListener(SAID, SAID);
    EXPECT_TRUE(res);
    DTEST_LOG << "AddSystemAbilityListener003 end" << std::endl;
}

/**
 * @tc.name: AddSystemAbilityListener004
 * @tc.desc: AddSystemAbilityListener, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddSystemAbilityListener004, TestSize.Level1)
{
    DTEST_LOG << "AddSystemAbilityListener004 start" << std::endl;
    LocalAbilityManager::GetInstance().listenerMap_[VAILD_SAID].push_back({VAILD_SAID, ListenerState::INIT});
    LocalAbilityManager::GetInstance().listenerMap_[VAILD_SAID].push_back({SAID, ListenerState::INIT});
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    bool res = LocalAbilityManager::GetInstance().AddSystemAbilityListener(VAILD_SAID, VAILD_SAID);
    EXPECT_TRUE(res);
    DTEST_LOG << "AddSystemAbilityListener004 end" << std::endl;
}

/**
 * @tc.name: RemoveSystemAbilityListener001
 * @tc.desc: RemoveSystemAbilityListener, SA or listenerSA invalid
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveSystemAbilityListener001, TestSize.Level1)
{
    DTEST_LOG << "RemoveSystemAbilityListener001 start" << std::endl;
    bool res = LocalAbilityManager::GetInstance().RemoveSystemAbilityListener(INVALID_SAID, SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "RemoveSystemAbilityListener001 end" << std::endl;
}

/**
 * @tc.name: RemoveSystemAbilityListener002
 * @tc.desc: RemoveSystemAbilityListener, SA or listenerSA invalid
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveSystemAbilityListener002, TestSize.Level1)
{
    DTEST_LOG << "RemoveSystemAbilityListener002 start" << std::endl;
    bool res = LocalAbilityManager::GetInstance().RemoveSystemAbilityListener(SAID, INVALID_SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "RemoveSystemAbilityListener002 end" << std::endl;
}

/**
 * @tc.name: RemoveSystemAbilityListener003
 * @tc.desc: RemoveSystemAbilityListener, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveSystemAbilityListener003, TestSize.Level1)
{
    DTEST_LOG << "RemoveSystemAbilityListener003 start" << std::endl;
    LocalAbilityManager::GetInstance().listenerMap_.clear();
    bool res = LocalAbilityManager::GetInstance().RemoveSystemAbilityListener(SAID, SAID);
    EXPECT_TRUE(res);
    DTEST_LOG << "RemoveSystemAbilityListener003 end" << std::endl;
}

/**
 * @tc.name: RemoveSystemAbilityListener004
 * @tc.desc: RemoveSystemAbilityListener, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveSystemAbilityListener004, TestSize.Level3)
{
    DTEST_LOG << "RemoveSystemAbilityListener004 start" << std::endl;
    LocalAbilityManager::GetInstance().listenerMap_[SAID].push_back({MUT_SAID, ListenerState::INIT});
    bool res = LocalAbilityManager::GetInstance().RemoveSystemAbilityListener(SAID, SAID);
    EXPECT_TRUE(res);
    DTEST_LOG << "RemoveSystemAbilityListener004 end" << std::endl;
}

/**
 * @tc.name: FindAndNotifyAbilityListeners001
 * @tc.desc: test FindAndNotifyAbilityListeners with listenerMap_ is empty
 * @tc.type: FUNC
 * @tc.require: I7G7DL
 */
HWTEST_F(LocalAbilityManagerTest, FindAndNotifyAbilityListeners001, TestSize.Level2)
{
    int32_t code = 1;
    std::string deviceId = "";
    LocalAbilityManager::GetInstance().listenerMap_.clear();
    LocalAbilityManager::GetInstance().FindAndNotifyAbilityListeners(SAID, deviceId, code);
    EXPECT_TRUE(LocalAbilityManager::GetInstance().listenerMap_.empty());
}

/**
 * @tc.name: OnStartAbility001
 * @tc.desc: OnStartAbility, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, OnStartAbility001, TestSize.Level1)
{
    DTEST_LOG << "OnStartAbility001 start" << std::endl;
    std::string deviceId = "";
    MockSaRealize *sysAby = new MockSaRealize(MUT_SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[MUT_SAID] = sysAby;
    LocalAbilityManager::GetInstance().NotifyAbilityListener(SAID, MUT_SAID, deviceId, STARTCODE);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = sysAby;
    LocalAbilityManager::GetInstance().NotifyAbilityListener(SAID, MUT_SAID, deviceId, STARTCODE);
    bool res = LocalAbilityManager::GetInstance().OnStartAbility(SAID);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete sysAby;
    EXPECT_TRUE(res);
    DTEST_LOG << "OnStartAbility001 end" << std::endl;
}

/**
 * @tc.name: OnStartAbility002
 * @tc.desc: OnStartAbility, return false
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, OnStartAbility002, TestSize.Level1)
{
    DTEST_LOG << "OnStartAbility002 start" << std::endl;
    std::string deviceId = "";
    int32_t removeAbility = 2;
    int32_t otherAbility = 3;
    MockSaRealize *sysAby = new MockSaRealize(MUT_SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[MUT_SAID] = sysAby;
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = sysAby;
    LocalAbilityManager::GetInstance().NotifyAbilityListener(SAID, MUT_SAID, deviceId, removeAbility);
    LocalAbilityManager::GetInstance().NotifyAbilityListener(SAID, MUT_SAID, deviceId, otherAbility);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    bool res = LocalAbilityManager::GetInstance().OnStartAbility(SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "OnStartAbility002 end" << std::endl;
}

/**
 * @tc.name: GetAbility001
 * @tc.desc: GetAbility, SA not register
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetAbility001, TestSize.Level1)
{
    DTEST_LOG << "GetAbility001 start" << std::endl;
    std::string deviceId = "";
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    SystemAbility* res = LocalAbilityManager::GetInstance().GetAbility(SAID);
    EXPECT_EQ(res, nullptr);
    DTEST_LOG << "GetAbility001 end" << std::endl;
}

/**
 * @tc.name: GetAbility002
 * @tc.desc: GetAbility, SA not register
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetAbility002, TestSize.Level1)
{
    DTEST_LOG << "GetAbility002 start" << std::endl;
    std::string deviceId = "";
    MockSaRealize *sysAby = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = sysAby;
    SystemAbility* res = LocalAbilityManager::GetInstance().GetAbility(SAID);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete sysAby;
    EXPECT_NE(res, nullptr);
    DTEST_LOG << "GetAbility002 end" << std::endl;
}

/**
 * @tc.name: GetRunningStatus001
 * @tc.desc: GetRunningStatus, return false
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetRunningStatus001, TestSize.Level1)
{
    DTEST_LOG << "GetRunningStatus001 start" << std::endl;
    std::string deviceId = "";
    bool res = LocalAbilityManager::GetInstance().GetRunningStatus(SAID);
    LocalAbilityManager::GetInstance().listenerMap_.clear();
    EXPECT_FALSE(res);
    DTEST_LOG << "GetRunningStatus001 end" << std::endl;
}

/**
 * @tc.name: GetRunningStatus002
 * @tc.desc: GetRunningStatus, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetRunningStatus002, TestSize.Level1)
{
    DTEST_LOG << "GetRunningStatus002 start" << std::endl;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    bool res = LocalAbilityManager::GetInstance().GetRunningStatus(SAID);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete mockSa;
    EXPECT_FALSE(res);
    DTEST_LOG << "GetRunningStatus002 end" << std::endl;
}

/**
 * @tc.name: StartOndemandSystemAbility001
 * @tc.desc: StartOndemandSystemAbility
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, StartOndemandSystemAbility001, TestSize.Level1)
{
    DTEST_LOG << "StartOndemandSystemAbility001 start" << std::endl;
    std::string profilePath = "/system/usr/profile_audio.json";
    LocalAbilityManager::GetInstance().profileParser_->saProfiles_.clear();
    LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    LocalAbilityManager::GetInstance().StartOndemandSystemAbility(SAID);
    EXPECT_EQ(LocalAbilityManager::GetInstance().profileParser_->saProfiles_.size(), 3);
    DTEST_LOG << "StartOndemandSystemAbility001 end" << std::endl;
}

/**
 * @tc.name: StartOndemandSystemAbility002
 * @tc.desc: StartOndemandSystemAbility
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, StartOndemandSystemAbility002, TestSize.Level1)
{
    DTEST_LOG << "StartOndemandSystemAbility002 start" << std::endl;
    std::string profilePath = "/system/usr/profile_audio.json";
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().profileParser_->saProfiles_.clear();
    LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    LocalAbilityManager::GetInstance().StartOndemandSystemAbility(SAID);
    delete mockSa;
    EXPECT_EQ(LocalAbilityManager::GetInstance().profileParser_->saProfiles_.size(), 3);
    DTEST_LOG << "StartOndemandSystemAbility002 end" << std::endl;
}

/**
 * @tc.name: InitializeSaProfiles001
 * @tc.desc: InitializeSaProfiles, sa profile is empty
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfiles001, TestSize.Level1)
{
    DTEST_LOG << "InitializeSaProfiles001 start" << std::endl;
    LocalAbilityManager::GetInstance().profileParser_->saProfiles_.clear();
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfiles(INVALID_SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "InitializeSaProfiles001 end" << std::endl;
}

/**
 * @tc.name: InitializeSaProfiles002
 * @tc.desc: InitializeSaProfiles
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfiles002, TestSize.Level1)
{
    DTEST_LOG << "InitializeSaProfiles002 start" << std::endl;
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfiles(SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "InitializeSaProfiles002 end" << std::endl;
}

/**
 * @tc.name: InitializeRunOnCreateSaProfiles001
 * @tc.desc: InitializeSaProfiles
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeRunOnCreateSaProfiles001, TestSize.Level1)
{
    DTEST_LOG << "InitializeRunOnCreateSaProfiles001 start" << std::endl;
    LocalAbilityManager::GetInstance().profileParser_->saProfiles_.clear();
    bool res = LocalAbilityManager::GetInstance().InitializeRunOnCreateSaProfiles(OTHERPHASE);
    EXPECT_FALSE(res);
    DTEST_LOG << "InitializeRunOnCreateSaProfiles001 end" << std::endl;
}

/**
 * @tc.name: InitializeRunOnCreateSaProfiles002
 * @tc.desc: InitializeSaProfiles
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeRunOnCreateSaProfiles002, TestSize.Level1)
{
    DTEST_LOG << "InitializeRunOnCreateSaProfiles002 start" << std::endl;
    std::string profilePath = "/system/usr/profile_audio.json";
    LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    bool res = LocalAbilityManager::GetInstance().InitializeRunOnCreateSaProfiles(OTHERPHASE);
    EXPECT_TRUE(res);
    DTEST_LOG << "InitializeRunOnCreateSaProfiles002 end" << std::endl;
}

/**
 * @tc.name: InitializeRunOnCreateSaProfiles003
 * @tc.desc: InitializeSaProfiles
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeRunOnCreateSaProfiles003, TestSize.Level1)
{
    DTEST_LOG << "InitializeRunOnCreateSaProfiles003 start" << std::endl;
    std::string profilePath = "/system/usr/profile_audio.json";
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    bool res = LocalAbilityManager::GetInstance().InitializeRunOnCreateSaProfiles(OTHERPHASE);
    delete mockSa;
    EXPECT_TRUE(res);
    DTEST_LOG << "InitializeRunOnCreateSaProfiles003 end" << std::endl;
}

/**
 * @tc.name: InitializeOnDemandSaProfile001
 * @tc.desc: InitializeOnDemandSaProfile
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeOnDemandSaProfile001, TestSize.Level1)
{
    DTEST_LOG << "InitializeOnDemandSaProfile001 start" << std::endl;
    std::string profilePath = "/system/usr/profile_audio.json";
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    bool res = LocalAbilityManager::GetInstance().InitializeOnDemandSaProfile(SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "InitializeOnDemandSaProfile001 end" << std::endl;
}

/**
 * @tc.name: InitializeOnDemandSaProfile002
 * @tc.desc: InitializeOnDemandSaProfile
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeOnDemandSaProfile002, TestSize.Level1)
{
    DTEST_LOG << "InitializeOnDemandSaProfile002 start" << std::endl;
    LocalAbilityManager::GetInstance().profileParser_->saProfiles_.clear();
    bool res = LocalAbilityManager::GetInstance().InitializeOnDemandSaProfile(SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "InitializeOnDemandSaProfile002 end" << std::endl;
}

/**
 * @tc.name: InitializeSaProfilesInnerLocked001
 * @tc.desc: InitializeSaProfilesInnerLocked, SA not found!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfilesInnerLocked001, TestSize.Level1)
{
    DTEST_LOG << "InitializeSaProfilesInnerLocked001 start" << std::endl;
    SaProfile saProfile;
    saProfile.saId = SAID;
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    EXPECT_FALSE(res);
    DTEST_LOG << "InitializeSaProfilesInnerLocked001 end" << std::endl;
}

/**
 * @tc.name: InitializeSaProfilesInnerLocked002
 * @tc.desc: InitializeSaProfilesInnerLocked, SA is null!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfilesInnerLocked002, TestSize.Level1)
{
    DTEST_LOG << "InitializeSaProfilesInnerLocked002 start" << std::endl;
    SaProfile saProfile;
    saProfile.saId = SAID;
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = nullptr;
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    EXPECT_FALSE(res);
    DTEST_LOG << "InitializeSaProfilesInnerLocked002 end" << std::endl;
}

/**
 * @tc.name: InitializeSaProfilesInnerLocked003
 * @tc.desc: InitializeSaProfilesInnerLocked, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfilesInnerLocked003, TestSize.Level1)
{
    DTEST_LOG << "InitializeSaProfilesInnerLocked003 start" << std::endl;
    SaProfile saProfile;
    saProfile.saId = SAID;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete mockSa;
    EXPECT_TRUE(res);
    DTEST_LOG << "InitializeSaProfilesInnerLocked003 end" << std::endl;
}

/**
 * @tc.name: InitializeSaProfilesInnerLocked004
 * @tc.desc: InitializeSaProfilesInnerLocked, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfilesInnerLocked004, TestSize.Level1)
{
    DTEST_LOG << "InitializeSaProfilesInnerLocked004 start" << std::endl;
    SaProfile saProfile;
    saProfile.saId = SAID;
    saProfile.bootPhase = BOOTPHASE;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete mockSa;
    EXPECT_TRUE(res);
    DTEST_LOG << "InitializeSaProfilesInnerLocked004 end" << std::endl;
}

/**
 * @tc.name: InitializeSaProfilesInnerLocked005
 * @tc.desc: InitializeSaProfilesInnerLocked, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfilesInnerLocked005, TestSize.Level1)
{
    DTEST_LOG << "InitializeSaProfilesInnerLocked005 start" << std::endl;
    SaProfile saProfile;
    saProfile.saId = SAID;
    saProfile.bootPhase = BOOTPHASE;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete mockSa;
    EXPECT_TRUE(res);
    DTEST_LOG << "InitializeSaProfilesInnerLocked005 end" << std::endl;
}

/**
 * @tc.name: CheckDependencyStatus001
 * @tc.desc: CheckDependencyStatus, return size is 1!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckDependencyStatus001, TestSize.Level1)
{
    DTEST_LOG << "CheckDependencyStatus001 start" << std::endl;
    std::string profilePath = "/system/usr/profile_audio.json";
    vector<int32_t> dependSa;
    dependSa.push_back(1499);
    bool ret = LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    EXPECT_TRUE(ret);
    LocalAbilityManager::GetInstance().RegisterOnDemandSystemAbility(SAID);
    vector<int32_t> res = LocalAbilityManager::GetInstance().CheckDependencyStatus(dependSa);
    EXPECT_EQ(res.size(), STARTCODE);
    DTEST_LOG << "CheckDependencyStatus001 end" << std::endl;
}

/**
 * @tc.name: CheckDependencyStatus002
 * @tc.desc: CheckDependencyStatus, return size is 0!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckDependencyStatus002, TestSize.Level1)
{
    DTEST_LOG << "CheckDependencyStatus002 start" << std::endl;
    vector<int32_t> dependSa;
    dependSa.push_back(-1);
    vector<int32_t> res = LocalAbilityManager::GetInstance().CheckDependencyStatus(dependSa);
    EXPECT_EQ(res.size(), 0);
    DTEST_LOG << "CheckDependencyStatus002 end" << std::endl;
}

/**
 * @tc.name: CheckDependencyStatus003
 * @tc.desc: CheckDependencyStatus, return size is 0!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckDependencyStatus003, TestSize.Level1)
{
    DTEST_LOG << "CheckDependencyStatus003 start" << std::endl;
    vector<int32_t> dependSa;
    dependSa.push_back(401);
    vector<int32_t> res = LocalAbilityManager::GetInstance().CheckDependencyStatus(dependSa);
    EXPECT_EQ(res.size(), 0);
    DTEST_LOG << "CheckDependencyStatus003 end" << std::endl;
}

/**
 * @tc.name: NeedRegisterOnDemand001
 * @tc.desc: NeedRegisterOnDemand, return false!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, NeedRegisterOnDemand001, TestSize.Level1)
{
    DTEST_LOG << "NeedRegisterOnDemand001 start" << std::endl;
    SaProfile saProfile;
    saProfile.runOnCreate = true;
    LocalAbilityManager::GetInstance().StartSystemAbilityTask(nullptr);
    bool res = LocalAbilityManager::GetInstance().NeedRegisterOnDemand(saProfile, INVALID_SAID);
    EXPECT_FALSE(res);
    DTEST_LOG << "NeedRegisterOnDemand001 end" << std::endl;
}

/**
 * @tc.name: NeedRegisterOnDemand002
 * @tc.desc: NeedRegisterOnDemand, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, NeedRegisterOnDemand002, TestSize.Level1)
{
    DTEST_LOG << "NeedRegisterOnDemand002 start" << std::endl;
    SaProfile saProfile;
    saProfile.runOnCreate = false;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().StartSystemAbilityTask(mockSa);
    bool res = LocalAbilityManager::GetInstance().NeedRegisterOnDemand(saProfile, INVALID_SAID);
    delete mockSa;
    EXPECT_TRUE(res);
    DTEST_LOG << "NeedRegisterOnDemand002 end" << std::endl;
}

/**
 * @tc.name: NeedRegisterOnDemand003
 * @tc.desc: NeedRegisterOnDemand, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, NeedRegisterOnDemand003, TestSize.Level3)
{
    DTEST_LOG << "NeedRegisterOnDemand003 start" << std::endl;
    SaProfile saProfile;
    saProfile.saId = INVALID_SAID;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    vector<int32_t> dependSa;
    dependSa.push_back(1499);
    mockSa->SetDependSa(dependSa);
    mockSa->SetDependTimeout(200);
    LocalAbilityManager::GetInstance().StartSystemAbilityTask(mockSa);
    LocalAbilityManager::GetInstance().RegisterOnDemandSystemAbility(SAID);
    bool res = LocalAbilityManager::GetInstance().NeedRegisterOnDemand(saProfile, SAID);
    delete mockSa;
    EXPECT_TRUE(res);
    DTEST_LOG << "NeedRegisterOnDemand003 end" << std::endl;
}

/**
 * @tc.name: Run001
 * @tc.desc: Run, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, Run001, TestSize.Level3)
{
    DTEST_LOG << "Run001 start" << std::endl;
    std::list<SystemAbility*> systemAbilityList;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    vector<int32_t> dependSa;
    dependSa.push_back(-1);
    mockSa->SetDependSa(dependSa);
    mockSa->SetDependTimeout(200);
    LocalAbilityManager::GetInstance().startTaskNum_ = STARTCODE;
    LocalAbilityManager::GetInstance().StartSystemAbilityTask(mockSa);
    LocalAbilityManager::GetInstance().StartPhaseTasks(systemAbilityList);
    bool res = LocalAbilityManager::GetInstance().Run(SAID);
    delete mockSa;
    EXPECT_FALSE(res);
    DTEST_LOG << "Run001 end" << std::endl;
}

/**
 * @tc.name: AddLocalAbilityManager001
 * @tc.desc: AddLocalAbilityManager, return false!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddLocalAbilityManager001, TestSize.Level3)
{
    DTEST_LOG << "AddLocalAbilityManager001 start" << std::endl;
    std::list<SystemAbility*> systemAbilityList;
    systemAbilityList.push_back(nullptr);
    LocalAbilityManager::GetInstance().StartPhaseTasks(systemAbilityList);
    LocalAbilityManager::GetInstance().procName_ = u"";
    bool res = LocalAbilityManager::GetInstance().AddLocalAbilityManager();
    EXPECT_FALSE(res);
    DTEST_LOG << "AddLocalAbilityManager001 end" << std::endl;
}

/**
 * @tc.name: AddLocalAbilityManager002
 * @tc.desc: AddLocalAbilityManager, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddLocalAbilityManager002, TestSize.Level3)
{
    DTEST_LOG << "AddLocalAbilityManager002 start" << std::endl;
    LocalAbilityManager::GetInstance().abilityPhaseMap_.clear();
    LocalAbilityManager::GetInstance().FindAndStartPhaseTasks(INVALID_SAID);
    LocalAbilityManager::GetInstance().procName_ = u"test";
    bool res = LocalAbilityManager::GetInstance().AddLocalAbilityManager();
    EXPECT_FALSE(res);
    DTEST_LOG << "AddLocalAbilityManager002 end" << std::endl;
}

/**
 * @tc.name: FoundationRestart001
 * @tc.desc:  FoundationRestart001
 * @tc.type: FUNC
 * @tc.require: I5N9IY
 */
HWTEST_F(LocalAbilityManagerTest, FoundationRestart001, TestSize.Level3)
{
    DTEST_LOG << "FoundationRestart001 start" << std::endl;
    std::ifstream foundationCfg;
    foundationCfg.open("/etc/init/foundation.cfg", std::ios::in);
    ASSERT_TRUE(foundationCfg.is_open());
    std::string cfg = "";
    char ch;
    while (foundationCfg.get(ch)) {
        cfg.push_back(ch);
    }
    foundationCfg.close();
    EXPECT_TRUE(cfg.find("critical") != std::string::npos);
    DTEST_LOG << "FoundationRestart001 end" << std::endl;
}

/**
 * @tc.name: NeedCheckUnused001
 * @tc.desc: test NeedCheckUnused001
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, NeedCheckUnused001, TestSize.Level1)
{
    DTEST_LOG << "NeedCheckUnused001 start" << std::endl;
    int32_t saId = 3002;
    auto ret = LocalAbilityManager::GetInstance().NoNeedCheckUnused(saId);
    EXPECT_EQ(ret, true);

    saId = 1494;
    ret = LocalAbilityManager::GetInstance().NoNeedCheckUnused(saId);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "NeedCheckUnused001 end" << std::endl;
}

/**
 * @tc.name: IsResident001
 * @tc.desc: test IsResident001, check process is resident process
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, IsResident001, TestSize.Level1)
{
    DTEST_LOG << "IsResident001 start" << std::endl;
    MockSaRealize *mockSa = new MockSaRealize(VAILD_SAID, true);
    LocalAbilityManager::GetInstance().abilityMap_[VAILD_SAID] = mockSa;
    auto ret = LocalAbilityManager::GetInstance().IsResident();
    delete mockSa;
    EXPECT_EQ(ret, true);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    DTEST_LOG << "IsResident001 end" << std::endl;
}

/**
 * @tc.name: IsResident002
 * @tc.desc: test IsResident002, check process is resident process
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, IsResident002, TestSize.Level1)
{
    DTEST_LOG << "IsResident002 start" << std::endl;
    MockSaRealize *sysAby = new MockSaRealize(MUT_SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[MUT_SAID] = sysAby;
    auto ret = LocalAbilityManager::GetInstance().IsResident();
    delete sysAby;
    EXPECT_EQ(ret, false);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    DTEST_LOG << "IsResident002 end" << std::endl;
}

/**
 * @tc.name: InitUnusedCfg001
 * @tc.desc: test InitUnusedCfg001, test config longtimeunused-unload
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitUnusedCfg001, TestSize.Level1)
{
    DTEST_LOG << "InitUnusedCfg001 start" << std::endl;
    constexpr int32_t timeoutUpLimit = 60 * 120;
    constexpr int32_t timeoutLowLimit = 60 * 1;

    EXPECT_EQ(LocalAbilityManager::GetInstance().IsConfigUnused(), false);

    auto& saProfileList = LocalAbilityManager::GetInstance().profileParser_->saProfiles_;
    SaProfile saInfo1;
    saInfo1.runOnCreate = false;
    saInfo1.saId = 1;
    saInfo1.stopOnDemand.unusedTimeout = timeoutUpLimit + 1;
    saProfileList.push_back(saInfo1);

    SaProfile saInfo2;
    saInfo2.runOnCreate = false;
    saInfo2.saId = 2;
    saInfo2.stopOnDemand.unusedTimeout = timeoutLowLimit - 1;
    saProfileList.push_back(saInfo2);

    SaProfile saInfo3;
    saInfo3.runOnCreate = false;
    saInfo3.saId = 3;
    saInfo3.stopOnDemand.unusedTimeout = timeoutLowLimit * 10;
    saProfileList.push_back(saInfo3);

    SaProfile saInfo4;
    saInfo4.runOnCreate = false;
    saInfo4.saId = 4;
    saInfo4.stopOnDemand.unusedTimeout = -1;
    saProfileList.push_back(saInfo4);

    SaProfile saInfo5;
    saInfo5.runOnCreate = true;
    saInfo5.saId = 5;
    saInfo5.stopOnDemand.unusedTimeout = 200;
    saProfileList.push_back(saInfo5);

    LocalAbilityManager::GetInstance().InitUnusedCfg();
    auto& unUsedCfgMap = LocalAbilityManager::GetInstance().unusedCfgMap_;
    EXPECT_EQ(unUsedCfgMap.count(saInfo1.saId), 1);
    EXPECT_EQ(unUsedCfgMap[saInfo1.saId], timeoutUpLimit * 1000);

    EXPECT_EQ(unUsedCfgMap.count(saInfo2.saId), 1);
    EXPECT_EQ(unUsedCfgMap[saInfo2.saId], timeoutLowLimit * 1000);

    EXPECT_EQ(unUsedCfgMap.count(saInfo3.saId), 1);
    EXPECT_EQ(unUsedCfgMap[saInfo3.saId], timeoutLowLimit * 10000);

    EXPECT_EQ(unUsedCfgMap.count(saInfo4.saId), 0);

    EXPECT_EQ(unUsedCfgMap.count(saInfo5.saId), 0);
    EXPECT_EQ(LocalAbilityManager::GetInstance().IsConfigUnused(), true);
    DTEST_LOG << "InitUnusedCfg001 end" << std::endl;
}
} // namespace SAFWK
} // namespace OHOS