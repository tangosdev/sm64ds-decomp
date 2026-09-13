# Handoff: issue-2497-trampoline2-source-review

This document describes this commit. The queue records its immutable output SHA.

## Identity and continuation

- [Class issue #2497](https://github.com/tangosdev/sm64ds-decomp/issues/2497), task `issue-2497-trampoline2-source-review`, producer stage `fix`.
- Producer session `codex-trampoline2-humanizer-producer-20260908-01`, Codex; branch `cpp/humanizer-trampoline2-0908` in its own wired worktree.
- Accepted source input, base and installed workflow/tool commit: `3536f090698231c852ad18dde07f8bf9db2f50f7`; compiler pin `2004/b56`.
- This continues the existing intact promotion from [merged PR #2270](https://github.com/tangosdev/sm64ds-decomp/pull/2270), merge `e351ffb090e75d1e753c6ef5f17c9a4f52101e01`. [PR #2232](https://github.com/tangosdev/sm64ds-decomp/pull/2232) was superseded, not restarted.
- Local reconciliation: the preserved legacy `sm64ds-mgtramp` checkout is clean at `d55c72d13`, an ancestor of the accepted input with no exclusive commits. Its tracked Trampoline2 work is already ancestral to the promotion; no distinct class facts/handoff or ignored class checkpoint was found (its build contained the shared object cache and TU map). The clean retrospective checkout and all research objects remain preserved. Neither checkout was edited or adopted as a writer.
- Next: an independent verifier, who did not author the adopted research, checks this exact candidate. The producer/research contributor cannot supply final independent acceptance. Integration and any composition with newer main remain the integrator's responsibility.

## Changes and finding dispositions

| Finding | Disposition in this candidate |
| --- | --- |
| TRAMP2-01 | Corrected the two callers of the real ov006 `void(void)` helper at `0x020cedf0`; the second helper at ov006 `0x020cac30` is also an ordinary `void(void)` call. Removed the counterfeit `Player` declaration. The old module-qualified numeric destinations already matched; this repairs false source identity/prototypes, not a demonstrated wrong destination. Actual `Player::St_Null_Init` is the unrelated ov002 function at the same RAM address. |
| TRAMP2-02 | Replaced the artificial 19-slot `Base` interface with the existing `OnYoshiTryEat(-1)` virtual self-call; removed the unused scaffold. |
| TRAMP2-03 | The local C++ `ApproachLinear(int&, int, int)` declaration now returns `int`, agreeing with its existing definition. This caller discards the result. |
| TRAMP2-04 | Corrected live-storage/state availability, vtable-emission, naming, pragma and neighbor-boundary claims. Header executable tokens and layout are unchanged. The next class is `dScMgBSC_c`, not `dScMgTrampoline3_c`. Historical scratch-attempt records remain labeled as history. |
| TRAMP2-05 | Open reconstruction under #2497: coherent typed PMF field/state records and conversion of the five existing state bodies; typed observed fields, including `0x7ac4/0x7ac8` and all six live tail words at `0x7b84..0x7b9c`. No claim that unknown original names or the unexplained word at `0x7acc` prevent the evidenced fields. |
| TRAMP2-06 | Open reconstruction under #2497: real Model/SharedFilePtr interfaces, remaining mangled base/Particle bridges, typed components, and manual factory/array lifecycle. The real Model API is a concrete next probe, not a measured rejection. No fresh local Particle experiment establishes a compiler barrier. |

The next producer scope for TRAMP2-05/06 belongs to the humanizer/integrator support owner `codex-humanizer-integrator-20260908` through #2497; reserve dependencies before editing shared interfaces. These open dimensions prevent describing the whole class as fully reconstructed. This candidate resolves the three measured local changes and their current provenance only.

The shared minigame `Virtual50` return-contract correction remains the separate [Cup continuation #2492](https://github.com/tangosdev/sm64ds-decomp/issues/2492); it is not duplicated here. Inherited minigame slot 19 and the actor hierarchy are unchanged.

## Evidence and scope

- TU: `src/minigames/d_s_mg_trampoline2.cpp`; own header: `include/dScMgTrampoline2_c.h`; manifest: `config/tu_manifest.d/ov006/dScMgTrampoline2_c.json`. Those three files plus this handoff are the complete tracked change.
- Text: all 42 functions, 9,052 bytes, ov006 `0x021225ac..0x02124908`. Existing ten explicit class-method definitions plus generated D1/D0 and 30 free-function slots remain the same identities.
- Owned data: all 328 bytes and 56 relocations, ov006 `0x0213fbc4..0x0213fd0c`. The existing intact-object policy, exact RTTI import checks, D1/D0 binding rewrites, vtable rebias, enrollment and layout are unchanged. No new data exclusions or aliases.
- RTTI evidences the class name/base relationship. English callback/factory spellings, component roles and original TU grouping remain reconstructed labels or structural inference, not recovered original source names.
- Separate baseline and individual experiments verified each of the three natural alternatives before this combined candidate. All 42 function bytes remained identical; the helper repair changes one relocation's source spelling to the real ov006 helper while retaining its correct module-qualified destination. The combined candidate was freshly compiled and strictly checked after the final comment changes.

## Producer proof

All local artifacts below are ignored evidence, not ROM/compiler files to publish. Source/header/manifest hashes in `build/trampoline2-proof/identity.json` bind the frozen build inputs. This handoff is added after source proof; the exact committed head and post-commit checks are recorded in queue evidence.

| Command / operation | Exit and result | Local evidence |
| --- | --- | --- |
| `python build/trampoline2-proof/run.py` | 0; all 42 explicit symbols VERIFIED, 9,052 bytes, zero blind/differing relocations. Compile once, `tu_production.prepare_intact_object`, then `linkcheck.linkcheck` for every exact manifest symbol/address/size/module. Full owned data and emitted-object audit pass; all 56 data relocations agree. | `strict.log`, `strict.json`, `intact.json`, `raw.o`, `production.o` in `build/trampoline2-proof/` |
| `python tools/affected_src.py include/dScMgTrampoline2_c.h` | 0; sole consumer is this TU, covered by the full 42-symbol check. Header tokens unchanged against the input. | `build/trampoline2-proof/consumers.txt`, `identity.json` |
| `python tools/check_header_offsets.py include/dScMgTrampoline2_c.h` | 0; 17 commented fields, zero mismatch/unparsed, span `0x7bac`; compiled size assertion also passes. | `build/trampoline2-proof/header-offsets.log` |
| `python tools/romdata_check.py --files src/minigames/d_s_mg_trampoline2.cpp --json build/trampoline2-proof/metadata.json` | 0; 19 VERIFIED (352 bytes), 6 PARTIAL (72 compared bytes), no DIFFERS/UNNAMED. All 25 raw rows match the clean input object exactly. | `metadata.log/json`, `metadata-records.json`, `metadata-control-records.json` in `build/trampoline2-proof/` |
| `python tools/prepush_linkcheck.py --files src/minigames/d_s_mg_trampoline2.cpp --json build/trampoline2-proof/prepush.json` | 0 with one legacy basename NO-SYM warning, zero verified by that shortcut. The complete explicit-symbol proof above supplies coverage; the warning itself is not a pass. | `build/trampoline2-proof/prepush.log/json` |
| `python tools/port_refcheck.py` | 0; 423 references, zero stale. | `build/trampoline2-proof/port.log` |
| `python tools/rombuild.py -j16 --report-json build/trampoline2-proof/production-rom.json` | 0; packed 16 MiB ROM, 106/106 modules exact; intact TU module/storage gates pass with zero new symbol errors. SHA-256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8` equals the configured stock identity. | `build/trampoline2-proof/production-rom.log/json` |

The nested no-TU baseline reports nine symbol-check errors; the candidate has the same nine and zero new errors. This is baseline-relative acceptance, not a claim that every symbol diagnostic passes. The tree-wide metadata report also retains 4 DIFFERS and 389 UNNAMED records; the changed TU has none and all its raw records agree with the clean input.

The six raw PARTIAL records are `_ZTS7fBase_c`, `_ZTS7dBase_c`, `_ZTS8dScene_c`, `_ZTS11dScMgBase_c`, `_ZTS14dScMgD3DBase_c` and `_ZTS18dScMgTrampoline2_c`: unchanged RTTI-name string extent/word-coverage limits, not newly differing data. The separate intact policy verifies the complete owned data range and exact imported records; it does not reclassify these raw measurements as VERIFIED.

All 42 `path#symbol` credit overrides and the whole `attribution.json` are unchanged. The post-commit `prepush_attribution.py --base 3536f090698231c852ad18dde07f8bf9db2f50f7 --head HEAD` result belongs in the immutable queue evidence. No public/private CI validation or final composition acceptance is claimed from this local producer proof.
