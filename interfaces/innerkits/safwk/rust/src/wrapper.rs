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

#![allow(unreachable_pub, missing_docs, unused)]
use std::collections::HashMap;
use std::fs::File;
use std::os::fd::FromRawFd;
use std::pin::Pin;

pub(crate) use ffi::*;
pub use ffi::{OnDemandReasonExtraData, OnDemandReasonId, SystemAbilityOnDemandReason};
use ipc::parcel::{Deserialize, MsgParcel, Serialize};
use ipc::remote::RemoteStub;
use ipc::IpcStatusCode;

use crate::ability::{Ability, Handler};

// `SystemAbility` trait is not used is because their methods are not recognized
// by cxx.
#[allow(non_snake_case)]
impl AbilityWrapper {
    fn OnDump(&self) {
        self.inner.on_dump()
    }

    fn OnStart(&self, system_ability: *mut ffi::SystemAbilityWrapper) {
        let handle = Handler {
            inner: system_ability,
        };
        self.inner.on_start(handle)
    }

    fn OnStartWithReason(
        &self,
        reason: ffi::SystemAbilityOnDemandReason,
        system_ability: *mut ffi::SystemAbilityWrapper,
    ) {
        let handle = Handler {
            inner: system_ability,
        };
        self.inner.on_start_with_reason(reason, handle);
    }

    fn OnIdle(&self, reason: ffi::SystemAbilityOnDemandReason) -> i32 {
        self.inner.on_idle(reason)
    }

    fn OnActive(&self, reason: ffi::SystemAbilityOnDemandReason) {
        self.inner.on_active(reason)
    }

    fn OnStop(&self) {
        self.inner.on_stop()
    }

    fn OnStopWithReason(&self, reason: ffi::SystemAbilityOnDemandReason) {
        self.inner.on_stop_with_reason(reason)
    }

    fn OnAddSystemAbility(&self, said: i32, device_id: String) {
        self.inner.on_system_ability_load_event(said, device_id)
    }

    fn OnRemoveSystemAbility(&self, said: i32, device_id: String) {
        self.inner.on_system_ability_remove_event(said, device_id)
    }

    fn OnDeviceLevelChanged(&self, change_type: i32, level: i32, action: String) {
        self.inner
            .on_device_level_changed(change_type, level, action)
    }

    fn OnExtension(
        &self,
        extension: String,
        data: Pin<&mut MessageParcel>,
        reply: Pin<&mut MessageParcel>,
    ) -> i32 {
        unsafe {
            let mut data = MsgParcel::from_ptr(data.get_unchecked_mut() as *mut MessageParcel);
            let mut reply = MsgParcel::from_ptr(reply.get_unchecked_mut() as *mut MessageParcel);
            self.inner.on_extension(extension, &mut data, &mut reply)
        }
    }
}

pub struct AbilityStub {
    inner: Box<dyn RemoteStub>,
}

impl AbilityStub {
    pub fn new<A: RemoteStub + 'static>(remote: A) -> Self {
        Self {
            inner: Box::new(remote),
        }
    }

    pub fn on_remote_request(
        &mut self,
        code: u32,
        data: Pin<&mut MessageParcel>,
        reply: Pin<&mut MessageParcel>,
    ) -> i32 {
        unsafe {
            let mut data = MsgParcel::from_ptr(data.get_unchecked_mut() as *mut MessageParcel);
            let mut reply = MsgParcel::from_ptr(reply.get_unchecked_mut() as *mut MessageParcel);
            self.inner.on_remote_request(code, &mut data, &mut reply)
        }
    }

    pub fn dump(&mut self, fd: i32, args: Vec<String>) -> i32 {
        let file = unsafe { File::from_raw_fd(fd) };
        self.inner.dump(file, args)
    }
}

pub struct AbilityWrapper {
    pub(crate) inner: Box<dyn Ability>,
}

impl Serialize for OnDemandReasonId {
    fn serialize(&self, parcel: &mut MsgParcel) -> ipc::IpcResult<()> {
        parcel.write(&self.repr)
    }
}

