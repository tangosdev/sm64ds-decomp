# shadowback-spiny-0920

Producer: `codex-shadowback-0920` (Codex). Initial source and workflow base:
`077483245976d72477eba0d2a7ccca4390da2b1b`. Branch: `fix/shadow-backports-0920`.
This describes the local candidate containing this handoff. The producer lease is
held in the v2 task named above. No source commit or review evidence has been
published through the queue; `formal_acceptance: false`.

The current scope is three already-enrolled production functions. Shared headers, symbols,
delinks, attribution, lifecycle definitions and the shadow TU are unchanged.
No whole-TU promotion or completion of the parent reconstruction is claimed.

Backport named members and call-contract repairs into
`src/_ZN5Spiny13InitResourcesEv.cpp`. Six resource/model/shadow calls now use their
existing real C++ interfaces. Collision and animation calls use the recovered
members. The state helper now matches its actual `void(char*, int)` definition;
the spurious third argument is removed. The animation bridge matches the actual
scalar definition's `u16` final argument. Its shared native declaration still
uses `u32`; this caller does not assert that broader interface is reconciled.

The mesh bridge explicitly follows its actual production C definition's integer
address parameters. This deliberately differs from the shadow pointer view;
restoring a coherent native mesh interface requires header/callee/caller work.
The native mesh header currently spells a different, unmapped symbol.

A fresh caller experiment substituted:
```cpp
Fix12<int> radius = {0x2d000};
Fix12<int> height = {0x3c000};
mdCcAc_c.Init(this, radius, height, 0x200000, 0x4a3d0);
```
With production C++ flags and `2004/b56`, it emits 332 bytes against 316 expected.
Strict linkcheck reports `NO-REPRO` / `requested-size-mismatch`. The failed form
was not adopted. Its source and machine-readable result are preserved locally in
`build/shadow-backport/spiny-native-cylinder.cpp` and `.json`. This is one bounded
experiment, not a universal claim about every possible native form.

SP2863-01 through SP2863-06 retain their historical dispositions. The three-arg
state-helper requirement in the old manifest is contradicted by the later
#2863 experiment and this production whole-object identity check. Raw matrix,
rotation and resource-handle views, retail class identity and complete-object
ownership remain under issue #2870. No shared-interface completion is claimed.
Source lineage: merged PR #2863, commit `aba8efc28`.

The continuation also updates `src/_ZN5Spiny8BehaviorEv.cpp`
(`ov077:0x02125f68`, 240 bytes) and `src/_ZN5Spiny6RenderEv.cpp`
(`ov077:0x02125ef0`, 120 bytes). Behavior uses the real ground-query,
collision, destruction and dust methods, a named despawn timer, and the named
camera-space position view. Its ground-height helper declaration now agrees
with `int(char*)`, and its sound declaration agrees with
`void(unsigned int, const Vector3*)`. The missing native distance-query
interface and existing state-dispatch bridge remain explicit partial work.
Render replaces the fabricated `Obj::doit` vtable interface with actual
`Model::Render(0)` and `ModelAnim::Render(0)` calls.

Complete objects remain 1,400 and 608 bytes, respectively, with SHA-256 values
`2ec66841508ed6e435b0a8ed99f4aa07ae547dc5d77f429d48a46f5217568241`
and `10930bc7f7f5f47c7462422c56db0475202a1acea10fd2a523c07c43a2441f5a`.
Neither emits a new function, RTTI, vtable or other nonzero symbol.

## Current six-function candidate

Current composition base: `5c075024562a469c6e4336d8540e3dfa9ffa84a1` (merged locally before
this continuation). Source candidate: `a5edafd2280d93bb61c002b9ca2849ec96e53341`.
The original workflow input remains `077483245976d72477eba0d2a7ccca4390da2b1b`;
its three-function evidence below is retained as history. Current production
source changes are BookShot InitResources/Behavior, Spiny
InitResources/Behavior/Render, and daKinopio_c St_Talk_Main. All six are enrolled
in the normal `src/` production build.

- Strict `prepush_linkcheck.py --range 5c075024562a469c6e4336d8540e3dfa9ffa84a1..a5edafd2280d93bb61c002b9ca2849ec96e53341`:
  six checked, six VERIFIED, zero warnings or blocking. Explicit per-function
  checks also report `diffs: []`, `blind: 0`.
- Independent compilation of all six sources at this source commit and the
  exact composition base: complete unisolated objects are byte-identical.
  All defined symbol bindings were inspected. The only extra nonzero function
  remains the existing four-byte Vector3 destructor in the Toad object.
