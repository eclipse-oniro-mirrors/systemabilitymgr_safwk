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
#include <utility>
#include <memory>
#include <unistd.h>
#include "safwk_log.h"
#include "string_ex.h"
#include "api_cache_manager.h"

namespace OHOS {
namespace {
const std::string TAG = "ApiCacheManager";
}

ApiCacheManager& ApiCacheManager::GetInstance()
{
    static ApiCacheManager instance;
    return instance;
}

void ApiCacheManager::AddCacheApi(const std::u16string& descriptor, uint32_t apiCode, int64_t expireTimeSec)
{
    constexpr size_t defaultCacheSize = 8;
    auto apiPair = std::make_pair(descriptor, apiCode);

    std::lock_guard<std::mutex> lock(cachesMutex_);
    auto iter = caches_.find(apiPair);
    if (iter == caches_.end()) {
        auto obj = new ExpireLruCache<std::vector<uint8_t>, std::vector<uint8_t>> (defaultCacheSize, expireTimeSec);
        caches_[apiPair] = obj;
        return;
    }

    HILOGD(TAG, "Cache api(%{public}s, apiCode:%{public}u) already exists", Str16ToStr8(descriptor).c_str(), apiCode);
    return;
}

void ApiCacheManager::DelCacheApi(const std::u16string& descriptor, uint32_t apiCode)
{
    auto apiPair = std::make_pair(descriptor, apiCode);
    std::lock_guard<std::mutex> lock(cachesMutex_);
    auto iter = caches_.find(apiPair);
    if (iter != caches_.end()) {
        if (iter->second != nullptr) {
            delete iter->second;
        }
        caches_.erase(apiPair);
        HILOGD(TAG, "Delete cache api(%{public}s, apiCode:%{public}u)", Str16ToStr8(descriptor).c_str(), apiCode);
    }

    return;
}

void ApiCacheManager::ClearCache()
{
    std::lock_guard<std::mutex> lock(cachesMutex_);
    for (auto &iter : caches_) {
        if (iter.second != nullptr) {
            iter.second->Clear();
            HILOGD(TAG, "Clear the api(%{public}s, apiCode:%{public}u) cache",
                Str16ToStr8(iter.first.first).c_str(), iter.first.second);
        }
    }
    return;
}

void ApiCacheManager::ClearCache(const std::u16string& descriptor)
{
    std::lock_guard<std::mutex> lock(cachesMutex_);
    for (auto &iter : caches_) {
        if ((iter.first.first == descriptor) && (iter.second != nullptr)) {
            HILOGD(TAG, "Clear the api(%{public}s, apiCode:%{public}u) cache",
                Str16ToStr8(descriptor).c_str(), iter.first.second);
            iter.second->Clear();
        }
    }
    return;
}

void ApiCacheManager::ClearCache(const std::u16string& descriptor, int32_t apiCode)
{
    std::lock_guard<std::mutex> lock(cachesMutex_);
    auto iter = caches_.find(std::make_pair(descriptor, apiCode));
    if ((iter != caches_.end()) && (iter->second != nullptr)) {
        HILOGD(TAG, "Clear the api(%{public}s, apiCode:%{public}u) cache",
            Str16ToStr8(descriptor).c_str(), apiCode);
        iter->second->Clear();
    }

    return;
}

bool ApiCacheManager::PreSendRequest(const std::u16string& descriptor, uint32_t apiCode, const MessageParcel& data,
    MessageParcel& reply)
{
    uint8_t *key = reinterpret_cast<uint8_t *>(data.GetData());
    size_t keySize = data.GetDataSize();
    std::vector<uint8_t> keyVec(key, key + keySize);

    std::pair<std::u16string, uint32_t> myPair = std::make_pair(descriptor, apiCode);
    std::lock_guard<std::mutex> lock(cachesMutex_);
    auto cache = caches_.find(myPair);
    if ((cache == caches_.end()) || (cache->second == nullptr)) {
        HILOGD(TAG, "Find cache api(%{public}s, apiCode:%{public}u) from map failed, maybe this api is no cacheable",
            Str16ToStr8(descriptor).c_str(), apiCode);
        return false;
    }

    std::shared_ptr<std::vector<uint8_t>> valueVec = cache->second->Get(keyVec);
    if (valueVec == nullptr) {
        HILOGD(TAG, "Cache hit failure");
        return false;
    }

    reply.WriteBuffer(reinterpret_cast<void *>(&(*valueVec)[0]), valueVec->size());
    HILOGD(TAG, "Cache hit success");
    return true;
}

bool ApiCacheManager::PostSendRequest(const std::u16string& descriptor, uint32_t apiCode, const MessageParcel& data,
    MessageParcel& reply)
{
    uint8_t *key = reinterpret_cast<uint8_t *>(data.GetData());
    size_t keySize = data.GetDataSize();
    std::vector<uint8_t> keyVec(key, key + keySize);

    std::pair<std::u16string, uint32_t> myPair = std::make_pair(descriptor, apiCode);
    std::lock_guard<std::mutex> lock(cachesMutex_);
    auto cache = caches_.find(myPair);
    if ((cache == caches_.end()) || (cache->second == nullptr)) {
        HILOGD(TAG, "Find cache api(%{public}s, apiCode:%{public}u) from map failed, maybe this api is no cacheable",
            Str16ToStr8(descriptor).c_str(), apiCode);
        return false;
    }

    uint8_t *value = reinterpret_cast<uint8_t *>(reply.GetData());
    size_t valueSize = reply.GetDataSize();
    std::vector<uint8_t> valueVec(value, value + valueSize);

    cache->second->Add(keyVec, valueVec);
    HILOGD(TAG, "Cache the reply of this call");

    return true;
}
}