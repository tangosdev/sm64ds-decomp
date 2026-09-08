# Handoff: prod-ukishima-0907 — ov045/daObjKm2_Ukishima_c

## Identity and resumption

| field | value |
|---|---|
| queue task | `ukishima-ov045-0907`, stage `produce`, role `producer` |
| session | `prod-ukishima-0907` |
| receipt | `C:/tmp/fleet-receipts/prod-ukishima-0907.json` |
| worktree | `C:/tmp/sm64ds-ukishima-0907` (leave it; tear down only with `wt-remove.ps1`) |
| branch | `cpp/daObjKm2_Ukishima_c-tu` |
| claimed input commit | `fd55979f074c7ad796e2846328933363315e7f06` |
| **base as shipped** | **`b0d348e536fb3304b6ab12d88fe95d780cbeb91d`** — see "The base moved" below |
| commits | `047948906` (the promotion), `6e8ab81e0` (attribution overrides) |
| route | text-only |
| no PR was opened, nothing was merged | |

## What changed and why

The cartridge names this class. Five symbols were renamed in place in
`config/arm9/overlays/ov045/symbols.txt`; **no alias or bounding row was added.**

| address | was | now |
|---|---|---|
| `0x02111b14` | `_ZN16FloatingFloorBfsD1Ev` | `_ZN19daObjKm2_Ukishima_cD1Ev` |
| `0x02111b64` | `_ZN16FloatingFloorBfsD0Ev` | `_ZN19daObjKm2_Ukishima_cD0Ev` |
| `0x02111bc8` | `_ZN16FloatingFloorBfs16CleanupResourcesEv` | `_ZN19daObjKm2_Ukishima_c16CleanupResourcesEv` |
| `0x02111bdc` | `_ZN16FloatingFloorBfs13InitResourcesEv` | `_ZN19daObjKm2_Ukishima_c13InitResourcesEv` |
| `0x02112f50` | `_ZTV16FloatingFloorBfs` | `_ZTV19daObjKm2_Ukishima_c` |

`daObjKm2_Ukishima_c_classInit` at `0x02111bf4` already carried the ROM name and
was not renamed.

### The fold is 5, and here is the address derivation

Derived **by address** from `config/arm9/overlays/ov045/delinks.txt`, not from
`build/tu_map.json`. The five legacy entries tile the run end-to-start with no gap:

| absorbed shard | .text start | .text end | size |
|---|---|---|---|
| `_ZN16FloatingFloorBfsD1Ev` | `0x02111b14` | `0x02111b64` | `0x50` |
| `_ZN16FloatingFloorBfsD0Ev` | `0x02111b64` | `0x02111bc8` | `0x64` |
| `_ZN16FloatingFloorBfs16CleanupResourcesEv` | `0x02111bc8` | `0x02111bdc` | `0x14` |
| `_ZN16FloatingFloorBfs13InitResourcesEv` | `0x02111bdc` | `0x02111bf4` | `0x18` |
| the factory shard, `daObjKm2_Ukishima_c_classInit` | `0x02111bf4` | `0x02111c30` | `0x3c` |

`0x50 + 0x64 + 0x14 + 0x18 + 0x3c = 0x11c`, and `0x02111b14 + 0x11c = 0x02111c30`
exactly. Each entry's `end` is the next entry's `start`, so the run is contiguous.

**Both bounds are a different class:**

- **below** — `src/d_a_obj_km2_nobiru.c`, whose `.text` ends at `0x02111b14`
  (the `daObjKm2_Nobiru_c` / `ExtendingPlatform` family factory);
- **above** — `src/game/actors/d_a_obj_km2_gura.cpp`, whose `.text` begins at
  `0x02111c30` (an already-promoted sibling in this overlay).

`build/tu_map.json` reports **4**, not 5, and orphans the factory: its factory
rule matches names ending `_Spawn`, which labels **zero** functions in the whole
image, so every `_classInit` is detached from its class and any `shard_count`
involving one is a floor.

