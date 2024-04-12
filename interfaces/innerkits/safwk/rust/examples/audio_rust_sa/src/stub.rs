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
use system_ability_fwk::ability::Handler;
use system_ability_fwk::cxx_share::SystemAbilityOnDemandReason;

use crate::{interface, Watch};

pub struct AudioService {
    system_ability: Handler,
    watch: Arc<Mutex<Watch>>,
}

impl AudioService {
    pub(crate) fn new(handler: Handler, watch: Arc<Mutex<Watch>>) -> Self {
        Self {
            system_ability: handler,
            watch,
        }
    }
}

impl RemoteStub for AudioService {
    fn on_remote_request(
        &self,
        code: u32,
        data: &mut ipc::parcel::MsgParcel,
        reply: &mut ipc::parcel::MsgParcel,
    ) -> i32 {
        self.system_ability.cancel_idle();
        match code {
            interface::START_REASON_TEST => self.start_reason(data, reply),
            interface::IDLE_REASON_TEST => self.idle_reason(data, reply),
            interface::ACTIVE_REASON_TEST => self.active_reason(data, reply),
            interface::ADD_TEST => self.on_add(data, reply),
            interface::REMOVE_TEST => self.on_remove(data, reply),
            interface::STOP_REASON_TEST => self.recv_stop_reason(data, reply),
            interface::STOP_REASON_TEST_GET => self.send_stop_reason(data, reply),
            _ => IpcStatusCode::Failed as i32,
        }
    }
}

impl AudioService {
    fn start_reason(&self, _data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        reply
            .write(&self.watch.lock().unwrap().start_reason)
            .unwrap();
        0
    }

    fn idle_reason(&self, _data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        reply
            .write(&self.watch.lock().unwrap().idle_reason)
            .unwrap();
        0
    }

    fn active_reason(&self, _data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        reply
            .write(&self.watch.lock().unwrap().active_reason)
            .unwrap();
        0
    }

    fn on_add(&self, _data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        reply.write(&self.watch.lock().unwrap().on_add).unwrap();
        0
    }

    fn on_remove(&self, _data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        reply.write(&self.watch.lock().unwrap().on_remove).unwrap();
        0
    }

    fn recv_stop_reason(&self, data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        self.watch.lock().unwrap().stop_reason = Some(data.read().unwrap());
        0
    }
    fn send_stop_reason(&self, data: &mut MsgParcel, reply: &mut MsgParcel) -> i32 {
        reply
            .write(&self.watch.lock().unwrap().stop_reason)
            .unwrap();
        0
    }
}
