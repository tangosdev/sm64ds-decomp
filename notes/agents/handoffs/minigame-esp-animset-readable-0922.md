# dMg3DEspAnimSet_c rendering and animation-loop readability

This document describes this commit. The queue records its immutable output SHA.

## Identity and scope

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2932.
- Task: `minigame-esp-animset-readable-0922`; producer `codex-tu-census-esp-animset-producer-0922` (Codex).
- Branch: `cleanup/minigame-esp-animset-0922`.
- Accepted source base and task workflow: `9c6104f1f4df5554b19b3a72a9a32d0eba9361d0`.
- The separately activated source-review policy remains `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Source proof checkpoint: `b4257ac0ff62de1a1bb712dc4644a5a1c0990e34`. This handoff adds no build input changes to that checkpoint.
- Production scope: `_ZN17dMg3DEspAnimSet_c6RenderEv`, ov006 `[0x020e777c, 0x020e7818)`, 156 bytes, and `_ZN17dMg3DEspAnimSet_c8BehaviorEv`, ov006 `[0x020e7818, 0x020e7910)`, 248 bytes.
- Only those two existing source files and this handoff change. Shared headers, configuration, enrollment and attribution are unchanged.

The existing methods and inferred class and role-based method spellings come from the earlier Psyche Out reconstruction, including PR #1940 and the recovered stack in PR #1960. This is an incremental production cleanup, not a new class identity claim or TU promotion. The separate ESP model callback task and its held report publication are untouched.

## Source changes and contract evidence

`Render()` indexes the existing three-element `mModels` and `mMaterialChangers` arrays. It accesses `mat4x3` and embedded `data` through the real model type, calls `MaterialChanger::Update(ModelComponents&)`, and uses the native virtual `ModelAnim::Render(const Vector3*)` slot 5 with a null scale. The numbered `Obj` facade, raw byte offsets, pointer strides and strength-reduction pragma are gone. `Matrix4x3_FromTranslation` now declares the `Matrix4x3*` receiver used by its actual C definition; coordinates and call order are unchanged.

`Behavior()` uses native `int ApproachLinear2(short&, short, short)`, `Animation::Advance()` and `int Animation::Finished()`. Existing state fields, array members and `currFrame` replace raw offsets and mangled bridges. The `activationPending` local describes the observed one-consumer flag: the loop activates at most one inactive entry after the timing branch makes it available. Its integer type, update order and the original signed field widths are retained. The direct field decrement matches without the old integer-address cast.

Evidence is in `include/dMg3DEspAnimSet_c.h`, `include/Model.h`, `include/ModelAnim.h`, `include/Animation.h`, `include/MaterialChanger.h`, and the production definitions of the six called helpers/methods. The array sizes and member offsets are already established by the class header; this task adds no shared layout claims. The `unk_168`, `unk_178`, `unk_17a` and `unk_17c` spellings remain unknown-field names.

## Measured experiments

All experiments use pinned mwccarm `2004/b56` with the source's production flags. Unchanged actual sources and copied controls produce identical objects. Adopted actual sources are also identical to those controls, including ELF symbols and relocations:

| Method | ROM function bytes | Complete ELF bytes | SHA256 before and after |
|---|---:|---:|---|
| Render | 156 | 904 | `be5724e96851ae9c0badf9d913092beb87246fe35fbbc107cb906678bb937f96` |
| Behavior | 248 | 1000 | `83d4c14325ddb6e05980abbb18a691418de4a0c0b926a9176852a8c60fb6b5f5` |

Typed pointer traversal matched Render with and without the old pragma. Indexed Render with the pragma retained changed its object despite keeping the same code size; removing the pragma restored the identical object. The adopted plain indexed `for` loop with literal null scale matches exactly.

The tested two-pointer Behavior traversal reduced its function from 248 to 236 bytes and its ELF from 1000 to 984 bytes. Retaining an integer-address decrement in that variant did not restore the match. The indexed form, then removal of the redundant zero local/block and flattening the inner `else if`, each matched the original whole object. These are results for the specific saved variants, not evidence that other loop spellings are impossible. No compiler barrier is retained by the adopted source.

Exact private experiment source, compiler output and hashes are preserved in the producer worktree under `build/esp-animset-readable/probes/`, `probes.json`, `probe.py`, `probe-clean.py` and `adopted.json`. No compiler or ROM bytes are committed.

## Proof

All checks below ran on source checkpoint `b4257ac0ff62de1a1bb712dc4644a5a1c0990e34` against the accepted `9c6104f1` base and exited 0. The final handoff-only commit retains identical source, headers, tools and configuration; the producer evidence records that identity and the final committed-range checks explicitly.

- `python -B build/esp-animset-readable/probe.py` and `probe-clean.py`: copied controls and adopted variants measured as above. Recompiling the actual adopted production files reproduces both baseline complete ELF objects; `adopted.json` records source/object hashes.
- `python tools/prepush_linkcheck.py --range 9c6104f1f4df5554b19b3a72a9a32d0eba9361d0..b4257ac0ff62de1a1bb712dc4644a5a1c0990e34 --json build/esp-animset-readable/strict.json`: 2/2 `VERIFIED`, 404 bytes, `diffs: []`, `blind: 0`.
- `python tools/romdata_check.py --files src/_ZN17dMg3DEspAnimSet_c6RenderEv.cpp src/_ZN17dMg3DEspAnimSet_c8BehaviorEv.cpp --json build/esp-animset-readable/romdata.json`: two sources, zero emitted data/metadata records, no differences. This is not a claim of class-wide metadata reconstruction.
- `python tools/check_decl_agreement.py --changed 9c6104f1f4df5554b19b3a72a9a32d0eba9361d0`: no new disagreements across 322 scoped files. Existing banked declaration debt is not cleared by this result.
- `python tools/prepush_attribution.py --base 9c6104f1f4df5554b19b3a72a9a32d0eba9361d0 --head b4257ac0ff62de1a1bb712dc4644a5a1c0990e34 --json build/esp-animset-readable/attribution.json`: 7,989 tracked credits, no moved/consolidated paths or changed/lost credits.
- `python tools/port_refcheck.py`: 408 references resolve. `python tools/layout_check.py --json`, `python tools/tiers_ratchet.py --check` and `git diff --check` pass.
- `python tools/langmode_audit.py --check build/esp-animset-readable/langmode-baseline.json`: passes against the artifact read from observed `chaos-data` commit `81db3eb6095df8800597bc015da4312c8045d6de`.
- `python tools/rombuild.py -j16 --report-json build/esp-animset-readable/rombuild.json --data-json build/esp-animset-readable/rombuild-data.json`: fresh full packaged ROM passes, 11,209 source-built functions reproduce, 106/106 modules exact, 26/26 source-owned data claims (4,984 bytes) and three BSS claims (172 bytes). Intact-TU gates report zero new symbol errors and exact storage aliases. The 16,777,216-byte ROM has SHA256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.

The global raw-object metadata diagnostic still reports 775 verified symbols, 272 partial, three differing and 272 unnamed. The three differing symbols are unrelated existing model/collision RTTI/vtable records; neither changed file emits such records. Do not conflate the exact module/source-owned data gates with full raw-metadata reconstruction.

Private commands, exit codes, logs and hashes are in `build/esp-animset-readable/gate-results.json`, `full-rom-result.json` and the corresponding reports/logs in the producer worktree. The producer's final evidence binds them to the exact candidate. No remote private validator or independent verification is claimed here.

## Remaining work and handoff

This is a locally proved partial reconstruction candidate, not independent acceptance or a merge. Both functions were native class methods before this change; this pass makes their bodies use the existing native structure. It does not reconstruct the entire animation-set class, its resource initialization, original names, metadata ownership or the meaning of every unknown state field.

The next owner is an independent verifier for the exact offered commit and tested base. Keep issue #2932 open for remaining agreed class work. The producer's private receipts, objects and logs remain in its own ignored build directory; the integrator owns later current-main composition and delivery.
