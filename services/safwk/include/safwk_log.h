/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef SERVICES_SAFWK_NATIVE_INCLUDE_SAM_LOG_H
#define SERVICES_SAFWK_NATIVE_INCLUDE_SAM_LOG_H

#include "hilog/log.h"

namespace OHOS {
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001810

#undef LOG_TAG
#define LOG_TAG "SAFWK"

#ifdef HILOGF
#undef HILOGF
#endif

#ifdef HILOGE
#undef HILOGE
#endif

#ifdef HILOGW
#undef HILOGW
#endif

#ifdef HILOGI
#undef HILOGI
#endif

#ifdef HILOGD
#undef HILOGD
#endif

#define HILOGF(TAG, fmt, ...) \
    HILOG_FATAL(LOG_CORE, "%{public}s " fmt, __FUNCTION__, ##__VA_ARGS__)
#define HILOGE(TAG, fmt, ...) \
    HILOG_ERROR(LOG_CORE, "%{public}s " fmt, __FUNCTION__, ##__VA_ARGS__)
#define HILOGW(TAG, fmt, ...) \
    HILOG_WARN(LOG_CORE, "%{public}s " fmt, __FUNCTION__, ##__VA_ARGS__)
#define HILOGI(TAG, fmt, ...) \
    HILOG_INFO(LOG_CORE, "%{public}s " fmt, __FUNCTION__, ##__VA_ARGS__)
#define HILOGD(TAG, fmt, ...) \
    HILOG_DEBUG(LOG_CORE, "%{public}s " fmt, __FUNCTION__, ##__VA_ARGS__)
}

#endif
