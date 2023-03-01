/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
use crate::{sa_binding};
use crate::safwk::ISystemAbility;
use crate::sa_binding::{CSystemAbility};
use std::ffi::{c_void, c_char, CString};
use std::marker::PhantomData;
extern crate rust_samgr;
extern crate ipc_rust;
use rust_samgr::{
    SAExtraProp, get_systemability_manager
};
use ipc_rust::{
    RemoteObj
};
use hilog_rust::{info, error, hilog, HiLogLabel, LogType};
const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xd001800,
    tag: "safwk_rust"
};
/// RSystemAbility packed the c_system_ability CSystemAbility
/// which must implement ISystemAbility trait.
#[repr(C)]
pub struct RSystemAbility<T: ISystemAbility> { 
    // c_system_ability will be free when RSystemAbility free
    c_system_ability: *mut CSystemAbility,
    _phantom: PhantomData<T>,
}

impl <T: ISystemAbility> RSystemAbility<T> {
    /// call C CreateSystemAbility and create a RSystemAbility object
    pub fn new(said: i32, run_on_create: bool) -> Option<Self> {
        let c_system_ability = unsafe {
            // set on_start and on_stop to c_system_ability, 
            // so we can callback on_start and on_stop
            sa_binding::CreateSystemAbility(said, run_on_create, Self::on_start,
                Self::on_stop)
        };
        
        if c_system_ability.is_null() {
            error!(LOG_LABEL,"RSystemAbility<T> c_system_ability is_null");
            None
        } else {
            Some( RSystemAbility { c_system_ability, _phantom : PhantomData } )
        }
    }

    /// call C MakeAndRegisterAbility and then register c_system_ability to local
    pub fn register_ability(&self, system_ability: &T) {
        let system_ability = Box::into_raw(Box::new(system_ability));
        unsafe {
            // register c_system_ability to local, so c_system_ability onstart can be called
            sa_binding::MakeAndRegisterAbility(self.c_system_ability, system_ability as *mut c_void);
        };
    } 

    /// pubish sa to samgr
    pub fn publish(&self, service: &RemoteObj, said : i32) {
        let sapro = SAExtraProp::default();
        let samgr_proxy = get_systemability_manager();
        let res = samgr_proxy.add_systemability(service, said, sapro);
        match res {
            Ok(())=> {
                info!(LOG_LABEL,"add systemAbilityId {} success", said);
            },
            Err(err)=> {
                error!(LOG_LABEL,"add server to samgr failed res code: {}", err);
            }
        }
    }
}

impl<T: ISystemAbility> Drop for RSystemAbility<T> {
    fn drop(&mut self) {
        unsafe {
            sa_binding::DeleteNative(self.c_system_ability);
        }
    }
}

/// C call Rust
impl<T: ISystemAbility> RSystemAbility<T> {
    unsafe extern "C" fn on_start(stub: *mut CSystemAbility) {
        let object = sa_binding::RemoteObjectGetUserData(stub);
        let rust_object: &T = &*(object as *const T);
        rust_object.on_start()
    }

    unsafe extern "C" fn on_stop(stub: *mut CSystemAbility) {
        let object = sa_binding::RemoteObjectGetUserData(stub);
        let rust_object: &T = &*(object as *const T);
        rust_object.on_stop()
    }
}