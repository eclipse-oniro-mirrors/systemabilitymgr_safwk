/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ffrt_inner.h"

#define private public
#include "local_ability_manager_dumper.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ffrt {
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
int ffrt_dump(ffrt_dump_cmd_t cmd, char *buf, uint32_t len)
{
    return -1;
}
#ifdef __cplusplus
}
#endif
}
class MockLocalAbilityManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void MockLocalAbilityManagerTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void MockLocalAbilityManagerTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void MockLocalAbilityManagerTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void MockLocalAbilityManagerTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/**
 * @tc.name: StartFfrtStatistics001
 * @tc.desc: StartFfrtStatistics
 * @tc.type: FUNC
 * @tc.require: IBSPUT
 */
HWTEST_F(MockLocalAbilityManagerTest, StartFfrtStatistics001, TestSize.Level3)
{
    DTEST_LOG << "StartFfrtStatistics001 begin" << std::endl;
    std::string result;
    EXPECT_FALSE(LocalAbilityManagerDumper::collectEnable);
    auto ret = LocalAbilityManagerDumper::StartFfrtStatistics(result);
    EXPECT_FALSE(ret);
    DTEST_LOG << "StartFfrtStatistics001 end" << std::endl;
}

/**
 * @tc.name: StopFfrtStatistics001
 * @tc.desc: StopFfrtStatistics
 * @tc.type: FUNC
 * @tc.require: IBSPUT
 */
HWTEST_F(MockLocalAbilityManagerTest, StopFfrtStatistics001, TestSize.Level3)
{
    DTEST_LOG << "StopFfrtStatistics001 begin" << std::endl;
    std::string result;
    LocalAbilityManagerDumper::collectEnable = true;
    auto ret = LocalAbilityManagerDumper::StopFfrtStatistics(result);
    EXPECT_FALSE(ret);
    DTEST_LOG << "StopFfrtStatistics001 end" << std::endl;
}

/**
 * @tc.name: ClearFfrtStatistics001
 * @tc.desc: ClearFfrtStatistics
 * @tc.type: FUNC
 * @tc.require: IBSPUT
 */
HWTEST_F(MockLocalAbilityManagerTest, ClearFfrtStatistics001, TestSize.Level3)
{
    DTEST_LOG << "ClearFfrtStatistics001 begin" << std::endl;
    LocalAbilityManagerDumper::collectEnable = true;
    LocalAbilityManagerDumper::ClearFfrtStatistics();
    EXPECT_FALSE(LocalAbilityManagerDumper::collectEnable);
    DTEST_LOG << "ClearFfrtStatistics001 end" << std::endl;
}
}