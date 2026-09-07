# The scalar-sized u8 markers

**Status:** done for the 71 mechanical cases. A handful remain for a person --
`gen_header.py --report` is the live count; it was 1 when this was written.
**Depends on:** #1118 (the evidence passes), #1121 (the base headers).

---

## 1. What a marker is

The original generator wrote a bare `u8` when it did not know a field's type, and
padded out to the next thing it did know:

```c
    u8  unk_0a4;            /* 0x0a4 */
    u8  pad_0a5[0x3];
```

That is a marker, not a claim that the field is one byte wide -- reading them as
width claims produced 229 false positives before `gen_header.py` learned the idiom.

But when the marker plus its pad spans **4 bytes or less** there is no room for an
object. The marker is standing over a scalar, and both evidence passes see accesses
of a definite width. Those are the 72 this note is about.

## 2. Evidence

| span | observed width | count | becomes |
|---:|---:|---:|---|
| 4 | 4 | 64 | `s32`, pad dropped |
| 2 | 2 | 5 | `s16`, pad dropped |
| 4 | 2 | 2 | `s16` + `u8 pad[2]` |
| 2 | 2 **and** 4 | 1 | **left alone** -- see §5 |

33 of the 71 are backed by both the history pass and the ROM pass independently; the
rest by one. 34 classes, 121 includer files in `src/`.

## 3. Signedness is not evidenced -- and two claims made here were wrong

> **Correction.** Two statements in this section and in the #1129 commit message were
> false. They are struck through below rather than deleted, because the commit message
> is permanent and someone will read it. Adversarial review caught both.

### 3a. "Zero of the 72 have an ancestor that declares the same offset" -- WITHDRAWN

~~Zero of the 72 have an ancestor that declares the same offset.~~

That was a **recall limit reported as a zero** -- exactly the failure this programme
exists to prevent. The hierarchy pass has no entry at all for 7 of the 34 retyped
classes, so for those the question was never asked, let alone answered.

`FaderWipe` is the counterexample and it is not a marginal one. ROM RTTI places
`Fader <- FaderBrightness <- FaderColor <- FaderWipe`, and `include/Fader.h:34-35` --
de-bannered, hand-reconstructed, the strongest reference in the tree -- declares
`Fix12i currInterp /* 0x04 */` and `Fix12i speed /* 0x08 */`. Those are precisely the
two FaderWipe offsets retyped as "no ancestor". The generator's own input at
`5ddf7d2d~1:src/_ZN9FaderWipeC1Ev.c` names them too.

Both are now declared `Fix12i` (a typedef of `s32`, so byte-identical). So is
`HeapAllocator` 0x018/0x01c, which its own constructor stores `void*` into.

**A pass that cannot see a class must say so, not return zero.**

### 3b. "No sub-word load" -- WITHDRAWN

~~No includer uses any of these fields in a way that distinguishes signed from
unsigned -- no sub-word load, no comparison, no right shift, no division.~~

There is a sub-word access: `src/game/actors/d_a_obj_wc_obj04.cpp:184` does an
`unsigned short*` read-modify-write on retyped `unk_3a0`, now `mStateTimer`.
(It was `src/_ZN20SwitchActivatedPlank8BehaviorEv.cpp:35` when this note was
written, before the class was renamed to its RTTI spelling and promoted to a
single translation unit.) The correct statement is
narrower and is what the experiment actually shows: **no access whose codegen depends
on the declared type.** Every compiled reference to these fields goes through an
address-cast (`*(int*)((char*)&self->unk_018)`), and a cast ignores the declaration.

### 3c. What the experiment does and does not prove

The whole batch was applied twice and taken through the full gate both ways:

| spelling | eligible | module fidelity |
|---|---|---|
| `s32`/`s16` | 10667 | 106/106 exact, PASS |
| `u32`/`u16` | 10667 | 106/106 exact, PASS |

Review reproduced this at object level, which is stronger: all 119 includers compiled
under both spellings, **117/117 byte-identical** (2 fail identically before and after),
plus a positive control -- a scratch TU doing `>> 4` and `/ 2` on these fields *does*
produce different bytes, so the method detects sign-sensitivity when it exists.

What that proves is **harmlessness**, not width:

- the **width** rests on the evidence passes, not on the gate. After the arm9 base
  fix (#1132), **52 of 71** are backed independently by both passes, up from the 33
  published; none lost its backing.
- the **signedness** is a convention matching `Actor.h`. The gate cannot confirm or
  refute it, and saying "the gate proved the width" would have been wrong.

`runbook-type-reconstruction.md` §2 warns that `s32 -> u32` flips `movgt/movle` to
`movhi/movls` and `asr` to `lsr`. The warning is real; it does not bite where every
access is cast anyway.

## 4. Preserving offsets

Every retype shrinks the following pad by the width gained and renames it to its new
offset. `tools/check_header_offsets.py` walks each struct, applies natural alignment, and
compares every field's computed position against its comment: **562 commented fields
across 33 headers, 0 mismatched**.

The type column is padded so the field names stay aligned -- a 3-character type takes
one less space than `u8`.

## 5. The one left for a person

```c
G2x.unk_004 @0x4  span=2  observed widths [2, 4]   include/G2x.h:13
```

The history pass sees both a 2-byte and a 4-byte access at the same offset, and the
4-byte one does not fit the 2-byte span. Plan §4 sends conflicting evidence to a
human rather than resolving it to the widest or the narrowest. Either the span is
wrong, or two fields overlap, or one access is misattributed.

## 6. What this does not do

- **No renaming.** `unk_0a4` stays `unk_0a4`. Renaming cannot change codegen, which
  is exactly why it belongs in its own commit -- mixing it in makes the byte gate's
  signal unreadable.
- **No touching the 179 object-extent markers.** Those have spans too large to be
  scalars and mostly address-only evidence; they need types, not widths.
- **No guessing signedness from a name.** `mAngle*` being signed elsewhere is a
  pattern, not evidence about these fields.
