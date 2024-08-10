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
    constexpr const char* EVENT_ID = "eventId";
    constexpr const char* NAME = "name";
    constexpr const char* VALUE = "value";
    constexpr const char* EXTRA_DATA_ID = "extraDataId";
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
}

/**
 * @tc.name: CheckTrustSa002
 * @tc.desc:  CheckTrustSa with all allow
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckTrustSa002, TestSize.Level1)
{
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
}

/**
 * @tc.name: DoStartSAProcess001
 * @tc.desc:  DoStartSAProcess001
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, DoStartSAProcess001, TestSize.Level2)
{
    LocalAbilityManager::GetInstance().DoStartSAProcess("profile_audio.json", SAID);
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_TRUE(sm != nullptr);
    auto ability = sm->GetSystemAbility(SAID);
    EXPECT_TRUE(ability == nullptr);
}

/**
 * @tc.name: DoStartSAProcess002
 * @tc.desc:  DoStartSAProcess002
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, DoStartSAProcess002, TestSize.Level2)
{
    string profilePath = "/system/usr/profile_audio.json";
    LocalAbilityManager::GetInstance().DoStartSAProcess(profilePath, SAID);
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto ability = sm->GetSystemAbility(SAID);
    EXPECT_EQ(ability, nullptr);
}

/**
 * @tc.name: DoStartSAProcess003
 * @tc.desc:  DoStartSAProcess, InitializeSaProfiles failed!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, DoStartSAProcess003, TestSize.Level2)
{
    string profilePath = "/system/usr/profile_audio.json";
    int32_t invalidSaid = -2;
    LocalAbilityManager::GetInstance().DoStartSAProcess(profilePath, invalidSaid);
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto ability = sm->GetSystemAbility(invalidSaid);
    EXPECT_EQ(ability, nullptr);
}

/**
 * @tc.name: GetTraceTag001
 * @tc.desc:  GetTraceTag
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetTraceTag001, TestSize.Level1)
{
    string profilePath = "/system/usr/profile_audio.json";
    string traceTag = LocalAbilityManager::GetInstance().GetTraceTag(profilePath);
    EXPECT_EQ(traceTag, "profile_audio");
}

/**
 * @tc.name: GetTraceTag002
 * @tc.desc:  GetTraceTag
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetTraceTag002, TestSize.Level1)
{
    string profilePath = "";
    string traceTag = LocalAbilityManager::GetInstance().GetTraceTag(profilePath);
    EXPECT_EQ(traceTag, "default_proc");
}

/**
 * @tc.name: GetTraceTag003
 * @tc.desc:  GetTraceTag
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetTraceTag003, TestSize.Level1)
{
    string profilePath = "/system/usr/test";
    string traceTag = LocalAbilityManager::GetInstance().GetTraceTag(profilePath);
    EXPECT_EQ(traceTag, "test");
}

/**
 * @tc.name: CheckAndGetProfilePath001
 * @tc.desc:  CheckAndGetProfilePath
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckAndGetProfilePath001, TestSize.Level3)
{
    string profilePath = "/system/usr/profile_audio.json";
    string realProfilePath = "";
    bool res = LocalAbilityManager::GetInstance().CheckAndGetProfilePath(profilePath, realProfilePath);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: CheckAndGetProfilePath002
 * @tc.desc:  CheckAndGetProfilePath, Doc dir is not matched!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckAndGetProfilePath002, TestSize.Level1)
{
    string profilePath = TEST_RESOURCE_PATH + "test_trust_all_allow.json";
    string realProfilePath = "";
    bool res = LocalAbilityManager::GetInstance().CheckAndGetProfilePath(profilePath, realProfilePath);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: CheckAndGetProfilePath003
 * @tc.desc: test CheckAndGetProfilePath, with invalid profilepath length
 * @tc.type: FUNC
 * @tc.require: I73XRZ
 */
HWTEST_F(LocalAbilityManagerTest, CheckAndGetProfilePath003, TestSize.Level2)
{
    string profilePath;
    for (; profilePath.size() <= PATH_MAX;) {
        profilePath = profilePath + TEST_RESOURCE_PATH;
    }
    string realProfilePath = "";
    bool res = LocalAbilityManager::GetInstance().CheckAndGetProfilePath(profilePath, realProfilePath);
    // cover ClearResource
    LocalAbilityManager::GetInstance().ClearResource();
    EXPECT_FALSE(res);
}

