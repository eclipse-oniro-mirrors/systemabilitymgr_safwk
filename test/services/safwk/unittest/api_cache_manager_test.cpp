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
#include <thread>
#include "gtest/gtest.h"
#include "test_log.h"
#include "api_cache_manager.h"
#include "message_parcel.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"

using namespace testing::ext;
namespace OHOS {
namespace {
    std::u16string g_descriptor1 = u"testdescriptor1";
    std::u16string g_descriptor2 = u"testdescriptor2";
    std::u16string g_descriptor3 = u"testdescriptor3";

    const std::u16string CACHE_KEY_STR_1 = u"ohos.cacheAPIKey.1";
    const std::u16string CACHE_KEY_STR_2 = u"ohos.cacheAPIKey.2";
    const std::u16string CACHE_VALUE_STR_1 = u"ohos.cacheAPIValue.1";
    const std::u16string CACHE_VALUE_STR_2 = u"ohos.cacheAPIValue.2";
    const std::string CACHE_KEY_STR_3 = "ohoscacheAPIKey3";
    const std::string CACHE_VALUE_STR_3 = "ohoscacheAPIValue3";
    constexpr int32_t CACHE_KEY_INT_1 = 9999;
    constexpr int32_t CACHE_VALUE_INT_1 = 6666;

    constexpr uint32_t CACHE_API_CODE_100 = 100;
    constexpr uint32_t CACHE_API_CODE_1 = 1;

