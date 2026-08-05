# The scalar-sized u8 markers

**Status:** done for the 71 mechanical cases. One left for a person (§5).
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

## 3. Signedness is not evidenced, and that is measured, not assumed

**Zero of the 72 have an ancestor that declares the same offset**, so the hierarchy
cannot supply a type. Only **one** has any ROM signedness signal at all -- a 4-byte
load does not reveal signedness, and 64 of these are 4-byte.

So the `s32`/`s16` spelling is not a finding. To establish what it is, the whole
batch was applied twice and taken through the full gate both ways:

| spelling | eligible | module fidelity |
|---|---|---|
| `s32`/`s16` | 10667 | 106/106 exact, PASS |
| `u32`/`u16` | 10667 | 106/106 exact, PASS |

**Identical.** No includer uses any of these fields in a way that distinguishes
signed from unsigned -- no sub-word load, no comparison, no right shift, no division.
Signedness here is *unobservable*, not merely unknown.

That is worth stating precisely, because the two are not the same:

- the **width** fix is proven -- `u8` contradicted the evidence and now agrees with it
- the **signedness** is a convention, chosen to match `Actor.h`, and the byte gate
  cannot confirm or refute it

`runbook-type-reconstruction.md` §2 warns that `s32 -> u32` flips `movgt/movle` to
`movhi/movls` and `asr` to `lsr`. That is exactly why this had to be measured rather
than reasoned about: the warning is real, it just does not bite where nothing reads
the field.

## 4. Preserving offsets

Every retype shrinks the following pad by the width gained and renames it to its new
offset. `build/check_offsets.py` walks each struct, applies natural alignment, and
compares every field's computed position against its comment: **562 commented fields
across 33 headers, 0 mismatched**.

The type column is padded so the field names stay aligned -- a 3-character type takes
one less space than `u8`.

## 5. The one left for a person

```
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
