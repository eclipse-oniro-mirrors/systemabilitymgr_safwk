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
#include "local_ability_manager.h"
#include "memory"
#include "mock_sa_realize.h"
#include "test_log.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace SAFWK {
namespace {
    constexpr int32_t SAID = 1489;
    constexpr int32_t LISTENER_ID = 1488;
}

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
    DTEST_LOG << "SetUp" << std::endl;
}

void SystemAbilityTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
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
}
}