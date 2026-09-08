# Handoff: prod-kuruma-0907 — ov043/daObjKm1_Kurumajiku_c

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Task `kurumajiku-ov043-0907`, stage `produce` (stage_index 0, mode `write`,
  role producer), session `prod-kuruma-0907`, harness Claude Code. Claimed with
  `tools/classqueue_v2.py claim`; receipt at
  `C:/tmp/fleet-receipts/prod-kuruma-0907.json`. Queue ref
  `refs/heads/agents/coordination`; workflow pin
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- Then task `kurumajiku-ov043-0907b`, stage `revise`, same session, receipt
  `C:/tmp/fleet-receipts/rev-kuruma-0907.json`, input commit `ddd3d8900`. The
  original task was cancelled. The revise stage repaired two coordinator errors
  and changed **nothing** about the class work: the TU moved to the layout the
  landed policy requires, and this rename's five rows were added to the rename
  ledger. The match, the fold, the route and the ROM hash are unchanged, and
  were re-measured rather than assumed — see Proof.
- Branch `cpp/daObjKm1_Kurumajiku_c-tu`, worktree `C:/tmp/sm64ds-kuruma-0907`.
  A second worktree `C:/tmp/sm64ds-kuruma-0907-ctl` on branch
  `ctl/kuruma-0907-baseline` holds the clean base and exists only to run
  controls; it carries no deliverable.
- **Base is `442dc178be177d7556ee6e7787772016ce2a6346`, not the `b7a48b1aa` the
  briefing named.** `b7a48b1aa` is an ancestor three commits behind
  `origin/main`, and the queue task's own `base_commit` and `input_commit` are
  both `442dc178b`. The three intervening commits touch only
  `contributions.json`, `nearmiss/db.jsonl` and `nearmiss/eval_pin.json` — no
  file this class reads — so the drift changed no measurement, but every
  base-versus-head gate below was run against `442dc178b` so the figures match
  what an integrator will reproduce.
- No previous accepted input; this is the first stage on this class under v2.
- Next action: independent verification (stage `verify`, role verifier, a
  different session). No PR was opened and nothing was merged.

## What changed and why

`RickshawBdw` is a coined tree name. The cartridge names this class: the bytes
at ov043 `0x02112350` are the NUL-terminated `21daObjKm1_Kurumajiku_c`, and the
typeinfo record's middle word at `0x0211233c` points at exactly that address.
Under the standing ruling this is a rename-and-promote.

Renamed **in place** in `config/arm9/overlays/ov043/symbols.txt` — five rows,
no alias added and no bounding row added:

| address | was | now |
|---|---|---|
| `0x021113fc` | `_ZN11RickshawBdwD1Ev` | `_ZN21daObjKm1_Kurumajiku_cD1Ev` |
| `0x0211144c` | `_ZN11RickshawBdwD0Ev` | `_ZN21daObjKm1_Kurumajiku_cD0Ev` |
| `0x021114b0` | `_ZN11RickshawBdw16CleanupResourcesEv` | `_ZN21daObjKm1_Kurumajiku_c16CleanupResourcesEv` |
| `0x021114c4` | `_ZN11RickshawBdw13InitResourcesEv` | `_ZN21daObjKm1_Kurumajiku_c13InitResourcesEv` |
| `0x0211238c` | `_ZTV11RickshawBdw` | `_ZTV21daObjKm1_Kurumajiku_c` |

`daObjKm1_Kurumajiku_c_classInit` at `0x021114dc` and both RTTI rows already
carried the ROM name and were not touched.

### The fold is 5, and here is the address derivation

`build/tu_map.json` cuts this run as `ov043/RickshawBdw`, 4 functions,
`0x21113fc-0x21114dc`, and puts the factory in a separate anonymous candidate
`@021114dc-02111518`. Its factory rule matches names ending `_Spawn`, and
nothing in this image ends that way, so the count is a floor.

Derived instead by address from `config/arm9/overlays/ov043/delinks.txt` before
the fold — five entries, each carrying `complete`, each one's `end` equal to the
next one's `start`:

