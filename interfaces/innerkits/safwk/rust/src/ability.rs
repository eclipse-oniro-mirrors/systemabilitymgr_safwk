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

//! Trait for build a system ability

use std::pin::Pin;

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteStub;

use crate::exts::SystemAbility;
use crate::wrapper::{
    AbilityStub, AbilityWrapper, BuildSystemAbility, SystemAbilityOnDemandReason,
    SystemAbilityWrapper,
};

/// The ability trait is used to define the behavior of the system ability.
#[allow(unused)]
pub trait Ability {
    /// Called when the system ability is loaded.
    ///
    /// # Arguments
    /// * `handler` - The handler of the system ability. You can use the handler
    ///   to publish a remote stub when all the preliminaries have been complete
    ///   and it is ready to process external requests. You can also keep the
    ///   handler for later use, such as canceling the idle state of the system
    ///   ability.
    fn on_start(&self, handler: Handler) {}

    /// Called when the system ability is unloaded.
    fn on_stop(&self) {}

    /// Called when the system ability is dumped.
    fn on_dump(&self) {}

    /// Called when the system ability is ready to idle. You can return a value
    /// to indicate whether the system ability can enter the idle state. If the
    /// system ability can enter the idle state, return a positive number (e.g.,
    /// 2) to sleep for 2 milliseconds before entering the idle state;
    /// otherwise, return -1 to abort the idle state.
    fn on_idle(&self, reason: SystemAbilityOnDemandReason) -> i32 {
        0
    }

    /// Called when the system ability is activated from the idle state.
    fn on_active(&self, reason: SystemAbilityOnDemandReason) {}

    /// Called when the system ability is loaded with a reason.
    fn on_start_with_reason(&self, reason: SystemAbilityOnDemandReason, handler: Handler) {
        self.on_start(handler);
    }

    /// Called when the system ability is unloaded with a reason.
    fn on_stop_with_reason(&self, reason: SystemAbilityOnDemandReason) {
        self.on_stop();
    }

    /// Called when the listening system ability starts.
    ///
    /// Call [`add_system_ability_listen`] to listen to the system ability.
    ///
    /// [`add_system_ability_listen`]: Handler::add_system_ability_listen
    fn on_system_ability_load_event(&self, said: i32, device_id: String) {}

    /// Called when the listening system ability is uninstalled.
    ///
    /// Call [`remove_system_ability_listen`] to stop listening to the system
    /// ability.
    ///
    /// [`remove_system_ability_listen`]: Handler::remove_system_ability_listen
    fn on_system_ability_remove_event(&self, said: i32, device_id: String) {}

    /// Called when the fusion control level including power consumption,
    /// temperature, and CPU utilization is changed, and system ability must be
    /// on the distribution list.
    fn on_device_level_changed(&self, change_type: i32, level: i32, action: String) {}

    /// Called when an extension is sent.
    fn on_extension(&self, extension: String, data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        0
    }

    /// Builds a system ability with the specified system ability ID. This
    /// method should be called after dlopen.
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

/// The handler of the system ability.
#[derive(Clone)]
pub struct Handler {
    pub(crate) inner: *mut SystemAbilityWrapper,
}

impl Handler {
    /// Publish a RemoteStub to process external requests.
    pub fn publish<A: RemoteStub + 'static>(&self, ability: A) -> bool {
        let ability = Box::new(AbilityStub::new(ability));
        let system_ability = unsafe { Pin::new_unchecked(&mut *self.inner) };
        system_ability.PublishWrapper(ability)
    }

    /// Cancel the idle state of the system ability.
    pub fn cancel_idle(&self) -> bool {
        let system_ability = unsafe { Pin::new_unchecked(&mut *self.inner) };
        system_ability.CancelIdleWrapper()
    }

    /// Start listening to the specific system ability.
    pub fn add_system_ability_listen(&self, system_ability_id: i32) -> bool {
        let system_ability = unsafe { Pin::new_unchecked(&mut *self.inner) };
        system_ability.AddSystemAbilityListener(system_ability_id)
    }

    /// Stop listening to the specific system ability.
    pub fn remove_system_ability_listen(&self, system_ability_id: i32) -> bool {
        let system_ability = unsafe { Pin::new_unchecked(&mut *self.inner) };
        system_ability.RemoveSystemAbilityListener(system_ability_id)
    }
}
