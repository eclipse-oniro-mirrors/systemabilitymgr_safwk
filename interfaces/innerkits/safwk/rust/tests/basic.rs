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

#![allow(unused)]

use std::sync::Once;

use audio_rust_sa::proxy::AudioProxy;
use ipc::parcel::MsgParcel;
use samgr::manage::SystemAbilityManager;
fn init() {
    #[cfg(gn_test)]
    super::init_access_token();
    static ONCE: Once = Once::new();
    SystemAbilityManager::load_system_ability(TEST_AUDIO_ID, 100).unwrap();
    ONCE.call_once(|| {
        std::thread::sleep(std::time::Duration::from_secs(20));
        SystemAbilityManager::load_system_ability(TEST_LISTEN_ID, 100);
        std::thread::sleep(std::time::Duration::from_secs(20));
    });
}

/// TEST_LISTEN_ID SAID
pub const TEST_LISTEN_ID: i32 = 1494;
/// TEST_AUDIO_ID SAID
pub const TEST_AUDIO_ID: i32 = 1499;

use system_ability_fwk::cxx_share::{OnDemandReasonId, SystemAbilityOnDemandReason};
#[test]
fn reason() {
    init();
    let audio = AudioProxy::new(SystemAbilityManager::check_system_ability(TEST_AUDIO_ID).unwrap());
    let start_reason = audio.on_start_reason().unwrap();
    assert_eq!(start_reason.reason_id, OnDemandReasonId::INTERFACE_CALL);

    assert!(audio.on_add());

    SystemAbilityManager::unload_system_ability(TEST_AUDIO_ID);
    std::thread::sleep(std::time::Duration::from_secs(5));
    let stop_reason = audio.on_idle_reason().unwrap();
    assert_eq!(stop_reason.name, "unload");

    SystemAbilityManager::unload_system_ability(TEST_AUDIO_ID);
    std::thread::sleep(std::time::Duration::from_secs(5));

    let start_reason = audio.on_start_reason().unwrap();
    assert_eq!(start_reason.reason_id, OnDemandReasonId::INTERFACE_CALL);

    std::thread::sleep(std::time::Duration::from_secs(5));
    let active_reason = audio.on_active_reason().unwrap();
    assert_eq!(active_reason.name, "cancelUnload");

    SystemAbilityManager::remove_system_ability(TEST_LISTEN_ID);
    std::thread::sleep(std::time::Duration::from_secs(5));
    assert!(audio.on_remove());
    assert!(audio.get_stop_reason().is_none());

    SystemAbilityManager::unload_system_ability(TEST_AUDIO_ID);
}

#[test]
fn stop() {}
