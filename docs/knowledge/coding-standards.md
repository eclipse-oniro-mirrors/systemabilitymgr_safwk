# 编码铁律

本篇将AGENTS.md约束与可在当前实现中核实的开发边界相结合。对现有源码的描述不自动构成修改授权；资料与实现差异单独写明。整体时序见[技术机制](safwk-Technology.md)。

## SA构造、配置与发布

- **禁止**绕过框架登记路径手动创建一个SA后即认定可用。C++使用REGISTER_SYSTEM_ABILITY_BY_ID，宏内部new后MakeAndRegisterAbility；Rust使用既有build/register适配。两者都只进入本地map，业务就绪后还需Publish远端对象。
- **禁止**在SA构造函数中等待samgr或GetSystemAbility；静态初始化发生在库加载期，阻塞会卡住加载和阶段初始化。新服务按README与listen_ability示例配套业务类/Stub、源Profile、ohos_sa_profile构建和init cfg。
- said、运行Profile.process与承载配置的服务名必须按项目规则对齐；路径以真实运行文件为准。禁止修改out下的派生JSON代替源配置。源JSON单SA规则不等于运行合并Profile单SA。
- Publish前后检查返回值并明确业务失败如何通知；Publish先保存publishObj_，失败不保证引用已释放。OnStartFail只向samgr通知，不替业务自动回滚。

