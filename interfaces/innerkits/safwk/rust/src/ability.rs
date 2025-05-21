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

//! trait for build a system ability

use std::pin::Pin;

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteStub;

use crate::exts::SystemAbility;
use crate::wrapper::{
    AbilityStub, AbilityWrapper, BuildSystemAbility, SystemAbilityOnDemandReason,
    SystemAbilityWrapper,
};

#[allow(unused)]
pub trait Ability {
    fn on_start(&self, handler: Handler) {}

    fn on_stop(&self) {}

    fn on_dump(&self) {}

    fn on_idle(&self, reason: SystemAbilityOnDemandReason) -> i32 {
        0
    }

    fn on_active(&self, reason: SystemAbilityOnDemandReason) {}

    fn on_start_with_reason(&self, reason: SystemAbilityOnDemandReason, handler: Handler) {
        self.on_start(handler);
    }

    fn on_stop_with_reason(&self, reason: SystemAbilityOnDemandReason) {
        self.on_stop();
    }

    fn on_system_ability_load_event(&self, said: i32, device_id: String) {}

    fn on_system_ability_remove_event(&self, said: i32, device_id: String) {}

    fn on_device_level_changed(&self, change_type: i32, level: i32, action: String) {}

    fn on_extension(&self, extension: String, data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        0
    }
    
    fn build_system_ability(self, said: i32, run_on_create: bool) -> Option<SystemAbility>
    where
        Self: Sized + 'static,
    {
        info!("build system ability");
        let ability = Box::new(AbilityWrapper {
            inner: Box::new(self),
        });
        let wrapper = BuildSystemAbility(ability, said, run_on_create);
        if wrapper.is_null() {
            None
        } else {
            Some(SystemAbility::new(wrapper))
        }
    }
}

#[derive(Clone)]
pub struct Handler {
    pub(crate) inner: *mut SystemAbilityWrapper,
}

impl Handler {
    pub fn publish<A: RemoteStub + 'static>(&self, ability: A) -> bool {
        let ability = Box::new(AbilityStub::new(ability));
        let system_ability = unsafe { Pin::new_unchecked(&mut *self.inner) };
        system_ability.PublishWrapper(ability)
    }

    pub fn cancel_idle(&self) -> bool {
        let system_ability = unsafe { Pin::new_unchecked(&mut *self.inner) };
        system_ability.CancelIdleWrapper()
    }

    pub fn add_system_ability_listen(&self, system_ability_id: i32) -> bool {
        let system_ability = unsafe { Pin::new_unchecked(&mut *self.inner) };
        system_ability.AddSystemAbilityListener(system_ability_id)
    }

    pub fn remove_system_ability_listen(&self, system_ability_id: i32) -> bool {
        let system_ability = unsafe { Pin::new_unchecked(&mut *self.inner) };
        system_ability.RemoveSystemAbilityListener(system_ability_id)
    }
}
