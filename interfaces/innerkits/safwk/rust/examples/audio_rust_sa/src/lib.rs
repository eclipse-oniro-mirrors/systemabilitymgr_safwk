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
pub mod proxy;
mod stub;
use std::sync::{Arc, Mutex, Once, RwLock};

use hilog_rust::{HiLogLabel, LogType};
use ipc::parcel::{Deserialize, MsgParcel, Serialize};
use samgr::manage::SystemAbilityManager;
use system_ability_fwk::ability::{Ability, Handler};
use system_ability_fwk::cxx_share::SystemAbilityOnDemandReason;

use crate::stub::AudioService;

/// TEST_LISTEN_ID SAID
const TEST_LISTEN_ID: i32 = 1494;

const TEST_AUDIO_ID: i32 = 1499;

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xd001800,
    tag: "rustSA",
};

struct AudioSA {
    handler: RwLock<Option<Handler>>,
    watch: Arc<Mutex<Watch>>,
}

pub struct Watch {
    pub start_reason: Option<SystemAbilityOnDemandReason>,
    pub idle_reason: Option<SystemAbilityOnDemandReason>,
    pub active_reason: Option<SystemAbilityOnDemandReason>,
    pub stop_reason: Option<SystemAbilityOnDemandReason>,
    pub on_add: bool,
    pub on_remove: bool,
}

impl Watch {
    fn new() -> Self {
        Self {
            start_reason: None,
            idle_reason: None,
            active_reason: None,
            stop_reason: None,
            on_add: false,
            on_remove: false,
        }
    }
}

impl Ability for AudioSA {
    fn on_start(&self, handler: system_ability_fwk::ability::Handler) {
        panic!();
    }

    fn on_start_with_reason(
        &self,
        reason: system_ability_fwk::cxx_share::SystemAbilityOnDemandReason,
        handler: system_ability_fwk::ability::Handler,
    ) {
        handler.add_system_ability_listen(TEST_LISTEN_ID);
        self.handler.write().unwrap().replace(handler.clone());
        self.watch.lock().unwrap().start_reason = Some(reason);
        assert!(handler.publish(AudioService::new(handler.clone(), self.watch.clone())));
    }

    fn on_stop(&self) {}
    
    fn on_active(&self, reason: SystemAbilityOnDemandReason) {
        self.watch.lock().unwrap().active_reason = Some(reason);
    }

    fn on_system_ability_load_event(&self, said: i32, device_id: String) {
        if said == TEST_LISTEN_ID {
            self.watch.lock().unwrap().on_add = true;
        }
    }

    fn on_system_ability_remove_event(&self, said: i32, device_id: String) {
        if said == TEST_LISTEN_ID {
            self.watch.lock().unwrap().on_remove = true;
        }
    }

    fn on_dump(&self) {}

    fn on_idle(&self, reason: system_ability_fwk::cxx_share::SystemAbilityOnDemandReason) -> i32 {
        static ONCE: Once = Once::new();
        let w = if ONCE.is_completed() { 0 } else { -1 };
        ONCE.call_once(|| {
            self.watch.lock().unwrap().idle_reason = Some(reason);
        });
        w
    }
}

#[used]
#[link_section = ".init_array"]
static A: extern "C" fn() = {
    extern "C" fn init() {
        let system_ability = AudioSA {
            handler: RwLock::new(None),
            watch: Arc::new(Mutex::new(Watch::new())),
        }
        .build_system_ability(TEST_AUDIO_ID, true)
        .unwrap();
        system_ability.register();
    }
    init
};
