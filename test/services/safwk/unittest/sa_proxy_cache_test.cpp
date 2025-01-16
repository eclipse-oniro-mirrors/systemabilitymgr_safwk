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
#include <cstdlib>
#include <string>
#include "gtest/gtest.h"
#include "message_parcel.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "listen_ability_proxy.h"
#include "test_sa_proxy_cache_proxy.h"
#include "test_log.h"

using namespace testing::ext;

namespace OHOS {
namespace {
    int g_mockReturn;
}

class SaProxyCacheTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SaProxyCacheTest::SetUpTestCase()
{}

void SaProxyCacheTest::TearDownTestCase()
{}

void SaProxyCacheTest::SetUp()
{}

void SaProxyCacheTest::TearDown()
{}

bool CheckCallGetDoubleFuncIpcTimes(sptr<ITestSaProxyCache>& proxy, int32_t input, int32_t expectIpcTimes)
{
    constexpr double pi = 3.14;
    double retDouble;

    auto ret = proxy->GetDoubleFunc(input, retDouble);
    if (ret != ERR_OK) {
        EXPECT_EQ(ret, ERR_OK);
        return false;
    }
    EXPECT_DOUBLE_EQ(retDouble, input * pi);
    int times = proxy->TestGetIpcSendRequestTimes();
    if (times != expectIpcTimes) {
        EXPECT_EQ(times, expectIpcTimes);
        return false;
    }

    return true;
}

/**
 * @tc.name: SaProxyCacheTest001
 * @tc.desc: test proxy cache
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaProxyCacheTest, SaProxyCacheTest001, TestSize.Level2)
{
    DTEST_LOG << "SaProxyCacheTest001 start" << std::endl;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(DISTRIBUTED_SCHED_TEST_TT_ID);
    sptr<ITestSaProxyCache> proxy = iface_cast<ITestSaProxyCache>(remoteObject);
    EXPECT_NE(proxy, nullptr);
    bool ret;

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 3, 1), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 4, 2), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 10, 3), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 10, 3), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 20, 4), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 20, 4), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 100, 5), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 1000, 6), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 2000, 7), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 3000, 8), true);

    // test cache hit
    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 4, 8), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 1000, 8), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 3000, 8), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 3000, 8), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 2000, 8), true);

    //  exceed cache map size, eliminate cache (3, 9.42)
    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 10000, 9), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 3, 10), true);

    std::string input("AABB");
    std::string output;
    ret = proxy->GetStringFunc(input, output);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(output, "AABBBBAA");
    EXPECT_EQ(proxy->TestGetIpcSendRequestTimes(), 11);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 10, 12), true);

    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 10, 12), true);

    // timeout
    usleep(1 * 1000 * 1000);
    EXPECT_EQ(CheckCallGetDoubleFuncIpcTimes(proxy, 10, 13), true);

    output.clear();
    EXPECT_EQ(proxy->GetStringFunc(input, output), ERR_OK);
    EXPECT_EQ(output, "AABBBBAA");
    EXPECT_EQ(proxy->TestGetIpcSendRequestTimes(), 14);
    DTEST_LOG << "SaProxyCacheTest001 end" << std::endl;
}

class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject(u"mock_i_remote_object") {}

    ~MockIRemoteObject() {}

    int32_t GetObjectRefCount() override
    {
        return 0;
    }

    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override
    {
        DTEST_LOG << "mockmockmock" << std::endl;
        reply.WriteInt32(ERR_PERMISSION_DENIED);
        return g_mockReturn;
    }

    bool IsProxyObject() const override
    {
        return true;
    }

    bool CheckObjectLegality() const override
    {
        return true;
    }

    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient) override
    {
        return true;
    }

    bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }

    sptr<IRemoteBroker> AsInterface() override
    {
        return nullptr;
    }

    int Dump(int fd, const std::vector<std::u16string> &args) override
    {
        return 0;
    }

    std::u16string GetObjectDescriptor() const
    {
        std::u16string descriptor = std::u16string();
        return descriptor;
    }
};

/**
 * @tc.name: SaProxyCacheTest002
 * @tc.desc: test abnormal barnch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaProxyCacheTest, SaProxyCacheTest002, TestSize.Level2)
{
    DTEST_LOG << "SaProxyCacheTest002 start" << std::endl;
    {
        g_mockReturn = ERR_TIMED_OUT;
        sptr<MockIRemoteObject> iRemoteObject = sptr<MockIRemoteObject>(new (std::nothrow) MockIRemoteObject());
        EXPECT_TRUE(iRemoteObject != nullptr);
        TestSaProxyCacheProxy p(iRemoteObject);
        std::string input("hello_test");
        std::string output;
        auto ret = p.GetStringFunc(input, output);
        EXPECT_EQ(ret, ERR_TIMED_OUT);
        EXPECT_EQ(p.TestGetIpcSendRequestTimes(), 1);

        output.clear();
        ret = p.GetStringFunc(input, output);
        EXPECT_EQ(p.TestGetIpcSendRequestTimes(), 2);
    }

    {
        g_mockReturn = ERR_OK;
        sptr<MockIRemoteObject> iRemoteObject = sptr<MockIRemoteObject>(new (std::nothrow) MockIRemoteObject());
        EXPECT_TRUE(iRemoteObject != nullptr);
        TestSaProxyCacheProxy p(iRemoteObject);

        double retDouble;
        auto ret = p.GetDoubleFunc(100, retDouble);
        EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
        EXPECT_EQ(p.TestGetIpcSendRequestTimes(), 1);

        ret = p.GetDoubleFunc(100, retDouble);
        EXPECT_EQ(p.TestGetIpcSendRequestTimes(), 2);
    }
    DTEST_LOG << "SaProxyCacheTest002 end" << std::endl;
}

/**
 * @tc.name: SaProxyCacheTest003
 * @tc.desc: test proxy object and cache are destroyed together
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaProxyCacheTest, SaProxyCacheTest003, TestSize.Level2)
{
    DTEST_LOG << "SaProxyCacheTest003 start" << std::endl;
    std::vector<bool> input;
    std::vector<int8_t> output;
    std::vector<int8_t> expect;
    input.push_back(true);
    input.push_back(false);
    input.push_back(true);
    for (auto i:input) {
        expect.push_back(i == true ? 1 : -1);
    }
    {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(DISTRIBUTED_SCHED_TEST_TT_ID);
        sptr<ITestSaProxyCache> proxy = iface_cast<ITestSaProxyCache>(remoteObject);
        EXPECT_NE(proxy, nullptr);

        int ret = proxy->GetVectorFunc(input, output);
        EXPECT_EQ(ret, ERR_OK);
        EXPECT_EQ((output == expect), 1);
        EXPECT_EQ(proxy->TestGetIpcSendRequestTimes(), 1);

        output.clear();
        ret = proxy->GetVectorFunc(input, output);
        EXPECT_EQ(ret, ERR_OK);
        EXPECT_EQ((output == expect), 1);
        EXPECT_EQ(proxy->TestGetIpcSendRequestTimes(), 1);
    }

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(DISTRIBUTED_SCHED_TEST_TT_ID);
    sptr<ITestSaProxyCache> proxy = iface_cast<ITestSaProxyCache>(remoteObject);
    EXPECT_NE(proxy, nullptr);

    output.clear();
    auto ret = proxy->GetVectorFunc(input, output);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ((output == expect), 1);
    EXPECT_EQ(proxy->TestGetIpcSendRequestTimes(), 1);
    DTEST_LOG << "SaProxyCacheTest003 end" << std::endl;
}

/**
 * @tc.name: SaProxyCacheTest004
 * @tc.desc: test clear cache when sa stub exits
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaProxyCacheTest, SaProxyCacheTest004, TestSize.Level2)
{
    DTEST_LOG << "SaProxyCacheTest004 start" << std::endl;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(DISTRIBUTED_SCHED_TEST_TT_ID);
    sptr<ITestSaProxyCache> proxy = iface_cast<ITestSaProxyCache>(remoteObject);

    sptr<IRemoteObject> remoteObject1 = systemAbilityManager->GetSystemAbility(1494);
    sptr<IListenAbility> listenProxy = iface_cast<IListenAbility>(remoteObject1);

    int pid;
    double retDouble, retDouble2;
    auto ret = proxy->GetSaPid(pid);

    std::string cmd = "kill -9 ";
    cmd += std::to_string(pid);

    CheckCallGetDoubleFuncIpcTimes(proxy, 3, 1);

    CheckCallGetDoubleFuncIpcTimes(proxy, 3, 1);

    std::string input("AABB");
    std::string output;
    ret = proxy->GetStringFunc(input, output);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_EQ(output, "AABBBBAA");
    EXPECT_EQ(proxy->TestGetIpcSendRequestTimes(), 2);

    EXPECT_EQ(listenProxy->AddVolume(100), 101);
    EXPECT_EQ(listenProxy->TestSaCallSa(100, retDouble2), ERR_OK);

    system(cmd.c_str());
    DTEST_LOG << cmd << std::endl;

    int trytime = 3;
    ret = ERR_OK;
    while ((ret == ERR_OK) && (trytime != 0)) {
        ret = proxy->GetDoubleFunc(3, retDouble);
        usleep(500000);
        trytime--;
    }
    EXPECT_NE(ret, ERR_OK);
    EXPECT_GT(proxy->TestGetIpcSendRequestTimes(), 2);

    int currIpcSendRequestTimes = proxy->TestGetIpcSendRequestTimes();
    ret = proxy->GetStringFunc(input, output);
    EXPECT_NE(ret, ERR_OK);
    EXPECT_GT(proxy->TestGetIpcSendRequestTimes(), currIpcSendRequestTimes);

    ret = listenProxy->TestSaCallSa(100, retDouble2);
    EXPECT_NE(ret, ERR_OK);

    int32_t times;
    ret = listenProxy->TestGetIpcSendRequestTimes(times);
    EXPECT_EQ(ret, ERR_OK);
    EXPECT_GT(times, 1);

    EXPECT_EQ(listenProxy->AddVolume(100), 101);
    EXPECT_EQ(listenProxy->TestClearSa1493Proxy_(), ERR_OK);
    DTEST_LOG << "SaProxyCacheTest004 end" << std::endl;
}

/**
 * @tc.name: SaProxyCacheTest005
 * @tc.desc: test clear cache when sa stub exits
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SaProxyCacheTest, SaProxyCacheTest005, TestSize.Level2)
{
    DTEST_LOG << "SaProxyCacheTest005 start" << std::endl;
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(DISTRIBUTED_SCHED_TEST_TT_ID);
    sptr<ITestSaProxyCache> proxy = iface_cast<ITestSaProxyCache>(remoteObject);
    EXPECT_NE(proxy, nullptr);

    sptr<IRemoteObject> remoteObject1 = systemAbilityManager->GetSystemAbility(1494);
    sptr<IListenAbility> listenProxy = iface_cast<IListenAbility>(remoteObject1);
    EXPECT_NE(listenProxy, nullptr);

    int pid;
    double retDouble, retDouble2;
    auto ret = proxy->GetSaPid(pid);
    EXPECT_EQ(ret, ERR_OK);
    DTEST_LOG << "SaProxyCacheTest005 end" << std::endl;
}
}