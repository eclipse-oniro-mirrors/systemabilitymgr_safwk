/*
* Copyright (C) 2022 Huawei Device Co., Ltd.
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

//! This demo is used by main.rs
extern crate ipc_rust;
extern crate system_ability_fwk_rust;
use std::ffi::{c_char, CString};
use hilog_rust::{info, hilog, HiLogLabel, LogType};
const LOG_LABEL: HiLogLabel = HiLogLabel {
    log_type: LogType::LogCore,
    domain: 0xd001800,
    tag: "rustSA"
};
use ipc_rust::{
    IRemoteBroker, IRemoteObj, RemoteStub, IpcResult, IpcStatusCode,
    RemoteObj, define_remote_object, MsgParcel, BorrowedMsgParcel
};

/// First request code
pub const FIRST_CALL_TRANSACTION: isize = 0x00000001;

/// Function code of ITest
pub enum ITestCode {
    /// unload code
    CodeUnLoad = FIRST_CALL_TRANSACTION,
    /// request_concurent code
    CodeRequestConcurrent,
}

/// Function between proxy and stub of ITestService
pub trait ITest: IRemoteBroker {
    /// Test unload 
    fn unload(&self) -> IpcResult<String>;
    /// Test request_concurent
    fn request_concurent(&self, is_async: bool) -> IpcResult<bool>;
}

fn on_remote_request(stub: &dyn ITest, code: u32, data: &BorrowedMsgParcel,
    reply: &mut BorrowedMsgParcel) -> IpcResult<()> {
    info!(LOG_LABEL,"on_remote_reuqest in Rust TestStub, code: {}", code);
    match code {
        1 => {
            let value: String = data.read().expect("should have a string");
            stub.unload()?;
            reply.write(&value)?;
            Ok(())
        }
        2 => {
            let value = stub.request_concurent(true)?;
            reply.write(&value)?;
            Ok(())
        }
        _ => Err(IpcStatusCode::Failed)
    }
}

define_remote_object!(
    ITest["ohos.ipc.test"] {
        stub: TestStub(on_remote_request),
        proxy: TestProxy,
    }
);

// Make RemoteStub<TestStub> object can call ITest function directly.
impl ITest for RemoteStub<TestStub> {
    fn unload(&self) -> IpcResult<String>{
        // self will be convert to TestStub automatic because RemoteStub<TestStub>
        // implement the Deref trait
        info!(LOG_LABEL,"ITest_unload");
        self.0.unload()
    }

    fn request_concurent(&self, is_async: bool) -> IpcResult<bool> {
        info!(LOG_LABEL,"request_concurent");
        self.0.request_concurent(is_async)
    }
}

impl ITest for TestProxy {
    fn unload(&self) -> IpcResult<String> {
        info!(LOG_LABEL,"TestProxy unload");
        let mut data = MsgParcel::new().expect("MsgParcel should success");
        let value: &str = "unload test";
        data.write(value)?;
        let reply =
            self.remote.send_request(ITestCode::CodeUnLoad as u32, &data, false)?;
        let ret: String = reply.read().expect("need reply value");
        Ok(ret)
    }

    fn request_concurent(&self, is_async: bool) -> IpcResult<bool> {
        info!(LOG_LABEL,"TestProxy request_concurent");
        let data = MsgParcel::new().expect("MsgParcel should success");
        let reply =
            self.remote.send_request(ITestCode::CodeRequestConcurrent as u32, &data, is_async)?;
        let ret: bool = reply.read().expect("need reply value");
        Ok(ret)
    }
}