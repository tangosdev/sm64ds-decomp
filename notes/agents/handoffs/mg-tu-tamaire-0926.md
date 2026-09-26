# Handoff: mg-tu-tamaire-0926

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue https://github.com/tangosdev/sm64ds-decomp/issues/3171, task
  `mg-tu-tamaire-0926`, stage `revise`, role producer, harness Claude Code
  (Opus 5.5). Round 1 session `claude-prod-mg-tu-tamaire-0926`; this round 2
  rework, session `claude-prod-mg-tu-tamaire-0926-r2`.
- Branch `promote/mg-tu-tamaire-0926`. Round-2 input commit: the round-1
  candidate `8ee6e73d9b0330b3ad51b0f12f0404b38f058da4`. Original base and
  installed workflow and tool SHA: `70f055a670e80271a79713cabfbf825c251de2ec`;
  main was not merged.
- Round-1 commits: the folded candidate (`6b91ced57a`), the promotion
  (`4a4865967c`), declaration agreement and baseline re-key (`12223a9ec0`),
  the first cut of this note (`354583ab2c`), and the two ledger edits with a
  revision of the note (`8ee6e73d9b`). Round-2 commits: the source repair
  (`e7971bd79f`) and this note. No separate evidence commits.
- Status: byte-verified production candidate, reworked for the round-1
  verification findings below; every required gate exits 0.
- Next action: independent verification (round 2) of this commit.
- Nothing uncommitted. Logs, variant sources and objects stayed in the
  producer worktree's ignored build directory.

## Findings

Round-1 verification of `8ee6e73d9b` passed bytes and packaging and failed
source review. Each repair below is byte-neutral: the TU object is whole-file
identical to `8ee6e73d9b`'s (see Proof).

| ID | Kind | Disposition | Repair |
|---|---|---|---|
| TU-V1 | correctness | fixed | The comment on `struct C` claimed that completing it first changes the code. It now states only the measurement: completing `C` before the typedef, completing it as `{ int dummy; }`, or never completing it compiles to the same object. |
| TU-V2 | correctness | fixed | The TU banner and the Layout line below said the header's offsets above 0x5260 are padding. Both now say what `include/dScMgPachinko2_c.h` declares: 0x5260..0x565f is the tail of `mBalls[0x40]`, which the helpers' views read as other records; `unk_5660`, `unk_566c` and `unk_566e` are named; only `pad_5664` and `pad_5670` are padding. |
| TU-V3 | provenance | fixed | "score popups" came from the sibling class. The 0x5620 view is now `func_ov006_020ff47c`'s view of the two 0x20-byte entries at 0x5620, with no role. |
| TU-V4 | reconstruction | fixed | `func_ov006_02102f3c` calls `func_ov006_02102864(base)` directly. The two-argument function-pointer cast and the `arg1 = (int)e` store that only fed it are gone; each step alone and both together give the same object. |
| TU-V5 | reconstruction | fixed | The rename and edit lists under "What changed and why" are complete: seven view renames, and every edit the fold made to a legacy body, marked forced or chosen, each chosen one with its measurement. |
| TU-V6 | reconstruction | fixed | The out-of-line destructor is named below as a deliberate divergence from checklist item 7, with its measurement. |

Carried forward unchanged from the round-1 verification (owner: minigame
readability pass 2, issue 3171):

| ID | Disposition at `8ee6e73d9b` |
|---|---|
| TU-V7 (`include/decl_common.h` declares `func_ov006_021024e0` with an `int` parameter) | deferred |
| TU-V8 (`mBalls[0x40]` overlaps records the helpers use; every ball loop stops at 0x30) | deferred |
| TAMAIRE-R1, R2, R5, R6, R9, R10, R12, R13 | fixed |
| TAMAIRE-R3, R4, R7, R8, R11, R14 | deferred |
| TAMAIRE-V1, V2 | deferred |

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
  body is carried as it was, apart from the edits listed below; the six
  members stay members, the 67 helpers stay free `extern "C"` functions under
  their current names, and the factory keeps its hand-built form. No new
  symbol or field names. The only new type names are the seven view renames.
