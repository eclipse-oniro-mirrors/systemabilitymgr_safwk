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
#include "dynamic_cache.h"

using namespace std;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
class SystemAbilityMgrCacheTest :
    public DynamicCache<int32_t, sptr<IRemoteObject>>, public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SystemAbilityMgrCacheTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SystemAbilityMgrCacheTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SystemAbilityMgrCacheTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SystemAbilityMgrCacheTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/**
 * @tc.name: QueryResult001
 * @tc.desc: test QueryResult
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityMgrCacheTest, QueryResult001, TestSize.Level2)
{
    QueryResult(0, 1);
    ClearCache();
    Recompute(0, 1);
    bool ret = SetKey("123456");
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name: InvalidateCache001
 * @tc.desc: test InvalidateCache
 * @tc.type: FUNC
 * @tc.require: I6Y5L8
 */
HWTEST_F(SystemAbilityMgrCacheTest, InvalidateCache001, TestSize.Level2)
{
    bool ret = InvalidateCache();
    EXPECT_EQ(ret, false);
}
}