    constexpr int64_t EXPIRE_TIME_1S = 1000;
    constexpr int64_t EXPIRE_TIME_100S = 100000;
    constexpr int64_t EXPIRE_TIME_3S = 3000;

}

class CacheManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CacheManagerTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void CacheManagerTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void CacheManagerTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void CacheManagerTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}
/**
 * @tc.name: AddAndDeleteCacheAPI001
 * @tc.desc: test AddCacheApi and DelCacheApi
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheManagerTest, AddAndDeleteCacheAPI001, TestSize.Level0)
{
    DTEST_LOG << "AddAndDeleteCacheAPI001 start" << std::endl;
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor1, CACHE_API_CODE_100, EXPIRE_TIME_100S);
    EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), 1);

    // repeated addition
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor1, CACHE_API_CODE_100, EXPIRE_TIME_100S);
    EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), 1);

    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor2, CACHE_API_CODE_100, EXPIRE_TIME_100S);
    EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), 2);

    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor1, CACHE_API_CODE_1);
    EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), 2);

    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor1, CACHE_API_CODE_100);
    EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), 1);

    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor2, CACHE_API_CODE_100);
    EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), 0);
    DTEST_LOG << "AddAndDeleteCacheAPI001 end" << std::endl;
}

/**
 * @tc.name: PreSendRequest001
 * @tc.desc: test AddCacheApi and DelCacheApi
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheManagerTest, PreSendRequest001, TestSize.Level2)
{
    DTEST_LOG << "PreSendRequest001 start" << std::endl;
    bool ret;

    MessageParcel data1;
    MessageParcel reply1;
    // (des1, apicode100) api has not enabled the proxy cache
    ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data1, reply1);
    EXPECT_EQ(ret, false);

    MessageParcel data2;
    MessageParcel reply2;
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor1, CACHE_API_CODE_100, EXPIRE_TIME_3S);
    ret = data2.WriteString16(CACHE_KEY_STR_1);
    EXPECT_EQ(ret, true);
    // cache miss
    ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data2, reply2);
    EXPECT_EQ(ret, false);

    MessageParcel data3;
    MessageParcel reply3;
    data3.WriteString16(CACHE_KEY_STR_1);
    reply3.WriteString16(CACHE_VALUE_STR_1);
    // add cache
    ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data3, reply3);
    EXPECT_EQ(ret, true);

    MessageParcel data4;
    MessageParcel reply4;
    std::u16string cacheReplyStr1;
    data4.WriteString16(CACHE_KEY_STR_1);
    // cache hit
    ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data4, reply4);
    EXPECT_EQ(ret, true);

    reply4.ReadString16(cacheReplyStr1);
    EXPECT_EQ(cacheReplyStr1.compare(CACHE_VALUE_STR_1), 0);

    MessageParcel data5;
    MessageParcel reply5;
    std::u16string cacheReplyStr2;
    data5.WriteString16(CACHE_KEY_STR_2);
    ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data5, reply5);
    EXPECT_EQ(ret, false);

    ret = reply5.ReadString16(cacheReplyStr2);
    EXPECT_EQ(ret, false);

    // cache expiration
    usleep(1 * 1000 * 1000);
    MessageParcel data6;
    MessageParcel reply6;
    ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data6, reply6);
    EXPECT_EQ(ret, false);
    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor1, CACHE_API_CODE_100);
    DTEST_LOG << "PreSendRequest001 end" << std::endl;
}

void LRUTest001AddCache1()
{
    bool testTrueBool = true;
    bool testFalseBool = true;
    bool ret;
    {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(data.WriteBool(testTrueBool), true);

        EXPECT_EQ(reply.WriteBool(testFalseBool), true);
        ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
        EXPECT_EQ(ret, true);
    }
    {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(data.WriteBool(testTrueBool), true);

        ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
        EXPECT_EQ(ret, true);
        EXPECT_EQ(reply.ReadBool(ret), true);
        EXPECT_EQ(ret, testFalseBool);
    }

    return;
}

void LRUTest001AddCache2()
{
    int32_t keyInt32T = -999;
    int16_t keyInt16T = -666;
    double keyDouble = -3.1445926358979323846;
    float keyFloat = 0.012345;
    std::u16string keyU16string = u"keyU16161616Ustring";
    double valDouble = -9.3284928942;
    float valFloat = 7.32489;
    std::string valString = "valstringvalvalvalvalvalvalvalval";
    bool ret;
    {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(data.WriteInt32(keyInt32T), true);
        EXPECT_EQ(data.WriteInt16(keyInt16T), true);
        EXPECT_EQ(data.WriteDouble(keyDouble), true);
        EXPECT_EQ(data.WriteFloat(keyFloat), true);
        EXPECT_EQ(data.WriteString16(keyU16string), true);

        EXPECT_EQ(reply.WriteDouble(valDouble), true);
        EXPECT_EQ(reply.WriteFloat(valFloat), true);
        EXPECT_EQ(reply.WriteString(valString), true);
        ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
        EXPECT_EQ(ret, true);
    }
    {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(data.WriteInt32(keyInt32T), true);
        EXPECT_EQ(data.WriteInt16(keyInt16T), true);
        EXPECT_EQ(data.WriteDouble(keyDouble), true);
        EXPECT_EQ(data.WriteFloat(keyFloat), true);
        EXPECT_EQ(data.WriteString16(keyU16string), true);

        ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
        EXPECT_EQ(ret, true);
        double retdouble;
        float retfloat;
        std::string retstring;
        EXPECT_EQ(reply.ReadDouble(retdouble), true);
        EXPECT_EQ(reply.ReadFloat(retfloat), true);
        EXPECT_EQ(reply.ReadString(retstring), true);
        EXPECT_EQ(retdouble, valDouble);
        EXPECT_EQ(retfloat, valFloat);
        EXPECT_EQ(retstring, valString);
    }
    return;
}

void LRUTest001AddCache3()
{
    double keyDouble = -3.1445926358979323846;
    float keyFloat = 0.012345;
    std::u16string keyU16string = u"keyU16161616Ustring";
    std::vector<std::string> keyStringVector = {"hello", "world", "!"};
    std::vector<std::u16string> key16StringVector = {u"key", u"vector", u"!!!"};

    double valDouble = -9.3284928942;
    float valFloat = 7.32489;
    std::string valString = "valstringvalvalvalvalvalvalvalval";
    std::vector<std::u16string> val16StringVector = {u"val", u"hhh", u"!!!"};

    MessageParcel data;
    MessageParcel reply;
    EXPECT_EQ(data.WriteStringVector(keyStringVector), true);
    EXPECT_EQ(data.WriteString16Vector(key16StringVector), true);
    EXPECT_EQ(data.WriteDouble(keyDouble), true);
    EXPECT_EQ(data.WriteFloat(keyFloat), true);
    EXPECT_EQ(data.WriteString16(keyU16string), true);

    EXPECT_EQ(reply.WriteDouble(valDouble), true);
    EXPECT_EQ(reply.WriteFloat(valFloat), true);
    EXPECT_EQ(reply.WriteString(valString), true);
    EXPECT_EQ(reply.WriteString16Vector(val16StringVector), true);
    auto ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
    EXPECT_EQ(ret, true);
}

void LRUTest001AddCache4()
{
    std::u16string keyU16string = u"keyU16161616Ustring";
    std::vector<std::string> keyStringVector = {"hello", "world", "!"};
    double valDouble = -9.3284928942;
    float valFloat = 7.32489;
    bool ret;

    {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(data.WriteStringVector(keyStringVector), true);
        EXPECT_EQ(data.WriteString16(keyU16string), true);

        EXPECT_EQ(reply.WriteDouble(valDouble), true);
        EXPECT_EQ(reply.WriteFloat(valFloat), true);
        ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
        EXPECT_EQ(ret, true);
    }

    {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(data.WriteStringVector(keyStringVector), true);
        EXPECT_EQ(data.WriteString16(keyU16string), true);

        ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
        EXPECT_EQ(ret, true);
        double retdouble;
        float retfloat;
        EXPECT_EQ(reply.ReadDouble(retdouble), true);
        EXPECT_EQ(reply.ReadFloat(retfloat), true);
        EXPECT_EQ(retdouble, valDouble);
        EXPECT_EQ(retfloat, valFloat);
    }
    return;
}

void LRUTest001AddCache5()
{
    bool ret;
    const std::u16string testKeyToken = u"ohos.keytesttoken.KEYTESTTOKEN";
    const std::u16string testValToken = u"ohos.valtesttoken.VALTESTTOKEN";
    MessageParcel data;
    MessageParcel reply;
    EXPECT_EQ(data.WriteInterfaceToken(testKeyToken), true);

    EXPECT_EQ(reply.WriteInterfaceToken(testValToken), true);
    ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
    EXPECT_EQ(ret, true);
    return;
}

void LRUTest001AddCache6()
{
    uintptr_t keyPtr = 6666;
    uintptr_t valPtr = 300;
    bool ret;
    {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(data.WritePointer(keyPtr), true);

        EXPECT_EQ(reply.WritePointer(valPtr), true);
        ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
        EXPECT_EQ(ret, true);
    }
    {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(data.WritePointer(keyPtr), true);

        ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
        EXPECT_EQ(ret, true);
        EXPECT_EQ(reply.ReadPointer(), valPtr);
    }
    return;
}

void LRUTest001AddCache7()
{
    std::string valString = "valstringvalvalvalvalvalvalvalval";
    uint16_t keyUint16T = 777;
    MessageParcel data;
    MessageParcel reply;
    EXPECT_EQ(data.WriteUint16Unaligned(keyUint16T), true);

    EXPECT_EQ(reply.WriteString(valString), true);
    auto ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
    EXPECT_EQ(ret, true);
    return;
}

void LRUTest001ParcelCache8(MessageParcel& data)
{
    int32_t keyInt32T = -999;
    uint32_t keyUint32T = 888;
    int16_t keyInt16T = -666;
    uint16_t keyUint16T = 777;
    double keyDouble = -3.1445926358979323846;
    float keyFloat = 0.012345;
    std::string keyString = "keystringabcdefghijklmn";
    std::u16string keyU16string = u"keyU16161616Ustring";
    std::vector<std::string> keyStringVector = {"hello", "world", "!"};
    std::vector<std::u16string> key16StringVector = {u"key", u"vector", u"!!!"};
    const std::u16string testKeyToken = u"ohos.keytesttoken.KEYTESTTOKEN";
    uintptr_t keyPtr = 6666;
    bool testTrueBool = true;

    EXPECT_EQ(data.WriteInterfaceToken(testKeyToken), true);
    EXPECT_EQ(data.WriteInt32(keyInt32T), true);
    EXPECT_EQ(data.WriteUint32(keyUint32T), true);
    EXPECT_EQ(data.WriteInt16(keyInt16T), true);
    EXPECT_EQ(data.WriteUint16(keyUint16T), true);
    EXPECT_EQ(data.WriteDouble(keyDouble), true);
    EXPECT_EQ(data.WriteFloat(keyFloat), true);
    EXPECT_EQ(data.WriteString(keyString), true);
    EXPECT_EQ(data.WriteString16(keyU16string), true);
    EXPECT_EQ(data.WriteStringVector(keyStringVector), true);
    EXPECT_EQ(data.WriteString16Vector(key16StringVector), true);
    EXPECT_EQ(data.WriteBool(testTrueBool), true);
    EXPECT_EQ(data.WritePointer(keyPtr), true);
    return;
}

void LRUTest001AddCache8()
{
    double valDouble = -9.3284928942;
    float valFloat = 7.32489;
    std::string valString = "valstringvalvalvalvalvalvalvalval";
    std::vector<std::string> valStringVector = {"val", "abc", "!"};
    std::vector<std::u16string> val16StringVector = {u"val", u"hhh", u"!!!"};
    const std::u16string testValToken = u"ohos.valtesttoken.VALTESTTOKEN";
    uintptr_t valPtr = 300;
    MessageParcel data;
    MessageParcel reply;

    LRUTest001ParcelCache8(data);

    EXPECT_EQ(reply.WriteDouble(valDouble), true);
    EXPECT_EQ(reply.WriteFloat(valFloat), true);
    EXPECT_EQ(reply.WriteString(valString), true);
    EXPECT_EQ(reply.WriteStringVector(valStringVector), true);
    EXPECT_EQ(reply.WriteString16Vector(val16StringVector), true);
    EXPECT_EQ(reply.WriteInterfaceToken(testValToken), true);
    EXPECT_EQ(reply.WritePointer(valPtr), true);
    auto ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
    EXPECT_EQ(ret, true);
    return;
}

void LRUTest001CheckCache8()
{
    double valDouble = -9.3284928942;
    float valFloat = 7.32489;
    std::string valString = "valstringvalvalvalvalvalvalvalval";
    std::vector<std::string> valStringVector = {"val", "abc", "!"};
    std::vector<std::u16string> val16StringVector = {u"val", u"hhh", u"!!!"};
    const std::u16string testValToken = u"ohos.valtesttoken.VALTESTTOKEN";
    uintptr_t valPtr = 300;

    MessageParcel data;
    MessageParcel reply;
    LRUTest001ParcelCache8(data);

    auto ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
    EXPECT_EQ(ret, true);
    double retdouble;
    float retfloat;
    std::string retString;
    std::vector<std::string> retStringVector;
    std::vector<std::u16string> retU16StringVector;
    std::u16string retdescriptor;
    EXPECT_EQ(reply.ReadDouble(retdouble), true);
    EXPECT_EQ(retdouble, valDouble);
    EXPECT_EQ(reply.ReadFloat(retfloat), true);
    EXPECT_EQ(retfloat, valFloat);
    EXPECT_EQ(reply.ReadString(retString), true);
    EXPECT_EQ(retString, valString);
    EXPECT_EQ(reply.ReadStringVector(&retStringVector), true);
    EXPECT_EQ(retStringVector, valStringVector);
    EXPECT_EQ(reply.ReadString16Vector(&retU16StringVector), true);
    EXPECT_EQ(retU16StringVector, val16StringVector);
    EXPECT_EQ(reply.ReadInterfaceToken(), testValToken);
    EXPECT_EQ(reply.ReadPointer(), valPtr);
}

void LRUTest001CheckCacheNums(int32_t expectNums)
{
    std::pair<std::u16string, uint32_t> myPair = std::make_pair(g_descriptor1, CACHE_API_CODE_100);
    auto apiCache = ApiCacheManager::GetInstance().caches_.find(myPair);
    EXPECT_NE(apiCache, ApiCacheManager::GetInstance().caches_.end());
    if (apiCache != ApiCacheManager::GetInstance().caches_.end()) {
        EXPECT_EQ(apiCache->second->data_.size(), expectNums);
        EXPECT_EQ(apiCache->second->timestamp_.size(), expectNums);
        EXPECT_EQ(apiCache->second->keys_.size(), expectNums);
    }
    return;
}

void LRUTest001AddCache9()
{
    int32_t keyInt32T = -999;
    std::string valString = "valstringvalvalvalvalvalvalvalval";
    MessageParcel data;
    MessageParcel reply;
    EXPECT_EQ(data.WriteInt32(keyInt32T), true);

    EXPECT_EQ(reply.WriteString(valString), true);
    auto ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
    EXPECT_EQ(ret, true);
    return;
}
/**
 * @tc.name: LRUTest001
 * @tc.desc: test LRU elimination
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheManagerTest, LRUTest001, TestSize.Level2)
{
    DTEST_LOG << "LRUTest001 start" << std::endl;
    bool ret;
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor1, CACHE_API_CODE_100, EXPIRE_TIME_1S);
    float keyFloat = 0.012345;
    std::u16string keyU16string = u"keyU16161616Ustring";
    bool testTrueBool = true;
    double valDouble = -9.3284928942;

    // add cache 1
    LRUTest001AddCache1();
    // add cache 2
    LRUTest001AddCache2();
    // add cache 3
    LRUTest001AddCache3();
    // add cache 4
    LRUTest001AddCache4();
    // add cache 5
    LRUTest001AddCache5();
    // add cache 6
    LRUTest001AddCache6();
    // add cache 7
    LRUTest001AddCache7();
    // add cache 8
    LRUTest001AddCache8();
    LRUTest001CheckCache8();
    // check the number of caches
    LRUTest001CheckCacheNums(8);
    // add cache 9, cache map full, will eliminate earliest recently used cache
    LRUTest001AddCache9();
    // check the earliest recently used cache is eliminated
    {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(data.WriteBool(testTrueBool), true);
        ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
        EXPECT_EQ(ret, false);
    }

    // wait for all caches expired
    usleep(1200000);
    // add cache, because the cache map is full, it will eliminate all expired cache in cache map
    {
        MessageParcel data;
        MessageParcel reply;
        EXPECT_EQ(data.WriteFloat(keyFloat), true);

        EXPECT_EQ(reply.WriteDouble(valDouble), true);
        ret = ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
    }
    LRUTest001CheckCacheNums(1);
    {
        MessageParcel data;
        MessageParcel reply;
        double retDouble;
        EXPECT_EQ(data.WriteFloat(keyFloat), true);

        ret = ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data, reply);
        EXPECT_EQ(ret, true);
        EXPECT_EQ(reply.ReadDouble(retDouble), true);
        EXPECT_EQ(retDouble, valDouble);
    }
    ApiCacheManager::GetInstance().ClearCache();
    DTEST_LOG << "LRUTest001 end" << std::endl;
}

void ClearCache001TestParcelData(MessageParcel& data, const std::u16string& data1, int32_t data2)
{
    data.WriteString16(data1);
    data.WriteInt32(data2);
    return;
}

void ClearCache001TestParcelData(MessageParcel& data, const std::string& data1, int32_t data2)
{
    data.WriteString(data1);
    data.WriteInt32(data2);
    return;
}

void ClearCache001TestParcelData(MessageParcel& data, int32_t data1, const std::u16string& data2)
{
    data.WriteInt32(data1);
    data.WriteString16(data2);
    return;
}

void ClearCache001TestParcelData(MessageParcel& data, int32_t data1, const std::string& data2)
{
    data.WriteInt32(data1);
    data.WriteString(data2);
    return;
}

void ClearCache001TestParcelData(MessageParcel& data, int32_t data1)
{
    data.WriteInt32(data1);
    return;
}

bool ClearCache001TestCheckParcelData(MessageParcel& data, const std::string& data1, int32_t data2)
{
    bool ret;
    std::string cacheReplyStr;
    int32_t cacheReplyInt;

    ret = data.ReadString(cacheReplyStr);
    if (ret != true) {
        EXPECT_EQ(ret, true);
        return false;
    }
    if (cacheReplyStr != data1) {
        EXPECT_EQ(cacheReplyStr, data1);
        return false;
    }
    ret = data.ReadInt32(cacheReplyInt);
    if (ret != true) {
        EXPECT_EQ(ret, true);
        return false;
    }
    if (cacheReplyInt != data2) {
        EXPECT_EQ(cacheReplyStr, data1);
        return false;
    }
    return true;
}

bool ClearCache001TestCheckParcelData(MessageParcel& data, const std::u16string& data1, int32_t data2)
{
    bool ret;
    std::u16string cacheReplyStr;
    int32_t cacheReplyInt;

    ret = data.ReadString16(cacheReplyStr);
    if (ret != true) {
        EXPECT_EQ(ret, true);
        return false;
    }
    if (cacheReplyStr != data1) {
        EXPECT_EQ(cacheReplyStr, data1);
        return false;
    }
    ret = data.ReadInt32(cacheReplyInt);
    if (ret != true) {
        EXPECT_EQ(ret, true);
        return false;
    }
    if (cacheReplyInt != data2) {
        EXPECT_EQ(cacheReplyStr, data1);
        return false;
    }
    return true;
}

bool ClearCache001TestCheckParcelData(MessageParcel& data, int32_t data1, const std::string& data2)
{
    bool ret;
    std::string cacheReplyStr;
    int32_t cacheReplyInt;

    ret = data.ReadInt32(cacheReplyInt);
    if (ret != true) {
        EXPECT_EQ(ret, true);
        return false;
    }
    if (cacheReplyInt != data1) {
        EXPECT_EQ(cacheReplyInt, data1);
        return false;
    }
    ret = data.ReadString(cacheReplyStr);
    if (ret != true) {
        EXPECT_EQ(ret, true);
        return false;
    }
    if (cacheReplyStr != data2) {
        EXPECT_EQ(cacheReplyStr, data2);
        return false;
    }
    return true;
}

bool ClearCache001TestCheckParcelData(MessageParcel& data, int32_t data1, const std::u16string& data2)
{
    bool ret;
    std::u16string cacheReplyStr;
    int32_t cacheReplyInt;

    ret = data.ReadInt32(cacheReplyInt);
    if (ret != true) {
        EXPECT_EQ(ret, true);
        return false;
    }
    if (cacheReplyInt != data1) {
        EXPECT_EQ(cacheReplyInt, data1);
        return false;
    }
    ret = data.ReadString16(cacheReplyStr);
    if (ret != true) {
        EXPECT_EQ(ret, true);
        return false;
    }
    if (cacheReplyStr != data2) {
        EXPECT_EQ(cacheReplyStr, data2);
        return false;
    }
    return true;
}

bool ClearCache001TestPrevSet(const std::u16string& descriptor, uint32_t apiCode,
    const std::u16string& key, const std::u16string& val)
{
    bool ret;
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(key);
    reply.WriteString16(val);
    ret = ApiCacheManager::GetInstance().PostSendRequest(descriptor, apiCode, data, reply);
    return ret;
}

bool ClearCache001TestPrevSet(const std::u16string& descriptor, uint32_t apiCode,
    const std::string& key, const std::string& val)
{
    bool ret;
    MessageParcel data;
    MessageParcel reply;
    data.WriteString(key);
    reply.WriteString(val);
    ret = ApiCacheManager::GetInstance().PostSendRequest(descriptor, apiCode, data, reply);
    return ret;
}

bool ClearCache001TestPrevSet(const std::u16string& descriptor, uint32_t apiCode,
    MessageParcel& data, MessageParcel& reply)
{
    bool ret = ApiCacheManager::GetInstance().PostSendRequest(descriptor, apiCode, data, reply);
    return ret;
}

bool ClearCache001TestCheck(const std::u16string& descriptor, uint32_t apiCode,
    const std::u16string& key, const std::u16string& val, bool expectRet)
{
    bool ret;
    MessageParcel data;
    MessageParcel reply;
    data.WriteString16(key);
    ret = ApiCacheManager::GetInstance().PreSendRequest(descriptor, apiCode, data, reply);
    if (ret != expectRet) {
        EXPECT_EQ(ret, expectRet);
        return false;
    }
    if (expectRet == false) {
        return true;
    }

    std::u16string cacheReplyStr;
    ret = reply.ReadString16(cacheReplyStr);
    if (ret != true) {
        EXPECT_EQ(ret, true);
        return false;
    }
    if (cacheReplyStr.compare(val) != 0) {
        EXPECT_EQ(cacheReplyStr.compare(val), 0);
        return false;
    }
    return true;
}

void ClearCache001TestStep1AddCache()
{
    // add SA:descriptor1 API_CODE:100, cache: [keystr1, valstr1], [keystr2, valstr2]
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor1, CACHE_API_CODE_100, EXPIRE_TIME_3S);
    EXPECT_EQ(ClearCache001TestPrevSet(g_descriptor1, CACHE_API_CODE_100, CACHE_KEY_STR_1, CACHE_VALUE_STR_1), true);
    EXPECT_EQ(ClearCache001TestPrevSet(g_descriptor1, CACHE_API_CODE_100, CACHE_KEY_STR_2, CACHE_VALUE_STR_2), true);

    // add SA:descriptor1 API_CODE:1, cache: [keystr2, valstr2], [keystr3, valstr3]
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor1, CACHE_API_CODE_1, EXPIRE_TIME_3S);
    EXPECT_EQ(ClearCache001TestPrevSet(g_descriptor1, CACHE_API_CODE_1, CACHE_KEY_STR_2, CACHE_VALUE_STR_2), true);
    EXPECT_EQ(ClearCache001TestPrevSet(g_descriptor1, CACHE_API_CODE_1, CACHE_KEY_STR_3, CACHE_VALUE_STR_3), true);

    // add SA:descriptor2 API_CODE:1, cache: [keystr3, valstr3_valint1], [keystr2, valstr2]
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor2, CACHE_API_CODE_1, EXPIRE_TIME_3S);
    {
        MessageParcel data;
        MessageParcel reply;
        ClearCache001TestParcelData(data, CACHE_KEY_STR_3, CACHE_KEY_INT_1);
        ClearCache001TestParcelData(reply, CACHE_VALUE_STR_3, CACHE_VALUE_INT_1);
        EXPECT_EQ(ClearCache001TestPrevSet(g_descriptor2, CACHE_API_CODE_1, data, reply), true);
    }

    EXPECT_EQ(ClearCache001TestPrevSet(g_descriptor2, CACHE_API_CODE_1, CACHE_KEY_STR_2, CACHE_VALUE_STR_2), true);

    // add SA:descriptor2 API_CODE:100, cache: [keyint1, valint1_valstr3], [keystr2, valstr2]
    {
        MessageParcel data;
        MessageParcel reply;
        ApiCacheManager::GetInstance().AddCacheApi(g_descriptor2, CACHE_API_CODE_100, EXPIRE_TIME_3S);
        ClearCache001TestParcelData(data, CACHE_KEY_INT_1);
        ClearCache001TestParcelData(reply, CACHE_VALUE_INT_1, CACHE_VALUE_STR_3);
        EXPECT_EQ(ClearCache001TestPrevSet(g_descriptor2, CACHE_API_CODE_100, data, reply), true);
        EXPECT_EQ(ClearCache001TestPrevSet(g_descriptor2, CACHE_API_CODE_100, CACHE_KEY_STR_2, CACHE_VALUE_STR_2),
            true);
    }

    // check number
    {
        EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), 4); // 4:expect_numbers
        for (auto i : ApiCacheManager::GetInstance().caches_) {
            EXPECT_EQ(i.second->data_.size(), 2); // 2:expect_numbers
        }
    }

    bool ret = ClearCache001TestCheck(g_descriptor1, CACHE_API_CODE_100, CACHE_KEY_STR_1, CACHE_VALUE_STR_1, true);
    EXPECT_EQ(ret, true);
}

bool ClearCache001TestCheckNumber(int32_t cacheSize, int32_t apiNums)
{
    if (ApiCacheManager::GetInstance().caches_.size() != cacheSize) {
        EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), cacheSize);
        return false;
    }
    for (auto i : ApiCacheManager::GetInstance().caches_) {
        if (i.second->data_.size() != apiNums) {
            EXPECT_EQ(i.second->data_.size(), apiNums);
            return false;
        }
    }
    return true;
}
/**
 * @tc.name: ClearCache001
 * @tc.desc: test clear api
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheManagerTest, ClearCache001, TestSize.Level2)
{
    DTEST_LOG << "ClearCache001 start" << std::endl;
    ClearCache001TestStep1AddCache();

    ApiCacheManager::GetInstance().ClearCache(g_descriptor1, CACHE_API_CODE_100);
    EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), 4);

    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor1, CACHE_API_CODE_100);
    EXPECT_EQ(ClearCache001TestCheckNumber(3, 2), true);

    bool ret = ClearCache001TestCheck(g_descriptor1, CACHE_API_CODE_100, CACHE_KEY_STR_1, CACHE_VALUE_STR_1, false);
    EXPECT_EQ(ret, true);
    ret = ClearCache001TestCheck(g_descriptor1, CACHE_API_CODE_100, CACHE_KEY_STR_2, CACHE_VALUE_STR_2, false);
    EXPECT_EQ(ret, true);
    ret = ClearCache001TestCheck(g_descriptor1, CACHE_API_CODE_1, CACHE_KEY_STR_2, CACHE_VALUE_STR_2, true);
    EXPECT_EQ(ret, true);

    MessageParcel data1, data2, reply1, reply2;
    ClearCache001TestParcelData(data1, CACHE_KEY_STR_3, CACHE_KEY_INT_1);
    EXPECT_EQ(ApiCacheManager::GetInstance().PreSendRequest(g_descriptor2, CACHE_API_CODE_1, data1, reply1), true);
    EXPECT_EQ(ClearCache001TestCheckParcelData(reply1, CACHE_VALUE_STR_3, CACHE_VALUE_INT_1), true);

    ClearCache001TestParcelData(data2, CACHE_KEY_INT_1);
    EXPECT_EQ(ApiCacheManager::GetInstance().PreSendRequest(g_descriptor2, CACHE_API_CODE_100, data2, reply2), true);
    EXPECT_EQ(ClearCache001TestCheckParcelData(reply2, CACHE_VALUE_INT_1, CACHE_VALUE_STR_3), true);

    ApiCacheManager::GetInstance().ClearCache(g_descriptor2);
    EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), 3);

    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor2, CACHE_API_CODE_100);
    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor2, CACHE_API_CODE_1);

    EXPECT_EQ(ClearCache001TestCheckNumber(1, 2), true);

    MessageParcel data3, data4, reply3, reply4;
    ClearCache001TestParcelData(data3, CACHE_KEY_STR_3, CACHE_KEY_INT_1);
    EXPECT_EQ(ApiCacheManager::GetInstance().PreSendRequest(g_descriptor2, CACHE_API_CODE_1, data3, reply4), false);

    EXPECT_EQ(data4.WriteInt32(CACHE_KEY_INT_1), true);
    EXPECT_EQ(ApiCacheManager::GetInstance().PreSendRequest(g_descriptor2, CACHE_API_CODE_100, data4, reply4), false);

    // add SA:descriptor2 API_CODE:1, cache: [keystr3, valstr3_valint1], [keystr2, valstr2]
    MessageParcel data5, data6, reply5, reply6;
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor2, CACHE_API_CODE_1, EXPIRE_TIME_3S);
    ClearCache001TestParcelData(data5, CACHE_KEY_STR_3, CACHE_KEY_INT_1);
    ClearCache001TestParcelData(reply5, CACHE_VALUE_STR_3, CACHE_VALUE_INT_1);
    EXPECT_EQ(ApiCacheManager::GetInstance().PostSendRequest(g_descriptor2, CACHE_API_CODE_1, data5, reply5), true);

    EXPECT_EQ(ClearCache001TestPrevSet(g_descriptor2, CACHE_API_CODE_1, CACHE_KEY_STR_2, CACHE_VALUE_STR_2), true);

    // add SA:descriptor2 API_CODE:100, cache: [keyint1, valint1_valstr3], [keystr2, valstr2]
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor2, CACHE_API_CODE_100, EXPIRE_TIME_3S);
    ClearCache001TestParcelData(data6, CACHE_KEY_INT_1);
    ClearCache001TestParcelData(reply6, CACHE_VALUE_INT_1, CACHE_VALUE_STR_3);
    EXPECT_EQ(ApiCacheManager::GetInstance().PostSendRequest(g_descriptor2, CACHE_API_CODE_100, data6, reply6), true);

    EXPECT_EQ(ClearCache001TestPrevSet(g_descriptor2, CACHE_API_CODE_100, CACHE_KEY_STR_2, CACHE_VALUE_STR_2), true);

    ApiCacheManager::GetInstance().ClearCache();

    EXPECT_EQ(ClearCache001TestCheckNumber(3, 0), true);

    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor1, CACHE_API_CODE_1);
    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor2, CACHE_API_CODE_100);
    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor2, CACHE_API_CODE_1);
    EXPECT_EQ(ApiCacheManager::GetInstance().caches_.size(), 0);
    DTEST_LOG << "ClearCache001 end" << std::endl;
}

void Task001(bool& stop)
{
    MessageParcel data1;
    MessageParcel data2;
    MessageParcel reply1;
    MessageParcel reply2;
    ClearCache001TestParcelData(data1, CACHE_KEY_STR_1, CACHE_KEY_INT_1);
    ClearCache001TestParcelData(reply1, CACHE_VALUE_STR_1, CACHE_VALUE_INT_1);

    ClearCache001TestParcelData(data2, CACHE_KEY_INT_1, CACHE_KEY_STR_1);
    ClearCache001TestParcelData(reply2, CACHE_VALUE_INT_1, CACHE_VALUE_STR_1);
    while (!stop) {
        ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_1, data1, reply1);
        ApiCacheManager::GetInstance().PostSendRequest(g_descriptor1, CACHE_API_CODE_100, data2, reply2);
    }
}

void Task002(bool& stop)
{
    MessageParcel data1;
    MessageParcel data2;
    MessageParcel reply1;
    MessageParcel reply2;
    ClearCache001TestParcelData(data1, CACHE_KEY_STR_2, CACHE_KEY_INT_1);
    ClearCache001TestParcelData(reply1, CACHE_VALUE_STR_2, CACHE_VALUE_INT_1);

    ClearCache001TestParcelData(data2, CACHE_KEY_INT_1, CACHE_KEY_STR_2);
    ClearCache001TestParcelData(reply2, CACHE_VALUE_INT_1, CACHE_VALUE_STR_2);
    while (!stop) {
        ApiCacheManager::GetInstance().PostSendRequest(g_descriptor2, CACHE_API_CODE_1, data1, reply1);
        ApiCacheManager::GetInstance().PostSendRequest(g_descriptor2, CACHE_API_CODE_100, data2, reply2);
    }
}

void Task003(bool& stop)
{
    MessageParcel data1;
    MessageParcel data2;
    MessageParcel data3;
    MessageParcel data4;
    MessageParcel reply1;
    MessageParcel reply2;
    MessageParcel reply3;
    MessageParcel reply4;
    ClearCache001TestParcelData(data1, CACHE_KEY_STR_1, CACHE_KEY_INT_1);
    ClearCache001TestParcelData(data2, CACHE_KEY_INT_1, CACHE_KEY_STR_1);
    ClearCache001TestParcelData(data3, CACHE_KEY_STR_2, CACHE_KEY_INT_1);
    ClearCache001TestParcelData(data4, CACHE_KEY_INT_1, CACHE_KEY_STR_2);

    while (!stop) {
        ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_1, data1, reply1);
        ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data2, reply2);
        ApiCacheManager::GetInstance().PreSendRequest(g_descriptor2, CACHE_API_CODE_1, data3, reply3);
        ApiCacheManager::GetInstance().PreSendRequest(g_descriptor2, CACHE_API_CODE_100, data4, reply4);
    }
}

void Task004(bool& stop)
{
    while (!stop) {
        ApiCacheManager::GetInstance().ClearCache();
        ApiCacheManager::GetInstance().ClearCache(g_descriptor2, CACHE_API_CODE_100);
        ApiCacheManager::GetInstance().ClearCache(g_descriptor1, CACHE_API_CODE_1);
        ApiCacheManager::GetInstance().ClearCache(g_descriptor1, CACHE_API_CODE_100);
        ApiCacheManager::GetInstance().ClearCache(g_descriptor3, CACHE_API_CODE_1);
        ApiCacheManager::GetInstance().ClearCache(g_descriptor3);
        ApiCacheManager::GetInstance().ClearCache(g_descriptor1);
    }
}

void Task005(bool& stop)
{
    while (!stop) {
        ApiCacheManager::GetInstance().AddCacheApi(g_descriptor1, CACHE_API_CODE_1, 0);
        ApiCacheManager::GetInstance().AddCacheApi(g_descriptor1, CACHE_API_CODE_100, 0);
        ApiCacheManager::GetInstance().AddCacheApi(g_descriptor2, CACHE_API_CODE_1, 0);
        ApiCacheManager::GetInstance().AddCacheApi(g_descriptor2, CACHE_API_CODE_100, 0);
    }
}

void Task006(bool& stop)
{
    while (!stop) {
        ApiCacheManager::GetInstance().DelCacheApi(g_descriptor1, CACHE_API_CODE_1);
        ApiCacheManager::GetInstance().DelCacheApi(g_descriptor1, CACHE_API_CODE_100);
        ApiCacheManager::GetInstance().DelCacheApi(g_descriptor2, CACHE_API_CODE_1);
        ApiCacheManager::GetInstance().DelCacheApi(g_descriptor2, CACHE_API_CODE_100);
        ApiCacheManager::GetInstance().DelCacheApi(g_descriptor3, CACHE_API_CODE_1);
        ApiCacheManager::GetInstance().DelCacheApi(g_descriptor3, CACHE_API_CODE_100);
    }
}

/**
 * @tc.name: Conc001
 * @tc.desc: test ApiCacheManager concurrency
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(CacheManagerTest, Conc001, TestSize.Level2)
{
    DTEST_LOG << "Conc001 start" << std::endl;
    bool stop = false;
    bool stopAdd = false;

    std::thread addapiT(Task005, std::ref(stop));
    std::thread delapiT(Task006, std::ref(stop));
    usleep(1 * 1000 * 1000);
    stop = true;
    addapiT.join();
    delapiT.join();

    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor1, CACHE_API_CODE_1, 0);
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor1, CACHE_API_CODE_100, 0);
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor2, CACHE_API_CODE_1, 0);
    ApiCacheManager::GetInstance().AddCacheApi(g_descriptor2, CACHE_API_CODE_100, 0);
    stop = false;
    std::thread t1(Task001, std::ref(stopAdd));
    std::thread t2(Task002, std::ref(stopAdd));
    std::thread t3(Task003, std::ref(stop));
    std::thread t4(Task004, std::ref(stop));

    usleep(1 * 1000 * 1000);
    stop = true;
    usleep(50000);
    stopAdd = true;

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    MessageParcel data1, data2, data3, data4;
    MessageParcel reply1, reply2, reply3, reply4;
    ClearCache001TestParcelData(data1, CACHE_KEY_STR_1, CACHE_KEY_INT_1);
    ClearCache001TestParcelData(data2, CACHE_KEY_INT_1, CACHE_KEY_STR_1);
    ClearCache001TestParcelData(data3, CACHE_KEY_STR_2, CACHE_KEY_INT_1);
    ClearCache001TestParcelData(data4, CACHE_KEY_INT_1, CACHE_KEY_STR_2);

    EXPECT_EQ(ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_1, data1, reply1), true);
    EXPECT_EQ(ClearCache001TestCheckParcelData(reply1, CACHE_VALUE_STR_1, CACHE_VALUE_INT_1), true);

    EXPECT_EQ(ApiCacheManager::GetInstance().PreSendRequest(g_descriptor1, CACHE_API_CODE_100, data2, reply2), true);
    EXPECT_EQ(ClearCache001TestCheckParcelData(reply2, CACHE_VALUE_INT_1, CACHE_VALUE_STR_1), true);

    EXPECT_EQ(ApiCacheManager::GetInstance().PreSendRequest(g_descriptor2, CACHE_API_CODE_1, data3, reply3), true);
    EXPECT_EQ(ClearCache001TestCheckParcelData(reply3, CACHE_VALUE_STR_2, CACHE_VALUE_INT_1), true);

    EXPECT_EQ(ApiCacheManager::GetInstance().PreSendRequest(g_descriptor2, CACHE_API_CODE_100, data4, reply4), true);
    EXPECT_EQ(ClearCache001TestCheckParcelData(reply4, CACHE_VALUE_INT_1, CACHE_VALUE_STR_2), true);

    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor1, CACHE_API_CODE_1);
    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor1, CACHE_API_CODE_100);
    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor2, CACHE_API_CODE_1);
    ApiCacheManager::GetInstance().DelCacheApi(g_descriptor2, CACHE_API_CODE_100);
    return;
    DTEST_LOG << "Conc001 end" << std::endl;
}
}