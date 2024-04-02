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

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteStub;
use ipc::IpcStatusCode;

use crate::interface::ListenInterfaceCode;

struct ListenService;

impl RemoteStub for ListenService {
    fn on_remote_request(
        &self,
        code: u32,
        data: &mut ipc::parcel::MsgParcel,
        reply: &mut ipc::parcel::MsgParcel,
    ) -> i32 {
        info!("on_remote_reuqest in Rust TestStub, code: {}", code);

        match code {
            _ if code == ListenInterfaceCode::EchoStr as u32 => self.echo_str(data, reply),
            _ if code == ListenInterfaceCode::RequestConcurrent as u32 => {
                self.request_example(data, reply)
            }
            _ => return IpcStatusCode::Failed as i32,
        }
        0
    }
}

impl ListenService {
    fn echo_str(&self, _data: &mut MsgParcel, _reply: &mut MsgParcel) {
        info!("echo_str");
    }

    fn request_example(&self, _data: &mut MsgParcel, _reply: &mut MsgParcel) {
        info!("request example");
    }
}
