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

#ifndef OHOS_ITESTSAPROXYCACHE_H
#define OHOS_ITESTSAPROXYCACHE_H

#include <vector>
#include <string_ex.h>
#include <cstdint>
#include <iremote_broker.h>

namespace OHOS {
class ITestSaProxyCache : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ITestSaProxyCache");

    virtual ErrCode GetStringFunc(
        const std::string& in_str,
        std::string& ret_str) = 0;

    virtual ErrCode GetDoubleFunc(
        int32_t number,
        double& ret_number) = 0;

    virtual ErrCode GetVectorFunc(
        const std::vector<bool>& in_vec,
        std::vector<int8_t>& ret_vec) = 0;

    virtual uint32_t TestGetIpcSendRequestTimes() = 0;

    virtual ErrCode GetSaPid(
        int32_t& pid) = 0;
protected:
    const int VECTOR_MAX_SIZE = 102400;
    const int LIST_MAX_SIZE = 102400;
    const int MAP_MAX_SIZE = 102400;
};
} // namespace OHOS
#endif // OHOS_ITESTSAPROXYCACHE_H

