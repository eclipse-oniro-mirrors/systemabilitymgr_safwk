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

#define private public
#include "local_ability_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace SAFWK {
namespace {
    const std::string TEST_RESOURCE_PATH = "/data/test/resource/safwk/profile/";
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
        (TEST_RESOURCE_PATH + "multi_sa_profile.xml");
    EXPECT_TRUE(ret);
    /**
     * @tc.steps: step2. CheckTrustSa with not all allow
     * @tc.expected: step2. load allowed sa
     */
    auto profiles = LocalAbilityManager::GetInstance().profileParser_->GetAllSaProfiles();
    auto path = TEST_RESOURCE_PATH + "test_trust_not_all_allow.xml";
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
        (TEST_RESOURCE_PATH + "multi_sa_profile.xml");
    EXPECT_TRUE(ret);
    /**
     * @tc.steps: step2. CheckTrustSa with all allow
     * @tc.expected: step2. load all sa
     */
    auto profiles = LocalAbilityManager::GetInstance().profileParser_->GetAllSaProfiles();
    auto path = TEST_RESOURCE_PATH + "test_trust_all_allow.xml";
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
    LocalAbilityManager::GetInstance().DoStartSAProcess("profile_audio.xml", 1499);
    sptr<ISystemAbilityManager> sm = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_TRUE(sm != nullptr);
    auto ability = sm->GetSystemAbility(1499);
    EXPECT_TRUE(ability == nullptr);
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
    EXPECT_TRUE(cfg.find("critical") == std::string::npos);
}
} // namespace SAFWK
} // namespace OHOS
