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

/// This macro can define a rust SA and give callback function on_start on_stop
#[macro_export]
macro_rules! define_system_ability { 
    {
        sa: $system_ability:ident($on_start:path, $on_stop:path),
    } => {
        /// $system_ability is passed in by the user
        pub struct $system_ability {
            r_system_ability: RSystemAbility<$system_ability>,
        }

        impl $system_ability {
            /// Create a $system_ability object
            #[allow(dead_code)]
            pub fn new_system_ability(said: i32, run_on_create: bool) -> Option<Self> {
                let r_system_ability = RSystemAbility::new(said, run_on_create);
                match r_system_ability {
                    Some(r_system_ability) => {
                        Some($system_ability { r_system_ability })
                    },
                    None => {
                        error!(LOG_LABEL,"RSystemAbility::new failed");
                        None
                    }
                }
            }
        }

        impl $crate::ISystemAbility for $system_ability {
            /// Callback to deal safwk onstart for this system_ability
            fn on_start(&self) {
                $on_start(self)
            }

            /// Callback to deal safwk onstop for this system_ability
            fn on_stop(&self) {
                $on_stop(self)
            }
        }

        impl $crate::IMethod for $system_ability {
            /// Call RSystemAbility<$system_ability> register_ability
            fn register(&self) {
                self.r_system_ability.register_ability(self);
            }

            /// Call RSystemAbility<$system_ability> publish
            fn publish(&self, service: &RemoteObj, said : i32) {
                self.r_system_ability.publish(service, said);
            }
        }
    };
}