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

#[macro_use]
mod hilog;

mod interface;

mod stub;

use audio_rust_sa::proxy::AudioProxy;
use hilog_rust::{HiLogLabel, LogType};
use samgr::manage::SystemAbilityManager;
use system_ability_fwk::ability::Ability;
/// TEST_LISTEN_ID SAID
pub const TEST_LISTEN_ID: i32 = 1489;
/// TEST_AUDIO_ID SAID
pub const TEST_AUDIO_ID: i32 = 1488;

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xd001800,
    tag: "rustSA",
};

pub struct ListenSA;

impl Ability for ListenSA {
    fn on_start(&self, _publish_handler: system_ability_fwk::ability::PublishHandler) {
        let audio_sa = SystemAbilityManager::get_system_ability(TEST_AUDIO_ID).unwrap();
        let audio_proxy = AudioProxy::new(audio_sa);
        audio_proxy.request_example();
        audio_proxy.unload();

        assert!(SystemAbilityManager::check_system_ability(TEST_AUDIO_ID).is_none());
    }

    fn on_stop(&self) {
        info!("on stop")
    }
}

#[used]
#[link_section = ".init_array"]
static A: extern "C" fn() = {
    extern "C" fn init() {
        let system_ability = ListenSA.build_system_ability(TEST_LISTEN_ID, true).unwrap();
        system_ability.register();
    }
    init
};