Shards are named by symbol, not by path: the paths are deleted by this commit
and are recorded per function as `legacy_source` in
`config/tu_manifest.d/ov043/daObjKm1_Kurumajiku_c.json`, which is where a
reference to a removed file belongs.

| absorbed shard | .text start | .text end | size |
|---|---|---|---|
| `_ZN11RickshawBdwD1Ev` | `0x021113fc` | `0x0211144c` | `0x50` |
| `_ZN11RickshawBdwD0Ev` | `0x0211144c` | `0x021114b0` | `0x64` |
| `_ZN11RickshawBdw16CleanupResourcesEv` | `0x021114b0` | `0x021114c4` | `0x14` |
| `_ZN11RickshawBdw13InitResourcesEv` | `0x021114c4` | `0x021114dc` | `0x18` |
| the factory shard, `daObjKm1_Kurumajiku_c_classInit` | `0x021114dc` | `0x02111518` | `0x3c` |

`0x50 + 0x64 + 0x14 + 0x18 + 0x3c = 0x11c`, and `0x021113fc + 0x11c =
0x02111518`. No gap and no overlap.

**What bounds it.** Below: `src/game/actors/d_a_obj_km1_ukishima.cpp`, a
different class (`daObjKm1_Ukishima_c`), whose `.text` ends at `0x021113fc`.
Above: `src/game/actors/d_a_obj_km1_kuruma.cpp`, a different class
(`daObjKm1_Kuruma_c`), whose `.text` begins at `0x02111518`. Both are already
promoted with their own manifests, so both ends are a neighbouring class's
first member, not a section boundary — the enclosing `.text` section is
`0x021111a0..0x021117fc` and this run touches neither end of it.

The factory join is registry-backed, not adjacency alone: `g_profile_KM1_KURUMAJIKU`
at `0x02112368` holds `0x021114dc` in its first word.

### The name trap, resolved from the ROM

`src/_ZN17daObjKurumajiku_cD1Ev.cpp` is a **different class and a different
overlay**. `config/arm9/overlays/ov002/symbols.txt` puts
`_ZN17daObjKurumajiku_cD1Ev` at `0x020b6a3c` and `ov002/delinks.txt` owns its
run; nothing of it lies in `0x021113fc..0x02111518`. It was not folded in.

It is, however, this class's **BASE**, which the briefing did not say.
`_ZTI21daObjKm1_Kurumajiku_c` at `0x02112338` reads
`{0x0209a764, 0x02112350, 0x021092f8}` — an `__si_class_type_info` with one
base at subobject offset 0, and `0x021092f8` is `_ZTI17daObjKurumajiku_c` in
ov002. Corroborated independently of the RTTI: both destructors store **three**
vptrs, `0x0211238c` then `0x02109320` then `0x0210ae38`, read out of the
literal pools at `0x02111440` and `0x021114a0`. A direct `dBgActor_c` subclass
stores two. The header already carried this base clause and it is confirmed,
not changed.

### Route: text-only, and why

`.text 0x021113fc..0x02111518` only. `.data` was **not** claimed. No delinks
entry in ov043 reaches `0x02112338..0x0211240c`, so this class's typeinfo,
type-name string, profile and vtable sit in unclaimed `.data`; the only ov043
`.data` claim anywhere is `d_a_obj_km1_dorifu.cpp` over
`0x021124dc..0x021125dc`. An intact-object claim would have to carve that band
out first, and the band also carries three vtable-interior phantom rows
(`data_ov043_021123e4`, `_021123f4`, `_02112404`). Both landed ov043 siblings
took text-only. I did not attempt the intact-object route and make no claim
about it.

Text-only costs nothing here: `rombuild` still proves all four of this class's
data symbols against the cartridge through the compiler-only policy — see Proof.

### The style oracle actually used

The briefing looked for `daObjKm1_Ukishima_c` under the `src/actors/`
directory, did not find it there, and concluded that what exists is a shard.
**Both halves of that are wrong.** `src/game/actors/d_a_obj_km1_ukishima.cpp` is
a genuinely promoted TU — `config/tu_manifest.d/ov043/daObjKm1_Ukishima_c.json`
reads `"status": "promoted"` with 7/7 functions matched and a full verification
block. The ov043 family simply lives in the `src/game/actors/` directory under
snake-case `d_a_obj_km1_<name>.cpp` stems, not as `<Class>.cpp` under
`src/actors/`.

