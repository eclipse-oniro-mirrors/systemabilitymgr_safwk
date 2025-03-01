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
#include "string_ex.h"

#define private public

#include "svc_control.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace SAFWK {

namespace {
char g_svcStr[] = "svc";
char g_wifiStr[] = "wifi";
char g_bluetoothStr[] = "bluetooth";
char g_nearlinkStr[] = "nearlink";
char g_helpStr[] = "help";
char g_otherStr[] = "other";
}

class SvcTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SvcTest::SetUpTestCase()
{
    DTEST_LOG << "SetUpTestCase" << std::endl;
}

void SvcTest::TearDownTestCase()
{
    DTEST_LOG << "TearDownTestCase" << std::endl;
}

void SvcTest::SetUp()
{
    DTEST_LOG << "SetUp" << std::endl;
}

void SvcTest::TearDown()
{
    DTEST_LOG << "TearDown" << std::endl;
}

/**
 * @tc.name: HelpInfo001
 * @tc.desc: Check HelpInfo
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, HelpInfo001, TestSize.Level2)
{
    DTEST_LOG << "HelpInfo001 start" << std::endl;
    int32_t fds[2];
    if (pipe(fds) == -1) {
        EXPECT_TRUE(false);
        DTEST_LOG << "create pipe failed" << std::endl;
    } else {
        SvcControl::HelpInfo(fds[1]);
        char buffer[0x400] = { 0 };
        EXPECT_NE(read(fds[0], buffer, sizeof(buffer) - 1), -1);
        const char *expect = "svc wifi | bluetooth | nearlink enable | disable | help\n";
        EXPECT_STREQ(buffer, expect);
        close(fds[0]);
        close(fds[1]);
    }
    DTEST_LOG << "HelpInfo001 end" << std::endl;
}

/**
 * @tc.name: Parse001
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse001, TestSize.Level2)
{
    DTEST_LOG << "Parse001 start" << std::endl;
    char *argv[] = { g_svcStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(sizeof(argv) / sizeof(argv[0]), argv);
    EXPECT_EQ(cmd, SvcCmd::ARGS_CNT_NOT_ENOUGH);
    DTEST_LOG << "Parse001 end" << std::endl;
}

/**
 * @tc.name: Parse002
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse002, TestSize.Level2)
{
    DTEST_LOG << "Parse002 start" << std::endl;
    char *argv[] = { g_svcStr, g_helpStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(SvcControl::ARG_MAX_COUNT + 1, argv);
    EXPECT_EQ(cmd, SvcCmd::ARGS_CNT_EXCEED);
    DTEST_LOG << "Parse002 end" << std::endl;
}

/**
 * @tc.name: Parse003
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse003, TestSize.Level2)
{
    DTEST_LOG << "Parse003 start" << std::endl;
    char *argv[] = { g_svcStr, g_otherStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(sizeof(argv) / sizeof(argv[0]), argv);
    EXPECT_EQ(cmd, SvcCmd::ARGS_INVALID);
    DTEST_LOG << "Parse003 end" << std::endl;
}

/**
 * @tc.name: Parse004
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse004, TestSize.Level2)
{
    DTEST_LOG << "Parse004 start" << std::endl;
    char *argv[] = { g_svcStr, g_helpStr, g_otherStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(sizeof(argv) / sizeof(argv[0]), argv);
    EXPECT_EQ(cmd, SvcCmd::ARGS_CNT_EXCEED);
    DTEST_LOG << "Parse004 end" << std::endl;
}

/**
 * @tc.name: Parse005
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse005, TestSize.Level2)
{
    DTEST_LOG << "Parse005 start" << std::endl;
    char *argv[] = { g_svcStr, g_wifiStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(sizeof(argv) / sizeof(argv[0]), argv);
    EXPECT_EQ(cmd, SvcCmd::ARGS_CNT_NOT_ENOUGH);
    DTEST_LOG << "Parse005 end" << std::endl;
}

/**
 * @tc.name: Parse006
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse006, TestSize.Level2)
{
    DTEST_LOG << "Parse006 start" << std::endl;
    char *argv[] = { g_svcStr, g_wifiStr, g_helpStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(sizeof(argv) / sizeof(argv[0]), argv);
    EXPECT_EQ(cmd, SvcCmd::WIFI_CMD);
    DTEST_LOG << "Parse006 end" << std::endl;
}

/**
 * @tc.name: Parse007
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse007, TestSize.Level2)
{
    DTEST_LOG << "Parse007 start" << std::endl;
    char *argv[] = { g_svcStr, g_bluetoothStr, g_otherStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(sizeof(argv) / sizeof(argv[0]), argv);
    EXPECT_EQ(cmd, SvcCmd::BLUETOOTH_CMD);
    DTEST_LOG << "Parse007 end" << std::endl;
}

/**
 * @tc.name: Parse008
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse008, TestSize.Level2)
{
    DTEST_LOG << "Parse008 start" << std::endl;
    char *argv[] = { g_svcStr, g_nearlinkStr, g_otherStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(sizeof(argv) / sizeof(argv[0]), argv);
    EXPECT_EQ(cmd, SvcCmd::NEARLINK_CMD);
    DTEST_LOG << "Parse008 end" << std::endl;
}

/**
 * @tc.name: Parse009
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse009, TestSize.Level2)
{
    DTEST_LOG << "Parse009 start" << std::endl;
    char *argv[] = { g_svcStr, g_helpStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(sizeof(argv) / sizeof(argv[0]), argv);
    EXPECT_EQ(cmd, SvcCmd::HELP_CMD);
    DTEST_LOG << "Parse009 end" << std::endl;
}

/**
 * @tc.name: Parse010
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse010, TestSize.Level2)
{
    DTEST_LOG << "Parse010 start" << std::endl;
    char *argv[] = { g_svcStr, nullptr, g_helpStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(sizeof(argv) / sizeof(argv[0]), argv);
    EXPECT_EQ(cmd, SvcCmd::ARGS_INVALID);
    DTEST_LOG << "Parse010 end" << std::endl;
}

/**
 * @tc.name: Parse011
 * @tc.desc: Check Parse
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Parse011, TestSize.Level2)
{
    DTEST_LOG << "Parse011 start" << std::endl;
    char longestStr[SvcControl::SINGLE_ARG_MAXLEN + 2];
    for (auto &ch: longestStr) {
        ch = 'a';
    }
    longestStr[SvcControl::SINGLE_ARG_MAXLEN + 1] = 0;
    char *argv[] = { g_svcStr, g_wifiStr, longestStr };
    SvcCmd cmd = SvcControl::GetInstance().Parse(sizeof(argv) / sizeof(argv[0]), argv);
    EXPECT_EQ(cmd, SvcCmd::ARGS_INVALID);
    DTEST_LOG << "Parse011 end" << std::endl;
}

/**
 * @tc.name: SetCmdArgs001
 * @tc.desc: Check SetCmdArgs
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, SetCmdArgs001, TestSize.Level2)
{
    DTEST_LOG << "SetCmdArgs001 start" << std::endl;
    char *argv[] = { g_svcStr, g_wifiStr, g_helpStr };
    std::vector<std::u16string> u16Args;
    SvcControl::GetInstance().SetCmdArgs(sizeof(argv) / sizeof(argv[0]), argv, u16Args);
    EXPECT_EQ(u16Args.size(), 1ULL);
    std::string args = Str16ToStr8(u16Args[0]);
    const char *expect = "help";
    EXPECT_STREQ(args.c_str(), expect);
    DTEST_LOG << "SetCmdArgs001 end" << std::endl;
}

/**
 * @tc.name: CmdErrorToString001
 * @tc.desc: Check CmdErrorToString
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, CmdErrorToString001, TestSize.Level2)
{
    DTEST_LOG << "CmdErrorToString001 start" << std::endl;
    std::string str = SvcControl::GetInstance().CmdErrorToString(SvcCmd::ARGS_INVALID);
    const char *expect = "argument invalid";
    EXPECT_STREQ(str.c_str(), expect);
    DTEST_LOG << "CmdErrorToString001 end" << std::endl;
}

/**
 * @tc.name: CmdErrorToString002
 * @tc.desc: Check CmdErrorToString
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, CmdErrorToString002, TestSize.Level2)
{
    DTEST_LOG << "CmdErrorToString002 start" << std::endl;
    std::string str = SvcControl::GetInstance().CmdErrorToString(SvcCmd::ARGS_CNT_EXCEED);
    const char *expect = "argument's cnt exceed";
    EXPECT_STREQ(str.c_str(), expect);
    DTEST_LOG << "CmdErrorToString002 end" << std::endl;
}

/**
 * @tc.name: CmdErrorToString003
 * @tc.desc: Check CmdErrorToString
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, CmdErrorToString003, TestSize.Level2)
{
    DTEST_LOG << "CmdErrorToString003 start" << std::endl;
    std::string str = SvcControl::GetInstance().CmdErrorToString(SvcCmd::ARGS_CNT_NOT_ENOUGH);
    const char *expect = "argument's cnt not enough";
    EXPECT_STREQ(str.c_str(), expect);
    DTEST_LOG << "CmdErrorToString003 end" << std::endl;
}

/**
 * @tc.name: CmdErrorToString004
 * @tc.desc: Check CmdErrorToString
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, CmdErrorToString004, TestSize.Level2)
{
    DTEST_LOG << "CmdErrorToString004 start" << std::endl;
    std::string str = SvcControl::GetInstance().CmdErrorToString(
        static_cast<SvcCmd>(static_cast<int32_t>(SvcCmd::ARGS_CNT_NOT_ENOUGH) - 1));
    const char *expect = "exception error code";
    EXPECT_STREQ(str.c_str(), expect);
    DTEST_LOG << "CmdErrorToString004 end" << std::endl;
}

/**
 * @tc.name: Main001
 * @tc.desc: Check Main
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Main001, TestSize.Level2)
{
    DTEST_LOG << "Main001 start" << std::endl;
    int32_t fds[2];
    if (pipe(fds) == -1) {
        EXPECT_TRUE(false);
        DTEST_LOG << "create pipe failed" << std::endl;
    } else {
        char *argv[] = { g_svcStr, g_helpStr };
        int32_t ret = SvcControl::GetInstance().Main(sizeof(argv) / sizeof(argv[0]), argv, fds[1]);
        char buffer[0x400] = { 0 };
        EXPECT_NE(read(fds[0], buffer, sizeof(buffer) - 1), -1);
        const char *expect = "svc wifi | bluetooth | nearlink enable | disable | help\n";
        EXPECT_STREQ(buffer, expect);
        EXPECT_EQ(ret, 0);
        close(fds[0]);
        close(fds[1]);
    }
    DTEST_LOG << "Main001 end" << std::endl;
}

/**
 * @tc.name: Main002
 * @tc.desc: Check Main
 * @tc.type: FUNC
 * @tc.require: I5KMF7
 */
HWTEST_F(SvcTest, Main002, TestSize.Level2)
{
    DTEST_LOG << "Main002 start" << std::endl;
    int32_t fds[2];
    if (pipe(fds) == -1) {
        EXPECT_TRUE(false);
        DTEST_LOG << "create pipe failed" << std::endl;
    } else {
        int32_t ret = SvcControl::GetInstance().Main(2, nullptr, fds[1]);
        char buffer[0x400] = { 0 };
        EXPECT_NE(read(fds[0], buffer, sizeof(buffer) - 1), -1);
        const char *expect = "argument is null\n"
                             "svc wifi | bluetooth | nearlink enable | disable | help\n";
        EXPECT_STREQ(buffer, expect);
        EXPECT_EQ(ret, -1);
        close(fds[0]);
        close(fds[1]);
    }
    DTEST_LOG << "Main002 end" << std::endl;
}
}
}