# Handoff: object-marker typing

> **STATUS: STALE — verify before use.** The **767 bare markers remaining** headline
> is now ~20x stale: `python tools/marker_census.py` reports 19 bannered headers and
> **37** bare markers remaining (measured 2026-09-05), not 767. The typing programme
> this handoff tracks is effectively finished; do not carry forward the 298/767/235
> counts anywhere in this file. The D1-vs-D2 soundness argument (§2 — why
> D1-and-never-D2 proves a member's declared type) and the five traps in §7
> (Capstone condition-code mnemonic collisions, "wrong-place scan reports like a
> clean scan", verify a merge by content after a squash) are general lessons and
> remain valid. Verified 2026-09-06.

**Written 2026-08-06.** Everything below is checkable; nothing here is a plan you have
to take on trust. Where a number appears, the command that produces it appears with it.

---

## 1. What this is

241 headers in `include/` carry `AUTO-GENERATED from matched-function evidence by
tools/gen_header.py`. **No such generator was ever committed** -- `5ddf7d2d` (PR #866)
added all 368 headers and zero `tools/` files. Nothing had ever checked what they claim.

Where the original generator did not know a field's type it wrote a bare `u8` and padded
to the next thing it did know:

```c
u8  mModelAnim;         /* 0x0d4 */
u8  pad_0d5[0x63];
```

That is an **object marker**. The `u8` is a stand-in, not a width claim -- reading it as
one produced 229 false positives early in this programme, and a variant of that mistake
recurred twice more. Typing them is what turns a byte blob into `ModelAnim mModelAnim;`,
which is the precondition for writing real C++ methods against it.

## 2. The evidence, and why it is trustworthy

The type comes from a **member-destructor relocation** in the class's own destructor:

```c
/* src/_ZN3AmpD0Ev.c */
_ZN9ModelAnimD1Ev((char *)t + 0xd4);
```

That callee is a relocation `tools/build_pin.py` checks against `config/**/relocs.txt`,
so the class name is ROM-backed rather than inferred from the placeholder name.

**D1 and never D2 is the soundness argument.** A true member of declared type `T` calls
`T`'s *complete-object* destructor. An inlined derived member would call the base's `D2`
at the same offset. Every exact call behind the typed markers is a D1; a review confirmed
zero D2 and zero C2 across the set.

Corroboration that is real: **constructor `C1` calls at the same offsets**, found
independently in `*_Spawn.c` files. Corroboration that is **not** real: "the span ends on
an interior boundary of the class" -- every 4-aligned offset of `Model`/`ModelAnim` is a
boundary, so the test cannot fail. Do not quote it.

## 3. State

```sh
python tools/marker_census.py           # the operative worklist
```

| | |
|---|---:|
| markers typed (this programme) | **298** |
| bare object markers remaining | **767** |
| decidable today on D1 evidence | **235** |
| of those, clean swaps (span == sizeof) | 66 |
| ...all of which are **blocked**, see §5 | |

Merged: **#1151** (131 markers + the `Matrix4x3` guard), **#1153** (census + the
transforms), **#1156** (the bucketing fix, and *only* that), plus the earlier programme
(#1118, #1121, #1129, #1131, #1132, #1134, #1137).

The marker batch this note was written alongside, and this note itself, are landing
separately. They were committed to #1156's branch *after* that PR squash-merged, so they
never reached main -- the §7 trap, third occurrence, caught by comparing the census
against this table's own numbers. Read the table as the state after that re-land.

Build state throughout: `106/106 exact, PASS`, eligible **10672**, attribution 0/0.

## 4. Tools

| tool | what it answers |
|---|---|
| `tools/marker_census.py` | how many markers are left and how many are decidable **now**. Reads the tree, not a report. **This is the worklist.** |
| `tools/marker_evidence.py` | full per-marker evidence, all verdicts |
| `tools/marker_apply.py` | the transform |
| `tools/gen_header.py` | the bucketed differential. Useful, but see §6 -- it is *not* the worklist |
| `tools/check_header_offsets.py` | recomputes every field position against its `/* 0x… */` comment |

## 5. The next step, precisely

**All 66 remaining clean swaps sit in 29 classes whose TUs carry local declarations that
collide once the header pulls in the real definition.** They are not a separate batch --
applying them costs ~26 files every time. I tried; I reverted it.

The blockers are not all `Model` stand-ins. Sampling the failing TUs:

```sh
Base2 (4)  Base1 (3)  Obj (2)  RaycastGround (2)  VB (1)  Model (1)  Base (1)  Scene (1)
```

These are ad-hoc local classes a recovery pass invented so a file could spell a call.
Each needs reading and removing individually.

**The worked example is `src/_ZN10FlameChomp13InitResourcesEv.cpp`** (commit on #1151):
it carried `typedef int Fix12;`, which collided with the real `Fix12<>` template once
`FlameChomp.h` reached `Model.h`. The typedef *was* `int`, so spelling `int` directly in
the two extern declarations was byte-neutral. `build_pin.verify` → `(True, '2004/b56')`.

Do that, per class, byte-verifying each. Then re-run the census and the clean swaps land.

The 29: `BobOmb BooCage Bowser BowserFire BowserPuzzlePiece Bullet Coffin FlyGuy Goomba
Koopa Lakitu LakituBro MadPiano MrI MrI_Projectile OneUpMushroom PrincessPeach
RotatingClockHand Scuttlebug SignPost SnowmanBody SnowmanHead Snufit Spindrift Stage
Swoop TtcConveyorBeltLarge WaterBomb YoshiEgg`

## 6. Gates -- run all of them, and know what each cannot see

```sh
python tools/check_header_offsets.py <changed headers>   # 0 mismatched, 0 unparsed
python tools/eligible.py                                 # BEFORE and AFTER, diff them
python tools/rombuild.py                                 # 106/106 exact, PASS
python tools/check_references.py                         # must say OK
python tools/prepush_attribution.py                      # 0 changed, 0 lost
python tools/langmode_audit.py --check langmode-baseline.json
python tools/test_gen_header.py                          # 17 tests
```

**`rombuild.py` compiles only *enrolled* files**, and the tree matches more than it
enrolls. It reported `106/106 PASS` while 237 files were broken.

**`eligible.py`'s before/after bracket** caught 237, then 121, then 26 files. It is the
load-bearing check for any header change.

**`check_references.py` caught three defects the other two were structurally blind to**,
every time by noticing a symbol left the unresolved set *without becoming eligible* --
it changed why it fails, which is not a fix. Files that were never eligible are invisible
to both gates above. **Keep this one closest.**

## 7. Traps, each of which cost real time here

- **A pass that looks in the wrong place reports like one that found nothing.** A probe
  matched 3 accesses where ~5,900 existed; `evidence_rom.py` decoded every arm9 function
  0x4000 bytes off and reported confidently (2.1% plausible prologues against 64.8%).
  Every pass now prints its own recall. Read it before believing a zero.
- **`gen_header.py` files a D1-evidenced marker as `confirmed`**, dropping it off the
  worklist -- the markers with the *best* evidence went missing because the evidence was
  good. Fixed in #1156, but it is why the census exists and why the census is the worklist.
- **The worklist was scoped to 3 types when the same evidence names 14.** Do not assume
  the current scope is the evidence's scope.
- **Capstone appends condition codes**, so `strhs` prefix-matches `strh`. Match on
  `insn.id`, never mnemonic text. Same trap for `ble` vs `bl`.
- **Sweep for duplicate type definitions before a batch**, not after. Doing it first took
  the collision round from 237 lost files to 26.
- **Check `git log origin/main..HEAD` after any merge.** Commits were stranded twice by a
  PR squash-merging while work was pushed to the same branch, and both times the work was
  reported landed when it was not. Verify by *content*: `git show origin/main:<file>`.

## 8. Deliberately left

- **36 undecided markers**, with reasons in `build/marker_evidence.json`. Honest.
- **9 "interior to a typed member"** findings -- header self-contradictions, not typing
  questions. Separate fix.
- **`Stage.unk_874`** -- rests only on a cast through a locally-declared struct.
- **`include/G2x.h`** -- describes hardware registers, not an object. Issue **#1148**,
  someone else is on it. Do not type it.
- **`ModelBase::SetFile` is declared `void`** but the ROM's call sites consume r0, so it
  returned a value. Two call sites keep the mangled spelling with a note. Fixing it means
  a byte-verified change across 216 includers.
- **`check_header_offsets.py` cannot parse `struct X *p;`** -- it reports 8 mismatched /
  4 unparsed on the C fallbacks as pure parse artifact. A measured two-line fix takes the
  tree from 65 to 30 problems; held back so batch numbers stayed comparable. Land it first.

## 9. The honest framing

The ROM has matched at `106/106 exact` throughout, and coverage is unchanged -- this work
does not add matched functions. It makes the **description** true, which is upstream of
migration: you cannot write `mModelAnim.SetAnim(...)` until the header says a `ModelAnim`
is there.

For scale on what remains after this: `python tools/langmode_audit.py` reports **1,341 of
2,515 mangled-symbol files still unmigrated (53.3%)** and **2,663 local shadow struct
declarations**. Those shadows are the same thing blocking §5 -- they are the shared
bottleneck between the header work and the migration work, and every one removed makes
both cheaper.