### Route: text-only, and why

`ov045`'s `.data` is unclaimed across `0x02112efc..0x02112fd0` — the whole band
holding this class's `_ZTI`, the three-word resource descriptor, `_ZTS`,
`g_profile_KM2_UKISHIMA` and `_ZTV`. The only `.data` claim anywhere in ov045's
delinks is gura's `0x02112fdc..0x021130a0`, which starts *past* gura's own
typeinfo record.

An intact-object claim would therefore have to carve that band out first, and the
band carries two vtable-interior phantom rows — `data_ov045_02112f58` and
`data_ov045_02112f7c`, both marked `ambiguous`, both *inside* `_ZTV` — plus the
`ambiguous` `g_profile_KM2_UKISHIMA` row. The landed ov045 sibling
`d_a_obj_km2_fall_block.cpp` is text-only for the same reason. Text-only here is a
**scope** decision, not a codegen limitation.

### Oracle used

`src/game/actors/d_a_obj_km2_gura.cpp` for overlay house style (it supplied the
profile-struct field spelling, since the two profiles are neighbours in the same
`.data` band). The ov043 class `daObjKm1_Kurumajiku_c` — identical member-size
signature `0x50/0x64/0x14/0x18/0x3c` — supplied the TU shape, manifest shape and
the factory spelling. `d_a_obj_km2_fall_block.cpp` supplied the text-only
precedent.

### The factory spelling, which was a real decision

`return new daObjKm2_Ukishima_c();`, over an inline `static void *operator new`
in the header that forwards to `_ZN7fBase_cnwEj`. Legal because the leaf overrides
**both** pure virtuals its abstract base leaves (`InitResources` slot 0,
`CleanupResources` slot 3) — checked, not assumed: `include/daObjUkiyuka_c.h`
declares them `virtual s32 ... = 0`, `s32` is `signed int`, and the leaf's `s32`
declarations override them.

This is not cosmetic. The inline `operator new` is what keeps the allocation
bound to the cartridge's `fBase_c::operator new`; without it a plain `new`
relocates to the unavailable global `_Znwm`, which is why
`d_a_obj_km2_fall_block.cpp` kept a hand-rolled C allocator veneer. And because
nothing in the TU names a mangled `_Z` symbol for the allocation, all five members
clear the ratchet's "no mangled `_Z`" criterion and are independently CONVERTED —
`tiers_ratchet --check` reports **4 clean ownership transitions, +8 gained, and
zero backslide exceptions were needed.**

## Proof, read from the cartridge

Every call target and vptr store was re-derived from
`extracted/overlays/overlay_0045.bin` (ov045 base `0x021111a0`) and its literal
pools, **not** from `relocs.txt` — `match.py` wildcards every relocated word.

| site | cartridge evidence | resolves to |
|---|---|---|
| class name | `0x02112f14` = NUL-terminated `19daObjKm2_Ukishima_c` | `_ZTI` middle word `0x02112f00` points at it |
| base class | `_ZTI+8` at `0x02112f04` = `0x02109104` | `_ZTI14daObjUkiyuka_c` (ov002) |
| `CleanupResources` tail call | literal at `0x02111bd4` | `0x020b6424` = `func_ov002_020b6424` |
| `InitResources` tail call | literal at `0x02111bec` | `0x020b6584` = `func_ov002_020b6584` |
| both resource methods, arg 1 | literals `0x02111bd8` / `0x02111bf0` | `0x02112f08` = `data_ov045_02112f08` |
| `InitResources` arg 2 | `0x02111be4` = `mov r2, #0xf50` | actor id `0xf50` |
| `classInit` allocation | `0x02111bf8` = `mov r0, #0x32c` | 812, the RTTI-proven class size |
| `classInit` calls | `BL` at `0x02111bfc` / `0x02111c08` | `0x02043444` = `_ZN7fBase_cnwEj`, `0x020eea50` = `_ZN10dBgActor_cC2Ev` |
| `classInit`, 2 vptr stores | pool `0x02111c28`/`0x02111c2c` | `0x0210912c` (`_ZTV14daObjUkiyuka_c`), then `0x02112f50` (own) |
| both destructors, 3 vptrs | pools at `0x02111b58`, `0x02111bb8` | `0x02112f50` (own), `0x0210912c`, `0x0210ae38` (`_ZTV10dBgActor_c`) |
| `D0` deallocation | `0x02111bc4` = `0x020a0eac` | `GAME_HEAP_PTR`, then `_ZN6Memory10DeallocateEPvP4Heap` — the inherited inline `operator delete` |

