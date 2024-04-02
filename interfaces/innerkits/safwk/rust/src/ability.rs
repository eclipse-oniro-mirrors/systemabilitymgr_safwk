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

use ipc::remote::RemoteStub;

use crate::exts::SystemAbility;
use crate::wrapper::{
    AbilityStub, AbilityWrapper, BuildSystemAbility, CancelIdle, StopAbility, StubPublish,
    SystemAbilityOnDemandReason, SystemAbilityWrapper,
};

#[allow(unused)]
pub trait Ability {
    fn on_start(&self, publish_handler: PublishHandler) {}

    fn on_stop(&self) {}

    fn on_dump(&self) {}

    fn on_idle(&self, reason: SystemAbilityOnDemandReason, cancel_handler: CancelHandler) -> i32 {
        0
    }

    fn on_active(&self, reason: SystemAbilityOnDemandReason) {}

    fn on_start_with_reason(
        &self,
        reason: SystemAbilityOnDemandReason,
        publish_handler: PublishHandler,
    ) {
        self.on_start(publish_handler);
    }

    fn on_stop_with_reason(&self, reason: SystemAbilityOnDemandReason) {
        self.on_stop();
    }

    fn on_add_sa(&self, said: i32, device_id: String) {}

    fn on_remove_sa(&self, said: i32, device_id: String) {}

    fn handle_cancel_idle_res(&self, cancel_res: bool) {}

    fn on_device_level_changed(&self, change_type: i32, level: i32, action: String) {}

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

pub trait AbilityExt: Ability {
    fn stop_ability(&self, system_ability: i32);
}

impl<T: Ability + ?Sized> AbilityExt for T {
    fn stop_ability(&self, system_ability: i32) {
        StopAbility(system_ability);
    }
}

use crate::wrapper::MessageParcel;

pub struct CancelHandler {
    pub(crate) inner: *mut SystemAbilityWrapper,
}
impl CancelHandler {
    pub fn cancle_idle(&self) -> bool {
        unsafe { CancelIdle(self.inner) }
    }
}

pub struct PublishHandler {
    pub(crate) inner: *mut SystemAbilityWrapper,
}

impl PublishHandler {
    pub fn publish<A: RemoteStub + 'static>(&self, ability: A) -> bool {
        let ability = Box::new(AbilityStub::new(ability));
        unsafe { StubPublish(self.inner, ability) }
    }
}
