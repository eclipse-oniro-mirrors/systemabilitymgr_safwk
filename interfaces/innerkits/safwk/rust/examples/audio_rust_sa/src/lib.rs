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
pub mod proxy;
mod stub;
use std::mem::MaybeUninit;
use std::sync::{Mutex, Once};

use hilog_rust::{HiLogLabel, LogType};
use ipc::parcel::MsgParcel;
use system_ability_fwk::ability::Ability;

use crate::stub::AudioService;

static mut TEST_MSG: MaybeUninit<Mutex<MsgParcel>> = MaybeUninit::uninit();

fn test_msg() -> &'static Mutex<MsgParcel> {
    static ONCE: Once = Once::new();
    ONCE.call_once(|| {
        unsafe { TEST_MSG.write(Mutex::new(MsgParcel::new())) };
    });
    unsafe { TEST_MSG.assume_init_ref() }
}

const TEST_AUDIO_ID: i32 = 1488;

const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xd001800,
    tag: "rustSA",
};

struct AudioSA;

impl Ability for AudioSA {
    fn on_start_with_reason(
        &self,
        reason: system_ability_fwk::reason::SystemAbilityOnDemandReason,
        handler: system_ability_fwk::ability::PublishHandler,
    ) {
        static ONCE: Once = Once::new();
        ONCE.call_once(|| {
            test_msg().lock().unwrap().write(&reason).unwrap();
        });
        assert!(handler.publish(AudioService));
    }

    fn on_stop(&self) {
        info!("on_stop");
    }

    fn on_idle(
        &self,
        _reason: system_ability_fwk::reason::SystemAbilityOnDemandReason,
        _handler: system_ability_fwk::ability::CancelHandler,
    ) -> i32 {
        0
    }
}

#[used]
#[link_section = ".init_array"]
static A: extern "C" fn() = {
    extern "C" fn init() {
        let system_ability = AudioSA.build_system_ability(TEST_AUDIO_ID, true).unwrap();
        system_ability.register();
    }
    init
};