/**
 * @tc.name: CheckSystemAbilityManagerReady001
 * @tc.desc:  CheckSystemAbilityManagerReady, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckSystemAbilityManagerReady001, TestSize.Level3)
{
    string profilePath = TEST_RESOURCE_PATH + "test_trust_all_allow.json";
    string realProfilePath = "";
    bool res = LocalAbilityManager::GetInstance().CheckSystemAbilityManagerReady();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: InitSystemAbilityProfiles001
 * @tc.desc:  InitSystemAbilityProfiles, ParseSaProfiles failed!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitSystemAbilityProfiles001, TestSize.Level1)
{
    string profilePath = "";
    bool res = LocalAbilityManager::GetInstance().InitSystemAbilityProfiles(profilePath, SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: InitSystemAbilityProfiles002
 * @tc.desc:  InitSystemAbilityProfiles, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitSystemAbilityProfiles002, TestSize.Level3)
{
    string profilePath = "/system/usr/profile_audio.json";
    bool res = LocalAbilityManager::GetInstance().InitSystemAbilityProfiles(profilePath, SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: InitSystemAbilityProfiles003
 * @tc.desc:  InitSystemAbilityProfiles, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitSystemAbilityProfiles003, TestSize.Level3)
{
    string profilePath = "/system/usr/profile_audio.json";
    int32_t defaultId = -1;
    bool res = LocalAbilityManager::GetInstance().InitSystemAbilityProfiles(profilePath, defaultId);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: AddAbility001
 * @tc.desc: AddAbility, try to add null ability!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddAbility001, TestSize.Level3)
{
    string path = "";
    string process = "process";
    std::list<SaProfile> saInfos;
    LocalAbilityManager::GetInstance().CheckTrustSa(path, process, saInfos);
    bool res = LocalAbilityManager::GetInstance().AddAbility(nullptr);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: AddAbility002
 * @tc.desc: AddAbility, try to add existed ability
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddAbility002, TestSize.Level3)
{
    MockSaRealize *sysAby = new MockSaRealize(MUT_SAID, false);
    bool ret = LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles
        (TEST_RESOURCE_PATH + "multi_sa_profile.json");
    EXPECT_TRUE(ret);
    LocalAbilityManager::GetInstance().abilityMap_[MUT_SAID] = sysAby;
    bool res = LocalAbilityManager::GetInstance().AddAbility(sysAby);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete sysAby;
    EXPECT_FALSE(res);
}

/**
 * @tc.name: RemoveAbility001
 * @tc.desc: RemoveAbility, invalid systemAbilityId
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveAbility001, TestSize.Level1)
{
    bool res = LocalAbilityManager::GetInstance().RemoveAbility(INVALID_SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: RemoveAbility002
 * @tc.desc: RemoveAbility, invalid systemAbilityId
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveAbility002, TestSize.Level3)
{
    bool res = LocalAbilityManager::GetInstance().RemoveAbility(SAID);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: AddSystemAbilityListener001
 * @tc.desc: AddSystemAbilityListener, SA or listenerSA invalid
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddSystemAbilityListener001, TestSize.Level1)
{
    bool res = LocalAbilityManager::GetInstance().AddSystemAbilityListener(SAID, INVALID_SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: AddSystemAbilityListener002
 * @tc.desc: AddSystemAbilityListener, SA or listenerSA invalid
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddSystemAbilityListener002, TestSize.Level1)
{
    bool res = LocalAbilityManager::GetInstance().AddSystemAbilityListener(INVALID_SAID, SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: AddSystemAbilityListener003
 * @tc.desc: AddSystemAbilityListener, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddSystemAbilityListener003, TestSize.Level1)
{
    std::pair<int32_t, int32_t> key = std::make_pair(SAID, MUT_SAID);
    LocalAbilityManager::GetInstance().listenerMap_[key] =
        new LocalAbilityManager::SystemAbilityListener(MUT_SAID);
    bool res = LocalAbilityManager::GetInstance().AddSystemAbilityListener(SAID, SAID);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: AddSystemAbilityListener004
 * @tc.desc: AddSystemAbilityListener, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddSystemAbilityListener004, TestSize.Level1)
{
    std::pair<int32_t, int32_t> key1 = std::make_pair(VAILD_SAID, VAILD_SAID);
    std::pair<int32_t, int32_t> key2 = std::make_pair(VAILD_SAID, SAID);
    LocalAbilityManager::GetInstance().listenerMap_[key1] =
        new LocalAbilityManager::SystemAbilityListener(VAILD_SAID);
    LocalAbilityManager::GetInstance().listenerMap_[key2] =
        new LocalAbilityManager::SystemAbilityListener(SAID);
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    bool res = LocalAbilityManager::GetInstance().AddSystemAbilityListener(VAILD_SAID, VAILD_SAID);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: RemoveSystemAbilityListener001
 * @tc.desc: RemoveSystemAbilityListener, SA or listenerSA invalid
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveSystemAbilityListener001, TestSize.Level1)
{
    bool res = LocalAbilityManager::GetInstance().RemoveSystemAbilityListener(INVALID_SAID, SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: RemoveSystemAbilityListener002
 * @tc.desc: RemoveSystemAbilityListener, SA or listenerSA invalid
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveSystemAbilityListener002, TestSize.Level1)
{
    bool res = LocalAbilityManager::GetInstance().RemoveSystemAbilityListener(SAID, INVALID_SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: RemoveSystemAbilityListener003
 * @tc.desc: RemoveSystemAbilityListener, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveSystemAbilityListener003, TestSize.Level1)
{
    LocalAbilityManager::GetInstance().listenerMap_.clear();
    bool res = LocalAbilityManager::GetInstance().RemoveSystemAbilityListener(SAID, SAID);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: RemoveSystemAbilityListener004
 * @tc.desc: RemoveSystemAbilityListener, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, RemoveSystemAbilityListener004, TestSize.Level3)
{
    std::pair<int32_t, int32_t> key = std::make_pair(SAID, MUT_SAID);
    LocalAbilityManager::GetInstance().listenerMap_[key] =
        new LocalAbilityManager::SystemAbilityListener(MUT_SAID);
    bool res = LocalAbilityManager::GetInstance().RemoveSystemAbilityListener(SAID, SAID);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: OnStartAbility001
 * @tc.desc: OnStartAbility, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, OnStartAbility001, TestSize.Level1)
{
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
}

/**
 * @tc.name: OnStartAbility002
 * @tc.desc: OnStartAbility, return false
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, OnStartAbility002, TestSize.Level1)
{
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
}

/**
 * @tc.name: GetAbility001
 * @tc.desc: GetAbility, SA not register
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetAbility001, TestSize.Level1)
{
    std::string deviceId = "";
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    SystemAbility* res = LocalAbilityManager::GetInstance().GetAbility(SAID);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name: GetAbility002
 * @tc.desc: GetAbility, SA not register
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetAbility002, TestSize.Level1)
{
    std::string deviceId = "";
    MockSaRealize *sysAby = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = sysAby;
    SystemAbility* res = LocalAbilityManager::GetInstance().GetAbility(SAID);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete sysAby;
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name: GetRunningStatus001
 * @tc.desc: GetRunningStatus, return false
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetRunningStatus001, TestSize.Level1)
{
    std::string deviceId = "";
    bool res = LocalAbilityManager::GetInstance().GetRunningStatus(SAID);
    LocalAbilityManager::GetInstance().listenerMap_.clear();
    EXPECT_FALSE(res);
}

/**
 * @tc.name: GetRunningStatus002
 * @tc.desc: GetRunningStatus, return true
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, GetRunningStatus002, TestSize.Level1)
{
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    bool res = LocalAbilityManager::GetInstance().GetRunningStatus(SAID);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete mockSa;
    EXPECT_FALSE(res);
}

/**
 * @tc.name: StartOndemandSystemAbility001
 * @tc.desc: StartOndemandSystemAbility
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, StartOndemandSystemAbility001, TestSize.Level1)
{
    std::string profilePath = "/system/usr/profile_audio.json";
    LocalAbilityManager::GetInstance().profileParser_->saProfiles_.clear();
    LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    LocalAbilityManager::GetInstance().StartOndemandSystemAbility(SAID);
    EXPECT_EQ(LocalAbilityManager::GetInstance().profileParser_->saProfiles_.size(), 3);
}

/**
 * @tc.name: StartOndemandSystemAbility002
 * @tc.desc: StartOndemandSystemAbility
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, StartOndemandSystemAbility002, TestSize.Level1)
{
    std::string profilePath = "/system/usr/profile_audio.json";
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().profileParser_->saProfiles_.clear();
    LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    LocalAbilityManager::GetInstance().StartOndemandSystemAbility(SAID);
    delete mockSa;
    EXPECT_EQ(LocalAbilityManager::GetInstance().profileParser_->saProfiles_.size(), 3);
}

/**
 * @tc.name: InitializeSaProfiles001
 * @tc.desc: InitializeSaProfiles, sa profile is empty
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfiles001, TestSize.Level1)
{
    LocalAbilityManager::GetInstance().profileParser_->saProfiles_.clear();
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfiles(INVALID_SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: InitializeSaProfiles002
 * @tc.desc: InitializeSaProfiles
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfiles002, TestSize.Level1)
{
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfiles(SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: InitializeRunOnCreateSaProfiles001
 * @tc.desc: InitializeSaProfiles
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeRunOnCreateSaProfiles001, TestSize.Level1)
{
    LocalAbilityManager::GetInstance().profileParser_->saProfiles_.clear();
    bool res = LocalAbilityManager::GetInstance().InitializeRunOnCreateSaProfiles(OTHERPHASE);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: InitializeRunOnCreateSaProfiles002
 * @tc.desc: InitializeSaProfiles
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeRunOnCreateSaProfiles002, TestSize.Level1)
{
    std::string profilePath = "/system/usr/profile_audio.json";
    LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    bool res = LocalAbilityManager::GetInstance().InitializeRunOnCreateSaProfiles(OTHERPHASE);
    EXPECT_TRUE(res);
}

/**
 * @tc.name: InitializeRunOnCreateSaProfiles003
 * @tc.desc: InitializeSaProfiles
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeRunOnCreateSaProfiles003, TestSize.Level1)
{
    std::string profilePath = "/system/usr/profile_audio.json";
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    bool res = LocalAbilityManager::GetInstance().InitializeRunOnCreateSaProfiles(OTHERPHASE);
    delete mockSa;
    EXPECT_TRUE(res);
}

/**
 * @tc.name: InitializeOnDemandSaProfile001
 * @tc.desc: InitializeOnDemandSaProfile
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeOnDemandSaProfile001, TestSize.Level1)
{
    std::string profilePath = "/system/usr/profile_audio.json";
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    bool res = LocalAbilityManager::GetInstance().InitializeOnDemandSaProfile(SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: InitializeOnDemandSaProfile002
 * @tc.desc: InitializeOnDemandSaProfile
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeOnDemandSaProfile002, TestSize.Level1)
{
    LocalAbilityManager::GetInstance().profileParser_->saProfiles_.clear();
    bool res = LocalAbilityManager::GetInstance().InitializeOnDemandSaProfile(SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: InitializeSaProfilesInnerLocked001
 * @tc.desc: InitializeSaProfilesInnerLocked, SA not found!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfilesInnerLocked001, TestSize.Level1)
{
    SaProfile saProfile;
    saProfile.saId = SAID;
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: InitializeSaProfilesInnerLocked002
 * @tc.desc: InitializeSaProfilesInnerLocked, SA is null!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfilesInnerLocked002, TestSize.Level1)
{
    SaProfile saProfile;
    saProfile.saId = SAID;
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = nullptr;
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: InitializeSaProfilesInnerLocked003
 * @tc.desc: InitializeSaProfilesInnerLocked, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfilesInnerLocked003, TestSize.Level1)
{
    SaProfile saProfile;
    saProfile.saId = SAID;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete mockSa;
    EXPECT_TRUE(res);
}

/**
 * @tc.name: InitializeSaProfilesInnerLocked004
 * @tc.desc: InitializeSaProfilesInnerLocked, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfilesInnerLocked004, TestSize.Level1)
{
    SaProfile saProfile;
    saProfile.saId = SAID;
    saProfile.bootPhase = BOOTPHASE;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete mockSa;
    EXPECT_TRUE(res);
}

/**
 * @tc.name: InitializeSaProfilesInnerLocked005
 * @tc.desc: InitializeSaProfilesInnerLocked, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, InitializeSaProfilesInnerLocked005, TestSize.Level1)
{
    SaProfile saProfile;
    saProfile.saId = SAID;
    saProfile.bootPhase = BOOTPHASE;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    bool res = LocalAbilityManager::GetInstance().InitializeSaProfilesInnerLocked(saProfile);
    LocalAbilityManager::GetInstance().abilityMap_.clear();
    delete mockSa;
    EXPECT_TRUE(res);
}

/**
 * @tc.name: CheckDependencyStatus001
 * @tc.desc: CheckDependencyStatus, return size is 1!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckDependencyStatus001, TestSize.Level1)
{
    std::string profilePath = "/system/usr/profile_audio.json";
    vector<int32_t> dependSa;
    dependSa.push_back(1499);
    bool ret = LocalAbilityManager::GetInstance().profileParser_->ParseSaProfiles(profilePath);
    EXPECT_TRUE(ret);
    LocalAbilityManager::GetInstance().RegisterOnDemandSystemAbility(SAID);
    vector<int32_t> res = LocalAbilityManager::GetInstance().CheckDependencyStatus(dependSa);
    EXPECT_EQ(res.size(), STARTCODE);
}

/**
 * @tc.name: CheckDependencyStatus002
 * @tc.desc: CheckDependencyStatus, return size is 0!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckDependencyStatus002, TestSize.Level1)
{
    vector<int32_t> dependSa;
    dependSa.push_back(-1);
    vector<int32_t> res = LocalAbilityManager::GetInstance().CheckDependencyStatus(dependSa);
    EXPECT_EQ(res.size(), 0);
}

/**
 * @tc.name: CheckDependencyStatus003
 * @tc.desc: CheckDependencyStatus, return size is 0!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckDependencyStatus003, TestSize.Level1)
{
    vector<int32_t> dependSa;
    dependSa.push_back(401);
    vector<int32_t> res = LocalAbilityManager::GetInstance().CheckDependencyStatus(dependSa);
    EXPECT_EQ(res.size(), 0);
}

/**
 * @tc.name: NeedRegisterOnDemand001
 * @tc.desc: NeedRegisterOnDemand, return false!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, NeedRegisterOnDemand001, TestSize.Level1)
{
    SaProfile saProfile;
    saProfile.runOnCreate = true;
    LocalAbilityManager::GetInstance().StartSystemAbilityTask(nullptr);
    bool res = LocalAbilityManager::GetInstance().NeedRegisterOnDemand(saProfile, INVALID_SAID);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: NeedRegisterOnDemand002
 * @tc.desc: NeedRegisterOnDemand, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, NeedRegisterOnDemand002, TestSize.Level1)
{
    SaProfile saProfile;
    saProfile.runOnCreate = false;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().StartSystemAbilityTask(mockSa);
    bool res = LocalAbilityManager::GetInstance().NeedRegisterOnDemand(saProfile, INVALID_SAID);
    delete mockSa;
    EXPECT_TRUE(res);
}

/**
 * @tc.name: NeedRegisterOnDemand003
 * @tc.desc: NeedRegisterOnDemand, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, NeedRegisterOnDemand003, TestSize.Level3)
{
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
}

/**
 * @tc.name: Run001
 * @tc.desc: Run, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, Run001, TestSize.Level3)
{
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
}

/**
 * @tc.name: AddLocalAbilityManager001
 * @tc.desc: AddLocalAbilityManager, return false!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddLocalAbilityManager001, TestSize.Level3)
{
    std::list<SystemAbility*> systemAbilityList;
    systemAbilityList.push_back(nullptr);
    LocalAbilityManager::GetInstance().StartPhaseTasks(systemAbilityList);
    LocalAbilityManager::GetInstance().procName_ = u"";
    bool res = LocalAbilityManager::GetInstance().AddLocalAbilityManager();
    EXPECT_FALSE(res);
}

/**
 * @tc.name: AddLocalAbilityManager002
 * @tc.desc: AddLocalAbilityManager, return true!
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, AddLocalAbilityManager002, TestSize.Level3)
{
    LocalAbilityManager::GetInstance().abilityPhaseMap_.clear();
    LocalAbilityManager::GetInstance().FindAndStartPhaseTasks(INVALID_SAID);
    LocalAbilityManager::GetInstance().procName_ = u"test";
    bool res = LocalAbilityManager::GetInstance().AddLocalAbilityManager();
    EXPECT_FALSE(res);
}

/**
 * @tc.name: FoundationRestart001
 * @tc.desc:  FoundationRestart001
 * @tc.type: FUNC
 * @tc.require: I5N9IY
 */
