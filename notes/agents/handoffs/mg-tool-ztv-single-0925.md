# Handoff: mg-tool-ztv-single-0925

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue URL, task ID, stage, session and harness: https://github.com/tangosdev/sm64ds-decomp/issues/3171, task `mg-tool-ztv-single-0925`, stage `revise`, role producer, session `claude-prod-mg-tool-ztv-single-0925`, Claude Code.
- Source branch and previous accepted input SHA: `tooling/mg-tool-ztv-single-0925`, input `41bc9b58106d354d2a8e730095775b055f4423c4` (origin main at claim).
- Original source base SHA and installed workflow and tool SHA: both `41bc9b58106d354d2a8e730095775b055f4423c4`.
- Separate evidence commits and required artifacts in this commit: none. The fix and its tests are `454dad238f25aa20bb081940c2b59c97e2c0d5c8`; this handoff is the only change after it.
- Next action, responsible role and blockers: independent verification of the tooling fix, whole-object neutrality and the positive control. No blockers.
- Status: verified candidate, on the local evidence below.
- Remaining uncommitted or local-only material and where it is preserved: the positive-control source edit was reverted and is reproduced in full below. Build logs and the baseline object copy lived in the producer worktree's ignored `build` folder and are not preserved.

## What changed and why

- Scope: tooling only. `tools/objisolate.py` and `tools/test_objisolate.py`. No source, header, config or attribution change, and no ROM function changes hands.
- Defect: `derive()` returned the compiler's bytes whenever `plan()` had nothing to drop and nothing to externalise, and `isolate()` skipped its write on the same test. `_apply()` already rebases an UNDEF `_ZTV` addend, but never ran for an object that is only the function. A `return new C;` factory is exactly that object: one `.text`, and an UNDEF `_ZTV<C>` vptr store at mwcc's addend 8. It kept the 8, and the link wrote the vptr one preamble past the ROM's.
- Fix: `plan()` returns `ztvRebase`, set by the same test `_apply()` makes before re-addending (a `_ZTV` relocation from the kept text, externalised or UNDEF, addend at least `VTABLE_PREAMBLE`). `derive()` returns the input unchanged only when there is nothing to drop, nothing to externalise and nothing to rebase. `isolate()` writes whenever the derived bytes differ. Every existing refusal is unchanged; `plan_many`, `_apply` and the other derive paths are untouched. The `plan`, `derive` and `isolate` docstrings say so.
- Behaviour change beyond the defect: re-running `isolate()` on its own output used to be a no-op, because the early return fired. A primary vptr store is still stable (0 is below the preamble), but a multiple-inheritance secondary store now loses another 8. A probe on the existing `M::~M` test source measured addends 8, 8, 8, 28, then 0, 0, 0, 20 after one `isolate()`, then 0, 0, 0, 12 after a second. No current caller re-isolates: rombuild caches the raw object and isolates each fetched copy once, and eligible and reloc_audit compile fresh. The `isolate()` docstring records this in place of the old idempotence claim.

## Reconstruction dimensions

Not applicable: a tooling fix with no source reconstruction. The positive control below is the first shape it unblocks.

## Tests

Three tests in `tools/test_objisolate.py`, compiled with the pinned mwccarm like the existing ones:

- `test_corrects_a_one_function_new_factory`: `struct F : B` with `B()` and both key functions declared elsewhere, `extern "C" void *F_classInit() { return new F; }`. The plan has empty drop and externalise lists and `ztvRebase` true, and the raw `_ZTV1F` addend is 8. `derive()` leaves the file alone and yields addend 0 at the same length, `isolate()` writes the same bytes, and a second `isolate()` changes nothing.
- `test_leaves_a_rom_convention_vptr_store_alone`: `extern int _ZTV1F[];` stored explicitly, addend 0. `ztvRebase` is false, `derive()` returns the input, and `isolate()` never calls `write_bytes`.
- `test_leaves_a_plain_function_alone`: `int f(int n) { return n + 1; }`, with the same three assertions.

Negative control: with `tools/objisolate.py` at the base, all three new tests error on the missing `ztvRebase` key. A direct probe of the factory object showed `derive()` returning the input unchanged with addend 8, and `isolate()` leaving 8 in the file.

## Proof

All commands ran in the producer worktree with the pinned `2004/b56` compiler.