- `rombuild.py -j 16 --no-rom`: PASS; 11,208/11,208 source-built functions,
  106/106 exact modules, 26/26 initialized-data claims and three BSS claims.
  8,001 object-cache hits, four fresh compiles. Candidate ROM packaging skipped.
  The newly generated stock control retains nine symbol errors; intact-TU gates
  introduce zero new symbol errors. The broad emitted-data diagnostic retains
  three differing and 296 unnamed records; these six object-identical changes
  introduce no emitted-data change.
- Declaration ratchet: no new disagreements. Port references: 418/418 resolve.
  Attribution: 8,182 tracked, zero changed or lost. Whitespace: pass.
- Language-mode ratchet: PASS against the preserved chaos-data baseline at
  `1203652ceff9b6e2c82e4d130762bb7366e31869`; CONVERTED ratchet: PASS,
  baseline 2,973 / candidate 2,990. No baseline was edited.
- Independent source/object review at the source commit found no source blocker.
  Finding SHADOWBACK2-REVIEW-01 identified the outdated one-function handoffs;
  this continuation section and corrected scope address that finding. Final
  review of this documentation-only successor is recorded in the verifier's
  local report. Private CI and formal queue review publication have not run;
  `formal_acceptance: false`.

The producer stage retains its original initializer/handoff artifact list.
Its approved file/range resource amendment additionally reserves Behavior and Render.
Source-review coverage is derived from the complete amended file resources,
not that original artifact list, and must cover every source at the exact
candidate and tested base.

Current local producer evidence is under `build/shadow-backport2/`:
`checks.json`, `gates.json`, `secondary-gates.json`, `rombuild-report.json`,
`rombuild.log`, `romdata.json` and `attribution.json`. Independent controls and
review live in the separate wired verifier's `build/shadow-backport-review2/`.
The additional 18 helper backports are a read-only proposal only: their queue
reservation was rejected by automatic approval review and none was edited.

## Initial slice proof (077483 base)

- Scope: `ov077:0x2126058`, 316 bytes,
  `_ZN5Spiny13InitResourcesEv`.
- Before and after explicit `tools/linkcheck.py --c <source> --name <symbol>
  --addr <address> --size <size> --module <module>`: `VERIFIED`, `diffs: []`,
  `blind: 0`.
- Complete unisolated compiler object: 1688 bytes, byte-for-byte
  identical to the source-base object. Both SHA-256 values:
  `6268d43c620aa0caff5045207734aa0c16f0e61f0ed0f8ad767a734c2f2e660a`.
  Only the target function is defined; no lifecycle/RTTI/vtable output was added.
- Compilation uses `2004/b56` and `rombuild.CFLAGS` with `-lang c++`.
- Three-file prepush linkcheck: 3 checked, 3 verified, no warnings or blocking.
- `check_decl_agreement.py --changed 077483245976d72477eba0d2a7ccca4390da2b1b`:
  no new declaration disagreements; no baseline was edited.
- `port_refcheck.py`: 418 references, all resolve. `git diff --check`: pass.
- `rombuild.py -j 16 --no-rom`: PASS. 11,208/11,208 source-built functions
  reproduce; 106/106 modules exact; all 26 owned initialized-data claims exact,
  plus three BSS claims. 8,021 object-cache hits and exactly these three fresh
  compiles. Final candidate ROM packaging was skipped. The stock control retains
  nine symbol errors, and intact-TU comparison introduces zero new errors.
  The broad emitted-data report still has 3 differing and 298 unnamed records;
  none is emitted by these three code-only, object-identical changes.
  Log: `build/shadow-backport/rombuild.log`; structured report:
  `build/shadow-backport/rombuild-report.json`.
- Language-mode check against chaos-data `c2a172f73a18dc02b8f416f8fe598341cbc28b7f`:
  reports `extern_vtable` 104 -> 105 and classes 105 -> 106. The independent
  clean worktree at the exact source base reproduces both failures. This is an
  unchanged cached-baseline failure. After refreshing the live chaos-data
  baseline at `1203652ceff9b6e2c82e4d130762bb7366e31869`, the language-mode
  ratchet PASSES. The CONVERTED ratchet passes at 2989 against 2973, identically
  on the base and this candidate. No repository baseline was edited.
- The initial-slice independent review later completed at
  `94c021afa4305bacd224f3ef1cbb523e4a6e9633`; its local report is retained in
  `build/shadow-backport-review/`. Private CI was not run.

Producer reservations are retained for continuation. Remaining full-class
reconstruction stays with its existing follow-up issue and ownership. Initial
reports remain under `build/shadow-backport/`; current reports are listed above.
Compiler objects are private ignored output and must not be committed or published.