impl Deserialize for OnDemandReasonId {
    fn deserialize(parcel: &mut MsgParcel) -> ipc::IpcResult<Self> {
        let res = match parcel.read::<i32>()? {
            0 => Self::INTERFACE_CALL,
            1 => Self::DEVICE_ONLINE,
            2 => Self::SETTING_SWITCH,
            3 => Self::PARAM,
            4 => Self::COMMON_EVENT,
            5 => Self::TIMED_EVENT,
            _ => return Err(IpcStatusCode::Failed),
        };
        Ok(res)
    }
}

impl Serialize for OnDemandReasonExtraData {
    fn serialize(&self, parcel: &mut MsgParcel) -> ipc::IpcResult<()> {
        if SerializeOnDemandReasonExtraData(self, parcel.pin_mut().unwrap()) {
            Ok(())
        } else {
            Err(IpcStatusCode::Failed)
        }
    }
}

impl Deserialize for OnDemandReasonExtraData {
    fn deserialize(parcel: &mut MsgParcel) -> ipc::IpcResult<Self> {
        Ok(DeserializeOnDemandReasonExtraData(
            parcel.pin_mut().unwrap(),
        ))
    }
}

impl Serialize for SystemAbilityOnDemandReason {
    fn serialize(&self, parcel: &mut MsgParcel) -> ipc::IpcResult<()> {
        parcel.write(&self.name)?;
        parcel.write(&self.value)?;
        parcel.write(&self.reason_id)?;
        parcel.write(&self.extra_data)?;
        parcel.write(&self.extra_data_id)
    }
}

impl Deserialize for SystemAbilityOnDemandReason {
    fn deserialize(parcel: &mut MsgParcel) -> ipc::IpcResult<Self> {
        let name = parcel.read()?;
        let value = parcel.read()?;
        let reason_id = parcel.read()?;
        let extra_data = parcel.read()?;
        let extra_data_id = parcel.read()?;
        Ok(Self {
            name,
            value,
            reason_id,
            extra_data,
            extra_data_id,
        })
    }
}

impl OnDemandReasonExtraData {
    pub fn want(self) -> HashMap<String, String> {
        let mut res = HashMap::new();
        let mut want = self.want.into_iter();
        while let Some(k) = want.next() {
            if let Some(v) = want.next() {
                res.insert(k, v);
            }
        }
        res
    }
}

/// AbilityWrapper used for cxx
#[cxx::bridge(namespace = "OHOS::SafwkRust")]
mod ffi {
    #[repr(i32)]
    #[derive(Debug)]
    pub enum OnDemandReasonId {
        INTERFACE_CALL = 0,
        DEVICE_ONLINE = 1,
        SETTING_SWITCH = 2,
        PARAM = 3,
        COMMON_EVENT = 4,
        TIMED_EVENT = 5,
    }

    #[derive(Debug)]
    pub struct SystemAbilityOnDemandReason {
        pub name: String,
        pub value: String,
        pub reason_id: OnDemandReasonId,
        pub extra_data: OnDemandReasonExtraData,
        pub extra_data_id: i64,
    }

    #[derive(Debug)]
    pub struct OnDemandReasonExtraData {
        pub data: String,
        pub code: i32,
        pub want: Vec<String>,
    }

    extern "Rust" {
        type AbilityWrapper;
        type AbilityStub;
        fn on_remote_request(
            self: &mut AbilityStub,
            code: u32,
            data: Pin<&mut MessageParcel>,
            reply: Pin<&mut MessageParcel>,
        ) -> i32;

        fn dump(self: &mut AbilityStub, fd: i32, args: Vec<String>) -> i32;

        fn OnDump(self: &AbilityWrapper);

        unsafe fn OnStart(self: &AbilityWrapper, system_ability: *mut SystemAbilityWrapper);
        unsafe fn OnStartWithReason(
            self: &AbilityWrapper,
            reason: SystemAbilityOnDemandReason,
            system_ability: *mut SystemAbilityWrapper,
        );
        fn OnIdle(self: &AbilityWrapper, reason: SystemAbilityOnDemandReason) -> i32;
        fn OnActive(self: &AbilityWrapper, reason: SystemAbilityOnDemandReason);
        fn OnStop(self: &AbilityWrapper);
        fn OnStopWithReason(self: &AbilityWrapper, reason: SystemAbilityOnDemandReason);

        fn OnAddSystemAbility(self: &AbilityWrapper, said: i32, device_id: String);
        fn OnRemoveSystemAbility(self: &AbilityWrapper, said: i32, device_id: String);
        fn OnDeviceLevelChanged(
            self: &AbilityWrapper,
            change_type: i32,
            level: i32,
            action: String,
        );
        fn OnExtension(
            self: &AbilityWrapper,
            extension: String,
            data: Pin<&mut MessageParcel>,
            reply: Pin<&mut MessageParcel>,
        ) -> i32;
    }

