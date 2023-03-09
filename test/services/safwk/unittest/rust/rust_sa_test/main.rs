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
extern crate sa_test;
extern crate system_ability_fwk_rust;
use std::ffi::{c_char, CString};
use hilog_rust::{error, info, hilog, HiLogLabel, LogType};
const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xd001800,
    tag: "rustSA"
};
use ipc_rust::{
    IRemoteBroker, Result, RemoteObj
};
use system_ability_fwk_rust::{
    define_system_ability, RSystemAbility, ISystemAbility, IMethod
};

use sa_test::{
    ITest, TestStub
};

/// TEST_SERVICE_ID SAID
pub const TEST_SERVICE_ID: i32 = 1494;

/// TestService type
pub struct TestService;

impl ITest for TestService {
    fn echo_str(&self, value: &str) -> Result<String> {
        info!(LOG_LABEL,"TestService echo_str: {}", value);
        Ok(String::from(value))
    }

    fn request_concurent(&self, is_async: bool) -> Result<bool> {
        info!(LOG_LABEL,"TestService request_concurent: {}", is_async);
        Ok(true)
    }
}

impl IRemoteBroker for TestService {}

fn on_start<T: ISystemAbility + IMethod>(ability: &T) {
    let service = TestStub::new_remote_stub(TestService).expect("create TestService failed");
    ability.publish(&service.as_object().expect("get ITest service failed"), TEST_SERVICE_ID);
    info!(LOG_LABEL,"on_start");
}

fn on_stop<T: ISystemAbility + IMethod>(ability: &T) {
    let service = TestStub::new_remote_stub(TestService).expect("create TestService failed");
    ability.publish(&service.as_object().expect("get ITest service failed"), TEST_SERVICE_ID);
    info!(LOG_LABEL,"on_stop");
}

define_system_ability!(
    sa: SystemAbility(on_start, on_stop),
);

#[used]
#[link_section = ".init_array"]
static A:extern fn() = {
    extern fn init() {
        let r_sa = SystemAbility::new_system_ability(TEST_SERVICE_ID, true).expect("create TestService failed");
        r_sa.register();
    }
    init
};