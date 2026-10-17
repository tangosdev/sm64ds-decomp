# Handoff: issue2410-reconstruction1015

This document describes this commit; the durable queue records its immutable output.

## Scope and identity

User-authorized attempt at the remaining reconstruction in [#2410](https://github.com/tangosdev/sm64ds-decomp/issues/2410). Task `issue2410-reconstruction1015`, producer session `codex-mky-producer1015`, coordinator `codex-mky-coordinator1015`, branch `cpp/mky2410-reconstruction-1015`, wired worktree `C:/tmp/sm64ds-mky2410-1015`. Predecessor `mky-native-normals1015` remains accepted; its recorded reconstruction limits are carried forward.

Original fetched input/workflow: `8d5689cb92452c46d0ce9888dbbecef6b839bccc`. Refreshed remote-main base: `79390c10a9fb3f6321618e4a6773da66e4df0f55`. No primary-checkout changes, resets, previous-branch rewrites, or queue reclamation. Reserved class `ov030/daMky_c`, production TU [d_a_mky.cpp](../../../src/game/actors/d_a_mky.cpp), [header](../../../include/daMky_c.h), [manifest](../../../config/tu_manifest.d/ov030/daMky_c.json), this handoff and the [experiment record](../../experiments/mky-reconstruction1015.json). Licensed text remains ov030 `[0x02111688,0x02114690)` including both factories. No source moves, symbol renames or attribution edits.

## Result

The state pointer now names a real 16-byte pair of int-returning member pointers. All 22 retail PMF records and initializer assignments were checked: zero adjustments and matching entry/update targets. The mouth writer in Player establishes the Player pointer at actor +0xd0. The actor's other player field is fed by that pointer, ClosestPlayer, or a guarded Player contact. Fields/substate/countdown names are reconstruction choices, not recovered identifiers; the class size remains 0x3cc.

Ordinary PathPtr calls, typed bone indexing and both real dBgPi copy constructions reproduce the existing code. The collision-result shadows, manual vptr stores/manual destructor calls and unused fake classes disappear. State7 uses the real Vector3 and ordinary address expressions. The cap +0xd0 write uses its real inherited field; the existing scalar declaration there still requires a pointer-to-integer cast.

Complete-object auditing is **not literal raw-ELF equality**: all 58 prior defined function/data payloads and symbol attributes remain equal, with two disclosed relocation changes from data_02099368+0 to the C++ dBgPi vtable+8 ABI spelling. Strict relocated checks establish the same actual destinations. The Bundle destructor's compiler-local uniquifier changes from 903 to 950. Natural dBgPi copying additionally emits the unreferenced four-byte SurfaceInfo destructor; exact compiler-only policy admits/removes only that output. No existing licensed function or owned data is removed. Emitted data verdicts are identical to the source control: four verified, five partial, zero differing.

## Validation

- `python tools/tubuild.py verify ov030/daMky_c`: 46/46 MATCH, clean object isolation, destinations and emission order; exact compiler-only output policy accepted. Verification metadata refreshed only for this manifest. Serialization-only rewrites of ten unrelated manifests were checked for JSON equality and restored.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_mky.cpp --json build/mky-final-linkcheck.json`: 46 VERIFIED, blind 0, diffs empty, warnings 0. `affected_src.py include/daMky_c.h` identifies this TU as its sole source consumer.
- `python tools/rombuild.py -j16 --no-rom --report-json build/mky-final-rombuild.json`: 106/106 modules exact; 11,334 functions and 332 source-owned data claims reproduce, zero mismatches; intact TU gate true, no new symbol errors. Baseline symbol-lookup errors and global ROM-data partial/differing advisories are retained separately. The build executed before the documentation-only base refresh; exact production-input identity carries the result forward, as recorded in the experiment file.
- `check_decl_agreement.py --changed 79390c10a9fb3f6321618e4a6773da66e4df0f55`: no new disagreements or local redeclarations. `port_refcheck.py`: 259 references pass. `check_dead_references.py`: no new dead references or broken Markdown links. Changed-line style check: zero findings and zero deferrals. Full experiment and report hashes are in the committed experiment record; ignored raw objects/logs remain under this worktree's build directory.
- Private PR validation: not run; no PR or merge performed for this continuation.

## Remaining scope and handoff

Completion is **partial**. Native matrix copying, ModelAnim SetAnim, collision Init and line-query lifetime still fail fresh pinned-compiler probes; exact alternatives/sizes are recorded. The shared base's carrier/cap +0xc8 remains raw, and helper names, unknown field/padding, legacy asset handles and C static initialization remain reconstruction work. Vtable/RTTI/data ownership and partial extents are unchanged. No blanket compiler-impossibility claim is made.

Next owner: independent Astra verifier. Review this exact commit and refreshed base, independently compile/check all licensed functions and metadata, inspect compiler-only admission and preserved attribution, carry previous findings forward, and publish an exact-head source review with partial completion. Keep #2410 open for the remaining scope. No merge is authorized by this handoff.
