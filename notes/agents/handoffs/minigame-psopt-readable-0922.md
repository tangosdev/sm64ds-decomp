# Pause/options production readability: first pass

This handoff describes this commit. The queue records any published immutable output.

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2930.
- Task: `minigame-psopt-readable-0922`; producer `codex-psopt-producer-0922`, Codex.
- Branch/worktree: `cpp/minigame-psopt-readable-0922`, `C:/tmp/sm64ds-mg-psopt0922`.
- Source and workflow base: `9c6104f1f4df5554b19b3a72a9a32d0eba9361d0`.
- Status: locally proved producer candidate; exact-commit attribution check and independent source/byte review remain before offer. No source acceptance, PR or merge is claimed.
- Next owner: root independently reviews the committed source, then the normal independent verifier/integrator stages follow. All private objects and scripts remain under `build/psopt-readable/` and `build/psopt-*.py`.

## Scope and source evidence

Five enrolled production CPP files change: the nested `TouchIcon_c::Render`, the outer constructor, `func_ov004_020b8f18`, `func_ov004_020b8f78`, and the return-contract caller `func_ov004_020aeb24`. Their five owned ROM functions total 2,476 bytes in ov004 `[0x020aeb24,0x020aeed8)`, `[0x020b8c18,0x020b8dc0)`, `[0x020b8f18,0x020b91fc)`, and `[0x020b92c4,0x020b9430)`. The constructor also emits the identical compiler-owned C2 alias. No ownership/configuration changes occur.

`include/dMgPsOpt_c.h` now calls the signed word at +0x120 `mIconCount`: the constructor stores 8 then 0, open/close helpers write 8/0, and update/render loops bound their icon traversal with it. The existing byte at +0x125 becomes `mCloseDelay`; the close-icon path stores 20 and enters state 2, and `DecIfAbove0_Byte` decrements it until zero before closing. These are coined semantic names supported by the accesses, not recovered original names. The trailing padding shrinks to two bytes at +0x126; element size0x24 and outer size0x128 are unchanged. Both constructor count assignments, all initialization/call order, and the absence of any constructor/open-path timer initialization are preserved.

The two traversal helpers now use the actual eight-element `mIcons` array and native virtual `Render`/`Behavior`, replacing local proxy classes and raw option offsets. The ordinary update loop remains byte-identical, including its count reloads and dispatch order. The shared `dThIcon_c` unknown fields retain their existing names; changing that interface is outside this reservation. The caller's return declaration is corrected to `unsigned char`. `TouchArea_Update` and the constructor's icon initializer now use their actual `char*` contracts, with explicit boundary casts. `func_02012790` agrees with its existing unsigned argument/result definition; the original wrapper destination is retained.

`TouchIcon_c::Render` uses the real `G2S` namespace and the existing definition's `u32 GetBG1ScrPtr()` address result, with explicit address conversion. Its local blitter declaration uses the actual definition's opaque `S*` and `M*` tags; no replacement class layout is invented. Screen-entry, extent, coordinate and palette locals are named. The incorrect vtable address is repaired: address point ov004:0x020bca7c, Render slot1 at0x020bca80, inherited Behavior slot0. The global arrays remain declared as their existing u16 storage.

The seventh implementation/reference path, existing `src_tu/actors/dMgPsOpt_c.cpp`, receives only the six mechanical `mSelectedIcon` to `mIconCount` references, including comments. This preserves compilation after the real shared-header correction. No new shadow source is created and none of these production improvements is staged only there. Its existing eleven-function text-verified manifest remains unpromoted; this explicitly smaller readability task makes no full-TU promotion claim.

## Findings and measured limits

- `PSOPT-READ-01`, fixed: typed array traversal, virtual calls and update/render locals replace the two local proxy classes and option-object raw offsets.
- `PSOPT-LAYOUT-01`, fixed: misleading selected-index name and hidden close-delay byte are replaced by the supported count/timer fields without width, order or initialization changes.
- `PSOPT-CONTRACT-01`, fixed: unsigned sound-wrapper result/argument, u8 update result, char* touch/initializer contracts and the native u32 G2S address result agree with their current definitions. These are present source contracts, not unique evidence of the original ABI types.
- `PSOPT-PROV-01`, fixed: the vtable address formerly inside Render's own text is corrected from actual ov004 symbols/relocations.
- `PSOPT-CODEGEN-01`, deferred: the volatile s16 coordinate pair remains because removing only `volatile` shrinks Render from424 to408 bytes, complete ELF2168 to2152. This is a bounded 2004/b56 result, not a claim that all cleaner forms are impossible. The minigame coordinator owns any later matching experiment under issue2930.
- `PSOPT-RECON-01`, partial: unnamed dThIcon fields, unnamed data/helper identities, raw C helper boundaries and the wider scene caller's pre-existing Obj facade/raw fields remain. The unpromoted combined candidate and original TU boundary still need separately scoped reconstruction/promotion; no extra method or original name is claimed here. Continuation belongs to the minigame coordinator under issue2930.
- `PSOPT-BASELINE-01`, inherited: three header consumers already carry NONMATCHING and are outside standard matched-source validation. Jump OnYoshiTryEat also fails to compile before and after this change; Slot1 Behavior and func_ov006_020e5450 compile identically but do not reproduce their target functions. No consumer repair or draft acceptance is included.