### The vtable claims are measured, not hypothesised

The shard comments asserted "vtable slot 0" and "vtable slot 3" and a three-vptr
chain. All were read back out of the ROM and all hold. The address point is
`0x02112f50` (`0x02112f48` = offset-to-top `0`, `0x02112f4c` = `0x02112efc` =
`&_ZTI19daObjKm2_Ukishima_c`); the table is **32 slots**, `0x02112f50..0x02112fd0`,
ending exactly where `_ZTI15daObjKm2_Gura_c` begins. Slot 0 = `0x02111bdc`, slot 3
= `0x02111bc8`, slots 16/17 = `0x02111b14`/`0x02111b64`. Slots 6 and 9 hold
`0x020b6494` and `0x020b646c` — `daObjUkiyuka_c`'s own ov002 `Behavior` and
`Render` — so they are **inherited and must not be redeclared**.

Independently of that reading, `romdata_check` compiles this TU and compares its
emitted data to the cartridge: `_ZTV19daObjKm2_Ukishima_c` verifies **byte-equal
at 128 bytes** (= 32 slots), `g_profile_KM2_UKISHIMA` at 28 bytes, and the whole
`_ZTI` chain (`fBase_c`, `dBase_c`, `dActor_c`, `dBgActor_c`, `daObjUkiyuka_c`,
this class). **8 VERIFIED, 6 PARTIAL, 0 DIFFERS** for this file.

## Gate table

All figures below are from the **rebased** branch on base `b0d348e53`.

| gate | verdict | control |
|---|---|---|
| `tubuild verify` | **5/5 MATCH**, objisolate clean, reloc-destinations clean, emission order ROM-ascending → TEXT-VERIFIED | perturbing `0xf50`→`0xf51` gives `4/5 MATCH`, `DIFF` on `InitResources` |
| `tubuild linkcheck` | **REFUSED** — "intact production requires one .text claim and at least one non-text claim" | **not ours:** identical refusal on the landed, untouched `ov045/daObjKm2_Fall_Block_c`; it is a text-only-route property |
| `linkcheck.py --name` ×5 | **VERIFIED**, `blind=0`, no diffs, on all five members | same perturbation returns `NO-REPRO` |
| `rombuild -j16` | **PASS**, 106/106 modules exact, 100.000000% of compared bytes, 11,192 functions reproducing / **0** mismatching | — |
| ROM sha256 | `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8` — **hashed off `build/sm64ds.nds` directly**, not read from the build log | — |
| `romdata_check` | **exit 0**; 706 verified / 224 partial / 4 differ tree-wide; this TU **0 differ** | the 4 differing are `_ZTV10dCcAcPos_c` and `_ZTV7daKrb_c`, in files this branch never touched |
| `validate_merge --base b0d348e53` | **exit 0**; byte-verified `+0`, module fidelity 106/106, **credit 0 added / 0 changed / 0 lost** | before the attribution commit the same invocation reported **5 changed** — the overrides are load-bearing |
| `premerge_check --base b0d348e53` | **exit 0**, 8/8 gates `pass → pass`, nothing green→red | — |
| `langmode_audit` | exit 0 | — |
| `check_rename_ledger` | exit 0, **2031** mangled/vtable rows all agree with symbols.txt | corrupting one of my rows to `...D9Ev` gives exit 1 naming `actor_renames.tsv:3539 ov045 0x02111b14` |
| `check_tubuild_conflicts` | exit 0, 162 manifest entries | — |
| `check_src_tu` | exit 0, every reference resolves | — |
| `layout_check` | exit 0, clean | — |
| `check_dead_references` | exit 0, no new dead references | **found a real one first** — see below |
| `check_duplicate_sources` | exit 0, no stem doubled | — |
| `cpp_tu_compat --require-ready` | exit 0, all nine facets READY | — |
| `port_refcheck` | exit 0, 423 references resolve | — |
| `queue_audit --check` | **exit 0**, "queue agrees with the tree" | on the old base it had 4 foreign disagreements; `b0d348e53` fixed them. Reverting my row alone raises `compiler-only` 3→4, proving the check sees it |
| `tiers_ratchet --check` | **PASS**, baseline 2701 → current 2705, +8 gained, 4 clean ownership transitions | `--check` only; `--update` was never run |
| `prepush_attribution` | **exit 1**, 1 changed / 1 lost | **not ours:** identical shape (1 changed, 1 lost, same `D1` member) on the verified `cpp/daObjKm1_Kurumajiku_c-tu` precedent |

