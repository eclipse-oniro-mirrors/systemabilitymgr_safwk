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

use crate::interface::AudioInterfaceCode;

pub struct AudioProxy {
    remote: RemoteObj,
}

impl AudioProxy {
    pub fn new(remote: RemoteObj) -> Self {
        Self { remote }
    }

    pub fn unload(&self) {
        let code = AudioInterfaceCode::Unload as u32;
        info!("TestProxy unload");

        let mut data = MsgParcel::new();
        data.write("unload sa").unwrap();

        let mut reply = self.remote.send_request(code, &mut data).unwrap();
        assert_eq!(reply.read::<String>().unwrap(), "unload success");
    }

    pub fn request_example(&self) {
        let code = AudioInterfaceCode::RequestExample as u32;
        info!("TestProxy request example");

        let mut data = MsgParcel::new();
        data.write("request example").unwrap();

        let mut reply = self.remote.send_request(code, &mut data).unwrap();
        assert_eq!(reply.read::<String>().unwrap(), "request success");
    }
}
