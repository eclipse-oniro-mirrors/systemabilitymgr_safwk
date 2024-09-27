/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#undef private
#undef protected
#define private public
#define protected public
#include "gtest/gtest.h"
#include "expire_lru_cache.h"
#include "message_parcel.h"
#include "test_log.h"

using namespace ::testing;
using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace {
    vector<char> g_Key1{'k', 'e', 'y', '1'};
    vector<char> g_Key2{'k', 'e', 'y', '2'};
    vector<char> g_Key3{'k', 'e', 'y', '3'};
    vector<char> g_Key4{'k', 'e', 'y', '4'};
    vector<char> g_Key5{'k', 'e', 'y', '5'};

    vector<char> g_Val1{'v', 'a', 'l', '1'};
    vector<char> g_Val2{'v', 'a', 'l', '2'};
    vector<char> g_Val3{'v', 'a', 'l', '3'};
    vector<char> g_Val4{'v', 'a', 'l', '4'};
    vector<char> g_Val5{'v', 'a', 'l', '5'};
}

class ExpireLruCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ExpireLruCacheTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void ExpireLruCacheTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void ExpireLruCacheTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void ExpireLruCacheTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/**
 * @tc.name: ConstructorTest001
 * @tc.desc: test constructor input parameter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ExpireLruCacheTest, ConstructorTest001, TestSize.Level2)
{
    DTEST_LOG << "ConstructorTest001 start" << std::endl;
    {
        ExpireLruCache<std::vector<char>, std::vector<char>> cache;
        EXPECT_EQ(cache.size_, 8);
        EXPECT_EQ(cache.expireTimeMilliSec_, 1000);
    }

    {
        ExpireLruCache<std::vector<char>, std::vector<char>> cache(0, 0);
        EXPECT_EQ(cache.size_, 1);
        EXPECT_EQ(cache.expireTimeMilliSec_, 0);
    }

    {
        ExpireLruCache<std::vector<char>, std::vector<char>> cache(0, -1);
        EXPECT_EQ(cache.size_, 1);
        EXPECT_EQ(cache.expireTimeMilliSec_, 1000);
    }

    {
        ExpireLruCache<std::vector<char>, std::vector<char>> cache(100, 2000);
        EXPECT_EQ(cache.size_, 100);
        EXPECT_EQ(cache.expireTimeMilliSec_, 2000);
    }
    DTEST_LOG << "ConstructorTest001 end" << std::endl;
}

bool ExpirelruCacheTestCheckNums(ExpireLruCache<std::vector<char>, std::vector<char>>& cache, size_t expectNums)
{
    size_t dataSize = cache.data_.size();
    size_t timeStampSize = cache.timestamp_.size();
    size_t keysSize = cache.keys_.size();

    if (dataSize != expectNums) {
        EXPECT_EQ(dataSize, expectNums);
        return false;
    }
    if (timeStampSize != expectNums) {
        EXPECT_EQ(timeStampSize, expectNums);
        return false;
    }
    if (keysSize != expectNums) {
        EXPECT_EQ(keysSize, expectNums);
        return false;
    }
    return true;
}

bool ExpirelruCacheTestCheckSequence(ExpireLruCache<std::vector<char>, std::vector<char>>& cache,
    std::list<vector<char>>& expectSequence)
{
    auto iterA = cache.keys_.begin();
    auto iterB = expectSequence.begin();
    for (size_t i = 0; i < cache.keys_.size(); i++) {
        if (*iterA != *iterB) {
            EXPECT_EQ(*iterA, *iterB);
            return false;
        }
        iterA++;
        iterB++;
    }
    return true;
}

bool ExpirelruCacheTestCheckGetRet(ExpireLruCache<std::vector<char>, std::vector<char>>& cache,
    vector<char>& key, vector<char>& val)
{
    std::shared_ptr<vector<char>> retVal = cache.Get(key);
    if (*retVal != val) {
        EXPECT_EQ(*retVal == val, 0);
        return false;
    }
    return true;
}
/**
 * @tc.name: ADD_GET_CLEAR_Test001
 * @tc.desc: test add、get、clear api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ExpireLruCacheTest, ADD_GET_CLEAR_Test001, TestSize.Level2)
{
    DTEST_LOG << "ADD_GET_CLEAR_Test001 start" << std::endl;
    ExpireLruCache<std::vector<char>, std::vector<char>> cache;

    cache.Add(g_Key1, g_Val1);
    cache.Add(g_Key2, g_Val2);
    cache.Add(g_Key3, g_Val3);
    cache.Add(g_Key4, g_Val4);
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 4), true);

    std::list<vector<char>> expectSequence1 = {g_Key4, g_Key3, g_Key2, g_Key1};
    EXPECT_EQ(ExpirelruCacheTestCheckSequence(cache, expectSequence1), true);

    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key4, g_Val4), true);
    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key3, g_Val3), true);
    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key2, g_Val2), true);
    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key1, g_Val1), true);

    std::list<vector<char>> expectSequence2 = {g_Key1, g_Key2, g_Key3, g_Key4};
    EXPECT_EQ(ExpirelruCacheTestCheckSequence(cache, expectSequence2), true);

    cache.Add(g_Key1, g_Val1);
    EXPECT_EQ(cache.data_.size(), 4);

    cache.Add(g_Key2, g_Val3);
    EXPECT_EQ(cache.data_.size(), 4);
    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key2, g_Val3), true);

    std::list<vector<char>> expectSequence3 = {g_Key2, g_Key1, g_Key3, g_Key4};
    EXPECT_EQ(ExpirelruCacheTestCheckSequence(cache, expectSequence3), true);

    cache.Remove(g_Key3);
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 3), true);
    {
        std::shared_ptr<vector<char>> retVal;
        retVal = cache.Get(g_Key3);
        EXPECT_EQ(retVal, nullptr);
    }

    cache.Clear();
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 0), true);

    {
        std::shared_ptr<vector<char>> retVal;
        retVal = cache.Get(g_Key2);
        ASSERT_TRUE(retVal == nullptr);
    }
    {
        std::shared_ptr<vector<char>> retVal;
        retVal = cache.Get(g_Key4);
        ASSERT_TRUE(retVal == nullptr);
    }
    DTEST_LOG << "ADD_GET_CLEAR_Test001 end" << std::endl;
}

/**
 * @tc.name: LRUTest001
 * @tc.desc: test the cache map LRU
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ExpireLruCacheTest, LRUTest001, TestSize.Level2)
{
    DTEST_LOG << "LRUTest001 start" << std::endl;
    // cacheMapSize 3 expireTime 10000000us
    ExpireLruCache<std::vector<char>, std::vector<char>> cache(3, 10000);

    cache.Add(g_Key1, g_Val1);
    cache.Add(g_Key2, g_Val2);
    cache.Add(g_Key3, g_Val3);
    // when [g_Key4, g_Val4] is added, the cache is full and the leasted recently used [g_Key1, g_Val1] will be deleted
    cache.Add(g_Key4, g_Val4);

    {
        std::list<vector<char>> expectSequence = {g_Key4, g_Key3, g_Key2};
        auto iterA = expectSequence.begin();
        auto iterB = cache.keys_.begin();
        for (int i = 0; i < cache.keys_.size(); i++) {
            ASSERT_TRUE(*iterA == *iterB);
            iterA++;
            iterB++;
        }
        auto retVal = cache.Get(g_Key1);
        EXPECT_EQ(retVal, nullptr);
    }
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 3), true);

    {
        auto retVal = cache.Get(g_Key2);
        EXPECT_EQ(*retVal, g_Val2);
    }
    // when [g_Key5, g_Val5] is added, the cache is full and the leasted recently used [g_Key3, g_Val3] will be deleted
    cache.Add(g_Key5, g_Val5);
    {
        std::list<vector<char>> expectSequence = {g_Key5, g_Key2, g_Key4};
        auto iterA = expectSequence.begin();
        auto iterB = cache.keys_.begin();
        for (int i = 0; i < cache.keys_.size(); i++) {
            ASSERT_TRUE(*iterA == *iterB);
            iterA++;
            iterB++;
        }
        auto retVal = cache.Get(g_Key3);
        EXPECT_EQ(retVal, nullptr);
    }
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 3), true);
    DTEST_LOG << "LRUTest001 end" << std::endl;
}

/**
 * @tc.name: LRUTest002
 * @tc.desc: test the cache map LRU
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ExpireLruCacheTest, LRUTest002, TestSize.Level2)
{
    DTEST_LOG << "LRUTest002 start" << std::endl;
    // cacheMapSize 3 expireTime 40000us
    ExpireLruCache<std::vector<char>, std::vector<char>> cache(3, 40);

    cache.Add(g_Key1, g_Val1);
    usleep(30000);
    cache.Add(g_Key2, g_Val2);
    cache.Add(g_Key3, g_Val3);

    {
        auto retVal = cache.Get(g_Key1);
        EXPECT_EQ(*retVal, g_Val1);
        std::list<vector<char>> expectSequence = {g_Key1, g_Key3, g_Key2};
        auto iterA = expectSequence.begin();
        auto iterB = cache.keys_.begin();
        for (int i = 0; i < cache.keys_.size(); i++) {
            ASSERT_TRUE(*iterA == *iterB);
            iterA++;
            iterB++;
        }
    }
    usleep(20000);
    // [g_Key1, g_Val1] is expired, add [g_Key4, g_Val4] will eliminate [g_Key1, g_Val1]
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 3), true);
    cache.Add(g_Key4, g_Val4);
    {
        std::list<vector<char>> expectSequence = {g_Key4, g_Key3, g_Key2};
        auto iterA = expectSequence.begin();
        auto iterB = cache.keys_.begin();
        for (int i = 0; i < cache.keys_.size(); i++) {
            ASSERT_TRUE(*iterA == *iterB);
            iterA++;
            iterB++;
        }
        auto retVal = cache.Get(g_Key1);
        EXPECT_EQ(retVal, nullptr);
    }
    usleep(50000);
    // all caches are expired, add [g_Key5, g_Val5] will eliminate [g_Key2, g_Val2][g_Key3, g_Val3][g_Key4, g_Val4]
    cache.Add(g_Key5, g_Val5);
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 1), true);
    {
        auto retVal = cache.Get(g_Key5);
        EXPECT_EQ(*retVal, g_Val5);
    }
    DTEST_LOG << "LRUTest002 end" << std::endl;
}

/**
 * @tc.name: GetTest001
 * @tc.desc: test get api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ExpireLruCacheTest, GetTest001, TestSize.Level2)
{
    DTEST_LOG << "GetTest001 start" << std::endl;
    ExpireLruCache<std::vector<char>, std::vector<char>> cache(3, 50);

    {
        auto retVal = cache.Get(g_Key1);
        EXPECT_EQ(retVal, nullptr);
    }

    cache.Add(g_Key1, g_Val1);
    {
        auto retVal = cache.Get(g_Key1);
        EXPECT_EQ(*retVal, g_Val1);
    }
    usleep(70000);
    // [g_Key1, g_Val1] is expired, get(g_Key1) will return nullptr and clear invalid cache [g_Key1, g_Val1]
    {
        auto retVal = cache.Get(g_Key1);
        EXPECT_EQ(retVal, nullptr);
        EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 0), true);
    }

    cache.Add(g_Key2, g_Val2);
    cache.Add(g_Key3, g_Val3);
    cache.Add(g_Key4, g_Val4);
    // Verify the cache sequence in the list
    {
        std::list<vector<char>> expectSequence = {g_Key4, g_Key3, g_Key2};
        auto iterA = expectSequence.begin();
        auto iterB = cache.keys_.begin();
        for (int i = 0; i < cache.keys_.size(); i++) {
            ASSERT_TRUE(*iterA == *iterB);
            iterA++;
            iterB++;
        }
    }
    {
        auto retVal = cache.Get(g_Key2);
        EXPECT_EQ(*retVal, g_Val2);
        auto retVal2 = cache.Get(g_Key3);
        EXPECT_EQ(*retVal2, g_Val3);
    }
    // Verify the cache sequence in the list
    {
        std::list<vector<char>> expectSequence = {g_Key3, g_Key2, g_Key4};
        auto iterA = expectSequence.begin();
        auto iterB = cache.keys_.begin();
        for (int i = 0; i < cache.keys_.size(); i++) {
            ASSERT_TRUE(*iterA == *iterB);
            iterA++;
            iterB++;
        }
    }
    DTEST_LOG << "GetTest001 end" << std::endl;
}

/**
 * @tc.name: Timestamp001
 * @tc.desc: test Class Timestamp
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ExpireLruCacheTest, Timestamp001, TestSize.Level2)
{
    DTEST_LOG << "Timestamp001 start" << std::endl;
    // test operator '-' and timestamp is increasing
    ExpireLruCache<int, int>::Timestamp t1;
    usleep(3000);
    ExpireLruCache<int, int>::Timestamp t2;
    int64_t diff = t2 - t1;
    bool ret;
    ASSERT_TRUE(diff > 0);

    // test expired func, expired
    usleep(5000);
    ret = t1.IsExpired(1);
    EXPECT_EQ(ret, true);

    // test expired func, not expired
    ExpireLruCache<int, int>::Timestamp t3;
    usleep(2000);
    ret = t3.IsExpired(10);
    EXPECT_EQ(ret, false);
    DTEST_LOG << "Timestamp001 end" << std::endl;
}

/**
 * @tc.name: RemoveTest001
 * @tc.desc: test remove api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(ExpireLruCacheTest, RemoveTest001, TestSize.Level2)
{
    DTEST_LOG << "RemoveTest001 start" << std::endl;
    ExpireLruCache<std::vector<char>, std::vector<char>> cache(3, 2000);

    cache.Add(g_Key1, g_Val1);
    cache.Add(g_Key3, g_Val3);
    cache.Add(g_Key2, g_Val2);
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 3), true);

    std::list<vector<char>> expectSequence1 = {g_Key2, g_Key3, g_Key1};
    EXPECT_EQ(ExpirelruCacheTestCheckSequence(cache, expectSequence1), true);

    cache.Remove(g_Key4);
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 3), true);
    std::list<vector<char>> expectSequence2 = {g_Key2, g_Key3, g_Key1};
    EXPECT_EQ(ExpirelruCacheTestCheckSequence(cache, expectSequence2), true);

    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key1, g_Val1), true);
    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key3, g_Val3), true);
    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key2, g_Val2), true);

    cache.Remove(g_Key3);
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 2), true);

    std::list<vector<char>> expectSequence3 = {g_Key2, g_Key1};
    EXPECT_EQ(ExpirelruCacheTestCheckSequence(cache, expectSequence3), true);
    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key2, g_Val2), true);
    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key1, g_Val1), true);
    {
        auto retVal3 = cache.Get(g_Key3);
        EXPECT_EQ(retVal3, nullptr);
    }
    cache.Remove(g_Key2);
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 1), true);

    EXPECT_EQ(ExpirelruCacheTestCheckGetRet(cache, g_Key1, g_Val1), true);
    {
        auto retVal2 = cache.Get(g_Key2);
        EXPECT_EQ(retVal2, nullptr);
    }
    cache.Remove(g_Key1);
    EXPECT_EQ(ExpirelruCacheTestCheckNums(cache, 0), true);
    {
        auto retval1 = cache.Get(g_Key1);
        EXPECT_EQ(retval1, nullptr);
    }
    DTEST_LOG << "RemoveTest001 end" << std::endl;
}
}