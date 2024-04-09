/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "local_ability_manager_dumper.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
class LocalAbilityManagerDumperTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void LocalAbilityManagerDumperTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void LocalAbilityManagerDumperTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void LocalAbilityManagerDumperTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void LocalAbilityManagerDumperTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/**
 * @tc.name: StartIpcStatistics001
 * @tc.desc: test StartIpcStatistics
 * @tc.type: FUNC
 * @tc.require: I9DR69
 */
HWTEST_F(LocalAbilityManagerDumperTest, StartIpcStatistics001, TestSize.Level2)
{
    std::string result;
    bool ret = LocalAbilityManagerDumper::StartIpcStatistics(result);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: StopIpcStatistics001
 * @tc.desc: test StopIpcStatistics
 * @tc.type: FUNC
 * @tc.require: I9DR69
 */
HWTEST_F(LocalAbilityManagerDumperTest, StopIpcStatistics001, TestSize.Level2)
{
    std::string result;
    bool ret = LocalAbilityManagerDumper::StopIpcStatistics(result);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: GetIpcStatistics001
 * @tc.desc: test GetIpcStatistics001
 * @tc.type: FUNC
 * @tc.require: I9DR69
 */
HWTEST_F(LocalAbilityManagerDumperTest, GetIpcStatistics001, TestSize.Level2)
{
    std::string result;
    bool ret = LocalAbilityManagerDumper::GetIpcStatistics(result);
    EXPECT_EQ(ret, true);
}
}