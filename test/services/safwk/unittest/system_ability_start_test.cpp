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
#include <cstdlib>
#include <vector>
#include <string>
#include <cstring>
#include "gtest/gtest.h"
#include "test_log.h"

using namespace testing;
using namespace testing::ext;

extern "C" int StartSA(int argc, char *argv[]);

namespace OHOS {
namespace {
    char g_execPath[] = "/system/bin/sa_main";
    char g_configPath[] = "/system/profile/media_analysis_service.json";
    char g_eventParam[] = "10120#4#usual.event.BATTERY_CHANGED#0#410#";
    char g_errorParam[] = "error";
    char g_longParam[] = "longlonglonglonglonglonglonglonglonglonglonglonglong";
    char g_randomParam[] = "12abcd34";
    char g_prefixMParam[] = "-m";
    char g_emptyParam[] = "";
    char g_execPath2[] = "/system/bin/sa_main";
    char g_configPath2[] = "/data/test/resource/safwk/profile/multi_sa_profile.json";
    char g_eventParam2[] = "1130#3#persist.bluetooth.switch_enable#2#-1#";
    char g_execPath3[] = "/system/bin/sa_main";
    char g_configPath3[] = "/data/test/resource/safwk/profile/listen_test.json";
    char g_eventParam3[] = "65952#3#persist.nearlink.switch_enable#1#-1#";
    char g_execPath4[] = "/system/bin/sa_main";
    char g_configPath4[] = "/system/profile/bluetooth_service.json";
    char g_eventParam4[] = "65990#0#load##-1#";
    constexpr int COMMON_ARGC = 3;
    constexpr int INVALID_MIN_ARGC = -1;
    constexpr int INVALID_MAX_ARGC = 999;
    constexpr int RET_ERROR = -1;
    constexpr int RET_OK = 0;
    constexpr int LONG_PARAM_SIZE = 3000;
}

class SystemAbilityStartTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SystemAbilityStartTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SystemAbilityStartTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SystemAbilityStartTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SystemAbilityStartTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}


