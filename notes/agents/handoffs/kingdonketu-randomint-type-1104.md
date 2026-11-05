# Handoff: daKing_Donketu LevelRng seed type

## Identity and resumption

- Scope: `src/game/actors/d_a_king_donketu.cpp`, King Donketu/Chief Chilly in overlay 073.
- Source branch: `kingdonketu-randomint-type1104` in the audit namespace, based on current `origin/main` `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Overlap check: no open PR or issue was found for the current source file. Historical TU-fold PR #2820 is merged; its old local branch is not an active worktree.
- Status: focused byte and relocation proof, declaration agreement, style, and full no-ROM build passed. Independent source review and hosted PR checks remain outstanding.
- Next action: review the LevelRng definition, the typed helper declaration and all three call boundaries on the exact PR head.

## What changed and why

This TU declared `data_0209e650` as `u16` and declared `RandomIntInternal(u16 *)`. Those spellings contradicted the source: `include/LevelRng.h` identifies the global as `LevelRng`, whose definition in `src/unnamed/arm9/0201/func_0201fec8.cpp` has one `int value`, and `src/RandomIntInternal.c` takes `int *`. The TU now uses the shared `LevelRng` declaration, matches the helper prototype to `int *`, and explicitly views the object address as `int *` at its three call sites. Other current callers use the same pointer view. No storage or call address changes.

`math/CalcLib.h` also declares `RandomIntInternal`, but cannot be included while this TU retains its conflicting local `DecIfAbove0_Short(void *)` declaration. The local `RandomIntInternal` prototype therefore carries the required reason.

## Proof on current main base

- Baseline and candidate objects are both 44,840 bytes and byte-identical.
- `python tools/tubuild.py verify ov073/daKing_Donketu_c --no-write --verdict`: 47/47 functions match; object isolation, relocation destinations, and ROM emission order are clean. The TU has one pre-existing unlicensed `__sinit_d_a_king_donketu.cpp` section (0x4d4 bytes), so promotion is refused by policy.
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_king_donketu.cpp`: 48 checked, 46 verified, two blind warnings for class-init and `__sinit`, zero blockers.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed origin/main`: no new declaration disagreements or local redeclarations.
- `python tools/source_style.py --base origin/main --files src/game/actors/d_a_king_donketu.cpp` with pinned clang-format 19.1.7: zero findings, zero deferrals.
- `python tools/port_refcheck.py`: 217/217 references resolve. `python tools/check_dead_references.py`: no new dead references or broken Markdown links.

These checks were run on the candidate source before the handoff was added. Re-run the focused proof and full build on the committed PR head before accepting it.