HWTEST_F(LocalAbilityManagerTest, FoundationRestart001, TestSize.Level3)
{
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
}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: OnRemoteRequest001
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, OnRemoteRequest001, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::string deviceId = "";
    LocalAbilityManager::SystemAbilityListener *sysListener =
        new LocalAbilityManager::SystemAbilityListener(SAID);
    sysListener->OnAddSystemAbility(SAID, deviceId);
    int32_t result = LocalAbilityManager::GetInstance().OnRemoteRequest(0, data, reply, option);
    delete sysListener;
    EXPECT_NE(result, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: OnRemoteRequest002
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, OnRemoteRequest002, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    MessageParcel reply;
    MessageOption option;
    std::string deviceId = "";
    LocalAbilityManager::SystemAbilityListener *sysListener =
        new LocalAbilityManager::SystemAbilityListener(INVALID_SAID);
    sysListener->OnAddSystemAbility(INVALID_SAID, deviceId);
    int32_t result = LocalAbilityManager::GetInstance().OnRemoteRequest(0, data, reply, option);
    delete sysListener;
    EXPECT_NE(result, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: OnRemoteRequest003
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, OnRemoteRequest003, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    MessageParcel reply;
    MessageOption option;
    std::string deviceId = "";
    LocalAbilityManager::SystemAbilityListener *sysListener =
        new LocalAbilityManager::SystemAbilityListener(INVALID_SAID);
    sysListener->OnRemoveSystemAbility(INVALID_SAID, deviceId);
    int32_t result = LocalAbilityManager::GetInstance().OnRemoteRequest(STARTCODE, data, reply, option);
    delete sysListener;
    EXPECT_NE(result, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: OnRemoteRequest004
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, OnRemoteRequest004, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    data.WriteInt32(STARTCODE);
    MessageParcel reply;
    MessageOption option;
    std::string deviceId = "";
    LocalAbilityManager::SystemAbilityListener *sysListener =
        new LocalAbilityManager::SystemAbilityListener(SAID);
    sysListener->OnRemoveSystemAbility(SAID, deviceId);
    int32_t result = LocalAbilityManager::GetInstance().OnRemoteRequest(STARTCODE, data, reply, option);
    delete sysListener;
    EXPECT_EQ(result, ERR_PERMISSION_DENIED);
}
/**
 * @tc.name: OnRemoteRequest005
 * @tc.desc: OnRemoteRequest005
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, OnRemoteRequest005, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInterfaceToken(LOCAL_ABILITY_MANAGER_INTERFACE_TOKEN);
    data.WriteInt32(STARTCODE);
    std::string eventStr = "test";
    data.WriteString(eventStr);
    MessageParcel reply;
    MessageOption option;
    std::string deviceId = "";
    LocalAbilityManager::SystemAbilityListener *sysListener =
        new LocalAbilityManager::SystemAbilityListener(SAID);
    sysListener->OnRemoveSystemAbility(SAID, deviceId);
    int32_t result = LocalAbilityManager::GetInstance().OnRemoteRequest(STARTCODE, data, reply, option);
    delete sysListener;
    EXPECT_EQ(result, ERR_PERMISSION_DENIED);
}

/**
 * @tc.name: StartAbilityInner001
 * @tc.desc: test StartAbilityInner with invalid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerTest, StartAbilityInner001, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInt32(INVALID_SAID);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StartAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: StartAbilityInner002
 * @tc.desc: test StartAbilityInner with data.ReadString() is empty
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerTest, StartAbilityInner002, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StartAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: StartAbilityInner003
 * @tc.desc: test StartAbilityInner with data.ReadString() is not  empty
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerTest, StartAbilityInner003, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    std::string eventStr = "test";
    data.WriteString(eventStr);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StartAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: StopAbilityInner001
 * @tc.desc: test StopAbilityInner with invalid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerTest, StopAbilityInner001, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInt32(INVALID_SAID);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StopAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: StopAbilityInner002
 * @tc.desc: test StopAbilityInner with data.ReadString() is empty
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerTest, StopAbilityInner002, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StopAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: StopAbilityInner003
 * @tc.desc: test StopAbilityInner with data.ReadString() is empty
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerTest, StopAbilityInner003, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    std::string eventStr = "test";
    data.WriteString(eventStr);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().StopAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: ActiveAbilityInner001
 * @tc.desc: test ActiveAbilityInner with invalid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerTest, ActiveAbilityInner001, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInt32(INVALID_SAID);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().ActiveAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: ActiveAbilityInner002
 * @tc.desc: test ActiveAbilityInner with valid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerTest, ActiveAbilityInner002, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().ActiveAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: IdleAbilityInner001
 * @tc.desc: test IdleAbilityInner with invalid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerTest, IdleAbilityInner001, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInt32(INVALID_SAID);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().IdleAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: IdleAbilityInner002
 * @tc.desc: test IdleAbilityInner with valid SaID
 * @tc.type: FUNC
 * @tc.require:I6LSSX
 */
HWTEST_F(LocalAbilityManagerTest, IdleAbilityInner002, TestSize.Level2)
{
    MessageParcel data;
    data.WriteInt32(STARTCODE);
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().IdleAbilityInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: OnStopAbility001
 * @tc.desc: test OnStopAbility, cover function with valid SaID
 * @tc.type: FUNC
 * @tc.require: I73XRZ
 */
HWTEST_F(LocalAbilityManagerTest, OnStopAbility001, TestSize.Level2)
{
    int32_t systemAbilityId = 1;
    bool ret = LocalAbilityManager::GetInstance().OnStopAbility(systemAbilityId);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: OnStopAbility002
 * @tc.desc: test OnStopAbility with said is in abilityMap_
 * @tc.type: FUNC
 * @tc.require: I7G7DL
 */
HWTEST_F(LocalAbilityManagerTest, OnStopAbility002, TestSize.Level2)
{
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    mockSa->abilityState_ = SystemAbilityState::NOT_LOADED;
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    bool ret = LocalAbilityManager::GetInstance().OnStopAbility(SAID);
    delete mockSa;
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ActiveAbility001
 * @tc.desc: test ActiveAbility, cover function with valid SaID
 * @tc.type: FUNC
 * @tc.require: I73XRZ
 */
HWTEST_F(LocalAbilityManagerTest, ActiveAbility001, TestSize.Level2)
{
    int32_t systemAbilityId = 1;
    nlohmann::json activeReason;
    bool ret = LocalAbilityManager::GetInstance().ActiveAbility(systemAbilityId, activeReason);
    // cover StartPhaseTasks
    std::list<SystemAbility*> systemAbilityList;
    LocalAbilityManager::GetInstance().StartPhaseTasks(systemAbilityList);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ActiveAbility002
 * @tc.desc: test ActiveAbility with sa is existed
 * @tc.type: FUNC
 * @tc.require: I7G7DL
 */
HWTEST_F(LocalAbilityManagerTest, ActiveAbility002, TestSize.Level2)
{
    nlohmann::json activeReason;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    mockSa->abilityState_ = SystemAbilityState::ACTIVE;
    bool ret = LocalAbilityManager::GetInstance().ActiveAbility(SAID, activeReason);
    delete mockSa;
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: IdleAbility001
 * @tc.desc: test IdleAbility, cover function
 * @tc.type: FUNC
 * @tc.require: I73XRZ
 */
HWTEST_F(LocalAbilityManagerTest, IdleAbility001, TestSize.Level2)
{
    int32_t systemAbilityId = 1;
    nlohmann::json activeReason;
    int32_t delayTime = 0;
    bool ret = LocalAbilityManager::GetInstance().IdleAbility(systemAbilityId, activeReason, delayTime);
    // cover WaitForTasks
    LocalAbilityManager::GetInstance().WaitForTasks();
    // cover FindAndStartPhaseTasks
    LocalAbilityManager::GetInstance().FindAndStartPhaseTasks(systemAbilityId);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: IdleAbility002
 * @tc.desc: test IdleAbility with sa is existed
 * @tc.type: FUNC
 * @tc.require: I7G7DL
 */
HWTEST_F(LocalAbilityManagerTest, IdleAbility002, TestSize.Level2)
{
    nlohmann::json idleReason;
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    mockSa->abilityState_ = SystemAbilityState::IDLE;
    int delayTime = 0;
    bool ret = LocalAbilityManager::GetInstance().IdleAbility(SAID, idleReason, delayTime);
    delete mockSa;
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: JsonToOnDemandReason001
 * @tc.desc: test JsonToOnDemandReason, with assignments
 * @tc.type: FUNC
 * @tc.require: I73XRZ
 */
HWTEST_F(LocalAbilityManagerTest, JsonToOnDemandReason001, TestSize.Level2)
{
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
}

/**
 * @tc.name: FfrtDumperProc001
 * @tc.desc: test FfrtDumperProc
 * @tc.type: FUNC
 * @tc.require: I7G7DL
 */
HWTEST_F(LocalAbilityManagerTest, FfrtDumperProc001, TestSize.Level2)
{
    std::string ffrtDumperInfo;
    bool result = LocalAbilityManager::GetInstance().FfrtDumperProc(ffrtDumperInfo);
    EXPECT_TRUE(result);
}

/**
 * @tc.name: SendStrategyToSA001
 * @tc.desc: test SendStrategyToSA001, cover function with valid SaID
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SendStrategyToSA001, TestSize.Level2)
{
    int32_t systemAbilityId = 1;
    std::string action = "";
    bool ret = LocalAbilityManager::GetInstance().SendStrategyToSA(1, systemAbilityId, 1, action);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: SendStrategyToSA002
 * @tc.desc: test SendStrategyToSA with said is in abilityMap_
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SendStrategyToSA002, TestSize.Level2)
{
    MockSaRealize *mockSa = new MockSaRealize(SAID, false);
    mockSa->abilityState_ = SystemAbilityState::NOT_LOADED;
    LocalAbilityManager::GetInstance().abilityMap_[SAID] = mockSa;
    std::string action = "";
    bool ret = LocalAbilityManager::GetInstance().SendStrategyToSA(1, SAID, 1, action);
    delete mockSa;
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: IpcStatCmdProc001
 * @tc.desc: test IpcStatCmdProc001, cover function with valid fd and valid cmd
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, IpcStatCmdProc001, TestSize.Level2)
{
    int32_t fd = 1;
    int32_t cmd = 0;
    bool ret = LocalAbilityManager::GetInstance().IpcStatCmdProc(fd, cmd);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: IpcStatCmdProc002
 * @tc.desc: test IpcStatCmdProc002, cover function with valid fd and invalid cmd
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, IpcStatCmdProc002, TestSize.Level2)
{
    int32_t fd = 1;
    int32_t cmd = -1;
    bool ret = LocalAbilityManager::GetInstance().IpcStatCmdProc(fd, cmd);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: IpcStatCmdProc003
 * @tc.desc: test IpcStatCmdProc003, cover function with invalid fd and valid cmd
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, IpcStatCmdProc003, TestSize.Level2)
{
    int32_t fd = -1;
    int32_t cmd = 0;
    bool ret = LocalAbilityManager::GetInstance().IpcStatCmdProc(fd, cmd);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: IpcStatCmdProc004
 * @tc.desc: test IpcStatCmdProc004, cover function with invalid fd and invalid cmd
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, IpcStatCmdProc004, TestSize.Level2)
{
    int32_t fd = -1;
    int32_t cmd = -1;
    bool ret = LocalAbilityManager::GetInstance().IpcStatCmdProc(fd, cmd);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CheckPermission001
 * @tc.desc: test CheckPermission001
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckPermission001, TestSize.Level2)
{
    uint32_t code = static_cast<uint32_t>(SafwkInterfaceCode::START_ABILITY_TRANSACTION);
    bool result = LocalAbilityManager::GetInstance().CheckPermission(code);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: CheckPermission002
 * @tc.desc: test CheckPermission002
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, CheckPermission002, TestSize.Level2)
{
    uint32_t code = static_cast<uint32_t>(SafwkInterfaceCode::SYSTEM_ABILITY_EXT_TRANSACTION);
    bool result = LocalAbilityManager::GetInstance().CheckPermission(code);
    EXPECT_FALSE(result);
}

/**
 * @tc.name: SystemAbilityExtProc001
 * @tc.desc: test SystemAbilityExtProc001
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SystemAbilityExtProc001, TestSize.Level2)
{
    std::string extension = "ext1";
    SystemAbilityExtensionPara callback;
    bool isAsync = false;
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProc(extension, INVALID_SAID, &callback, isAsync);
    EXPECT_FALSE(ret == ERR_NONE);
}

/**
 * @tc.name: SystemAbilityExtProc002
 * @tc.desc: test SystemAbilityExtProc002
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SystemAbilityExtProc002, TestSize.Level2)
{
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
}

/**
 * @tc.name: SendStrategyToSAInner001
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SendStrategyToSAInner001, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().SendStrategyToSAInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: SendStrategyToSAInner002
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SendStrategyToSAInner002, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int type = 1;
    data.WriteInt32(type);
    int32_t ret = LocalAbilityManager::GetInstance().SendStrategyToSAInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: SendStrategyToSAInner003
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SendStrategyToSAInner003, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int type = 1;
    data.WriteInt32(type);
    int32_t saId = -1;
    data.WriteInt32(saId);
    int32_t ret = LocalAbilityManager::GetInstance().SendStrategyToSAInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: SendStrategyToSAInner004
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SendStrategyToSAInner004, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int type = 1;
    data.WriteInt32(type);
    int32_t saId = 1;
    data.WriteInt32(saId);
    int32_t ret = LocalAbilityManager::GetInstance().SendStrategyToSAInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: SendStrategyToSAInner005
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SendStrategyToSAInner005, TestSize.Level2)
{
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
}

/**
 * @tc.name: SendStrategyToSAInner006
 * @tc.desc: test SendStrategyToSAInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SendStrategyToSAInner006, TestSize.Level2)
{
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
}

/**
 * @tc.name: IpcStatCmdProcInner001
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, IpcStatCmdProcInner001, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().IpcStatCmdProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: IpcStatCmdProcInner002
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, IpcStatCmdProcInner002, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t fd = 1;
    data.WriteFileDescriptor(fd);
    int32_t ret = LocalAbilityManager::GetInstance().IpcStatCmdProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: IpcStatCmdProcInner003
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, IpcStatCmdProcInner003, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t fd = 1;
    int cmd = -1;
    data.WriteInt32(cmd);
    data.WriteFileDescriptor(fd);
    int32_t ret = LocalAbilityManager::GetInstance().IpcStatCmdProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: FfrtDumperProcInner001
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, FfrtDumperProcInner001, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    std::string ffrtDumperInfo = "Test Ffrt Dumper Info";
    int32_t ret = LocalAbilityManager::GetInstance().FfrtDumperProcInner(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: SystemAbilityExtProcInner001
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SystemAbilityExtProcInner001, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProcInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
}

/**
 * @tc.name: SystemAbilityExtProcInner002
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SystemAbilityExtProcInner002, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int saId = 1;
    data.WriteInt32(saId);
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProcInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
}

/**
 * @tc.name: SystemAbilityExtProcInner003
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SystemAbilityExtProcInner003, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int saId = -1;
    data.WriteInt32(saId);
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProcInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
}

/**
 * @tc.name: SystemAbilityExtProcInner004
 * @tc.desc: test IpcStatCmdProcInner.
 * @tc.type: FUNC
 */
HWTEST_F(LocalAbilityManagerTest, SystemAbilityExtProcInner004, TestSize.Level2)
{
    MessageParcel data;
    MessageParcel reply;
    int saId = 1;
    data.WriteInt32(saId);
    std::string extension = "TestExtension";
    data.WriteString(extension);
    int32_t ret = LocalAbilityManager::GetInstance().SystemAbilityExtProcInner(data, reply);
    EXPECT_EQ(ret, INVALID_DATA);
}
} // namespace SAFWK
} // namespace OHOS