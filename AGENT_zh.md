# AGENT.md — safwk（系统服务框架）

> 路径: `foundation/systemabilitymgr/safwk` | 子系统: `systemabilitymgr` | 仅标准系统
>
> **维护规则**: 使用agent进行代码修改时，如新增代码结构或功能特性，必须同步更新本AGENT文档。

## 代码地图

```
interfaces/innerkits/safwk/
  system_ability.h                # SystemAbility基类 — 所有SA必须继承
  system_ability_ondemand_reason.h  # 按需原因结构（启动/空闲/唤醒/停止）
  api_cache_manager.h             # IPC响应缓存（基于ExpireLruCache）
  expire_lru_cache.h              # 带TTL的LRU缓存模板
  rust/                           # Rust绑定(system_ability_fwk_rust)
services/safwk/
  src/main.cpp                    # sa_main入口 — 解析参数、加载profile、启动SA
  src/local_ability_manager.cpp   # LSA单例 — 管理当前进程所有SA
  src/local_ability_manager_stub.cpp  # IPC Stub — 接收samgr的控制指令
  src/system_ability.cpp          # SystemAbility实现 — Publish()、生命周期分发
  src/system_ability_start.cpp    # saspawn fork优化（默认关闭）
  include/local_ability_manager.h # LSA类定义
  include/ffrt_handler.h          # FFRT任务封装
svc/                              # svc命令行工具 — 向SA发送IPC命令（dump等）
etc/profile/
  foundation.cfg                  # foundation进程init配置
  foundation_trust.json           # foundation进程受信SA列表
test/
  services/safwk/unittest/listen_ability/  # 完整SA示例（优先学习）
  mock/common/                    # Mock SA库（demo_sa, ondemand_ability, connect_ability...）
```

| 构建目标 | 类型 | 产物 |
|---|---|---|
| `services/safwk:sa_main` | 可执行文件 | `/system/bin/sa_main`（SA进程入口） |
| `interfaces/innerkits/safwk:system_ability_fwk` | 共享库 | `libsystem_ability_fwk.z.so`（SA开发框架） |
| `interfaces/innerkits/safwk:system_ability_ondemand_reason` | 静态库 | 按需原因序列化 |
| `interfaces/innerkits/safwk:api_cache_manager` | 共享库 | API缓存管理 |
| `svc:svc` | 可执行文件 | `/system/bin/svc`（SA调试工具） |

## 知识路由

| 问题 | 先读 |
|---|---|
| 开发新SA | `system_ability.h` → `test/services/safwk/unittest/listen_ability/`（完整示例） |
| SA注册宏 | `system_ability.h` → `REGISTER_SYSTEM_ABILITY_BY_ID` 宏 |
| sa_main启动流程 | `src/main.cpp` → `LocalAbilityManager::DoStartSAProcess` |
| SA生命周期回调 | `system_ability.h`（OnStart/OnStop/OnIdle/OnActive）→ `system_ability.cpp` |
| Profile JSON格式 | samgr: `sa_profiles.h`（SaProfile结构） |
| 依赖SA机制 | `local_ability_manager.cpp` → `StartDependSaTask` |
| SA状态变更通知 | `local_ability_manager.cpp` → `FindAndNotifyAbilityListeners` |
| BootPhase并行启动 | `local_ability_manager.cpp` → `StartPhaseTasks` |
| IPC Stub（samgr指令） | `local_ability_manager_stub.cpp`（StartAbility/StopAbility/IdleAbility/ActiveAbility） |
| svc工具用法 | `svc/src/svc_control.cpp` |
| API缓存 | `api_cache_manager.cpp` |
| saspawn特性 | `config.gni` → `system_ability_start.cpp` |

**SA生命周期状态机**: `NOT_LOADED → ACTIVE → IDLE → (CancelIdle→ACTIVE | stop)`。回调: `OnStart`→ACTIVE、`OnIdle`→IDLE（返回delayTime毫秒）、`OnActive`→ACTIVE、`OnStop`→终止。

**SA开发四件套**: (1) SA实现（.cpp，继承SystemAbility + REGISTER宏）、(2) profile JSON（每个文件一个SA）、(3) `ohos_sa_profile` BUILD.gn、(4) 进程.cfg（init通过 `sa_main /system/profile/xxx.json` 启动）。

## 专家约束

1. **必须使用REGISTER_SYSTEM_ABILITY_BY_ID宏** — 禁止直接 `new` SA对象。宏在静态初始化阶段执行。
2. **OnStart中必须调用Publish(this)** — 否则SA对其他进程不可见。
3. **进程名三处一致**: .cfg服务名 = profile JSON的`process`字段 = sa_main推导名。不一致导致按需加载静默失败。
4. **Profile JSON每个文件只能一个systemability节点** — 多个导致编译失败。
5. **dependTimeout范围**: [200ms, 60000ms]，默认6000ms，超范围会被截断。
6. **SA构造函数中禁止调用GetSystemAbility** — samgr可能未就绪。在OnStart中调用。
7. **OnStart禁止阻塞** — 会卡住整个进程启动，导致依赖链级超时。
8. **OnIdle必须返回有意义的delayTime** — samgr据此调度卸载。
9. **SA的.so命名**: `libXXX.z.so` — sa_main的dlopen按此模式匹配。
10. **safwk → samgr单向编译依赖**: safwk链接 `samgr:samgr_proxy` + `samgr:samgr_common`；samgr链接safwk的 `system_ability_ondemand_reason` 静态库。此循环为设计预期。
11. **innerapi可见性**: `system_ability_fwk` 标记为 `platformsdk_indirect` + `sasdk`。
12. **foundation_trust.json**: 安全门控 — 只有列表中的SA能在foundation进程中加载。
13. **AccessToken**: `safwk_support_access_token` 控制调用方校验（access_token组件存在时默认开启）。

**反模式**: 手动new SA对象 · OnStart忘记Publish · OnStart中阻塞 · Profile多systemability节点 · 进程名跨文件不一致 · 使用std::thread而非FfrtHandler。

## 编译与测试

```bash
# 编译safwk组件
./build.sh --product-name {product} --build-target safwk

# 编译特定目标
./build.sh --product-name {product} \
  --build-target foundation/systemabilitymgr/safwk/services/safwk:sa_main

# 单元测试
./build.sh --product-name {product} \
  --build-target foundation/systemabilitymgr/safwk/test:unittest

# Fuzz测试
./build.sh --product-name {product} \
  --build-target foundation/systemabilitymgr/safwk/test/fuzztest/systemabilityfwk_fuzzer:fuzztest
```

**特性开关** (`config.gni`): `safwk_enable_run_on_demand_qos`(F)、`safwk_feature_support_saspawn`(F)。变量(`var.gni`): `safwk_support_access_token`(T，条件)。

**设备调试**: `ps -ef | grep sa_main`（列出SA进程）· `hidumper -s <said>` · `svc <command>` · `cat /system/profile/<process>.json` · `sa_main /system/profile/<process>.json`（手动启动）。
