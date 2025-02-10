// Copyright (C) 2024 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//! SystemAbility methods.

use cxx::UniquePtr;

use crate::wrapper::{RegisterAbility, SystemAbilityWrapper};

/// SystemAbility to register.
pub struct SystemAbility {
    inner: UniquePtr<SystemAbilityWrapper>,
}

impl SystemAbility {
    pub(crate) fn new(c_wrapper: UniquePtr<SystemAbilityWrapper>) -> Self {
        Self { inner: c_wrapper }
    }

    /// register a system ability
    ///
    /// # Example
    /// ```
    /// #[used]
    /// #[link_section = ".init_array"]
    /// static A: extern "C" fn() = {
    ///     #[link_section = ".text.startup"]
    ///     extern "C" fn init() {
    ///         let system_ability = RequestAbility::new()
    ///             .build_system_ability(samgr::definition::DOWNLOAD_SERVICE_ID, false)
    ///             .unwrap();
    ///         system_ability.register();
    ///     }
    ///     init
    /// };
    /// ```
    pub fn register(self) {
        info!("SA registry");
        unsafe {
            RegisterAbility(self.inner.into_raw());
        }
    }
}