I used two better oracles than the briefing's fallbacks:

- **`src/game/actors/d_a_obj_km3_kurumajiku.cpp`** (ov047) — the same class's
  twin: same base `daObjKurumajiku_c`, same size `0x330`, same five-member fold,
  and identical member sizes (`InitResources` `0x18`, `CleanupResources` `0x14`,
  `classInit` `0x3c`). This is where the shape came from.
- **`src/game/actors/d_a_obj_km1_kuruma.cpp`** (ov043) — the immediately
  adjacent class in the same overlay, same route (text-only), same
  compiler-only policy shape. This is where the `operator new` spelling came
  from.

`ov102/daBmb_c` was used only as a control, not copied.

### The factory spelling, which was a real decision

Written first the way the ov047 twin writes it — a raw `_ZN7fBase_cnwEj(0x330)`
call plus two hand-rolled vptr stores — it byte-matched 5/5. But
`tiers_ratchet --check` then failed: the `classInit` member fails the "Calls
things by real names, not mangled `_Z`" criterion, which makes the whole fold a
member regression and would have needed four `converted-backslide-exceptions`
rows.

Rewritten as `return new daObjKm1_Kurumajiku_c();` over an inline
`operator new` in the header — the pairing `include/daObjKm1_Kuruma_c.h` uses —
it still byte-matches 5/5, and all five members are now independently
CONVERTED. This is legal here only because the leaf overrides both of the
abstract base's pure virtuals and is therefore concrete. Result:
`tiers_ratchet` reports PASS with a clean ownership transition, `config/converted-baseline.json`
is untouched, and no backslide exception was added. The readability tier went
**up** by 1.

## Proof

`tools/tubuild.py verify ov043/daObjKm1_Kurumajiku_c`:

```
MATCH _ZN21daObjKm1_Kurumajiku_cD1Ev                 0x021113fc  0x050
MATCH _ZN21daObjKm1_Kurumajiku_cD0Ev                 0x0211144c  0x064
MATCH _ZN21daObjKm1_Kurumajiku_c16CleanupResourcesEv 0x021114b0  0x014
MATCH _ZN21daObjKm1_Kurumajiku_c13InitResourcesEv    0x021114c4  0x018
MATCH daObjKm1_Kurumajiku_c_classInit                0x021114dc  0x03c

byte comparison   : 5/5 MATCH
objisolate check  : clean
reloc-destinations: clean
emission order    : all 5 in expected ROM-ascending section order
-> TEXT-VERIFIED, 0 unlicensed symbols
```

`tools/rombuild.py -j16`: **106/106 modules exact**, 11,192 source-built
functions reproducing, **0 mismatching**; ROM sha256
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.

**References were re-derived from the cartridge, not from `relocs.txt`**, because
`match.py` wildcards every relocated word:

| site | cartridge evidence | resolves to |
|---|---|---|
| `CleanupResources` tail call | literal at `0x021114bc` | `0x020b6ac8` = `func_ov002_020b6ac8` |
| `InitResources` tail call | literal at `0x021114d4` | `0x020b6c54` = `func_ov002_020b6c54` |
| both resource methods, arg 1 | literals `0x021114c0` / `0x021114d8` | `0x02112344` = `data_ov043_02112344` |
| `InitResources` arg 2 | `0x021114cc` = `mov r2, #0x88` | profile ID of `KM1_KURUMA`, the cart this axle drives |
| `classInit` allocation | `0x021114e0` = `mov r0, #0x330` | class size, matching the RTTI-proven `0x330` |
| `classInit` calls | `BL` at `0x021114e4` / `0x021114f0` | `0x02043444` = `_ZN7fBase_cnwEj`, `0x020eea50` = `_ZN10dBgActor_cC2Ev` |
| both destructors, 3 vptrs | pools at `0x02111440`, `0x021114a0` | `0x0211238c`, `0x02109320` (`_ZTV17daObjKurumajiku_c`), `0x0210ae38` (`_ZTV10dBgActor_c`) |

