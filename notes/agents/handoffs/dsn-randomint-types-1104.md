# Handoff: daDsnBase RandomIntInternal parameter type

## Identity and resumption

- Scope: `src/game/actors/d_a_dsn_base.cpp`, the shared Thwomp/Grindel base TU in overlay 091.
- Source branch: `dsn-randomint-type1104` in the audit namespace, based on current `origin/main` `a5bb6f569f6e7382ec1b5ef993b7e4fe4eae16a7`.
- Overlap check: no open PR or issue was found for the current source file. The earlier cleanup PR #2708 is closed; its extra local type correction was never pushed and predates the current source migration.
- Status: focused byte and relocation proof passed; full no-ROM build passed; independent source review and hosted checks remain outstanding.
- Next action: review the exact source/head and the `int *` declaration against `src/RandomIntInternal.c` and both call sites.

## What changed and why

`src/RandomIntInternal.c` defines `int RandomIntInternal(int *seed)`, and `include/math/CalcLib.h` declares the same parameter type. This TU declared the argument as `void *`, which hid that one caller passed `data_0209e650` while the other passed `&data_0209e650`. Since the global is an `int[]`, both callers should pass its base, which decays to `int *`. The local declaration now matches the definition, and the `StateRise` call no longer takes the address of the whole array. No storage or runtime pointer value changed.

## Proof on current main base

- Baseline control: focused TU verification on the unchanged source reported 12/12 matches, relocation destinations clean, expected ROM emission order, and promotion allowed.
- Candidate: `python tools/tubuild.py verify ov091/daDsnBase_c --no-write --verdict`: 12/12 exact; object isolation, relocation destinations, and emission order clean; promotion allowed.
- The complete candidate ELF object (10,792 bytes) is byte-identical to the saved pre-edit object (10,792 bytes).
- `python tools/prepush_linkcheck.py --files src/game/actors/d_a_dsn_base.cpp`: 12 checked, 12 verified, no warnings or blockers.
- `python tools/rombuild.py -j16 --no-rom`: PASS; 106/106 modules exact, 11,395/11,395 source-built functions exact, 470/470 source-owned data claims exact, zero mismatches.
- `python tools/check_decl_agreement.py --changed origin/main`: no new disagreements or local redeclarations.
- `python tools/port_refcheck.py`: 217/217 references resolve. `python tools/check_dead_references.py`: no new dead references or broken Markdown links.
- A fresh local source-style check could not run because clang-format 19.1.7 is unavailable; use the hosted source-style result.

These proofs were run on the candidate tree before the handoff was added. Re-run the focused check and full build on the committed PR head before accepting it.
