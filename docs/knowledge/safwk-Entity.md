# 业务实体知识

safwk核心业务实体的定义、状态、关系与代码映射。整体职责见[业务背景](business-context.md)，时序与变更验收见[技术机制](safwk-Technology.md)；以下状态按当前实现解释。

## 系统服务实例（SystemAbility）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 系统服务实例（SystemAbility） |
| 实体定义 | 一个said对应的业务生命周期对象；其业务IRemoteObject可以与该C++对象不同。进程内登记后仍需Publish才进入全局查询体系。 |
| 核心特征 | saId_为关联键；abilityState_初始NOT_LOADED，isRunning_初始false；Profile覆盖runOnCreate、libPath、dependSa/timeout、distributed、dumpLevel；publishObj_持有已提交对象。 |
| 类型/分类 | 本地三态NOT_LOADED/ACTIVE/IDLE；另按是否runOnCreate区分启动方式，不能把此分类当不同C++基类。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | 宏或Rust入口创建→AddAbility→框架Start→业务OnStart→Publish→Idle/Active/Stop。RemoveAbility只删map，不负责delete或samgr移除；独立卸载时该调用是清引用与CloseSo之前的步骤。 |
| 状态流转 | Publish返回ERR_OK后ACTIVE；Start尾置isRunning=true，即使未Publish；Idle返回delayTime=0才置IDLE；Active回调后ACTIVE；Stop回调后先置NOT_LOADED/false再请求远端移除；OnStopAbility随后可按配置执行独立卸载和按需重登记。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | 拒绝在非NOT_LOADED状态Start；Stop在NOT_LOADED直接返回。Publish先保留publishObj_再访问samgr，失败仍可能持有对象；普通Stop本身不清空publishObj_；若Profile.independentUnload=true，OnStopAbility随后执行清引用、CloseSo与按需信息重登记。不要把OnStartFail视为自动回滚。 |
| 并发约束 | Start受onStartLock_串行；abilityLock为递归锁，回调执行区域与状态读写锁边界不同。GetRunningStatus直接返回标记，不可推断其提供完整跨线程快照。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | LocalAbilityManager.localAbilityMap_保存裸指针；初始阶段表也保存该指针。业务SA实现回调和Stub。 |
| 下游影响 | 影响samgr注册、加载通知、监听和unused查询；状态与远端注册可能暂时/失败后不一致。 |
| 平级关联 | 与Profile按said关联；与原因对象在各生命周期调用时关联；与Rust wrapper形成继承/代理。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [interfaces/innerkits/safwk/system_ability.h：class SystemAbility](../../interfaces/innerkits/safwk/system_ability.h#L53)；[services/safwk/src/system_ability.cpp：SystemAbility::Start](../../services/safwk/src/system_ability.cpp#L181) |
| 核心方法 | MakeAndRegisterAbility/Publish/Start/Idle/Active/Stop/OnStartFail/CancelIdle分别承担本地登记、发布、转换及向samgr请求。 |

## 承载进程管理器（LocalAbilityManager）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 承载进程管理器（LocalAbilityManager） |
| 实体定义 | 每个SA进程中的组织者与samgr可调用的控制对象；它不是全局注册表。 |
| 核心特征 | procName_、profileParser_、localAbilityMap_、abilityPhaseMap_、原因/监听表、初始化池和idleTimer_共同描述进程内服务。 |
| 类型/分类 | 按进程实例存在；同一进程内多个said共享一组管理设施。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | 单例构造解析器和线程池；启动后AddSystemProcess发布LSA对象；之后承接控制；析构只显式StopTimedQuery。 |
| 状态流转 | 没有定义完整manager状态枚举；初始化失败返回、不进入JoinWorkThread；成功运行后主线程加入IPC。不能根据单例已创建判断已注册成功。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | AddAbility要求非空、Profile存在及said不重复；GetAbility返回空意味着本地对象不可得。ClearResource仅转给Profile解析器，不是清空所有map和远端服务；独立卸载另走OnStopAbility的专门清理链。 |
| 并发约束 | 本地map读写shared_mutex，监听mutex，原因mutex，阶段计数mutex各自保护；返回裸指针不延长对象寿命。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | sa_main/StartSA驱动初始化，samgr通过AddSystemProcess得到远端对象。 |
| 下游影响 | 组织SA执行、依赖、监听和unused；某SA阻塞可影响同进程其他服务。 |
| 平级关联 | 业务实例、Profile、任务、监听与原因都以said或进程名与其关联。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [services/safwk/include/local_ability_manager.h：class LocalAbilityManager](../../services/safwk/include/local_ability_manager.h#L46)；[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::AddLocalAbilityManager](../../services/safwk/src/local_ability_manager.cpp#L885) |
| 核心方法 | DoStartSAProcess连接初始化主链；Run注册控制端并安排启动；GetAbility/RemoveAbility负责本地索引；ClearResource只处理解析资源。 |

## 运行配置与受信集合（SaProfile / Trust）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 运行配置与受信集合（SaProfile / Trust） |
| 实体定义 | 描述进程名与SA部署/启动属性的运行配置；Trust是在加载前过滤可承载said的可选材料。结构与解析器由samgr仓提供。 |
| 核心特征 | process、saId、libPath、runOnCreate、bootPhase、dependSa、dependTimeout、independentUnload和stopOnDemand.unusedTimeout进入不同控制路径。 |
| 类型/分类 | 区分单SA源JSON、构建合并后的运行Profile、按进程命名的Trust JSON；不是同一格式限制。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | ParseSaProfiles→GetProcessName/GetAllSaProfiles→可用Trust过滤→按需或首阶段LoadSaLib/OpenSo→AddAbility注入属性。 |
| 状态流转 | 解析失败阻止启动；过滤从解析器移除未允许said，后续登记也会受影响。不存在通用“配置已读即SA已加载”状态。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | Trust路径为/system/profile/<process>_trust.json；仅存在且解析成功、map非空时按集合删除。文件缺失、解析失败/空map与有效map缺该process的行为不同。dependTimeout越界保留此前值，默认6000ms；不是clamp。 |
| 并发约束 | Profile初始化与dlopen静态注册紧密关联；不要在回调/线程中随意ClearResource或重建解析器。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | init进程配置和构建生成规则决定运行文件；LSA持有解析器。 |
| 下游影响 | 决定对象是否可登记、按何阶段启动、依赖等待和unused策略。 |
| 平级关联 | SystemAbility构造参数是初始输入，AddAbility用Profile覆盖部分属性；运行配置与业务实现said须一致。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::InitSystemAbilityProfiles](../../services/safwk/src/local_ability_manager.cpp#L206)；[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::CheckTrustSa](../../services/safwk/src/local_ability_manager.cpp#L237)；相邻samgr的interfaces/innerkits/common/include/sa_profiles.h、services/common/src/parse_util.cpp。 |
| 核心方法 | InitializeOnDemandSaProfile/InitializeRunOnCreateSaProfiles选择加载；CheckTrustSa过滤；AddAbility赋值；SystemAbility::SetDependTimeout验证范围。 |

## 启动阶段与依赖任务（abilityPhaseMap_ / SaInit）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 启动阶段与依赖任务（abilityPhaseMap_ / SaInit） |
| 实体定义 | 承载进程首次启动期间的任务组织单元，将已登记SA按bootPhase分组，并在每个任务内等待其依赖可查询。 |
| 核心特征 | bootPhase=1/2/3；startTaskNum_跟踪待完成数，ThreadPool的线程数来自hardware_concurrency，任务上限配置20。 |
| 类型/分类 | BootStart、CoreStart、OtherStart；普通启动与指定said启动走不同初始化路径。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | 建阶段表→StartPhaseTasks入池并计数→StartSystemAbilityTask→CheckDependencyStatus/Start→减计数通知→WaitForTasks。 |
| 状态流转 | 任务完成计数归零，或阶段等待100秒后清零；不代表所有业务Publish成功。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | 依赖为有效said且CheckSystemAbility非空才视作就绪；无效依赖ID被跳过。50ms轮询至依赖超时，最终仍缺依赖则不执行Start。当前后续StartOndemandSystemAbility直接调用Start，不复用该依赖等待器。 |
| 并发约束 | 阶段计数以startPhaseLock_+CV同步；普通启动会在本阶段任务执行时初始化下一阶段库。跨阶段依赖或慢回调必须分析等待关系。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | LSA持有阶段表和线程池；Profile提供阶段与依赖列表。 |
| 下游影响 | 决定首次回调时序；下一阶段是否开始不能证明上一阶段全部发布成功。 |
| 平级关联 | SA实例、依赖远端对象和watchdog共同参与；没有完整拓扑排序或环检测实体。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::StartDependSaTask](../../services/safwk/src/local_ability_manager.cpp#L694)；[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::FindAndStartPhaseTasks](../../services/safwk/src/local_ability_manager.cpp#L813) |
| 核心方法 | StartPhaseTasks/StartSystemAbilityTask/WaitForTasks安排、执行与收尾；CheckDependencyStatus返回尚不可用的依赖集合。 |

## 生命周期原因（SystemAbilityOnDemandReason）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 生命周期原因（SystemAbilityOnDemandReason） |
| 实体定义 | 把控制事件的来源、名称、值和附加数据标识传递给业务回调，支持相同SA按不同触发原因处理。 |
| 核心特征 | reasonId_、reasonName_、reasonValue_、extraDataId_和extraData_；extraDataId默认-1。C++默认构造没有显式初始化reasonId_，不能编造缺省INTERFACE_CALL保证。 |
| 类型/分类 | INTERFACE_CALL/DEVICE_ONLINE/SETTING_SWITCH/PARAM/COMMON_EVENT/TIMED_EVENT/UNREF_EVENT；Rust反序列化当前只接受0～5。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | argv事件或LSA JSON→按said保存start/stop原因→JsonToOnDemandReason按字段类型赋值→回调前必要时取extraData。 |
| 状态流转 | 不是全局SA状态；同said原因可被后续请求覆盖，GetStartReason/GetStopReason用map下标读取可创建空项。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | JSON字段必须以正确类型传入，缺字段保留当前构造结果而不是自动补完整业务默认；HasExtraData只检查COMMON_EVENT，不能以ID非负替代。 |
| 并发约束 | 原因map在ReasonLock_下读写，但读取所得快照不保证请求与执行一一绑定；同said快速重复请求须关注覆盖时机。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | LSA保存原因，启动参数和samgr控制请求是上游。 |
| 下游影响 | 决定业务OnStart/OnStop/OnIdle/OnActive的上下文及额外数据请求。 |
| 平级关联 | 附加数据通过extraDataId关联samgr缓存；Rust wrapper按值复制reason。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [services/safwk/include/system_ability_ondemand_reason.h：class SystemAbilityOnDemandReason](../../services/safwk/include/system_ability_ondemand_reason.h#L48)；[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::JsonToOnDemandReason](../../services/safwk/src/local_ability_manager.cpp#L607) |
| 核心方法 | SetStartReason/SetStopReason保存；JsonToOnDemandReason转换；HasExtraData决定是否补取；GetOnDemandReasonExtraData补入数据。 |

## 公共事件附加数据（OnDemandReasonExtraData）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 公共事件附加数据（OnDemandReasonExtraData） |
| 实体定义 | 跨进程传递事件code、data和字符串want map的Parcelable快照；它不是任意类型Want对象。 |
| 核心特征 | 默认code=-1，data字符串和map；序列化写code、data、want的JSON字符串。 |
| 类型/分类 | 与COMMON_EVENT配套的数据载荷；其他原因不由HasExtraData自动补取。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | safwk依据extraDataId向samgr取parcel→ReadParcelable分配对象→复制到reason→delete；Rust桥转成键值交替的Vec。 |
| 状态流转 | 取数失败或解码失败时回调流程仍继续，不能把未取到数据说成生命周期被拒绝。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | want只保留字符串项；按当前Marshalling/Unmarshalling顺序配对。C++读出对象由调用方释放；Rust Deserialize桥读取失败可返回空默认载荷，而外层仍Ok。 |
| 并发约束 | 按值快照；借用parcel/桥指针只在本次调用有效，不能跨异步任务持有引用。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | SystemAbility在回调前补取；samgr维护源缓存及extraDataId有效期。 |
| 下游影响 | 业务条件判断与Rust字段映射；缺字段/缓存失效不能默默当作真实空事件。 |
| 平级关联 | 生命周期reason携带ID与已取快照，Rust通过交替key/value列表映射map。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [services/safwk/src/system_ability_ondemand_reason.cpp：OnDemandReasonExtraData::Marshalling](../../services/safwk/src/system_ability_ondemand_reason.cpp#L43)；[services/safwk/src/system_ability.cpp：SystemAbility::GetOnDemandReasonExtraData](../../services/safwk/src/system_ability.cpp#L153) |
| 核心方法 | Marshalling/Unmarshalling约定线格式；SetExtraData复制；BuildReasonWrapper及Serialize/Deserialize桥负责语言转换。 |

## 本地服务监听关系（localListenerMap_）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 本地服务监听关系（localListenerMap_） |
| 实体定义 | 以被监听said分桶，把samgr一次远端订阅复用给本进程多个监听SA。 |
| 核心特征 | 目标said→(listenerSaId, ListenerState)列表，状态为INIT或NOTIFIED；共享statusChangeListener_对象。 |
| 类型/分类 | 添加通知去重；移除通知会向当前监听项分发并重置NOTIFIED。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | 业务AddSystemAbilityListener→检查/去重并加入本地列表→必要时远端Subscribe；状态回调选出目标，解锁后NotifyAbilityListener；最后一项移除再UnSubscribe。 |
| 状态流转 | INIT收到添加变NOTIFIED；再添加被抑制；移除后回INIT。首次订阅失败可能保留本地项，不能把false当无副作用。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | 两个said均须合法；同listener ID去重。已有桶内新增监听者可能先CheckSystemAbility并直接通知；监听SA若本地查不到，不执行业务钩子。 |
| 并发约束 | listenerLock_保护表和共享listener创建；选择通知目标后解锁调用业务，减少持锁重入。不能用表锁推出监听对象一直存活。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | SystemAbility用自身said作为listenerSaId；samgr负责远端订阅和原始通知。 |
| 下游影响 | 控制业务依赖出现/消失的反应，不等于启动前依赖等待；错误残留会影响后续重试/去重。 |
| 平级关联 | 监听SA实例与被监听远端服务是两个角色。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::AddSystemAbilityListener](../../services/safwk/src/local_ability_manager.cpp#L302)；[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::FindAndNotifyAbilityListeners](../../services/safwk/src/local_ability_manager.cpp#L422) |
| 核心方法 | Add/RemoveSystemAbilityListener维护两层订阅；SystemAbilityListener转发；NotifyAbilityListener调用业务OnAdd/OnRemove。 |

## LSA控制事务（SafwkInterfaceCode / MessageParcel）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | LSA控制事务（SafwkInterfaceCode / MessageParcel） |
| 实体定义 | samgr或工具向进程内LSA发出的管理协议，独立于SA向客户端提供的业务协议。声明与码位于samgr仓。 |
| 核心特征 | code、LSA接口令牌、调用token、said和对应payload；诊断/svc事务还携带fd。 |
| 类型/分类 | Start/Stop/Idle/Active/策略、IPC统计、FFRT统计/快照、扩展、svc共10项分发。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | OnRemoteRequest→令牌→CheckPermission→memberFuncMap_→解包与参数检查→manager/业务回调→reply或状态。 |
| 状态流转 | 传输status、reply布尔/延时与最终业务完成分开。Start/Stop Inner不写业务bool；Active/Idle写reply，manager返回true不证明内部状态守卫发生转换。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | 扩展权限ACCESS_EXT_SYSTEM_ABILITY，svc权限CONTROL_SVC_CMD，其他事务MANAGE_SYSTEM_ABILITY；完整前缀均为ohos.permission。令牌错与权限拒绝均可ERR_PERMISSION_DENIED。 |
| 并发约束 | 同步分发可直接进入业务钩子；Start/Stop再创建独立任务。跨模块传入裸指针/parcel不可延长至异步执行。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | samgr的ILocalAbilityManager Proxy和svc使用方构造payload。 |
| 下游影响 | 影响业务生命周期、诊断和扩展；fd在接收侧有效出口均应关闭，不等于调用端fd失效。 |
| 平级关联 | LSA对象承担管理面，业务IRemoteObject是数据面；权限不能互相替代。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [services/safwk/src/local_ability_manager_stub.cpp：LocalAbilityManagerStub::OnRemoteRequest](../../services/safwk/src/local_ability_manager_stub.cpp#L89)；[services/safwk/include/local_ability_manager_stub.h](../../services/safwk/include/local_ability_manager_stub.h#L1) |
| 核心方法 | CheckPermission/EnforceInterceToken建立门禁；各Inner明确读写顺序；SystemAbilityExtProc只在调用期间借用data_/reply_。 |

## 长期未使用策略（unusedCfgMap_ / idleTimer_）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 长期未使用策略（unusedCfgMap_ / idleTimer_） |
| 实体定义 | 对满足条件的按需SA按最后IPC请求时间提出卸载建议，不在定时回调中直接Stop业务。 |
| 核心特征 | said→超时毫秒表、Timer及ondemandTimer_；Profile中unusedTimeout以秒转毫秒后限制到1～120分钟。 |
| 类型/分类 | 只接入runOnCreate=false且unusedTimeout!=-1的配置；若当前已登记对象中有runOnCreate SA，则IsResident阻止开启该查询。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | DoStartSAProcess运行后StartTimedQuery→InitUnusedCfg/StartOnDemandTimer→每分钟IdentifyUnusedOndemand→取发布Stub最后请求→向samgr UnloadSystemAbility。 |
| 状态流转 | 超时仅发起卸载；返回值未被用于停止或删除配置，后续周期仍可再次请求。StopTimedQuery注销timer并Shutdown。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | 获取不到对象/发布指针、当前时间不大于lastRequestTime则跳过；比较为idleTime>threshold。NoNeedCheckUnused当前白名单为空；IsResident依据localAbilityMap_已登记对象，不是遍历所有Profile推断。 |
| 并发约束 | 定时回调与生命周期并发；GetAbility出锁后的裸指针、reinterpret_cast到IPCObjectStub及跨层时间基准都需核查。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | LSA持有定时器，Profile给策略，IPC对象给访问时间。 |
| 下游影响 | samgr再做卸载状态协调，不能把定时器输出等同卸载完成。 |
| 平级关联 | 与isRunning_/abilityState_并非同一指标；业务长任务不一定产生新的IPC访问时间。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::IdentifyUnusedOndemand](../../services/safwk/src/local_ability_manager.cpp#L1132)；[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::StartTimedQuery](../../services/safwk/src/local_ability_manager.cpp#L1177) |
| 核心方法 | LimitUnusedTimeout换算；GetSaLastRequestTime取Stub；Start/StopTimedQuery控制回调；IdentifyUnusedOndemand请求卸载。 |

## 可缓存接口（ApiCacheManager.caches_）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 可缓存接口（ApiCacheManager.caches_） |
| 实体定义 | 对(descriptor, apiCode)建立独立请求/回复缓存桶，是Proxy主动接入的性能设施。 |
| 核心特征 | descriptor与uint32 apiCode构成接口键；每桶默认容量8，值是ExpireLruCache的裸指针，由DelCacheApi释放。 |
| 类型/分类 | AddCacheApi登记、PreSendRequest查缓存、PostSendRequest写缓存；ClearCache有全部/descriptor/单API范围。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | 登记→发送前按完整data字节查→命中将value字节追加到reply→未命中由调用者真实发送→调用者决定是否写入→失效清理/注销。 |
| 状态流转 | 重复登记不更新已有桶的TTL；ClearCache只清内容保留桶；DelCacheApi删除整个接口注册。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | PostSendRequest拒绝data或reply有对象offset的情况；不会检查业务错误码，也不自动监听服务死亡。调用者需筛选回复、在写请求前形成稳定key并定义失效时机。 |
| 并发约束 | cachesMutex_保护注册表和一次操作；内部LRU另有mutex。多线程调用可加锁不等于跨调用Pre→真实IPC→Post整体原子。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | 业务Proxy选择可缓存API；其协议变化会改变字节键和兼容性。 |
| 下游影响 | 命中跳过远端执行会改变调用链；不适合有副作用、隐含身份或强实时语义的操作。 |
| 平级关联 | 桶包含多个缓存项；descriptor隔离接口而不是自动隔离不同远端实例/用户。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [interfaces/innerkits/safwk/api_cache_manager.h](../../interfaces/innerkits/safwk/api_cache_manager.h#L1)；[services/safwk/src/api_cache_manager.cpp：ApiCacheManager::PreSendRequest](../../services/safwk/src/api_cache_manager.cpp#L105) |
| 核心方法 | Add/DelCacheApi管理桶；Pre/PostSendRequest查写；ClearCache按范围失效。 |

## 带过期时间的缓存项（ExpireLruCache）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 带过期时间的缓存项（ExpireLruCache） |
| 实体定义 | 同一接口桶内按请求字节关联回复字节，并维护最近使用顺序与写入时间。 |
| 核心特征 | data_、timestamp_与keys_三份结构应同步；TValue以shared_ptr返回；写入时记录GetTickCount毫秒。 |
| 类型/分类 | 命中、缺失、过期；TTL>0启用时间淘汰，0不按时间过期，负数构造时变为1000。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | Add刷新值及时间并移动到前端；Get命中只移动LRU，不续TTL；过期Get删项；满容量Add优先清过期，否则淘汰尾部。 |
| 状态流转 | 过期判定diff>interval；等于阈值尚不过期。Clear/Remove同步移除三结构；capacity=0转成1。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | 构造参数名expireTimeSec易误导，直接赋expireTimeMilliSec_且无乘1000，当前有效单位是毫秒；不是滑动过期。 |
| 并发约束 | lock_保护内部一致性；shared_ptr使已返回值可在移除后继续被调用者持有，不代表缓存项仍有效。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | ApiCacheManager为每个接口创建一个该模板实例。 |
| 下游影响 | 影响数据新鲜度、命中率和跨业务变更后的旧回复可见性。 |
| 平级关联 | LRU与TTL是独立淘汰条件；不能用“被频繁Get”解释TTL续期。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [interfaces/innerkits/safwk/expire_lru_cache.h：class ExpireLruCache](../../interfaces/innerkits/safwk/expire_lru_cache.h#L31)；[test/services/safwk/unittest/expire_lru_cache_test.cpp：ConstructorTest001](../../test/services/safwk/unittest/expire_lru_cache_test.cpp#L72) |
| 核心方法 | DoAdd/DoGet/DoClearExpiredCache维护值/时间/LRU；Timestamp::IsExpired比较毫秒差；测试ConstructorTest001断言默认与负值处理。 |

## Rust服务适配与句柄（Ability / SystemAbilityWrapper / Handler）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | Rust服务适配与句柄（Ability / SystemAbilityWrapper / Handler） |
| 实体定义 | 将Rust trait实现纳入C++ SystemAbility生命周期，借Handler提供发布、监听、取消空闲入口。 |
| 核心特征 | AbilityWrapper持Box<dyn Ability>；C++Wrapper保留Box转移出的指针；Handler可Clone但只保存*mut SystemAbilityWrapper。 |
| 类型/分类 | 业务Ability、生命周期Wrapper、已发布RemoteServiceStub分别承担不同角色；句柄Clone不是引用计数所有权。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | build_system_ability→UniquePtr包装→register(self)把指针移交RegisterAbility；框架回调进入trait；publish再把Rust RemoteStub装入C++远端Stub。 |
| 状态流转 | 注册包装的Rust方法无返回值并忽略底层bool；Wrapper析构通过Box::from_raw回收Rust对象。失败路径和对象寿命需单独审查，不能假设register成功即Publish。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | Handler使用Pin::new_unchecked解引用裸指针，必须在Wrapper有效期内；该接口不自动提供Send/Sync保证。理由枚举/字段和Parcelable转换也有跨语言差异。 |
| 并发约束 | C++框架决定调用线程，trait中的&self不等于回调串行；Rust内部状态必须按真实执行模型组织。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | 业务Rust库初始化数组示例负责构建和register；CXX生成桥连接手写包装。 |
| 下游影响 | 回到相同samgr发布/状态链；额外承担指针、fd、parcel借用和错误映射边界。 |
| 平级关联 | Handler依赖Wrapper，RemoteServiceStub持有另一份Box，不能复用同一raw指针形成多个所有者。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [interfaces/innerkits/safwk/rust/src/ability.rs：pub struct Handler](../../interfaces/innerkits/safwk/rust/src/ability.rs#L130)；[interfaces/innerkits/safwk/rust/src/exts.rs：pub fn register](../../interfaces/innerkits/safwk/rust/src/exts.rs#L47)；[interfaces/innerkits/safwk/rust/src/cxx/system_ability_wrapper.cpp：SystemAbilityWrapper::PublishWrapper](../../interfaces/innerkits/safwk/rust/src/cxx/system_ability_wrapper.cpp#L50) |
| 核心方法 | BuildSystemAbility/RegisterAbility构造登记；PublishWrapper发布；各On*包装转换回调；析构from_raw恢复并释放所有权。 |

## 服务命令请求（SvcControl / ServiceControlCmd）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 服务命令请求（SvcControl / ServiceControlCmd） |
| 实体定义 | 以限定顶层服务名称定位said，再向该进程LSA转发输出fd与业务子命令的工具请求。 |
| 核心特征 | wifi=1120、bluetooth=1130、nearlink=1190，help本地结束；参数数目2～64、每项1～256字符。 |
| 类型/分类 | 本地help/输入错误；转发给业务OnSvcCmd的命令。enable/disable等子命令语义由目标业务决定。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | Main→Parse→取samgr→GetLocalAbilityManagerProxy；空则LoadSystemAbility(saId,4)后再查→ServiceControlCmd→业务OnSvcCmd→fd输出。 |
| 状态流转 | 工具返回成功说明该转发链返回NO_ERROR，不能证明设备已达到目标状态；真实硬件/服务状态须由业务提供。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | 不支持任意said。Stub验证CONTROL_SVC_CMD权限和fd，读取args失败也关闭fd；业务不得异步保存已关闭的接收fd。 |
| 并发约束 | 命令处理同步进入业务钩子，阻塞会占用控制IPC执行资源。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | shell使用svc；samgr提供承载进程LSA代理与必要的加载。 |
| 下游影响 | 由业务SA执行控制及输出；框架默认OnSvcCmd=-1。 |
| 平级关联 | 与通用Dump、init进程启停及LSA Start/Stop是不同路径。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [svc/include/svc_control.h](../../svc/include/svc_control.h#L1)；[svc/src/svc_control.cpp：SvcControl::Main](../../svc/src/svc_control.cpp#L55)；[services/safwk/src/local_ability_manager_stub.cpp：LocalAbilityManagerStub::ServiceControlCmdInner](../../services/safwk/src/local_ability_manager_stub.cpp#L348) |
| 核心方法 | Parse/ParseCmd检查顶层；SetCmdArgs保留剩余参数；ServiceControlCmd定位本地SA并转OnSvcCmd。 |

## 进程诊断会话（LocalAbilityManagerDumper）

### 实体概念

| 子维度 | 内容 |
|---|---|
| 实体名称 | 进程诊断会话（LocalAbilityManagerDumper） |
| 实体定义 | 在当前SA进程采集IPC统计或FFRT运行信息，包含有状态的统计开始/停止/读取过程。 |
| 核心特征 | FFRT缓冲区、collectEnable、ffrtMetricLock、handler_和清理任务名；与即时FFRT快照的dlsym路径不同。 |
| 类型/分类 | IPC统计；FFRT统计START/STOP/GET；FFRT即时dump。 |

### 上下文与场景

| 子维度 | 内容 |
|---|---|
| 交互流程 | Stub接收命令→manager调用Dumper→写fd/字符串→关闭接收fd；FFRT START分配缓冲并投递延时清理，STOP停采集，GET解析并清缓冲。 |
| 状态流转 | 采集中GET失败、未采集/无缓冲GET失败；GET成功消费缓冲，不能反复读取同一结果。超时清理可停止采集并清数据。 |

### 规格与约束

| 子维度 | 内容 |
|---|---|
| 业务规则 | 统计cmd范围及缓冲布局随ffrt接口；FfrtDumperProc符号缺失写说明并false，而Stub仍可返回承载该说明的字符串。传输成功不是采集成功。 |
| 并发约束 | 统计状态在ffrtMetricLock下操作；FFRTHandler的同名任务覆盖句柄不意味着旧任务自动取消。 |

### 知识关联

| 子维度 | 内容 |
|---|---|
| 上层依赖 | samgr hidumper聚合调用本进程LSA，调用者给输出fd；权限由Stub检验。 |
| 下游影响 | 诊断文本帮助确定耗时/阻塞，但采集本身会改变会话状态，缺结果不证明业务未执行。 |
| 平级关联 | FFRT清理任务与会话缓冲生存期关联；IPCPayloadStatistics是另一组外部统计接口。 |

### 代码与符号

| 子维度 | 内容 |
|---|---|
| 核心类 | [services/safwk/src/local_ability_manager_dumper.cpp：LocalAbilityManagerDumper::CollectFfrtStatistics](../../services/safwk/src/local_ability_manager_dumper.cpp#L227)；[services/safwk/src/ffrt_handler.cpp：FFRTHandler::PostTask](../../services/safwk/src/ffrt_handler.cpp#L34) |
| 核心方法 | Start/Stop/GetFfrtStatistics和ClearFfrtStatistics管理会话；FfrtStatisticsParser解析结构；IpcStatCmdProc写输出。 |

## 独立卸载对既有实体关系的补充

独立卸载不是新的全局状态枚举：Profile中的`independentUnload`改变本地SA对象、发布对象强引用、动态库句柄及samgr按需信息之间的清理顺序。[services/safwk/src/local_ability_manager.cpp：LocalAbilityManager::OnStopAbility](../../services/safwk/src/local_ability_manager.cpp#L471)与[services/safwk/src/system_ability.cpp：SystemAbility::ClearResourceForIndependentUnload](../../services/safwk/src/system_ability.cpp#L482)是对应入口。`localAbilityMap_`移除后，后续GetAbility可能已空，但清引用仍在等待；samgr的按需信息重登记在CloseSo调用之后，不能将“本地不可见”视为“库已卸载且可以再加载”。