**The vtable claim was checked three ways, not honoured on assertion.** The
words were read out of `extracted/overlays/overlay_0043.bin`: `0x02112384` is
the offset-to-top `0`, `0x02112388` is `0x02112338` (this class's `_ZTI`), so
`0x0211238c` is the address point and the table is 32 slots ending at
`0x0211240c` where `_ZTI17daObjKm1_Kuruma_c` begins. Slot 0 = `0x021114c4`
(InitResources), slot 3 = `0x021114b0` (CleanupResources), slots 16/17 =
`0x021113fc`/`0x0211144c` (D1 then D0) — every one inside this run. Slots 6 and
9 hold `0x020b6b38` and `0x020b6b10`, both **ov002** and both
`daObjKurumajiku_c`'s own Behavior and Render, so they are inherited and are
deliberately not redeclared. Member offsets the bodies touch (`+0x124`
dBgW_KcMbg, `+0xd4` Model) lie inside `dBgActor_c`, consistent with a class that
adds no fields.

**`rombuild` byte-verifies this class's data even on the text-only route.** All
four appear in `romData.verifiedSymbols`: `_ZTI21daObjKm1_Kurumajiku_c` (12 B),
`_ZTS21daObjKm1_Kurumajiku_c` (24 B), `g_profile_KM1_KURUMAJIKU` (28 B) and
`_ZTV21daObjKm1_Kurumajiku_c` (128 B = the 32 slots). So the vtable's call
targets are proven byte-identical to the cartridge, which is the thing a byte
match on `.text` alone cannot establish.

Every edit was asserted separately from the match: the symbols.txt rewrite
asserts exactly one substitution per row and that no `RickshawBdw` remains; the
delinks fold re-parses the five spans, asserts each carries `complete`, asserts
the run tiles with no gap and asserts the resulting bounds are
`0x021113fc..0x02111518`.

## Gate table

Re-run in full after the revise stage's path move and ledger append. Every
figure below is from that re-run, not carried over from the produce stage.

| gate | verdict | control |
|---|---|---|
| `tubuild verify` | **5/5 MATCH**, objisolate clean, reloc-destinations clean, TEXT-VERIFIED | — |
| `rombuild -j16` | **PASS**, 106/106 exact, sha256 matches | — |
| `romdata_check` | **pass** | pass on clean base |
| `validate_merge --base 442dc178b` | **pass**, +0 byte-verified, credit 0 added/0 changed/**0 lost**, 106/106 exact, 700 data symbols exact | — |
| `premerge_check --base 442dc178b` | **pass**, 8/8 gates ok, `-4` entries read as consolidation | — |
| `check_tubuild_conflicts` | **pass** | pass |
| `check_src_tu` | **pass** | pass |
| `check_src_tu_compiles` | **pass**, 162/162 TUs compile | — |
| `check_dead_references` | **pass** | pass |
| `check_rename_ledger` | **pass**, `checked 2010` (was 2005 — the 5 new rows), 0 findings | pass |
| `check_profile_campaign` | **pass**, 786 live coined claims, 0 diverging | — |
| `check_duplicate_sources` | **pass**, 9144 stems, none doubled | — |
| `check_header_offsets include/daObjKm1_Kurumajiku_c.h` | **pass**, struct spans `0x330`, 0 mismatched | — |
| `layout_check` | **pass** | pass |
| `langmode_audit` | **pass** | pass |
| `port_refcheck` | **pass** | pass |
| `cpp_tu_compat --require-ready` | **pass** | pass |
| `tiers_ratchet --check` | **pass**, 4 clean ownership transitions, +8 gained | pass |
| `cpp_tu_state --check-note` | **pass** after `--write-note` | **red on the clean base**; regenerating there alone moves 15 lines |
| `queue_audit --check` | red | **red identically on the clean base** — byte-for-byte the same counts |
| `tubuild linkcheck` | red | **red identically on landed `ov102/daBmb_c`** |
| `linkcheck.py --name` (×5) | could not run | **same failure on the clean base** |
| `prepush_attribution` | red, 2 lost | **red on the landed `daBmb_c` commit, 13 lost** |

Two gates emit an expected *warning* while still passing, and both are the fold
speaking, not a defect:

- `validate_merge` warns that **5 address ranges left the byte-verified set
  while enrolled totals held steady** — `0x021113fc-0x0211144c`,
  `0x0211144c-0x021114b0`, `0x021114b0-0x021114c4`, `0x021114c4-0x021114dc`,
  `0x021114dc-0x02111518`. Those are the five per-shard `complete` entries,
  replaced by the single entry `0x021113fc-0x02111518` covering the identical
  `0x11c` bytes. The range *keys* changed; the coverage did not, which is what
  the `+0` on byte-verified functions and code bytes says.
- `premerge_check` reports `entries: 9011 -> 9007 (-4)` and labels it
  "consolidation, not a loss -- bytes are flat" itself.

### Each non-green, paired with the control I ran

- **`queue_audit --check`** — red on my branch and red on the clean base with
  identical output: `already_promoted 4, compiler-only 3, shard_count 4,
  total_lines 4, unmatched 1`. Re-run after the revise stage and `diff`ed
  against a fresh control run in the clean worktree at `442dc178b`: **byte-for-byte
  identical**, and no line mentions this class. Foreign rows, unchanged by me. This class's own
  row **was** refreshed: `RickshawBdw 4 79 … no` became
  `daObjKm1_Kurumajiku_c 1 157 … yes` with the blocker re-derived. I did not run
  `--write` broadly — a bare `--write` rewrites 49 foreign rows and renames 8
  keys, so I ran it, took only my line, restored the file from a pre-image and
  patched that one line back. `git diff` on the file is exactly `1 insertion, 1
  deletion`.
- **`cpp_tu_state --check-note`** — stale on main. Regenerating with
  `--write-note` **on the clean base alone** moves 15 lines; that drift is
  main's. Diffing my regenerated note against the control's regenerated note
  isolates my contribution exactly: CONVERTED functions `2704 -> 2705`, manifest
  entries `161 -> 162`, `promoted 131 -> 132`, functions named by manifest
  `2973 -> 2978` (+5), multi-function owners `132 -> 133`, tracked sources
  `9148 -> 9144` (five shards out, one TU in). Every figure moves by exactly what
  this change accounts for. The regenerated note is committed.
- **`tubuild linkcheck`** — refuses with `intact production requires one .text
  claim and at least one non-text claim`. Reproduced by me on landed
  `ov102/daBmb_c` in the clean worktree: same message, same exit 1. That run
  also failed `ov036/daObjRcCarpet_c` and `ov070/daPropeller_Heyho_c` on a
  related vtable-partition refusal, so this is a tree-wide post-promotion
  condition. Not this class's.
- **`linkcheck.py --name`** — cannot run anywhere in this environment: it reads
  `progress/matched.jsonl`, and `progress/` is gitignored and absent from the
  main checkout and both worktrees. Same `FileNotFoundError` on the clean base.
  The coverage it would have given is supplied instead by `rombuild`, which links
  the real ROM and reports 0 mismatching functions and 106/106 exact modules.
- **`prepush_attribution`** — 2 CREDIT LOST, `d_a_obj_km1_kurumajiku` and
  `_ZN11RickshawBdwD1Ev`, both `tangosdev`. Control: the **already-merged**
  `daBmb_c` promotion reports **13 lost** under the same command. The key is a
  basename, so any shard whose basename does not survive as a symbol name reads
  as lost. Note that `validate_merge` — the gate that actually decides the merge
  — computes `0 added, 0 changed, 0 lost` for this branch. I added the five
  `src/game/actors/d_a_obj_km1_kurumajiku.cpp#<symbol>` override entries so per-member
  credit survives the fold in the durable record, including `lunavyqo`'s
  `CleanupResources`. I did **not** add override keys under the dead
  `_ZN11RickshawBdw*` symbol names, which would have silenced the gate by
  asserting a symbol lives in a file that does not define it.

  **The two lost entries cannot be recovered by rename detection, and that is
  now measured rather than assumed.** The revise stage's own move is recorded by
  git as a clean `R100` onto
  `src/game/actors/d_a_obj_km1_kurumajiku.cpp`, so that lineage is intact.
  But `git diff -M` from the base finds **zero**
  rename pairs among the five absorbed shards, and lowering the threshold to
  `-M50%`, `-M30%`, `-M20%` and even `-M10%` still finds zero. A 5-into-1 fold
  leaves no single shard similar enough to the TU for git to pair, so no commit
  split and no `git mv` ordering can attach those two. The `attribution.json`
  overrides are the only mechanism that carries this credit, and they are in
  place.

## Judgement calls the briefing did not cover

1. **The base was three commits stale.** Used the queue's `442dc178b`.
2. **Output path — REPAIRED in the revise stage, and I was wrong to call it
   cosmetic.** The produce stage shipped the TU as
   `daObjKm1_Kurumajiku_c.cpp` under `src/actors/`, because the task's
   `produces` declared that path and `classqueue_v2.publish` validates against it
   (`classqueue_v2.py:477`), so the queue's path was binding. I flagged the
   family inconsistency but judged it cosmetic. It is not: #1055 decided the
   layout and #2270 executed it, moving 132 files to `src/game/actors/` with
   stems renamed to `d_a_<snake>`, so the old path contradicted landed policy.

   The revise stage moved it to `src/game/actors/d_a_obj_km1_kurumajiku.cpp`
   with `git mv`, so lineage reads as a rename rather than a delete plus an add.
   The strongest argument is in `delinks.txt`, which now lists the family as
   three consecutive entries tiling one address range:

   ```
   src/game/actors/d_a_obj_km1_ukishima.cpp    .text 0x021111a0..0x021113fc
   src/game/actors/d_a_obj_km1_kurumajiku.cpp  .text 0x021113fc..0x02111518
   src/game/actors/d_a_obj_km1_kuruma.cpp      .text 0x02111518..0x02111630
   ```

   The two classes that bound this run are its neighbours in the file as well as
   in the ROM. The old path filed it away from the family it was cut out of.
   Note `src/actors/` still holds 39 other classes, so the migration is partial
   and the wrong path looked plausible — but every ov043 `km1` sibling was
   already in `src/game/actors/`, which settles it for this class.
3. **`daObjKurumajiku_c` is the base, not merely a trap.** Recorded above.
4. **The rename ledger already records a one-class shift on this exact stem, and
   it is not this run.** `symbols/actor_renames.tsv` carries nine rows mentioning
   `RickshawBdw`. Five are a *repair*: at `0x02111518`, `0x02111568`,
   `0x021115cc`, `0x021115e0` and `0x0211245c` they read `func_ov043_… ->
   _ZN17daObjKm1_Kuruma_c…` with notes `(was _ZN11RickshawBdwD1Ev)` and friends.
   That is `tools/actor_names.py`'s old `parse_spawnfunc` defect — it scanned
   past a factory's body into the *next* class's D1 and shifted every vtable and
   method name one class late — fixed on main, with those rows corrected to
   `daObjKm1_Kuruma_c`.

   This is corroboration, not trivia: `0x02111518` is exactly where my fold's
   upper bound lands, and the ledger independently attests that it is a different
   class. It also means the coined `RickshawBdw` stem has a *documented history*
   of being anchored one class too high in this overlay, which anyone
   re-deriving these bounds should expect to meet.

   Only **two** rows put `RickshawBdw` in column 4: `RickshawBdw_Spawn`
   (`0x021114dc`) and `RickshawBdw_SpawnInfo` (`0x02112368`). Both sit on *this*
   class's real addresses, both are already superseded by a following row
   (`daObjKm1_Kurumajiku_c_classInit`, `g_profile_KM1_KURUMAJIKU`), and both are
   coined spellings `check_rename_ledger` counts as out of scope. So ov043's
   coined rows do **not** carry the shift that checker's docstring warns about
   for ov009.

   **Five rows for this rename were added in the revise stage** — one per
   renamed address, in the five-column `module / addr / old / new / why` shape
   `actor_names.py` emits, column 3 the name the base commit actually carried
   and column 4 the name `symbols.txt` carries now:

   | addr | old | new |
   |---|---|---|
   | `0x021113fc` | `_ZN11RickshawBdwD1Ev` | `_ZN21daObjKm1_Kurumajiku_cD1Ev` |
   | `0x0211144c` | `_ZN11RickshawBdwD0Ev` | `_ZN21daObjKm1_Kurumajiku_cD0Ev` |
   | `0x021114b0` | `_ZN11RickshawBdw16CleanupResourcesEv` | `_ZN21daObjKm1_Kurumajiku_c16CleanupResourcesEv` |
   | `0x021114c4` | `_ZN11RickshawBdw13InitResourcesEv` | `_ZN21daObjKm1_Kurumajiku_c13InitResourcesEv` |
   | `0x0211238c` | `_ZTV11RickshawBdw` | `_ZTV21daObjKm1_Kurumajiku_c` |

   The `why` column keeps the tool's `vtable slot N` / `vtable alloc=` shape and
   the `(was …)` suffix, and adds the ROM evidence for the rename. The vtable
   row carries `alloc=0x330`, which is a true claim about the class that
   actually lives at `0x0211238c` — the factory loads `0x330` at `0x021114e0`
   and `include/daObjKm1_Kurumajiku_c.h` asserts that size — rather than the
   `alloc=?` the checker substitutes when a figure has come loose from its
   class. `check_rename_ledger` goes from `checked 2005` to `checked 2010`, 0
   findings; `check_profile_campaign` stays green (786 live coined claims, 0
   diverging).

   **No tool in this tree appends a ledger row, and I checked all four before
   editing by hand.** This matters, because the instruction was to add them
   *through* the rename tooling:
   - `tools/actor_names.py` is the file's original generator but opens it
     `"w"` and rewrites it wholesale from its own proposals. Per
     `check_rename_ledger`'s own docstring, `propose()` now refuses any address
     that already carries a real name, so a fresh run emits **1 row instead of
     2,573**. Running it would destroy the ledger.
   - `tools/class_rename.py` is the right tool for a class rename and does
     rewrite `.tsv` under `symbols/` with length-aware mangled substitution.
     But dry-run against `symbols/` shows it would edit **5 lines and add
     none**, and those 5 lines are the `(was _ZN11RickshawBdw…)` notes on
     **`daObjKm1_Kuruma_c`'s** rows at `0x02111518`–`0x0211245c`. It would
     rewrite them to `(was _ZN21daObjKm1_Kurumajiku_c…)` — asserting those
     symbols once carried a name they never carried, destroying the off-by-one
     record described above, and touching addresses outside this run. **Do not
     run it for this class.**
   - `check_rename_ledger.py --fix` only corrects column 4 on rows that already
     exist; it cannot add an address the file does not mention.
   - `cpp_rename.py` and `cpp_index.py` are readers.

   That is what the checker's docstring means by "the file has stopped being a
   reproducible artifact and is now maintained in place": the ledger's
   maintenance path is a hand edit in the generator's shape, with
   `check_rename_ledger` as the guard. The rows above were written that way and
   verified with the guard.
5. **Two tooling behaviours worth knowing.** `check_dead_references` reads the
   **git index**, not the working tree: a fix is invisible to it until staged.
   And `git add` renormalizes CRLF, which re-stamps `config/**/delinks.txt` and
   makes `queue_audit` declare `build/tu_map.json` stale — regenerate the map
   *after* the final `git add`, not before.
6. **`/tmp` splits between bash and Python on this machine.** A verification
   script read `C:\tmp\q-before.tsv` (a file from two days earlier) where bash had
   written `/tmp/q-before.tsv`, and reported a 237-line diff that did not exist.
   `git diff` is the authority; the real change was one line.

## Not done, deliberately

- No PR opened, nothing merged, no `--delete-branch`.
- `.data` not claimed; no intact-object attempt.
- No `tiers_ratchet --update`.
- No broad `queue_audit --write`.
- No alias or bounding row added to any `symbols.txt` — the five rows were
  rewritten in place.
- `tools/actor_names.py` and `tools/class_rename.py --apply` were **not** run.
  Reasons under judgement call 4; both would have damaged the ledger.
- Neither worktree removed. `git worktree remove` was never run.