- Deliberate divergence from checklist item 7 of
  `notes/tu-promotion-conventions.md` (destructor inline in the header and
  unmarked): the destructor stays declared in `include/dScMgPachinko2_c.h` and
  is defined out of line in the TU under two stacked symbol markers, as
  `src/actors/dScMgPachinko_c.cpp` does. Measured: with
  `virtual ~dScMgPachinko2_c() {}` in the header and no out-of-line
  definition, all 74 members still match and D2 disappears, but D1 and D0 come
  out after the last helper (sections 149 and 151, after the factory's 130),
  so the function order stops matching the ROM. Out of line they are sections
  5 and 7, first, as in the ROM.
- View types renamed where two legacy files used one name. The first user
  keeps the name (`Obj` and `E` in `func_ov006_020ff47c`, `Entry` in
  `func_ov006_020fff84`); each later one takes its helper's address as a
  suffix: `Obj4ec`, `Obj4c0`, `E68c`, `Obj68c`, `Obje3c`, `Entryd6c`,
  `Objd6c`.
- Edits forced by one declaration per symbol in one C++ TU, all byte-neutral:
  - 44 casts at helper call sites, where the caller's legacy declaration
    differed from the helper's definition: Behavior 20, Render 8,
    InitResources 1 (`(int)this` becomes `(char *)this`),
    `func_ov006_02100554` 3, `func_ov006_02102864` 6 (`(int)c`, since
    `func_ov006_021027e4` takes an `int` first parameter), and one each in
    `func_ov006_02100bac`, `0210246c`, `0210258c`, `02102fe8`, `02103608`
    and `02103994`. Render's pass-1 form leaned on the `void *` spellings in
    `include/decl_common.h`, which this TU cannot include because its helper
    spellings contradict the definitions.
  - 4 `(void *)` casts on the data word passed to `func_ov004_020afdd0`,
    whose one declaration takes `void *`: in `func_ov006_020ff47c`,
    `02100314`, `0210068c` and `02102de4`. The last three declared an `int`
    first parameter; in `func_ov006_020ff47c` the cast follows from
    `data_ov006_02136b80` becoming `int []` (last bullet of this section).
  - `data_020a0e40` is read as a scalar in `func_ov006_02102f3c`, which read
    `data_020a0e40[0]` through a `u8 []` declaration. The TU declares it once
    as `u8`, as `func_ov006_02102fe8` did; measured, the array declaration
    does not compile at `func_ov006_02102fe8`'s two scalar reads.
  - `func_ov006_021024e0`'s parameter, named `this` in its C source, is now
    `c`: `this` is a keyword in C++.
  - `func_ov006_0210246c`'s `class C { public: int dummy; }` merges into the
    one `struct C { char pad[1]; }`; per TU-V1 the choice of completion does
    not change the object.
  - The D1 and D0 files each defined the same empty destructor; the TU has
    one definition carrying both markers.
  - The factory's definition gains `extern "C"`: it has no forward
    declaration, and without it the name would be mangled.
- Edits the fold chose, each measured neutral:
  - 7 `_ZN5Sound12PlayBank2_2DEj` calls in 4 helpers (`func_ov006_0210076c`
    2, `0210265c` 3, `02102c3c` 1, `02103ac0` 1) are spelled
    `Sound::PlayBank2_2D`, the spelling `func_ov006_02102fe8` already used.
    Keeping the mangled `extern "C"` name next to the namespace declaration
    gives the same object.
  - `func_ov006_02102fe8`'s `volatile` moved from its `data_020a0dea`
    declaration to two use-site casts, so the TU's one declaration stays the
    plain `u8 []` that `func_ov006_02102f3c` read. Declaring it `volatile`
    once without the casts gives the same object.
  - The `.cpp` helpers' definitions drop their own `extern "C"`; the forward
    block gives every helper C linkage.
  - `func_ov006_02102f3c` calls `func_ov006_02102864(base)` directly (TU-V4).
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
  argument (Behavior now passes `0x10` without a cast),
  `func_ov004_020b1a5c`'s `void` return (unused at its one call), and
  `data_ov006_02136b80` as `int []` with a `(void *)` cast at its use, as
  `src/actors/dScMgPachinko_c.cpp` spells it.

## Pragma table

Delete-outright control: each pragma removed alone, the whole TU recompiled,
all 74 members compared against the ROM. 39 of 40 change their own member
and no other; one is inert and is not carried. Members with two pragmas hold
both in one bracket. Round 2 changed no bracket.

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

## Ledger edits

The first cut of this note left two gates red on files outside the
reservation. The coordinator ruled that both edits belong to this
promotion, as in-place edits of the class's own rows: #3112 (`6e972a7d8e`)
banked a historical handoff's retired paths, and #3159 (`93fa5f1e8b`) and
the other 09-25 promotions each flipped their own queue row.

1. `config/dead-reference-baseline.json`: seven entries for
   `notes/agents/handoffs/mg-p1-tamaire-0925.md`, one per retired source it
   cites, inserted by hand at their place in the existing order. That note
   describes its own commit and is unchanged. No `--update`, no re-sort.
2. `notes/data/tu-promotion-queue.tsv`: in the row
   `dScMgPachinko2_c+dScMgPachinko_c`, only the `already_promoted` cell
   changed, from no to yes. No `--write`.

Round 2 touched no ledger.

## Reconstruction dimensions

- Coverage: 74 of 74 functions MATCH, objisolate clean, relocation
  destinations clean, ROM emission order; 74 VERIFIED by prepush_linkcheck.
- Genuine methods: D1, D0, OnYoshiTryEat, Render, Behavior, InitResources.
- Still `extern "C"`:
  - the 67 helpers and the factory;
  - the external GX, GXS, G2S and `dScMgBase_c` constructor calls, which
    keep their mangled names.
- Layout: the helpers still reach the scene through byte offsets and local
  view structs. `include/dScMgPachinko2_c.h` types 0x5260..0x565f as the
  tail of `mBalls[0x40]`, which those views read as other records (TU-V8),
  and names `unk_5660`, `unk_566c` and `unk_566e`; only `pad_5664` and
  `pad_5670` are padding.
- Attribution: 74 `path#symbol` overrides carry the original credit;
  prepush_attribution reports 74 consolidated, 0 changed, 0 lost.
- Remaining issue scope:
  - naming the helpers and their fields;
  - the three banked declaration disagreements (`DecompressLZ16`,
    `func_ov004_020aff38`, `func_ov004_020b19f0`);
  - the factory as a `new` expression, which is blocked on the separate
    objisolate fix;
  - TU-V7 and TU-V8 above.

## Proof

Pinned mwccarm `2004/b56`, producer worktree, `-j8` throughout.

### Round 2, on the source commit `e7971bd79f`

This note is the only file after that commit. The last two gates below ran
with this note in the working tree.

- `git fetch origin` exit 0.
- `git worktree add -f ... promote/mg-tu-tamaire-0926` exit 0, at
  `8ee6e73d9b`; the five ignored inputs copied, not linked.
- `python tools/classqueue.py v2 claim mg-tu-tamaire-0926 --stage revise ...`
  exit 0.
- Variant compiles of the whole TU on `8ee6e73d9b`, each compared object
  against object and member against ROM:
  - `C` completed before the typedef, completed as `{ int dummy; }`, never
    completed: each object identical, 74 of 74 (TU-V1);
  - the direct call alone, and the direct call without the `arg1` store:
    each object identical, 74 of 74 (TU-V4);
  - the mangled Sound spelling kept, and `data_020a0dea` declared
    `volatile` without the casts: each object identical, 74 of 74 (TU-V5);
  - `data_020a0e40` as `u8 []`: compile error at `func_ov006_02102fe8`
    (TU-V5);
  - destructor inline in a header copy, no out-of-line definition: 74 of 74,
    no D2, D1 and D0 at sections 149 and 151 against 5 and 7 (TU-V6).
- Object comparison, `8ee6e73d9b` against `e7971bd79f`, compiled with the
  build's flags: whole-file identical, 42,184 bytes, sha256
  654cf77a...5694d50. Sections (155, by name, type, size and content),
  relocations (283, by section, offset, type, symbol and addend), defined
  symbols (215) and undefined symbols (110) are all identical.
- `python tools/tubuild.py verify ov006/dScMgPachinko2_c` exit 0: 74 of 74
  MATCH, objisolate clean, reloc destinations clean, ROM-ascending emission,
  TEXT-VERIFIED.
- `python tools/rombuild.py -j8 --report-json ...` exit 0, status passed:
  - `intactTuRom.identical` true, sha256 d1506e90...c478e8;
  - 6996 enrolled, all compiled, 0 reused from cache;
  - 106 of 106 exact, 11,214 reproducing, 0 mismatching;
  - no gap fallbacks; intact TU gates pass with zero new symbol errors;
  - the link map places all 74 sized `.text` symbols of
    0x020ff420..0x0210428c in `dScMgPachinko2_c.o`, contiguous, with no
    duplicate owner and no gap object.
- `python tools/prepush_linkcheck.py --range 70f055a670..HEAD` exit 0: 74
  checked, 74 VERIFIED, 0 warnings, 0 blocking.
- `python tools/prepush_attribution.py --base 70f055a670 --head HEAD` exit
  0: 7168 tracked, 74 consolidated with credit intact, 0 changed, 0 lost.
- `python tools/check_decl_agreement.py --changed 70f055a670` exit 0: no
  new disagreements, no new local redeclarations. The full run exits 0:
  14,592 disagreements, no new one.
- `python tools/romdata_check.py --files src/actors/dScMgPachinko2_c.cpp --json build/romdata.json -j8`
  exit 0. Of 11 emitted symbols, 6 VERIFIED (`_ZTV16dScMgPachinko2_c`
  144 bytes, `_ZTI16dScMgPachinko2_c` and the four ancestor `_ZTI`), 5
  PARTIAL (the `_ZTS` strings), 0 DIFFERS.
- `python tools/tiers_ratchet.py --check` exit 0: CONVERTED PASS, 3216
  against a banked 3001.
- `python tools/check_tubuild_conflicts.py` exit 0, 303 manifest entries.
- `python tools/queue_audit.py --check-promoted` exit 0: every row's
  `already_promoted` agrees with the manifests.
- `python tools/check_src_tu_compiles.py` exit 0, 303 of 303.
- `python tools/port_refcheck.py` exit 0, 408 checked, 0 stale.
- `python tools/check_dead_references.py` exit 0: no new dead references,
  no broken markdown links.
- `git diff --check 70f055a670..HEAD` exit 0, and `git diff --check` with
  this note exit 0.
- `tubuild linkcheck` was not re-run: it is the pre-promotion step, and the
  object it would link is unchanged.
- Private validation and Source review: not run; they belong to later stages.

### Round 1, on `8ee6e73d9b` and its parents

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
- `python tools/check_dead_references.py`: exit 1 before the ledger edits
  (the seven paths above), exit 0 after.
- `python tools/tiers_ratchet.py --check` exit 0.
- `python tools/check_tubuild_conflicts.py` exit 0.
- `python tools/queue_audit.py --check-promoted`: exit 1 before the ledger
  edits (the unflipped cell), exit 0 after.
- `python tools/check_src_tu_compiles.py` exit 0, 303 of 303.
- `python tools/port_refcheck.py` exit 0, 408 checked, 0 stale.
- `git diff --check 70f055a670..HEAD` exit 0, and `git diff --check` on the
  ledger edits exit 0.
