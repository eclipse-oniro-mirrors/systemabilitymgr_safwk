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

pub mod r_system_ability;
pub mod macros;
extern crate ipc_rust;
use ipc_rust::{
    RemoteObj
};
/// define Callback function for rust system_ability
pub trait ISystemAbility {
    /// Callback for rust SA on_start
    fn on_start(&self);

    /// Callback for rust SA on_stop
    fn on_stop(&self);
}

/// define Interface function for rust system_ability
pub trait IMethod {
    /// register sa
    fn register(&self);

    /// publish sa
    fn publish(&self, service: &RemoteObj, said : i32);
}