    // C++ types and signatures exposed to Rust.
    unsafe extern "C++" {
        include!("system_ability_wrapper.h");
        include!("system_ability.h");
        include!("system_ability_ondemand_reason.h");
        #[namespace = "OHOS"]
        type OnDemandReasonId;

        #[namespace = "OHOS"]
        type MessageParcel = ipc::cxx_share::MessageParcel;

        type SystemAbilityWrapper;

        fn BuildSystemAbility(
            ability: Box<AbilityWrapper>,
            system_ability_id: i32,
            run_on_create: bool,
        ) -> UniquePtr<SystemAbilityWrapper>;

        unsafe fn RegisterAbility(system_ability: *mut SystemAbilityWrapper) -> bool;

        fn AddSystemAbilityListener(
            self: Pin<&mut SystemAbilityWrapper>,
            mut system_ability_id: i32,
        ) -> bool;

        fn RemoveSystemAbilityListener(
            self: Pin<&mut SystemAbilityWrapper>,
            mut system_ability_id: i32,
        ) -> bool;

        fn CancelIdleWrapper(self: Pin<&mut SystemAbilityWrapper>) -> bool;

        fn PublishWrapper(self: Pin<&mut SystemAbilityWrapper>, ability: Box<AbilityStub>) -> bool;

        fn DeserializeOnDemandReasonExtraData(
            data: Pin<&mut MessageParcel>,
        ) -> OnDemandReasonExtraData;

        fn SerializeOnDemandReasonExtraData(
            extraData: &OnDemandReasonExtraData,
            data: Pin<&mut MessageParcel>,
        ) -> bool;

        fn OnExtension(
            self: Pin<&mut SystemAbilityWrapper>,
            extension: &CxxString,
            data: Pin<&mut MessageParcel>,
            reply: Pin<&mut MessageParcel>,
        ) -> i32;
    }
}

#[cfg(test)]
mod test {
    use std::cell::RefCell;
    use std::rc::Rc;

    use cxx::let_cxx_string;

    use super::*;
    #[test]
    fn on_extension_ut() {
        const TEST_EXTENSION: &str = "test";
        const TEST_RETURN: i32 = 0;

        struct TestAbility {
            inner: Rc<RefCell<String>>,
        }
        impl Ability for TestAbility {
            fn on_extension(
                &self,
                extension: String,
                data: &mut MsgParcel,
                reply: &mut MsgParcel,
            ) -> i32 {
                self.inner.borrow_mut().push_str(&extension);
                data.write(TEST_EXTENSION).unwrap();
                reply.write(TEST_EXTENSION).unwrap();
                TEST_RETURN
            }
        }

        let extension = Rc::new(RefCell::new(String::new()));
        let ability = TestAbility {
            inner: extension.clone(),
        };
        let ability = Box::new(AbilityWrapper {
            inner: Box::new(ability),
        });
        let mut sys = BuildSystemAbility(ability, 3706, true);
        let mut data = MsgParcel::new();
        let mut reply = MsgParcel::new();
        let_cxx_string!(cxx_s = TEST_EXTENSION);
        assert_eq!(
            sys.pin_mut()
                .OnExtension(&cxx_s, data.pin_mut().unwrap(), reply.pin_mut().unwrap(),),
            TEST_RETURN
        );
        assert_eq!(*extension.borrow_mut(), TEST_EXTENSION);
        assert_eq!(data.read::<String>().unwrap(), TEST_EXTENSION);
        assert_eq!(reply.read::<String>().unwrap(), TEST_EXTENSION);
    }
}
