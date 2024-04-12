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

use std::sync::{Arc, Mutex};

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteStub;
use ipc::IpcStatusCode;
use system_ability_fwk::cxx_share::SystemAbilityOnDemandReason;

use crate::interface::ListenInterfaceCode;

pub(crate) struct Watch {
    pub(crate) stop_reason: Option<SystemAbilityOnDemandReason>,
}

pub struct ListenService {
    watch: Arc<Mutex<Watch>>,
}

impl RemoteStub for ListenService {
    fn on_remote_request(&self, code: u32, data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        match code {
            0 => self.recv_stop_reason(data, reply),
            1 => self.send_stop_reason(data, reply),
            _ => IpcStatusCode::Failed as i32,
        }
    }
}

impl ListenService {
    pub(crate) fn new(watch: Arc<Mutex<Watch>>) -> Self {
        Self { watch }
    }
    fn recv_stop_reason(&self, data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        self.watch.lock().unwrap().stop_reason = Some(data.read().unwrap());
        0
    }
    fn send_stop_reason(&self, data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        reply.write(&self.watch.lock().unwrap().stop_reason);
        0
    }
}
