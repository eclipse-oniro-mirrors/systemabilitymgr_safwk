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

//! Used for test meanwhile.

#![allow(missing_docs)]
#![allow(unused)]

#[macro_use]
mod hilog;

mod interface;

mod stub;
use std::sync::{Arc, Mutex};

use audio_rust_sa::proxy::AudioProxy;
use hilog_rust::{HiLogLabel, LogType};
use samgr::manage::SystemAbilityManager;
use stub::{ListenService, Watch};
use system_ability_fwk::ability::{Ability, Handler};

/// TEST_LISTEN_ID SAID
pub const TEST_LISTEN_ID: i32 = 1494;
/// TEST_AUDIO_ID SAID
pub const TEST_AUDIO_ID: i32 = 1499;

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xd001800,
    tag: "rustSA",
};

pub struct ListenSA {
    watch: Arc<Mutex<stub::Watch>>,
}

impl Ability for ListenSA {
    fn on_start(&self, handler: system_ability_fwk::ability::Handler) {
        handler.publish(ListenService::new(self.watch.clone()));
    }

    fn on_stop(&self) {
        panic!();
    }

    fn on_stop_with_reason(
        &self,
        reason: system_ability_fwk::cxx_share::SystemAbilityOnDemandReason,
    ) {
        let audio =
            AudioProxy::new(SystemAbilityManager::check_system_ability(TEST_AUDIO_ID).unwrap());
        audio.send_stop_reason(&reason);
    }
}

#[used]
#[link_section = ".init_array"]
static A: extern "C" fn() = {
    extern "C" fn init() {
        let watch = Arc::new(Mutex::new(Watch { stop_reason: None }));
        let system_ability = ListenSA { watch }
            .build_system_ability(TEST_LISTEN_ID, true)
            .unwrap();
        system_ability.register();
    }
    init
};
