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

//! demo rust SA
extern crate ipc_rust;
extern crate listen_rust_ipc;
extern crate audio_rust_ipc;
extern crate system_ability_fwk_rust;
extern crate rust_samgr;
use std::ffi::{c_char, CString};
use hilog_rust::{error, info, hilog, HiLogLabel, LogType};
const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xd001800,
    tag: "rustSA"
};

use ipc_rust::{
    IRemoteBroker, IpcResult, RemoteObj
};
use system_ability_fwk_rust::{
    define_system_ability, RSystemAbility, ISystemAbility, IMethod
};

use rust_samgr::{
    get_service_proxy
};

use listen_rust_ipc::{
    IListen, TestStub
};

use audio_rust_ipc::{
    ITest
};

/// TEST_LISTEN_ID SAID
pub const TEST_LISTEN_ID: i32 = 1494;
/// TEST_AUDIO_ID SAID
pub const TEST_AUDIO_ID: i32 = 1499;

/// TestService type
pub struct TestService;

impl IListen for TestService {
    fn echo_str(&self, value: &str) -> IpcResult<String> {
        info!(LOG_LABEL,"TestService echo_str: {}", value);
        Ok(String::from(value))
    }

    fn request_concurent(&self, is_async: bool) -> IpcResult<bool> {
        info!(LOG_LABEL,"TestService request_concurent: {}", is_async);
        Ok(true)
    }
}

impl IRemoteBroker for TestService {}

fn on_start<T: ISystemAbility + IMethod>(ability: &T) {
    info!(LOG_LABEL,"1494on_start");
    let object = get_service_proxy::<dyn ITest>(TEST_AUDIO_ID);
    let object = match object {
        Ok(object) => object,
        Err(error) => {
            error!(LOG_LABEL,"convert RemoteObj to TestProxy failed: {}", error);
            panic!();
        }
    };
    let _value = object.unload().expect("should return string");
    let service = TestStub::new_remote_stub(TestService).expect("create TestService failed");
    ability.publish(&service.as_object().expect("get IListen service failed"), -1);
}

fn on_stop<T: ISystemAbility + IMethod>(ability: &T) {
    info!(LOG_LABEL,"on_stop");
    let service = TestStub::new_remote_stub(TestService).expect("create TestService failed");
    ability.publish(&service.as_object().expect("get IListen service failed"), -1);
}

define_system_ability!(
    sa: SystemAbility(on_start, on_stop),
);

#[used]
#[link_section = ".init_array"]
static A:extern fn() = {
    extern fn init() {
        let r_sa = SystemAbility::new_system_ability(TEST_LISTEN_ID, true).expect("create TestService failed");
        r_sa.register();
    }
    init
};