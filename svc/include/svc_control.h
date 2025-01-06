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
#ifndef SVC_CONTROL_H
#define SVC_CONTROL_H

#include <vector>
#include <string>
#include "singleton.h"

namespace OHOS {
enum class SvcCmd : int32_t {
    ARGS_CNT_NOT_ENOUGH = -4,
    ARGS_CNT_EXCEED = -3,
    ARGS_INVALID = -2,

    HELP_CMD = -1,

    WIFI_CMD = 1120,
    BLUETOOTH_CMD = 1130,
    NEARLINK_CMD = 1190,
    TEST_CMD = 1494,
};

class SvcControl : public Singleton<SvcControl> {
public:
    SvcControl();
    ~SvcControl();

    int32_t Main(int32_t argc, char *argv[], int32_t outFd);

    static void HelpInfo(int32_t fd);

private:
    static constexpr int32_t ARG_MAX_COUNT = 64;
    static constexpr int32_t ARG_MIN_COUNT = 2;
    static constexpr int32_t SINGLE_ARG_MAXLEN = 256;
    static constexpr int32_t TIME_OUT = 4;

    SvcCmd Parse(int32_t argc, char *argv[]);
    virtual SvcCmd ParseCmd(char *argv[], int32_t cnt, bool &isEnd);
    std::string CmdErrorToString(SvcCmd cmd);
    void SetCmdArgs(int32_t argc, char *argv[], std::vector<std::u16string> &args);
};
}

#endif
