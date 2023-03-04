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

#include "gtest/gtest.h"
#include "string_ex.h"
#include "test_log.h"

#define private public
#include "c_system_ability_internal.h"
#include "c_system_ability.h"
#undef private
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace SAFWK {
namespace {
    constexpr int VAILD_SAID = 1494;
}

class CSystemAbilityTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void func(CSystemAbility *sa)
{
}

void CSystemAbilityTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void CSystemAbilityTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void CSystemAbilityTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void CSystemAbilityTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/**
 * @tc.name: CreateSystemAbility001
 * @tc.desc:  Check CreateSystemAbility
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, CreateSystemAbility001, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    OnStartCb onStart = nullptr;
    OnStartCb onStop = nullptr;
    int32_t saId = VAILD_SAID;
    bool runOncreate = true;
    cSystemability = CreateSystemAbility(saId, runOncreate, onStart, onStop);
    EXPECT_EQ(cSystemability, nullptr);
}

/**
 * @tc.name: CreateSystemAbility002
 * @tc.desc:  Check CreateSystemAbility
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, CreateSystemAbility002, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    OnStartCb onStart = func;
    OnStartCb onStop = nullptr;
    int32_t saId = VAILD_SAID;
    bool runOncreate = true;
    cSystemability = CreateSystemAbility(saId, runOncreate, onStart, onStop);
    EXPECT_EQ(cSystemability, nullptr);
}

/**
 * @tc.name: CreateSystemAbility003
 * @tc.desc:  Check CreateSystemAbility
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, CreateSystemAbility003, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    OnStartCb onStart = func;
    OnStartCb onStop = func;
    int32_t saId = VAILD_SAID;
    bool runOncreate = true;
    cSystemability = CreateSystemAbility(saId, runOncreate, onStart, onStop);
    EXPECT_NE(cSystemability, nullptr);
}

/**
 * @tc.name: RemoteObjectGetUserData001
 * @tc.desc: Check RemoteObjectGetUserData
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, RemoteObjectGetUserData001, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    auto res = RemoteObjectGetUserData(cSystemability);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name: RemoteObjectGetUserData002
 * @tc.desc: Check RemoteObjectGetUserData
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, RemoteObjectGetUserData002, TestSize.Level3)
{
    CSystemAbilityInner *cSystemability = nullptr;
    auto res = RemoteObjectGetUserData(cSystemability);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name: MakeAndRegisterAbility001
 * @tc.desc: Check MakeAndRegisterAbility
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, MakeAndRegisterAbility001, TestSize.Level3)
{
    CSystemAbility *cSystemability = nullptr;
    const void *userData = nullptr;
    bool res = MakeAndRegisterAbility(cSystemability, userData);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: MakeAndRegisterAbility002
 * @tc.desc: Check MakeAndRegisterAbility
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, MakeAndRegisterAbility002, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    const void *userData = nullptr;
    bool res = MakeAndRegisterAbility(cSystemability, userData);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: DeleteNative001
 * @tc.desc: Check DeleteNative
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, DeleteNative001, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    DeleteNative(cSystemability);
    CSystemAbilityInner *cSystemabilityEmpty = nullptr;
    DeleteNative(cSystemabilityEmpty);
    EXPECT_EQ(cSystemabilityEmpty, nullptr);
}

/**
 * @tc.name: OnStart001
 * @tc.desc:  Check OnStart
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, OnStart001, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    OnStartCb onStart = func;
    OnStartCb onStop = func;
    int32_t saId = VAILD_SAID;
    bool runOncreate = true;
    cSystemability->cSystemabilityInnerservice_ = new(std::nothrow)
        CSystemAbilityInnerService(cSystemability, onStart, onStop, saId, runOncreate);
    cSystemability->cSystemabilityInnerservice_ -> onStartcallback_ = nullptr;
    cSystemability->cSystemabilityInnerservice_ -> OnStart();
    EXPECT_NE(cSystemability->cSystemabilityInnerservice_, nullptr);
}

/**
 * @tc.name: OnStart002
 * @tc.desc:  Check OnStart
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, OnStart002, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    OnStartCb onStart = func;
    OnStartCb onStop = func;
    int32_t saId = VAILD_SAID;
    bool runOncreate = true;
    cSystemability->cSystemabilityInnerservice_ =
        new (std::nothrow) CSystemAbilityInnerService(cSystemability, onStart, onStop, saId, runOncreate);
    cSystemability->cSystemabilityInnerservice_ -> OnStart();
    EXPECT_NE(cSystemability->cSystemabilityInnerservice_, nullptr);
}

/**
 * @tc.name: OnStart003
 * @tc.desc: Check OnStart
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, OnStart003, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    OnStartCb onStart = func;
    OnStartCb onStop = func;
    int32_t saId = VAILD_SAID;
    bool runOncreate = true;
    cSystemability->cSystemabilityInnerservice_ =
        new (std::nothrow) CSystemAbilityInnerService(cSystemability, onStart, onStop, saId, runOncreate);
    cSystemability->cSystemabilityInnerservice_ -> cSystemability_ = nullptr;
    cSystemability->cSystemabilityInnerservice_ -> OnStart();
    EXPECT_NE(cSystemability->cSystemabilityInnerservice_, nullptr);
}

/**
 * @tc.name: OnStop001
 * @tc.desc: Check OnStop
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, OnStop001, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    OnStartCb onStart = func;
    OnStartCb onStop = func;
    int32_t saId = VAILD_SAID;
    bool runOncreate = true;
    cSystemability->cSystemabilityInnerservice_ =
        new (std::nothrow) CSystemAbilityInnerService(cSystemability, onStart, onStop, saId, runOncreate);
    cSystemability->cSystemabilityInnerservice_ -> OnStop();
    EXPECT_NE(cSystemability->cSystemabilityInnerservice_, nullptr);
}

/**
 * @tc.name: OnStop002
 * @tc.desc: Check OnStop
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, OnStop002, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    OnStartCb onStart = func;
    OnStartCb onStop = func;
    int32_t saId = VAILD_SAID;
    bool runOncreate = true;
    cSystemability->cSystemabilityInnerservice_ =
        new (std::nothrow) CSystemAbilityInnerService(cSystemability, onStart, onStop, saId, runOncreate);
    cSystemability->cSystemabilityInnerservice_ -> onStopcallback_ = nullptr;
    cSystemability->cSystemabilityInnerservice_ -> OnStop();
    EXPECT_NE(cSystemability->cSystemabilityInnerservice_, nullptr);
}

/**
 * @tc.name: OnStop003
 * @tc.desc: Check OnStop
 * @tc.type: FUNC
 */
HWTEST_F(CSystemAbilityTest, OnStop003, TestSize.Level3)
{
    auto cSystemability = new (std::nothrow) CSystemAbilityInner(nullptr);
    OnStartCb onStart = func;
    OnStartCb onStop = func;
    int32_t saId = VAILD_SAID;
    bool runOncreate = true;
    cSystemability->cSystemabilityInnerservice_ =
        new (std::nothrow) CSystemAbilityInnerService(cSystemability, onStart, onStop, saId, runOncreate);
    cSystemability->cSystemabilityInnerservice_ -> cSystemability_ = nullptr;
    cSystemability->cSystemabilityInnerservice_ -> OnStop();
    EXPECT_NE(cSystemability->cSystemabilityInnerservice_, nullptr);
}
}
}