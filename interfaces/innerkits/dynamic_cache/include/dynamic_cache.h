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

#ifndef DYNAMIC_CACHE_H
#define DYNAMIC_CACHE_H

#include <map>
#include <string>
#include <vector>

#include "datetime_ex.h"
#include "iremote_object.h"
#include "parameter.h"
#include "refbase.h"
#include "sam_log.h"
#include "sysparam_errno.h"

namespace OHOS {
using namespace std;
template <class Query, class Result>
class DynamicCache {
public:
    Result QueryResult(Query query, int32_t code)
    {
        int32_t waterLineLength = 128;
        char waterLine[128] = {0};
        string defaultValue = "default";
        GetParameter(key_.c_str(), defaultValue.c_str(), waterLine, waterLineLength);
        std::lock_guard<std::mutex> autoLock(queryCacheLock_);
        if (localPara_.count(key_) != 0 && cacheMap_.count(query) != 0 &&
            defaultValue != string(waterLine) && string(waterLine) == localPara_[key_]) {
            HILOGD("DynamicCache QueryResult Return Cache");
            return cacheMap_[query];
        }
        HILOGD("DynamicCache QueryResult Recompute");
        Result res = Recompute(query, code);
        localPara_[key_] = waterLine;
        cacheMap_[query] = res;
        return res;
    }

    void ClearCache()
    {
        cacheMap_.clear();
    }

    bool InvalidateCache()
    {
        HILOGD("DynamicCache InvalidateCache Begin");
        string tickCount = to_string(GetTickCount());
        int32_t ret = SetParameter(key_.c_str(), tickCount.c_str());
        if (ret != EC_SUCCESS) {
            HILOGE("DynamicCache InvalidateCache SetParameter error:%{public}d!", ret);
            return false;
        }
        HILOGD("DynamicCache InvalidateCache End");
        return true;
    }

    bool SetKey(const string& key)
    {
        int32_t maxLength = 256;
        if (key.size() == 0 || (int32_t)key.size() > maxLength) {
            HILOGE("DynamicCache SetKey size error:%{public}zu!", key.size());
            return false;
        }
        key_ = key;
        return true;
    }

    virtual Result Recompute(Query query, int32_t code)
    {
        return cacheMap_[query];
    }
private:
    map<string, string> localPara_;
    string key_;
    map<Query, Result> cacheMap_;
    std::mutex queryCacheLock_;
};
}
#endif /* DYNAMIC_CACHE_H */