To reproduce the negative experiment without editing production: copy Render to an ignored probe directory with its basename intact; replace exactly `volatile s16 position[2];` with `s16 position[2];`. From the repository root compile that file with `match.compile_c(probe, build_pin.version_for(original), build_pin.flags_for(probe))` after adding `tools` to Python's import path. The pin is2004/b56 and the flags are `-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off`. Inspect `_ZN10dMgPsOpt_c11TouchIcon_c6RenderEv` in the ELF symbol table: expected408 instead of424. Saved reproduction: `python build/psopt-cleaner-probes.py`, `build/psopt-readable/cleaner-probes/render-without-volatile/`, and `cleaner-probes/results.json`. The measured ordinary update loop and integer-address BG1 expression both produce exact complete objects and are adopted.

## Proof and coverage limits

Fresh before/final objects use `match.compile_c` with `build_pin.version_for` and `flags_for` in this wired worktree. The baseline header closure has144 paths, and the final closure146 after the two helpers include the real header. Adding the contract caller gives147 production paths; the existing shadow is an additional compilation. **147/148 complete raw objects are identical** (all successful compilations), covering sections, symbols, emitted metadata and relocation records. The single compilation failure is the unchanged NONMATCHING Jump draft. Exact source snapshots/hashes and per-object sizes/hashes are in `before/results.json` and `final/results.json`.

| Changed production object | Complete ELF bytes | SHA256 (unchanged) |
| --- | ---: | --- |
| TouchIcon Render | 2168 | ee50b0b14914dbe1a206458ff4a94881814a099fcc7d96e86b2fffb0bc73e624 |
| options constructor | 2136 | 4ebb50a61f31ffd84b6da97606235c8b5ebff5e300524d83cb1ba15944728f60 |
| func_ov004_020aeb24 | 2408 | 2a5721fc9a5007a125efe782bc8b27c9d817e6c8a0dfdefde5bc71c228c3ef42 |
| func_ov004_020b8f18 | 576 | ac45a1fa2e4de701f6792e8deed7d02a8670f3f0d61b1b31e4c6534de6e42e58 |
| func_ov004_020b8f78 | 1864 | 71536c6bf10558c03a6c9abe956ba535855067f4e8dd7b15c5e88f1089fd2bb4 |

- Explicit `prepush_linkcheck.verify` runs over all147 production paths: **1045 VERIFIED**, all blind0/diffs empty. The three extra diagnostic rows are exactly the existing drafts: Jump ERROR, Slot1/020e5450 NO-REPRO. Standard prepush intentionally skips those banners; its eligible population is144 files/1045 function checks. `strict.json`, `strict.log` and the exact path inventory preserve both sets. This is not a claim that all144 original header consumers are matched.
- `romdata_check.py --files <the same147 paths> -j8`: **113 VERIFIED /5732 bytes**, **37 PARTIAL /540 compared bytes**, **0 DIFFERS**, **1 UNNAMED**. Raw-object identity establishes no emitted-data regression; inherited partial/unnamed coverage remains. `metadata.json` and `metadata.log`.
- `rombuild.py -j16 --report-json build/psopt-readable/rombuild.json --data-json build/psopt-readable/rombuild-data.json`: **PASS**, packaged16MiB ROM, **11209/11209** source-built functions, **106/106** exact modules, **26** exact initialized-data claims, **3 BSS claims /172 bytes**. The independently generated ROM-gap control and output both hash `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`. Nine stock missing-symbol messages remain, zero new. The build reused7671 objects and compiled141; the fresh per-consumer object experiment above is separate. Whole-tree metadata remains775 verified/272 partial/3 differing/272 unnamed; this is not universal metadata acceptance.
- `check_decl_agreement.py --changed 9c6104f1f4df5554b19b3a72a9a32d0eba9361d0`: exit0, no new disagreements,2867 scoped inherited rows. No baseline edits. `declarations.log`.
- `port_refcheck.py`: exit0,408 references resolve. `port.log`.
- `check_tubuild_conflicts.py ov004/dMgPsOpt_c`: exit0, no conflicts or markers. The shadow remains a historical candidate, not the verified production endpoint.
- `tiers_ratchet.py --check`: PASS,2995 baseline/3008 current. `langmode_audit.py --check build/psopt-readable/langmode-baseline.json`: PASS against observed chaos-data81db3eb6095df8800597bc015da4312c8045d6de. `langmode.log`, `langmode-input.json`.
- `git diff --check`: PASS. Enrollment, symbols, attribution and shared baselines are untouched. Exact-commit attribution audit follows this local checkpoint before any producer offer.

All source/byte evidence here predates the handoff-only commit material and is bound by unchanged file hashes. Private validation, formal independent source acceptance and current-base integration are not claimed.
