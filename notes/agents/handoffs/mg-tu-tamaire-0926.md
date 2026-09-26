# Handoff: mg-tu-tamaire-0926

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue https://github.com/tangosdev/sm64ds-decomp/issues/3171, task
  `mg-tu-tamaire-0926`, stage `revise`, role producer, session
  `claude-prod-mg-tu-tamaire-0926`, harness Claude Code (Opus 5.5).
- Branch `promote/mg-tu-tamaire-0926`. Input commit, original base and
  installed workflow and tool SHA: `70f055a670e80271a79713cabfbf825c251de2ec`.
- Commits on the input: the folded candidate (`6b91ced57a`), the promotion
  (`4a4865967c`), declaration agreement and baseline re-key (`12223a9ec0`),
  and this note. No separate evidence commits.
- Status: byte-verified production candidate, NOT published. Two required
  gates fail, and each fix needs a file outside this task's reservation
  (see Blockers).
- Next action: coordinator ruling on the two blockers, then independent
  verification of the resulting commit.
- Nothing uncommitted. Logs, probes and the assembler script stayed in the
  producer worktree's ignored build directory.

## What changed and why

- Scope: class `dScMgPachinko2_c` (Tamaire, scene 0x171, profile
  MG_TAMAIRE), ov006 .text 0x020ff420..0x0210428c, 74 functions: D1 and D0,
  67 `func_ov006_*` helpers, OnYoshiTryEat, Render, Behavior, InitResources
  and `dScMgPachinko2_c_classInit`.
- Manifest id `ov006/dScMgPachinko2_c`; the inspect id carried
  `+dScMgPachinko_c` (see RTTI ownership). Promoted source
  `src/actors/dScMgPachinko2_c.cpp`: tubuild's own guess for that id, the
  first reserved candidate, accepted by tu_promote.
- Form: ROM-ascending order under `#pragma defer_codegen off`. Each legacy
  body is carried as it was; the six members stay members, the 67 helpers
  stay free `extern "C"` functions under their current names, and the
  factory keeps its hand-built form. No new symbol, type or field names.
- Edits forced by one declaration per symbol in one TU, all byte-neutral:
  - `(char *)this` and similar casts at helper call sites. Render now casts
    `this` once per helper. Its pass-1 form leaned on the `void *`
    spellings in `include/decl_common.h`, which this TU cannot include
    because its helper spellings contradict the definitions.
  - Per-helper view structs renamed where two legacy files used one name
    (`Obj4ec`, `Obj4c0`, `Obj68c`, `Obje3c`, `Objd6c`).
  - `func_ov006_02102f3c` calls `func_ov006_02102864` through a
    two-argument function-pointer cast, as its legacy source declared it.
- `func_ov006_02100380`: the C body DIFFs under `-lang c++` (0x9c bytes
  against 0x88). The repeated `((View*)c)` casts let the compiler share
  addresses with the raw `c + 0x5330` and `c + 0x5335` increments. Hoisting
  the cast into one local, `View *v = (View*)c;`, restores the ROM code.
  Measured and rejected: member increments (0x90), `!active` (0x9c),
  `unsigned char` casts (0x9c), `+= 1` (0x9c), a do-while loop (0x9c).
  All-raw offsets also match but drop the view.
- Declarations aligned with their definitions, still 74 of 74: the returns of
  `G2S::GetBG2ScrPtr` and `G2S::GetBG3CharPtr` (`unsigned`),
  `func_ov004_020af2f8`'s `char` argument, `func_ov004_020b0a54`'s `int`
  argument, `func_ov004_020b1a5c`'s `void` return (unused at its one call),
  and `data_ov006_02136b80` as `int []` with a `(void *)` cast at its use,
  as `src/actors/dScMgPachinko_c.cpp` spells it.

## Pragma table

Delete-outright control: each pragma removed alone, the whole TU recompiled,
all 74 members compared against the ROM. 39 of 40 change their own member
and no other; one is inert and is not carried. Members with two pragmas hold
both in one bracket.