| Command | Exit | Result |
|---|---|---|
| `python tools/classqueue.py v2 claim mg-tool-ztv-single-0925 --stage revise --role producer ...` | 0 | lease running on input `41bc9b5810` |
| `python tools/rombuild.py -j8 --no-cache` at the base | 0 | `intactTuRom.identical` true, ROM sha256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`; 11,214 of 11,214 source functions reproducing, 0 mismatching; 106 of 106 modules exact |
| `python tools/rombuild.py -j8 --no-cache` with the fix | 0 | identical figures: `intactTuRom.identical` true, same sha256, 11,214 reproducing, 0 mismatching, 106 of 106 exact |
| `python -m unittest tools.test_objisolate -v` | 0 | 43 tests OK (40 existing, 3 new) |
| `python -m unittest tools.test_rombuild` | 0 | 39 OK |
| `python -m unittest tools.test_linkcheck` | 0 | 30 OK |
| `python -m unittest tools.test_tu_production` | 0 | 22 OK |
| `python -m unittest tools.test_tubuild` | 0 | collects 0 tests; the file uses module-level test functions |
| `python -m pytest tools/test_tubuild.py -q -p no:cacheprovider` | 1 | 65 passed, 5 failed, 1 skipped; the same 5 fail with the base objisolate (below) |
| `python -m unittest tools.test_tubuild_owned_relocs` | 0 | 6 OK |
| `python -m unittest tools.test_reloc_audit_flags` | 0 | 7 OK |
| `python -m unittest tools.test_symscope` | 0 | 16 OK |
| `python tools/prepush_linkcheck.py --range 41bc9b5810..HEAD` | 0 | no matched source files in the range, nothing to verify |
| `python tools/check_dead_references.py` | 0 | no new dead references, no broken markdown links |
| `python tools/check_src_tu_compiles.py` | 0 | 302 of 302 translation units compile |
| `python tools/port_refcheck.py` | 0 | 408 references resolve |
| `python tools/tiers_ratchet.py --check` | 0 | CONVERTED ratchet PASS, baseline 3001, current 3216 |

The five pytest failures are identical with the base objisolate and come from fixture state, not isolation. `test_list_finds_polelift_and_its_module_neighbours` and `test_inspect_polelift_reproduces_the_pilots_static_findings` find no ov045 candidate TU `daObjKm2_Ami_Bou_c` in the local tu_map. `test_verify_reproduces_pilot_1s_7_of_7_and_clean_objisolate` reports all seven functions matching and objisolate clean, but not the text `_ZTV18daObjKm2_Ami_Bou_c` it expects. `test_promote_dry_run_refuses_a_tu_that_is_not_link_verified_but_still_explains` finds no `OneUpLogo` manifest entry. `test_splice_refuses_a_span_it_cannot_tile_exactly` reports 18 legacy sources outside their span.

### Whole-object comparison

After the base build, `build/src` and `build/tu` were copied aside, the fixed build ran with `--no-cache`, and every file in both trees was compared whole against its copy.

- `build/src`: 7,069 files, all objects. 0 differ, none added or missing.
- `build/tu`: 14,786 files, 14,350 of them objects. 0 objects differ, none added or missing.
- Two non-object files differ: `final_link.o.xMAP` in its link start and end times only, and `linkcheck.json` in timings, in `controlToolsSha256` (a hash that includes the changed tool), and in `matchesStockRom: true`. The first build in a fresh worktree had no earlier `build/sm64ds.nds` to compare against, so it omitted that field.

### Positive control (not committed)

`src/d_s_mg_pachinko2.c` was replaced in the worktree by the smallest C++ `new` factory, following `dScMgMemory2_c_classInit`. The `//cpp` marker compiles the `.c` file as C++, so no rename was needed:

```cpp
//cpp
// @symbol dScMgPachinko2_c_classInit
#include "dScMgPachinko2_c.h"

extern "C" void *dScMgPachinko2_c_classInit()
{
    return new dScMgPachinko2_c;
}
```

Its object holds a single 0x34-byte `.text` with three relocations: `_ZN7fBase_cnwEj`, `_ZN11dScMgBase_cC2Ev`, and UNDEF `_ZTV16dScMgPachinko2_c` at addend 8.

| Command | Exit | Result |
|---|---|---|
| `python tools/rombuild.py -j8` with the base objisolate | 1 | addend stays 8. The strict stock control fails ov006: 105 of 106 modules exact, one differing byte in `d_s_mg_pachinko2` (0x02104258, size 0x34). The word at 0x02104288 is 0x0213dbc4 where ov006 has 0x0213dbbc. No ROM is built and the report has no `intactTuRom` |
| `python tools/rombuild.py -j8` with the fix | 0 | addend 0, `intactTuRom.identical` true, 11,214 of 11,214 reproducing, 106 of 106 exact |
| `git checkout HEAD -- src/d_s_mg_pachinko2.c`, then `python tools/rombuild.py -j8` | 0 | `git status` shows only the two tool files; the rebuilt `d_s_mg_pachinko2.o` is byte-identical to the base build's |

Independent verification of this exact candidate is a separate acceptance gate.
