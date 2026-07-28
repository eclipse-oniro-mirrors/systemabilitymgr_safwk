# AGENT.md — safwk (System Ability Framework)

> Path: `foundation/systemabilitymgr/safwk` | Subsystem: `systemabilitymgr` | Standard system only
>
> **Maintenance rule**: When modifying code via agent, if new code structures or features are added, this document must be updated synchronously.

## Code Map

```
interfaces/innerkits/safwk/
  system_ability.h                # SystemAbility base class — ALL SAs inherit this
  system_ability_ondemand_reason.h  # OnDemand reason struct (start/idle/active/stop)
  api_cache_manager.h             # IPC response cache (ExpireLruCache-backed)
  expire_lru_cache.h              # TTL + LRU cache template
  rust/                           # Rust bindings (system_ability_fwk_rust)
services/safwk/
  src/main.cpp                    # sa_main entry — parses args, loads profile, starts SAs
  src/local_ability_manager.cpp   # LSA singleton — manages all SAs in this process
  src/local_ability_manager_stub.cpp  # IPC stub — receives control cmds from samgr
  src/system_ability.cpp          # SystemAbility impl — Publish(), lifecycle dispatch
  src/system_ability_start.cpp    # saspawn fork optimization (feature-gated off by default)
  include/local_ability_manager.h # LSA class definition
  include/ffrt_handler.h          # FFRT task wrapper
svc/                              # svc CLI tool — sends IPC commands to SAs (dump etc.)
etc/profile/
  foundation.cfg                  # foundation process init config
  foundation_trust.json           # Trusted SA list for foundation process
test/
  services/safwk/unittest/listen_ability/  # Complete SA example (study this first)
  mock/common/                    # Mock SA libs (demo_sa, ondemand_ability, connect_ability...)
```

| Build target | Type | Output |
|---|---|---|
| `services/safwk:sa_main` | executable | `/system/bin/sa_main` (SA process entry) |
| `interfaces/innerkits/safwk:system_ability_fwk` | shared lib | `libsystem_ability_fwk.z.so` (SA dev framework) |
| `interfaces/innerkits/safwk:system_ability_ondemand_reason` | static lib | OnDemand reason serialization |
| `interfaces/innerkits/safwk:api_cache_manager` | shared lib | API cache manager |
| `svc:svc` | executable | `/system/bin/svc` (SA debug tool) |

## Knowledge Routing

| Problem | Read first |
|---|---|
| Develop a new SA | `system_ability.h` → `test/services/safwk/unittest/listen_ability/` (full example) |
| SA registration macro | `system_ability.h` → `REGISTER_SYSTEM_ABILITY_BY_ID` macro |
| sa_main startup flow | `src/main.cpp` → `LocalAbilityManager::DoStartSAProcess` |
| SA lifecycle callbacks | `system_ability.h` (OnStart/OnStop/OnIdle/OnActive) → `system_ability.cpp` |
| Profile JSON format | samgr: `sa_profiles.h` (SaProfile struct) |
| Dependency SA mechanism | `local_ability_manager.cpp` → `StartDependSaTask` |
| SA status change notification | `local_ability_manager.cpp` → `FindAndNotifyAbilityListeners` |
| BootPhase parallel startup | `local_ability_manager.cpp` → `StartPhaseTasks` |
| IPC stub (cmds from samgr) | `local_ability_manager_stub.cpp` (StartAbility/StopAbility/IdleAbility/ActiveAbility) |
| svc tool usage | `svc/src/svc_control.cpp` |
| API cache | `api_cache_manager.cpp` |
| saspawn feature | `config.gni` → `system_ability_start.cpp` |

**SA lifecycle state machine**: `NOT_LOADED → ACTIVE → IDLE → (CancelIdle→ACTIVE | stop)`. Callbacks: `OnStart`→ACTIVE, `OnIdle`→IDLE (returns delayTime ms), `OnActive`→ACTIVE, `OnStop`→terminate.

**SA dev four-file pattern**: (1) SA impl (.cpp, inherits SystemAbility + REGISTER macro), (2) profile JSON (one SA per file), (3) `ohos_sa_profile` BUILD.gn, (4) process .cfg (init starts via `sa_main /system/profile/xxx.json`).

## Expert Constraints

1. **Must use REGISTER_SYSTEM_ABILITY_BY_ID macro** — never `new` an SA directly. Macro runs at static init.
2. **OnStart must call Publish(this)** — otherwise the SA is invisible to other processes.
3. **Process name triple-match**: .cfg service name = profile JSON `process` field = sa_main-derived name. Mismatch = ondemand load fails silently.
4. **Profile JSON: one systemability node per file** — multiple causes build failure.
5. **dependTimeout range**: [200ms, 60000ms], default 6000ms. Out-of-range values are clamped.
6. **Don't call GetSystemAbility in SA constructor** — samgr may not be ready. Use OnStart.
7. **OnStart must not block** — blocks entire process startup, cascading dependency timeouts.
8. **OnIdle must return a meaningful delayTime** — samgr uses it to schedule unload.
9. **SA .so naming**: `libXXX.z.so` — dlopen in sa_main expects this pattern.
10. **safwk → samgr one-way compile dependency**: safwk links `samgr:samgr_proxy` + `samgr:samgr_common`. samgr links safwk's `system_ability_ondemand_reason` static lib. This cycle is intentional.
11. **innerapi visibility**: `system_ability_fwk` tagged `platformsdk_indirect` + `sasdk`.
12. **foundation_trust.json**: Security gate — only listed SAs can load in foundation process.
13. **AccessToken**: `safwk_support_access_token` gates caller verification (defaults true when access_token component present).

**Anti-patterns**: Manual `new` of SA · Forgetting Publish() in OnStart · Blocking in OnStart · Profile with multiple systemability nodes · Unmatched process names across .cfg/profile · Using std::thread instead of FfrtHandler.

## Build & Test

```bash
# Build safwk component
./build.sh --product-name {product} --build-target safwk

# Build specific target
./build.sh --product-name {product} \
  --build-target foundation/systemabilitymgr/safwk/services/safwk:sa_main

# Unit tests
./build.sh --product-name {product} \
  --build-target foundation/systemabilitymgr/safwk/test:unittest

# Fuzz test
./build.sh --product-name {product} \
  --build-target foundation/systemabilitymgr/safwk/test/fuzztest/systemabilityfwk_fuzzer:fuzztest
```

**Feature switches** (`config.gni`): `safwk_enable_run_on_demand_qos`(F), `safwk_feature_support_saspawn`(F). Variable (`var.gni`): `safwk_support_access_token`(T, conditional).

**Device debug**: `ps -ef | grep sa_main` (list SA processes) · `hidumper -s <said>` · `svc <command>` · `cat /system/profile/<process>.json` · `sa_main /system/profile/<process>.json` (manual start).
