# Plan: fix the two base headers that poison their descendants

**Status:** in progress. First change in the gen_header programme that touches `include/`.
**Depends on:** #1118 (the evidence passes and the differential that found this).
**Scope:** `include/Enemy.h` and `include/CapEnemy.h`, six field declarations.

---

## 1. What is wrong

`tools/gen_header.py` buckets every field declared in the 241 bannered headers. 49 of
them conflict with a base class whose **own header is also bannered** -- so the report
grades them *weak*: two never-verified headers disagreeing is not evidence about which
is right.

Resolving them turns out to be easy, because all 49 collapse onto **six declarations in
two headers**:

| base | offset | descendants disagreeing | base says | they say |
|---|---|---:|---|---|
| `Enemy` | 0x094 | 27 | `u8` | `s16` (unanimous) |
| `Enemy` | 0x0a8 | 10 | `u8` | `s32` (unanimous) |
| `Enemy` | 0x0ac | 6 | `u8` | `s32` (unanimous) |
| `Enemy` | 0x10c | 4 | `u8` | `s32` (unanimous) |
| `CapEnemy` | 0x05c | 1 | `u8` | `s32` |
| `CapEnemy` | 0x08c | 1 | `u8` | `s16` |

In every case the base declares a bare `u8` followed by a pad -- the original
generator's marker for *"something starts here and I do not know what"* -- and the
descendants declare a real type. **The base is the wrong one.** The descendants already
recovered these fields correctly; the base is masking them.

Note what this does *not* mean. `Enemy.h` has **3** includers and `CapEnemy.h` has **4**;
the 27 descendants carry their own flat headers and never include `Enemy.h`. So this
change does not propagate a fix to them. It removes 49 false conflicts and makes the two
headers usable as references, which is what unblocks the rest of the programme.

## 2. Independent evidence, per site

Neither pass was told what the headers claim.

| site | history | ROM | `Actor.h` says |
|---|---|---|---|
| `Enemy` 0x094 | 3 x width 2 | 1 x width 2, **signed load** | `s16 mPrevAngleY` (:93) |
| `Enemy` 0x0a8 | 2 x width 4 | 1 x width 4 | `s32 mVertSpeed` (:99) |
| `Enemy` 0x0ac | 1 x width 4 | 1 x width 4 | -- |
| `Enemy` 0x10c | 4 x width 4 | 2 x width 4 | -- |
| `CapEnemy` 0x05c | address taken x2, **no width** | -- | `s32 mPosX` (:65) |
| `CapEnemy` 0x08c | address taken x2, **no width** | -- | `s16 mAngleX` (:89) |

Two of the four `Enemy` sites are Actor fields that `Enemy.h` re-declared as markers, and
`Actor.h` is de-bannered -- hand-reconstructed, the strongest reference in the tree. The
ROM independently proves 0x094 is **signed**, which no source pass can settle.

## 3. The two sites in CapEnemy are a different thing, and are NOT being retyped

`CapEnemy` 0x05c and 0x08c have **address-only** evidence: the code takes their address
and never loads a scalar from them. Against `Actor.h`, 0x05c is the start of the position
triple (`mPosX/mPosY/mPosZ`) and 0x08c the start of the rotation triple
(`mAngleX/mAngleY/mAngleZ`).

So these markers stand over **objects**, and the lone descendant declaring `s32`/`s16` is
describing only the first component. Retyping the marker to `s32` would be adopting a
narrower lie. They stay as markers; the comment is corrected to say what the span is.

This is the distinction the differential exists to make, and it is why "49 conflicts"
was never "49 retypes."

## 4. Mechanics, and where this can go wrong

Each retype must preserve every following offset. `u8 x; u8 pad[n];` becomes
`sN x; u8 pad[n - (sizeof(sN) - 1)];`, and the pad is **renamed** to its new offset:

```c
    u8  unk_094;            /* 0x094 */        ->    s16 unk_094;            /* 0x094 */
    u8  pad_095[0xf];                                u8  pad_096[0xe];
```

Checks that must hold at every site:

- **Alignment.** 0x094 is 2-aligned; 0x0a8, 0x0ac, 0x10c are 4-aligned. None forces the
  compiler to insert padding *before* the field, which would shift everything after it.
- **Span preserved.** 1 + pad before == sizeof(new) + pad after.
- **`Enemy` 0x10c is the last field**, with no trailing pad, so retyping it to `s32`
  grows `sizeof(Enemy)` by 3. Nothing should depend on that, but it is the one site
  whose effect is not offset-neutral, so it is byte-verified with particular care.

## 5. Verification

A header change affects **every** includer, and `rombuild.py` compiles only *enrolled*
files while the tree matches more than it enrolls
(`notes/runbook-reference-repair.md` §1). So the build alone cannot see a file that
silently stopped matching.

```sh
python tools/eligible.py                     # BEFORE, on a clean tree
# ... make the edits ...
python tools/build_pin.py --verify <each of the 7 includers>
python tools/rombuild.py                     # 106/106 exact, PASS
python tools/eligible.py                     # AFTER -- diff against BEFORE
python tools/prepush_attribution.py          # credit must not move
python tools/gen_header.py --root . --report # the 49 weak conflicts should be 0
```

The `eligible.py` bracket is the load-bearing one. A drop in matched files is the failure
mode this change is most likely to produce, and it is the one the ROM build cannot report.

## 6. Definition of done

- [ ] 4 retypes in `Enemy.h`, spans and alignment preserved
- [ ] 2 `CapEnemy` markers left as markers, comments corrected
- [ ] `ROM-build analysis: PASS`, module fidelity 106/106 exact
- [ ] `eligible.py` before/after: no file lost its match
- [ ] attribution 0 changed, 0 lost
- [ ] `base-conflict (weak)` falls 49 -> 0
- [ ] enrolled vs byte-verified-only split stated in the PR body

## 7. What this deliberately does not do

- **No restructuring to real inheritance.** `Enemy.h` and `CapEnemy.h` duplicate Actor's
  layout flat rather than inheriting it. Fixing that is the right end state and a much
  larger change with codegen risk in every includer. Retyping six fields is testable in
  one batch; rebuilding two class hierarchies is not.
- **No renaming.** `unk_094` stays `unk_094` even though `Actor.h` calls it
  `mPrevAngleY`. Renaming is free and cannot change codegen, which is exactly why it
  belongs in a separate commit -- mixing it in here would make the byte gate's signal
  unreadable.
- **No touching the 27 descendants.** They are already right. Their markers are a
  separate bucket (`marker: scalar-sized, retype`, 72 fields) and a separate batch.