| Member | Pragma | Bracket | Control (pragma deleted) |
|---|---|---|---|
| `func_ov006_020ff534` | `opt_strength_reduction off` | own push and pop | size changes |
| `func_ov006_020ff534` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_020ff690` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_020ff8c8` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_020ffb54` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_020ffde4` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_020fff84` | `opt_strength_reduction off` | own push and pop | size changes |
| `func_ov006_020fffec` | `opt_strength_reduction off` | own push and pop | 11 words change |
| `func_ov006_021006f4` | `opt_strength_reduction off` | own push and pop | size changes |
| `func_ov006_0210076c` | `opt_strength_reduction off` | own push and pop | size changes |
| `func_ov006_0210076c` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_021009b8` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02100bac` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02100f7c` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02101088` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02101148` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02101224` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_021012cc` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_021016ec` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_021019e0` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02101af0` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02101e88` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_021020c4` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02102274` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_021024e0` | `opt_strength_reduction off` | own push and pop | size changes |
| `func_ov006_021027e4` | `opt_loop_invariants off` | own push and pop | 10 words change |
| `func_ov006_02102864` | `opt_loop_invariants off` | own push and pop | size changes |
| `func_ov006_02102864` | `opt_dead_assignments off` | own push and pop | 4 words change |
| `func_ov006_02102c3c` | `opt_strength_reduction off` | own push and pop | size changes |
| `func_ov006_02102c3c` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02102de4` | `opt_strength_reduction off` | own push and pop | size changes |
| `func_ov006_02102e8c` | `opt_strength_reduction off` | own push and pop | size changes |
| `func_ov006_02103360` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02103608` | `opt_strength_reduction off` | not carried | no change: inert |
| `func_ov006_02103608` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_0210371c` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02103870` | `opt_common_subs off` | own push and pop | size changes |
| `func_ov006_02103994` | `opt_common_subs off` | own push and pop | 63 words change |
| `func_ov006_02103ac0` | `opt_strength_reduction off` | own push and pop | size changes |
| `func_ov006_02103bfc` | `opt_strength_reduction off` | own push and pop | size changes |

`func_ov006_02103608` was re-checked with both of its pragmas deleted: its
size changes, so its remaining `opt_common_subs off` bracket stays proven.

## RTTI ownership

- `include/dScMgPachinko2_c.h` declares the destructor out of line and first,
  so this TU owns the key function. The object emits `_ZTV16dScMgPachinko2_c`
  (0x98 with its preamble), `_ZTI16dScMgPachinko2_c`, `_ZTS16dScMgPachinko2_c`,
  the `_ZTI` and `_ZTS` of dScMgBase_c, dScene_c, dBase_c and fBase_c, and
  a homeless D2.
- The manifest's `compiler_only_output` licenses each of them:
  - D2 as deadstrip. D0 sits at D1 + 0x24, so there is no room for a D2,
    and no symbols or delinks row names one.
  - Everything else as deadstrip-data at its cartridge address.
- Nothing of `dScMgPachinko_c` is defined or referenced by the object.
  That label reaches the inspect id only because rtti_vtables reads past the
  36 words of dScMgPachinko_c's vtable into the neighbouring
  g_profile_MG_TAMAIRE descriptor. That lands spurious slots 36 and 38 on
  `func_ov006_021019e0` and `dScMgPachinko2_c_classInit`;
  `include/dScMgPachinko_c.h` already records both slots as spurious.
- The factory stores the vtable address point through
  `data_ov006_0213dbbc`, the same address as `_ZTV16dScMgPachinko2_c`.

## Promotion plan

`tu_promote.py --dry-run` and the applied run agree:

- 74 functions from 74 legacy sources go into
  `src/actors/dScMgPachinko2_c.cpp`.
- 74 attribution overrides are added.
- 3 CONVERTED identities (D1, D0, Render) are re-keyed to the TU.
- The ov006 delinks now hold one complete claim for the range, and srcpath
  names the TU as the owner of all 74 addresses.
- Enrolled sources went from 7069 to 6996 (74 retired, one added).
  Source-built functions stay at 11,214.

## Blockers

1. `check_dead_references.py` exits 1.
   `notes/agents/handoffs/mg-p1-tamaire-0925.md` names seven retired sources
   by repo path, and that note is a historical record. Either fix touches a
   file this task has not reserved:
   - bank those seven paths in `config/dead-reference-baseline.json`, as
     #3112 did for a historical handoff; or
   - reword that note.
2. `queue_audit.py --check-promoted` exits 1. In
   `notes/data/tu-promotion-queue.tsv`, the row
   `dScMgPachinko2_c+dScMgPachinko_c` has `already_promoted` set to no, and
   it needs yes. That file is not reserved here, and the running task
   `pr2877-interface-repair-0921` holds it.

## Reconstruction dimensions

- Coverage: 74 of 74 functions MATCH, objisolate clean, relocation
  destinations clean, ROM emission order; 74 VERIFIED by prepush_linkcheck.
- Genuine methods: D1, D0, OnYoshiTryEat, Render, Behavior, InitResources.
- Still `extern "C"`:
  - the 67 helpers and the factory;
  - the external GX, GXS, G2S and `dScMgBase_c` constructor calls, which
    keep their mangled names.
- Layout: the helpers still reach the scene through byte offsets and local
  view structs. The class header's fields above 0x5260 are still padding.
- Attribution: 74 `path#symbol` overrides carry the original credit;
  prepush_attribution reports 74 consolidated, 0 changed, 0 lost.
