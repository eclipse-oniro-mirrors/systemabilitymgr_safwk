# 术语表

术语以当前源码中的业务含义为准。数值属于本地实现；跨仓/产品行为仍需结合配置验证。与状态/所有权相关的详细关系见[业务实体](safwk-Entity.md)。

## 系统能力（SA / SystemAbility）

以said标识的系统服务及其生命周期对象；业务远端对象可能由另一个Stub承载。不要与应用Ability、samgr注册表项混用。 依据：[interfaces/innerkits/safwk/system_ability.h：class SystemAbility](../../interfaces/innerkits/safwk/system_ability.h#L53)。

## 服务承载进程（sa_main / procName_）

承载一个或多个SA的运行进程。sa_main是入口，Profile.process是LSA向samgr注册的进程名；进程启动成功不等于各SA已发布。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::AddLocalAbilityManager](../../services/safwk/src/local_ability_manager.cpp#L885)。

## 进程内管理器（LSA / LocalAbilityManager）

每进程单例，管理本地对象、启动阶段、原因和监听，同时承接samgr控制。它不拥有全局服务表。 依据：[services/safwk/include/local_ability_manager.h：class LocalAbilityManager](../../services/safwk/include/local_ability_manager.h#L46)。

## 本地登记（MakeAndRegisterAbility / AddAbility）

把构造的SA关联到本地said索引，需要已有Profile并注入属性；不能替代Publish。注册宏内部完成new和该调用。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::AddAbility](../../services/safwk/src/local_ability_manager.cpp#L261)。

## 发布（Publish）

向samgr AddSystemAbility提交业务IRemoteObject与SAExtraProp；返回ERR_OK后才置本地ACTIVE。保存对象引用先于远端调用，失败也可能持有引用。 依据：[services/safwk/src/system_ability.cpp：SystemAbility::Publish](../../services/safwk/src/system_ability.cpp#L77)。

## 运行标记与状态（isRunning_ / abilityState_）

前者记录Start回调流程结束/Stop复位，后者为NOT_LOADED、ACTIVE、IDLE。两者与远端注册均是不同证据。 依据：[services/safwk/src/system_ability.cpp：SystemAbility::Start](../../services/safwk/src/system_ability.cpp#L181)。

## 随进程启动（runOnCreate）

运行Profile决定的SA启动属性；普通进程初始化按阶段加载并尝试启动。构造传入值可被AddAbility从Profile覆盖。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::AddAbility](../../services/safwk/src/local_ability_manager.cpp#L261)。

## 按需加载（run-on-demand）

可先由samgr拉进程并指定said，也可在已运行进程中通过LSA StartAbility装载/启动其他SA。两条路径的依赖检查与初始化步骤不同。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::StartOndemandSystemAbility](../../services/safwk/src/local_ability_manager.cpp#L515)。

## 启动阶段（BootPhase）

进程内任务分组：BootStart=1、CoreStart=2、OtherStart=3。任务等待完成不代表每个SA业务发布成功；下一阶段库初始化可与当前任务执行交叠。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::FindAndStartPhaseTasks](../../services/safwk/src/local_ability_manager.cpp#L813)。

## 依赖等待（dependSa / dependTimeout）

初始启动任务对依赖said执行CheckSystemAbility轮询。合法超时200～60000ms、初值6000ms；越界保持原值，最终缺依赖不调用Start。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::StartDependSaTask](../../services/safwk/src/local_ability_manager.cpp#L694)。

## 运行配置（Profile / SaProfile）

由业务源JSON经构建组织成按进程承载的配置，ParseUtil位于samgr仓；与单SA源文件及Trust文件分别解释。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::InitSystemAbilityProfiles](../../services/safwk/src/local_ability_manager.cpp#L206)。

## 受信集合（Trust）

按process过滤可承载said的集合；当前只有可解析且非空map才执行过滤。缺文件与map缺本process不是同一行为。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::CheckTrustSa](../../services/safwk/src/local_ability_manager.cpp#L237)。

## 空闲延时（OnIdle / delayTime）

业务返回值用于卸载协调；当前本地仅0使ACTIVE→IDLE，正数保持ACTIVE等待后续，非法前态输出-1。 依据：[services/safwk/src/system_ability.cpp：SystemAbility::Idle](../../services/safwk/src/system_ability.cpp#L212)。

## 取消空闲（CancelIdle）

IDLE状态向samgr请求CancelUnloadSystemAbility；非IDLE直接true。返回不是本地OnActive已执行的证据。 依据：[services/safwk/src/system_ability.cpp：SystemAbility::CancelIdle](../../services/safwk/src/system_ability.cpp#L119)。

## 生命周期原因（OnDemandReason）

对触发来源、name、value与extraDataId的值对象；原因map按said保存，后续可覆盖，并非请求队列。缺eventId没有已核实的默认INTERFACE_CALL保证。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::JsonToOnDemandReason](../../services/safwk/src/local_ability_manager.cpp#L607)。

## 附加数据（OnDemandReasonExtraData）

公共事件的code、data及字符串want map快照，按extraDataId向samgr补取；有ID不保证缓存仍有效，取数失败也可能继续回调。 依据：[services/safwk/src/system_ability_ondemand_reason.cpp：OnDemandReasonExtraData::Marshalling](../../services/safwk/src/system_ability_ondemand_reason.cpp#L43)。

## 监听去重状态（INIT / NOTIFIED）

每目标said下监听SA的通知标记，添加通知后NOTIFIED，移除通知再置INIT；用于事件去重，不反映SA生命周期状态。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::FindAndNotifyAbilityListeners](../../services/safwk/src/local_ability_manager.cpp#L422)。

## 长期未使用（unusedTimeout）

配置秒数转换并限制到1～120分钟；非常驻进程的定时查询依据已发布Stub最后IPC访问时间，满足条件请求samgr卸载。不是引用计数或业务健康度。 依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::InitUnusedCfg](../../services/safwk/src/local_ability_manager.cpp#L1101)。

## 接口响应缓存（ApiCacheManager）

Proxy显式接入的请求字节→回复字节缓存，descriptor+apiCode分桶，不自动按远端实例/身份隔离。它与samgr的SA对象查询缓存不同。 依据：[services/safwk/src/api_cache_manager.cpp：ApiCacheManager::PostSendRequest](../../services/safwk/src/api_cache_manager.cpp#L140)。

## TTL与LRU（ExpireLruCache）

TTL按写入时间与GetTickCount毫秒差比较，Get不续TTL；LRU记录最近访问顺序以容量淘汰。expireTimeSec名称不能按秒解释当前实现。 依据：[interfaces/innerkits/safwk/expire_lru_cache.h：class ExpireLruCache](../../interfaces/innerkits/safwk/expire_lru_cache.h#L31)。

## Rust能力与句柄（Ability / Handler）

trait描述业务回调，C++Wrapper承接原生命周期；Handler保存Wrapper裸指针，Clone不会扩展对象生存期或提供线程安全保证。 依据：[interfaces/innerkits/safwk/rust/src/ability.rs：pub struct Handler](../../interfaces/innerkits/safwk/rust/src/ability.rs#L130)。

## 语言桥（CXX / SystemAbilityWrapper）

wrapper.rs生成桥声明，手写C++包装调用SystemAbility。Box/UniquePtr所有权转移与借用parcel是需要单独核查的边界。 依据：[interfaces/innerkits/safwk/rust/src/cxx/system_ability_wrapper.cpp：SystemAbilityWrapper::SystemAbilityWrapper](../../interfaces/innerkits/safwk/rust/src/cxx/system_ability_wrapper.cpp#L28)。

## 服务命令（svc / OnSvcCmd）

只识别wifi、bluetooth、nearlink和help的工具；子命令经LSA交业务，不等于通用SA管理或init启停。 依据：[svc/src/svc_control.cpp：SvcControl::ParseCmd](../../svc/src/svc_control.cpp#L148)。

## 诊断会话与快照（FFRT statistics / dump）

统计需按START/STOP/GET管理状态，GET消费结果；即时dump通过动态符号读取，不与统计会话混用。 依据：[services/safwk/src/local_ability_manager_dumper.cpp：LocalAbilityManagerDumper::CollectFfrtStatistics](../../services/safwk/src/local_ability_manager_dumper.cpp#L227)。

## 可选启动入口（StartSA / sa_start）

特性开关决定是否引入导出StartSA的共享库；外部启动/fork调度不在本仓实现，不代表默认替代sa_main。 依据：[services/safwk/src/system_ability_start.cpp：StartSA](../../services/safwk/src/system_ability_start.cpp#L95)。

## 管理权限与接口令牌

LSA先验证专用令牌，再按管理、扩展、svc事务选择AccessToken权限；不是业务IRemoteObject的接口描述符与业务授权。 依据：[services/safwk/src/local_ability_manager_stub.cpp：LocalAbilityManagerStub::CheckPermission](../../services/safwk/src/local_ability_manager_stub.cpp#L72)。

## 独立卸载（independentUnload）

Profile启用后，LSA在Stop后继续执行移除本地索引、等待发布对象强引用、清引用、CloseSo与按需信息重登记，目标是SA级资源清理而不是无条件结束整个进程。当前引用等待没有显式超时。依据：[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::OnStopAbility](../../services/safwk/src/local_ability_manager.cpp#L471)。

## subfoundation承载配置

本仓新增的一组独立进程启动、Trust和权限配置，使用自己的uid及运行Profile，相关通知数据目录包含属主迁移。它是部署边界，不是另一个LocalAbilityManager实现。依据：[etc/profile/subfoundation/subfoundation.cfg："services"](../../etc/profile/subfoundation/subfoundation.cfg#L15)。

## 文件描述符所有权标记（FDSAN tag）

LSA为接收的IPC统计、FFRT统计和svc控制fd分别设置owner tag，并使用相同tag关闭，以核对资源所有权；不能再按裸close路径描述当前实现。依据：[services/safwk/src/local_ability_manager_stub.cpp：FDSAN_TAG_IPC_STAT](../../services/safwk/src/local_ability_manager_stub.cpp#L43)。