/**
 * @tc.name: SystemAbilityStartTest001
 * @tc.desc: test error argc
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest001, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest001 start" << std::endl;
    int ret = StartSA(INVALID_MAX_ARGC, nullptr);
    EXPECT_TRUE(ret == RET_ERROR);
    ret = StartSA(INVALID_MIN_ARGC, nullptr);
    EXPECT_TRUE(ret == RET_ERROR);
    ret = StartSA(COMMON_ARGC, nullptr);
    EXPECT_TRUE(ret == RET_ERROR);
    ret = StartSA(COMMON_ARGC - 1, nullptr);
    EXPECT_TRUE(ret == RET_ERROR);
    DTEST_LOG << "SystemAbilityStartTest001 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest002
 * @tc.desc: test StartSA
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest002, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest002 start" << std::endl;
    char *argv[] = {
        g_execPath,
        g_configPath,
        g_eventParam,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest002 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest003
 * @tc.desc: test error param
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest003, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest003 start" << std::endl;
    char *argv[] = {
        g_errorParam,
        g_errorParam,
        g_errorParam,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest003 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest004
 * @tc.desc: test long param
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest004, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest004 start" << std::endl;
    char *argv[] = {
        g_longParam,
        g_longParam,
        g_longParam,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest004 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest005
 * @tc.desc: test empty param
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest005, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest005 start" << std::endl;
    char *argv[] = {
        g_emptyParam,
        g_emptyParam,
        g_emptyParam,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest005 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest006
 * @tc.desc: test random param
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest006, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest006 start" << std::endl;
    char *argv1[] = {
        g_randomParam,
        g_configPath,
        g_eventParam,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv1);
    EXPECT_TRUE(ret == RET_OK);
    char *argv2[] = {
        g_execPath,
        g_randomParam,
        g_eventParam,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv2);
    EXPECT_TRUE(ret == RET_OK);
    char *argv3[] = {
        g_execPath,
        g_configPath,
        g_randomParam,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv3);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest006 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest007
 * @tc.desc: test single empty param
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest007, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest007 start" << std::endl;
    char *argv1[] = {
        g_execPath,
        g_configPath,
        g_emptyParam,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv1);
    EXPECT_TRUE(ret == RET_OK);
    char *argv2[] = {
        g_execPath,
        g_emptyParam,
        g_eventParam,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv2);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest007 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest008
 * @tc.desc: test error path
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest008, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest008 start" << std::endl;
    char path1[] = "/system/profile/medi.json";
    char *argv1[] = {
        g_execPath,
        path1,
        g_eventParam,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv1);
    EXPECT_TRUE(ret == RET_OK);
    char path2[] = "/system/profile/media_analysis_service";
    char *argv2[] = {
        g_execPath,
        path2,
        g_eventParam,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv2);
    EXPECT_TRUE(ret == RET_OK);
    char path3[] = "/system/profile/installs.json";
    char *argv3[] = {
        g_execPath,
        path3,
        g_eventParam,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv3);
    EXPECT_TRUE(ret == RET_OK);
    char path4[] = "/system/profile/installs.on";
    char *argv4[] = {
        g_execPath,
        path4,
        g_eventParam,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv4);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest008 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest009
 * @tc.desc: test g_prefixMParam
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest009, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest009 start" << std::endl;
    char *argv[] = {
        g_prefixMParam,
        g_execPath,
        g_configPath,
        g_eventParam,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest009 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest010
 * @tc.desc: test long data
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest010, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest010 start" << std::endl;
    char longData[LONG_PARAM_SIZE];
    for (int i = 0; i < LONG_PARAM_SIZE - 1; ++i) {
        longData[i] = 'x';
    }
    longData[LONG_PARAM_SIZE - 1] = '\0';
    char *argv[] = {
        g_execPath,
        g_configPath,
        longData,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest010 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest011
 * @tc.desc: test error id
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest011, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest011 start" << std::endl;
    char errorIdEventParm[] = "error#4#usual.event.BATTERY_CHANGED#0#410#";
    char *argv[] = {
        g_execPath,
        g_configPath,
        errorIdEventParm,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest011 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest012
 * @tc.desc: test StartSA
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest012, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest012 start" << std::endl;
    char *argv[] = {
        g_execPath2,
        g_configPath2,
        g_eventParam2,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest012 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest013
 * @tc.desc: test random param
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest013, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest013 start" << std::endl;
    char *argv1[] = {
        g_randomParam,
        g_configPath2,
        g_eventParam2,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv1);
    EXPECT_TRUE(ret == RET_OK);
    char *argv2[] = {
        g_execPath2,
        g_randomParam,
        g_eventParam2,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv2);
    EXPECT_TRUE(ret == RET_OK);
    char *argv3[] = {
        g_execPath2,
        g_configPath2,
        g_randomParam,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv3);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest013 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest014
 * @tc.desc: test error path
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest014, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest014 start" << std::endl;
    char path1[] = "/system/profile/medi.json";
    char *argv1[] = {
        g_execPath2,
        path1,
        g_eventParam2,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv1);
    EXPECT_TRUE(ret == RET_OK);
    char path2[] = "/system/profile/media_analysis_service";
    char *argv2[] = {
        g_execPath2,
        path2,
        g_eventParam2,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv2);
    EXPECT_TRUE(ret == RET_OK);
    char path3[] = "/system/profile/installs.json";
    char *argv3[] = {
        g_execPath2,
        path3,
        g_eventParam2,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv3);
    EXPECT_TRUE(ret == RET_OK);
    char path4[] = "/system/profile/installs.on";
    char *argv4[] = {
        g_execPath2,
        path4,
        g_eventParam2,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv4);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest014 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest015
 * @tc.desc: test g_prefixMParam
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest015, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest015 start" << std::endl;
    char *argv[] = {
        g_prefixMParam,
        g_execPath2,
        g_configPath2,
        g_eventParam2,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest015 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest016
 * @tc.desc: test long data
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest016, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest016 start" << std::endl;
    char longData[LONG_PARAM_SIZE];
    for (int i = 0; i < LONG_PARAM_SIZE - 1; ++i) {
        longData[i] = 'x';
    }
    longData[LONG_PARAM_SIZE - 1] = '\0';
    char *argv[] = {
        g_execPath2,
        g_configPath2,
        longData,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest016 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest017
 * @tc.desc: test error id
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest017, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest017 start" << std::endl;
    char errorIdEventParm[] = "error#4#usual.event.BATTERY_CHANGED#0#410#";
    char *argv[] = {
        g_execPath2,
        g_configPath2,
        errorIdEventParm,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest017 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest018
 * @tc.desc: test StartSA
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest018, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest018 start" << std::endl;
    char *argv[] = {
        g_execPath3,
        g_configPath3,
        g_eventParam3,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest018 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest019
 * @tc.desc: test random param
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest019, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest019 start" << std::endl;
    char *argv1[] = {
        g_randomParam,
        g_configPath3,
        g_eventParam3,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv1);
    EXPECT_TRUE(ret == RET_OK);
    char *argv2[] = {
        g_execPath3,
        g_randomParam,
        g_eventParam3,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv2);
    EXPECT_TRUE(ret == RET_OK);
    char *argv3[] = {
        g_execPath3,
        g_configPath3,
        g_randomParam,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv3);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest019 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest020
 * @tc.desc: test error path
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest020, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest020 start" << std::endl;
    char path1[] = "/system/profile/medi.json";
    char *argv1[] = {
        g_execPath3,
        path1,
        g_eventParam3,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv1);
    EXPECT_TRUE(ret == RET_OK);
    char path2[] = "/system/profile/media_analysis_service";
    char *argv2[] = {
        g_execPath3,
        path2,
        g_eventParam3,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv2);
    EXPECT_TRUE(ret == RET_OK);
    char path3[] = "/system/profile/installs.json";
    char *argv3[] = {
        g_execPath3,
        path3,
        g_eventParam3,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv3);
    EXPECT_TRUE(ret == RET_OK);
    char path4[] = "/system/profile/installs.on";
    char *argv4[] = {
        g_execPath3,
        path4,
        g_eventParam3,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv4);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest020 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest021
 * @tc.desc: test g_prefixMParam
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest021, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest021 start" << std::endl;
    char *argv[] = {
        g_prefixMParam,
        g_execPath3,
        g_configPath3,
        g_eventParam3,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest021 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest022
 * @tc.desc: test long data
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest022, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest022 start" << std::endl;
    char longData[LONG_PARAM_SIZE];
    for (int i = 0; i < LONG_PARAM_SIZE - 1; ++i) {
        longData[i] = 'x';
    }
    longData[LONG_PARAM_SIZE - 1] = '\0';
    char *argv[] = {
        g_execPath3,
        g_configPath3,
        longData,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest022 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest023
 * @tc.desc: test error id
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest023, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest023 start" << std::endl;
    char errorIdEventParm[] = "error#4#usual.event.BATTERY_CHANGED#0#410#";
    char *argv[] = {
        g_execPath3,
        g_configPath3,
        errorIdEventParm,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest023 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest024
 * @tc.desc: test StartSA
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest024, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest024 start" << std::endl;
    char *argv[] = {
        g_execPath4,
        g_configPath4,
        g_eventParam4,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest024 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest025
 * @tc.desc: test random param
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest025, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest025 start" << std::endl;
    char *argv1[] = {
        g_randomParam,
        g_configPath4,
        g_eventParam4,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv1);
    EXPECT_TRUE(ret == RET_OK);
    char *argv2[] = {
        g_execPath4,
        g_randomParam,
        g_eventParam4,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv2);
    EXPECT_TRUE(ret == RET_OK);
    char *argv3[] = {
        g_execPath4,
        g_configPath4,
        g_randomParam,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv3);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest025 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest026
 * @tc.desc: test error path
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest026, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest026 start" << std::endl;
    char path1[] = "/system/profile/medi.json";
    char *argv1[] = {
        g_execPath4,
        path1,
        g_eventParam4,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv1);
    EXPECT_TRUE(ret == RET_OK);
    char path2[] = "/system/profile/media_analysis_service";
    char *argv2[] = {
        g_execPath4,
        path2,
        g_eventParam4,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv2);
    EXPECT_TRUE(ret == RET_OK);
    char path3[] = "/system/profile/installs.json";
    char *argv3[] = {
        g_execPath4,
        path3,
        g_eventParam4,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv3);
    EXPECT_TRUE(ret == RET_OK);
    char path4[] = "/system/profile/installs.on";
    char *argv4[] = {
        g_execPath4,
        path4,
        g_eventParam4,
        nullptr
    };
    argc = COMMON_ARGC;
    ret = StartSA(argc, argv4);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest026 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest027
 * @tc.desc: test g_prefixMParam
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest027, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest027 start" << std::endl;
    char *argv[] = {
        g_prefixMParam,
        g_execPath4,
        g_configPath4,
        g_eventParam4,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest027 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest028
 * @tc.desc: test long data
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest028, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest028 start" << std::endl;
    char longData[LONG_PARAM_SIZE];
    for (int i = 0; i < LONG_PARAM_SIZE - 1; ++i) {
        longData[i] = 'x';
    }
    longData[LONG_PARAM_SIZE - 1] = '\0';
    char *argv[] = {
        g_execPath4,
        g_configPath4,
        longData,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest028 end" << std::endl;
}

/**
 * @tc.name: SystemAbilityStartTest029
 * @tc.desc: test error id
 * @tc.type: FUNC
 */
HWTEST_F(SystemAbilityStartTest, SystemAbilityStartTest029, TestSize.Level1)
{
    DTEST_LOG << "SystemAbilityStartTest029 start" << std::endl;
    char errorIdEventParm[] = "error#4#usual.event.BATTERY_CHANGED#0#410#";
    char *argv[] = {
        g_execPath4,
        g_configPath4,
        errorIdEventParm,
        nullptr
    };
    int argc = COMMON_ARGC;
    int ret = StartSA(argc, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc + 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    ret = StartSA(argc - 1, argv);
    EXPECT_TRUE(ret == RET_OK);
    DTEST_LOG << "SystemAbilityStartTest029 end" << std::endl;
}
}