- Remaining issue scope:
  - naming the helpers and their fields;
  - the three banked declaration disagreements (`DecompressLZ16`,
    `func_ov004_020aff38`, `func_ov004_020b19f0`);
  - the factory as a `new` expression, which is blocked on the separate
    objisolate fix.

## Proof

Pinned mwccarm `2004/b56`, producer worktree, `-j8` throughout.

- `git fetch origin` exit 0; `git worktree add -f ... 70f055a670` exit 0.
- `python tools/classqueue.py v2 claim mg-tu-tamaire-0926 --stage revise ...`
  exit 0.
- `python tools/rombuild.py -j8 --no-rom` on the input, exit 0:
  - 7069 enrolled;
  - 11,214 reproducing and 0 mismatching;
  - 106 of 106 modules exact.
- `python tools/tubuild.py inspect ov006/dScMgPachinko2_c+dScMgPachinko_c`
  exit 0.
- `tubuild.py create` on the same id exit 0. Its output was reassembled by
  hand in ROM order with the pragmas restored.
- `python tools/tubuild.py verify ov006/dScMgPachinko2_c`:
  - the first round gave 73 of 74, with `func_ov006_02100380` diverging;
  - the final run exits 0: 74 of 74 MATCH, objisolate clean, reloc
    destinations clean, compiler-only exact deadstrip, TEXT-VERIFIED;
  - re-run after each later source edit, exit 0 each time.
- Delete-outright control, 40 compiles: see the table above.
- `python tools/tubuild.py linkcheck --baseline --module ov006 -j8 --clean`
  exit 0. The control ROM has sha256 d1506e90...c478e8, and dsd check
  symbols carries 9 pre-existing errors.
- `python tools/tubuild.py linkcheck ov006/dScMgPachinko2_c -j8` exit 0:
  - SCRATCH-LINK-VERIFIED;
  - 106 of 106 modules exact, 0 new symbol errors;
  - ROM IDENTICAL to the stock control.
- `python tools/tu_promote.py ov006/dScMgPachinko2_c --dry-run` exit 0, then
  the applied run exit 0.
- `python tools/rombuild.py -j8` on the final source, exit 0:
  - `intactTuRom.identical` true, sha256 d1506e90...c478e8;
  - 106 of 106 exact, 11,214 reproducing, 0 mismatching;
  - no gap fallbacks, and one TU compiled.
- `python tools/prepush_linkcheck.py --range 70f055a670..HEAD` exit 0: 74
  checked, 74 verified, 0 warnings, 0 blocking.
- `python tools/prepush_attribution.py --base 70f055a670 --head HEAD` exit
  0: 74 consolidated with credit intact, 0 changed, 0 lost.
- `python tools/check_decl_agreement.py --changed 70f055a670`:
  - before the re-key it exited 1, first with 11 disagreements from the new
    file and then with 6;
  - after the re-key it exits 0, and the full run also exits 0;
  - the 23 rows keyed to retired sources were removed, and three rows were
    inserted in place under the TU path. Nothing was re-sorted or
    regenerated.
- `python tools/romdata_check.py --files src/actors/dScMgPachinko2_c.cpp --json build/romdata.json`
  exit 0. Of 11 emitted symbols:
  - 6 VERIFIED: `_ZTV16dScMgPachinko2_c` (144 bytes),
    `_ZTI16dScMgPachinko2_c` and the four ancestor `_ZTI`;
  - 5 PARTIAL: the five `_ZTS` strings;
  - 0 DIFFERS.
- `python tools/check_dead_references.py` exit 1 (Blocker 1).
- `python tools/tiers_ratchet.py --check` exit 0.
- `python tools/check_tubuild_conflicts.py` exit 0.
- `python tools/queue_audit.py --check-promoted` exit 1 (Blocker 2).
- `python tools/check_src_tu_compiles.py` exit 0, 303 of 303.
- `python tools/port_refcheck.py` exit 0, 408 checked, 0 stale.
- `git diff --check 70f055a670..HEAD` exit 0.
- Private validation and Source review: not run; they belong to later stages.
