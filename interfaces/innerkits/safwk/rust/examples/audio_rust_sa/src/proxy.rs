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
use ipc::remote::RemoteObj;
use system_ability_fwk::cxx_share::SystemAbilityOnDemandReason;

use crate::interface;
pub struct AudioProxy {
    pub remote: RemoteObj,
}

impl AudioProxy {
    pub fn new(remote: RemoteObj) -> Self {
        Self { remote }
    }

    pub fn on_start_reason(&self) -> Option<SystemAbilityOnDemandReason> {
        let mut data = MsgParcel::new();
        let mut reply = self
            .remote
            .send_request(interface::START_REASON_TEST, &mut data)
            .unwrap();
        reply.read().unwrap()
    }

    pub fn on_idle_reason(&self) -> Option<SystemAbilityOnDemandReason> {
        let mut data = MsgParcel::new();
        let mut reply = self
            .remote
            .send_request(interface::IDLE_REASON_TEST, &mut data)
            .unwrap();
        reply.read().unwrap()
    }

    pub fn on_active_reason(&self) -> Option<SystemAbilityOnDemandReason> {
        let mut data = MsgParcel::new();
        let mut reply = self
            .remote
            .send_request(interface::ACTIVE_REASON_TEST, &mut data)
            .unwrap();
        reply.read().unwrap()
    }

    pub fn on_add(&self) -> bool {
        let mut data = MsgParcel::new();
        let mut reply = self
            .remote
            .send_request(interface::ADD_TEST, &mut data)
            .unwrap();
        reply.read().unwrap()
    }

    pub fn on_remove(&self) -> bool {
        let mut data = MsgParcel::new();
        let mut reply = self
            .remote
            .send_request(interface::REMOVE_TEST, &mut data)
            .unwrap();
        reply.read().unwrap()
    }

    pub fn send_stop_reason(&self, reason: &SystemAbilityOnDemandReason) {
        let mut data = MsgParcel::new();
        data.write(reason).unwrap();
        self.remote
            .send_request(interface::STOP_REASON_TEST, &mut data)
            .unwrap();
    }
    pub fn get_stop_reason(&self) -> Option<SystemAbilityOnDemandReason> {
        let mut data = MsgParcel::new();
        let mut reply = self
            .remote
            .send_request(interface::STOP_REASON_TEST_GET, &mut data)
            .unwrap();
        reply.read().unwrap()
    }
}
