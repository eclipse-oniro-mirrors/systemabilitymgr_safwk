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
use samgr::manage::SystemAbilityManager;

use crate::interface::AudioInterfaceCode;
use crate::{test_msg, TEST_AUDIO_ID};

pub struct AudioService;

impl RemoteStub for AudioService {
    fn on_remote_request(
        &self,
        code: u32,
        data: &mut ipc::parcel::MsgParcel,
        reply: &mut ipc::parcel::MsgParcel,
    ) -> i32 {
        match code {
            _ if code == AudioInterfaceCode::Unload as u32 => {
                self.unload(data, reply);
            }
            _ if code == AudioInterfaceCode::RequestExample as u32 => {
                self.request_example(data, reply);
            }
            _ if code == AudioInterfaceCode::CheckResult as u32 => {
                self.check_result(data, reply);
            }
            _ => return IpcStatusCode::Failed as i32,
        }
        0
    }
}

impl AudioService {
    fn unload(&self, data: &mut MsgParcel, reply: &mut MsgParcel) {
        info!("TestService unload");

        assert_eq!(data.read::<String>().unwrap(), "unload sa");

        assert_eq!(
            0,
            SystemAbilityManager::unload_system_ability(TEST_AUDIO_ID)
        );

        reply.write("unload success").unwrap();
    }

    fn request_example(&self, data: &mut MsgParcel, reply: &mut MsgParcel) {
        info!("Test Request example recv");

        assert_eq!(data.read::<String>().unwrap(), "request example");

        reply.write("request success").unwrap();
    }

    fn check_result(&self, _data: &mut MsgParcel, reply: &mut MsgParcel) {
        let mut msg = test_msg().lock().unwrap();
        let len = msg.readable();
        let v = msg.read_buffer(len).unwrap();
        reply.write_buffer(&v).unwrap();
    }
}