## Things worth flagging to the next stage

1. **The base moved under this lane.** `origin/main` went `fd55979f0` →
   `b0d348e53` mid-run (three commits: the ov022/ov013 batch-2 promotions, a
   progress refresh, and the denominator-gate carve-out). Settled with
   `git merge-tree`, not a file list: the file **sets** overlapped in five paths,
   of which two actually conflicted. `notes/cpp-tu-current-state.md` was
   **regenerated**, not hand-resolved. `symbols/actor_renames.tsv` is an append
   log, so both sides' appends were kept — main's 21 rows then my 5;
   `check_rename_ledger` going 2005 → 2031 checked rows is the arithmetic proof
   the union survived. **The branch is rebased and merges clean; every gate figure
   above was re-run after the rebase.**

2. **`git mv` did not carry credit, and could not have.** The brief expected
   lineage to follow the rename. It does not: both files are rewritten in the
   same commit, so git pairs neither even at `--find-renames=30%`, and
   `prepush_attribution` says as much ("A commit may rewrite a file, or move it —
   not both"). What actually preserves credit is the five per-member
   `attribution.json` overrides in `6e8ab81e0`, and `validate_merge` proves it:
   **5 changed without them, 0 changed with them.** Any future fold of this shape
   should plan on the overrides rather than on `git mv`.

3. **`check_dead_references` caught me.** My first header draft cited
   the `daObjKm1_Kurumajiku_c` header by path — a real file, but one that
   exists only on the unlanded precedent branch. The gate flagged it correctly. The trap is
   slightly wider than "retired paths": **any** repo-rooted path that does not
   resolve *in this tree* fails, including one that is merely not landed yet.
   Rewritten to name the class rather than the path.

4. **The header's own prose was wrong about the vtable and is corrected.** The
   retired `FloatingFloorBfs.h` under `include/` said the typeinfo record "sits at V-4".
   The *pointer* to it sits at V-4 (`0x02112f4c`); the record itself is at
   `0x02112efc` and the vtable's storage starts at V-8 (`0x02112f48`). The new
   header states the measurement.

5. **The `validate_merge` warning is accounting, not loss.** It names five
   address ranges that "left the byte-verified set" — the five old delinks
   entries, now replaced by the single `0x02111b14..0x02111c30` entry covering the
   same span. Byte-verified functions and bytes are both `+0`, and
   `premerge_check` labels the matching source-coverage move `-4` as
   "consolidation, not a loss — bytes are flat".

6. **Neither `actor_names.py` nor `class_rename.py` was run.** The five ledger
   rows were hand-edited in the generator's five-column shape.

7. **`config/converted-baseline.json` was deliberately left alone.** The four
   shard paths still listed there are handled by the ratchet's ownership-transition
   logic, which reports them as MOVED with nothing readable lost. The precedent
   did the same.