依据：[interfaces/innerkits/safwk/system_ability.h：REGISTER_SYSTEM_ABILITY_BY_ID](../../interfaces/innerkits/safwk/system_ability.h#L29)、[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::AddAbility](../../services/safwk/src/local_ability_manager.cpp#L261)、[services/safwk/src/system_ability.cpp：SystemAbility::Publish](../../services/safwk/src/system_ability.cpp#L77)、[README_zh.md](../../README_zh.md#L1)。

## 状态、返回与完成

- **禁止**把isRunning_、abilityState_、本地map和samgr记录当成同一状态。Start尾置isRunning=true，ACTIVE只在Publish成功后建立；OnStart返回不能充当“SA可查询”验收。
- **禁止**按LSA方法bool或IPC status推断业务状态转换已发生。ActiveAbility/IdleAbility可在找到对象后返回true，而内部状态守卫拒绝转换；Idle需结合delayTime判断。
- OnIdle返回0才进入本地IDLE；正数是延后，-1表示拒绝/不适用。CancelIdle只在IDLE向samgr请求取消卸载，非IDLE直接true，不立刻回调OnActive。
- Stop先OnStop、再本地NOT_LOADED/false、后远端Remove；移除失败要保留定界证据。静态StopAbility只发Remove，不等于执行完整OnStop链。

依据：[services/safwk/src/system_ability.cpp：SystemAbility::Start](../../services/safwk/src/system_ability.cpp#L181)、[services/safwk/src/system_ability.cpp：SystemAbility::Idle](../../services/safwk/src/system_ability.cpp#L212)、[services/safwk/src/system_ability.cpp：SystemAbility::Stop()](../../services/safwk/src/system_ability.cpp#L260)。

## 回调、并发与对象生存期

- OnStart及其他控制回调不得无界阻塞；初始任务、IPC调用、按需线程和Timer有不同占用范围。不要把“阶段等待最多100秒”当成整个进程启动耗时保证。
- 保持localAbilityMapLock_、listenerLock_、ReasonLock_和startPhaseLock_各自的不变量。GetAbility返回后表锁已释放，裸指针并未获得自动生存期保护；清map与释放对象也不是同一操作。
- 新增线程依照AGENTS的FFRT约束；当前Start/Stop已有detach std::thread且初始化使用ThreadPool，是需要理解的现状。不得为让文档看似符合规则而宣称全仓无std::thread。
- onStartLock_仅串行Start；abilityLock的状态检查与业务回调并非全程同一临界区。改Stop/Idle重入前审查调用关系，避免持表锁跨业务回调。

依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::StartAbility](../../services/safwk/src/local_ability_manager.cpp#L551)、[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::GetAbility](../../services/safwk/src/local_ability_manager.cpp#L493)、[services/safwk/src/system_ability.cpp：SystemAbility::Start](../../services/safwk/src/system_ability.cpp#L181)。

## 依赖、阶段和数值单位

- 新增依赖需核对阶段、是否会发布、失败如何退出；初次StartSystemAbilityTask才执行StartDependSaTask等待，后续按需启停不自动复用该门禁。
- dependTimeout合法范围200～60000毫秒，初值6000；实际SetDependTimeout只接受范围内值，越界保留旧值。AGENTS“截断”表述不能作为当前实现验收结果。
- 时间单位逐层核对：依赖等待毫秒/轮询usleep微秒；unusedTimeout秒转毫秒并限1～120分钟；FFRTHandler输入毫秒乘1000；缓存名expireTimeSec实际按毫秒比较。禁止仅凭变量后缀判断单位或重复换算。

依据：[services/safwk/include/local_ability_manager.h：MIN_DEPENDENCY_TIMEOUT](../../services/safwk/include/local_ability_manager.h#L37)、[services/safwk/src/system_ability.cpp：SystemAbility::SetDependTimeout](../../services/safwk/src/system_ability.cpp#L371)、[interfaces/innerkits/safwk/expire_lru_cache.h：expireTimeMilliSec_](../../interfaces/innerkits/safwk/expire_lru_cache.h#L34)、[services/safwk/src/ffrt_handler.cpp：CONVERSION_FACTOR](../../services/safwk/src/ffrt_handler.cpp#L26)。

## 管理IPC、扩展与文件描述符

- **禁止**把业务IRemoteObject令牌用于LSA管理协议；LSA令牌是ohos.localabilitymanager.accessToken。令牌检查与AccessToken权限都必须保留，不能用“调用者是原生进程”替代授权。
- 改事务码、公共SystemAbility签名、Profile schema或foundation_trust条目需按AGENTS另行明确变更范围；本组文档不修改它们。协议码/参数读写还需与samgr消费者配对。
- 接收fd后遍历每个正常与异常出口；统计/svc当前接收fd后设置各自FDSAN owner tag，处理结束用对应tag关闭，业务异步保留fd需先建立独立所有权，不保存已关闭的整数。
- SystemAbilityExtensionPara持有当前parcel指针，OnExtension必须遵守同步借用边界；不能把isAsync参数名称解读成服务端自动投递。

依据：[services/safwk/src/local_ability_manager_stub.cpp：LocalAbilityManagerStub::OnRemoteRequest](../../services/safwk/src/local_ability_manager_stub.cpp#L89)、[services/safwk/src/local_ability_manager_stub.cpp：LocalAbilityManagerStub::IpcStatCmdProcInner](../../services/safwk/src/local_ability_manager_stub.cpp#L245)、[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::SystemAbilityExtProc](../../services/safwk/src/local_ability_manager.cpp#L1050)。

## 配置、原因与监听失败

- **禁止**把Trust配置缺失/损坏描述成统一拒绝；当前行为取决于存在性、解析成功、map是否非空与process是否匹配。安全策略调整必须单独定义验收。
- reason JSON要保证关键字段完整、类型明确；C++默认reasonId_没有显式初始化，不能依赖缺失eventId得到固定默认。HasExtraData只认COMMON_EVENT，取数失败仍可能继续业务回调。
- Subscribe失败前本地监听项可能已写入，UnSubscribe失败前本地桶可能已删；重试应依据实际副作用设计，不能把false当未修改。通知去重标志先于业务回调更新，不能假设目标对象消失后自动补发。

依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::CheckTrustSa](../../services/safwk/src/local_ability_manager.cpp#L237)、[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::JsonToOnDemandReason](../../services/safwk/src/local_ability_manager.cpp#L607)、[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::AddSystemAbilityListener](../../services/safwk/src/local_ability_manager.cpp#L302)。

## 缓存一致性与资源

- 缓存接入要先证明API可缓存性，并定义远端实例/身份隔离和失效时机。PostSendRequest不判断业务错误，调用者不能无条件缓存失败回复；对象offset不为0的parcel被拒绝。
- 对TTL按当前毫秒实现解释，Get不会续期；重复Add不修改桶配置。Clear保留接口桶，Del释放桶；不要混用。
- Pre把字节追加reply，调用方应提供期望的干净/正确位置缓冲；不要假定框架会重置任意已有reply。没有对象offset也不自动证明所有缓冲内容都可安全复用。

依据：[services/safwk/src/api_cache_manager.cpp：ApiCacheManager::PreSendRequest](../../services/safwk/src/api_cache_manager.cpp#L105)、[services/safwk/src/api_cache_manager.cpp：ApiCacheManager::PostSendRequest](../../services/safwk/src/api_cache_manager.cpp#L140)、[interfaces/innerkits/safwk/expire_lru_cache.h：DoAdd](../../interfaces/innerkits/safwk/expire_lru_cache.h#L44)。

## Rust与派生桥

- 只改源wrapper.rs和明确的手写C++包装，不直接编辑CXX生成文件；BUILD.gn将两者合并，不能凭目录名把所有src/cxx文件都判为生成物。
- Box::into_raw与from_raw必须唯一配对；Handler Clone复制裸指针，不能越过SystemAbilityWrapper生存期，不自动授予Send/Sync或串行回调保证。
- 读取Parcelable的分配对象按调用方约定释放；borrowed MsgParcel不能留给后台任务。Rust dump的File::from_raw_fd会接管fd，应核对C++/Rust释放边界。
- 变更reason枚举需同时检查C++与Rust反序列化；当前UNREF_EVENT=6未被RustDeserialize接受，缺数据也存在不同降级，不能只校验C++编译。

依据：[interfaces/innerkits/safwk/rust/BUILD.gn](../../interfaces/innerkits/safwk/rust/BUILD.gn#L1)、[interfaces/innerkits/safwk/rust/src/cxx/system_ability_wrapper.cpp：SystemAbilityWrapper::~SystemAbilityWrapper](../../interfaces/innerkits/safwk/rust/src/cxx/system_ability_wrapper.cpp#L35)、[interfaces/innerkits/safwk/rust/src/wrapper.rs：impl Deserialize for OnDemandReasonId](../../interfaces/innerkits/safwk/rust/src/wrapper.rs#L137)。

## 诊断与验证

- 生命周期耗时与退出路径维护AGENTS要求的DFX；调用点在本仓、适配器来自samgr，不能虚构本仓另有适配实现或所有错误分支均上报。
- FFRT START/STOP/GET是有状态操作；GET消费缓冲、清理任务可使结果失效。不能把快照符号缺失或fd写失败解释成“没有业务请求”。
- 构建和测试只报告真实执行结果。当前仅文档生成与静态核查，未运行产品编译、单元测试、模糊测试、XTS或设备命令；建议回归点不充作已通过用例。

依据：[services/safwk/src/local_ability_manager_dumper.cpp：LocalAbilityManagerDumper::GetFfrtStatistics](../../services/safwk/src/local_ability_manager_dumper.cpp#L145)、[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::FfrtDumperProc](../../services/safwk/src/local_ability_manager.cpp#L1024)、[test/services/safwk/unittest/BUILD.gn](../../test/services/safwk/unittest/BUILD.gn#L1)。

## 独立卸载与部署配置

- `Profile.independentUnload`只在对应OnStopAbility分支触发额外资源清理：Stop→RemoveAbility→ClearResourceForIndependentUnload→CloseSo→AddOnDemandSystemAbilityInfo。不要把普通Stop与CloseSo混为一条无条件路径，也不能把RemoveAbility等同delete。
- ClearResourceForIndependentUnload在publishObj_为空时直接返回；否则每秒检查强引用，超过1时持续等待，没有显式重试上限。异步回调、业务持有或缓存对象可能延长等待，不得在文档中承诺有限时卸载。清引用与CloseSo之间的对象生存期必须按当前代码核对。
- subfoundation的uid、权限、Trust及数据目录属主迁移是部署契约；改业务承载进程时同步核对cfg/Profile/Trust和已有数据访问，不直接照搬foundation身份。

依据：[services/safwk/src/system_ability.cpp：SystemAbility::ClearResourceForIndependentUnload](../../services/safwk/src/system_ability.cpp#L482)、[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::OnStopAbility](../../services/safwk/src/local_ability_manager.cpp#L471)、[etc/profile/subfoundation/subfoundation.cfg："services"](../../etc/profile/subfoundation/subfoundation.cfg#L15)。
