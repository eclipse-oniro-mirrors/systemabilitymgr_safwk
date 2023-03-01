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

 use std::ffi::{c_void};

 #[repr(C)]
 pub struct CSystemAbility {
     _private: [u8; 0],
 }
  
 // Callback function type for OnStart() from native, this
 // callback will be called when native SystemAbility start
 pub type OnStartCb = unsafe extern "C" fn (
    sa: *mut CSystemAbility,
);
 
// Callback function type for OnStop() from native,
// this callback will be called when native SystemAbility stop
pub type OnStopCb = unsafe extern "C" fn (
   sa: *mut CSystemAbility,
);

#[link(name = "system_ability_fwk_c")]
extern "C" {
    pub fn CreateSystemAbility(said: i32, run_on_create: bool, on_start: OnStartCb,
        on_stop: OnStopCb) -> *mut CSystemAbility;
    pub fn RemoteObjectGetUserData(system_ability: *mut CSystemAbility) -> *const c_void;
    pub fn MakeAndRegisterAbility(system_ability: *mut CSystemAbility, user_data: *const c_void) -> bool;
    pub fn DeleteNative(system_ability: *mut CSystemAbility);
}