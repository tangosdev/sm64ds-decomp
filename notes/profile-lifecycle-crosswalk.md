# SM64DS profile fields and lifecycle crosswalk

Status: evidence crosswalk for the profile-reconstruction pilot.  It records
Troy's NSMBW comparison as a hypothesis, then separates the portions confirmed by
SM64DS consumers from fields and spellings that remain unresolved.

Second pass adds the recovered create path -- which settles where the group-type
byte comes from -- a measured partition of the actor-flags word into
profile-authored and framework-written bits, and a parse defect in three registry
rows.

## Runtime descriptor fields

The recovered constructors give the following offset-level view.  The member names
below describe observed runtime use; they are not claims about Nintendo's original
SM64DS declarations.

| offset | width | observed SM64DS use | evidence status |
|---|---:|---|---|
| `+0x00` | 4 | class-construction factory | Tier A: the registry descriptor points to an allocating wrapper |
| `+0x04` | 2 | registry/profile index **and** behavior/execute-list order | Tier A: all 391 registry entries have a valid descriptor interpretation whose value equals the registry index; `fBase_c::fBase_c` copies the same halfword into `manager.behaviorNode` priority |
| `+0x06` | 2 | render/draw-list order | Tier A: `fBase_c::fBase_c` copies it into `manager.renderNode` priority |
| `+0x08` | 4 | actor flags/properties | Tier A for storage: `dActor_c::dActor_c` copies it to `dActor_c::mFlags`; exactly fifteen bits are ever authored and four of them have recovered consumers -- see the flags section |
| `+0x0c` | 4 | vertical clip-volume centre offset | Tier A consumer semantics: first argument to `dActor_c::SetRanges`, stored without scaling in `mClipOffsetY` |
| `+0x10` | 4 | clip radius | Tier A consumer semantics: second `SetRanges` argument, shifted right three and stored in `mClipRadius` |
| `+0x14` | 4 | clip distance | Tier A consumer semantics: third `SetRanges` argument; game mode 2 adds `0x7d0000` before the call |
| `+0x18` | 4 | far distance | Tier A consumer semantics: fourth `SetRanges` argument |

The 0x08 base/scene form ends after `+0x06`; it has the factory and the same two
ordering halfwords but none of the actor-only fields.

This sharpens Troy's initial field list in two ways:

1. The apparent profile-ID field is not separate storage from execute order in the
   ROM layout.  SM64DS stores the registry index in `+0x04` and then uses that exact
   value as behavior/execute order.  This closely resembles NSMBW's `BASE_PROFILE`
   and `ACTOR_PROFILE` macros, which use the profile number as execute order by
   default.
2. A distinct group-flags member is absent from both runtime descriptors because it
   is not descriptor data at all.  The `fBase_c +0x12` byte is an argument of the
   framework's create call, and the create path traced below identifies it as
   NSMBW's group type.  Troy's group flags are a per-spawn parameter supplied by
   whoever creates the object, not a field the profile carries.

The four actor range words are not merely adjacent values inferred from magnitude.
They flow through the matched `dActor_c` constructors and `SetRanges`, and their
later culling consumers establish offset-Y, radius, clip-distance, and far-distance
roles.  A source-level declaration macro could still have accepted conceptually
different or differently ordered arguments, but the emitted runtime object has the
layout above.

## The create path, and where the group-type byte comes from

The registry is reached through one function, and reading it settles two of the
questions this note previously left open.

`func_02043098` is the framework's create entry point.  Its four parameters are the
profile index, a parent scene node, a caller-supplied parameter word, and a byte:

```c
int func_02043098(int idx, SceneNode *parent, int param, u8 groupType)
{
    data_020a4b50 = idx;                    /* the id being constructed */
    data_020a4b4c = 2;                      /* creation-phase state machine */
    if (func_02043060(idx) == 3) return 0;
    data_020a4b4c = 3;
    func_02043180(idx, parent, param, groupType);   /* stash all four in globals */
    data_020a4b4c = 4;
    r5 = (*data_020a4bb8[idx])();           /* registry[idx]->factory() */
    ...
}
```

Two Tier-A consequences.

**`data_020a4bb8` is the registry itself, and `+0x00` really is the factory.**
`(*data_020a4bb8[idx])()` dereferences the descriptor pointer and calls the word it
finds there.  The same `data_020a4bb8[idx]` is read by `fBase_c::fBase_c` as a
struct to fetch the two priority halfwords.  One table, two consumers, and the
call proves the first word's role rather than inferring it from shape.

**The `+0x12` byte is a create argument, not profile data.**  It travels
`func_02043098`'s fourth parameter -> `func_02043180` -> the global `data_020a4b48`
-> `fBase_c::fBase_c`'s `unk_012 = data_020a4b48`.  Nothing reads it from the
descriptor, because it never was in the descriptor.  The globals exist only to
carry the four arguments across the factory call, which takes no arguments of its
own.

Two thin wrappers complete the shape, and both are byte-confirmed:

| function | bytes at | what it does | NSMBW analogue |
|---|---|---|---|
| `func_02042fe4` | `0x02042fe4` | `mov r3,r2; mov r2,r1; mov r1,#0; bx` -- shifts the arguments up and passes a null parent | `createRoot(profName, param, groupType)` |
| `func_02042ffc` | `0x02042ffc` | returns 0 on a null parent, else `add r1,r1,#0x14` and calls `func_02043098` -- converts an `fBase_c *` to `&parent->manager` | `createChild(profName, parent, param, groupType)` |
| `func_02043098` | `0x02043098` | the core; takes the parent scene node directly | -- |

The pinned NSMBW header declares
`createChild(ProfileName profName, fBase_c *parent, unsigned long param, u8 groupType)`
and `createRoot(ProfileName profName, unsigned long param, u8 groupType)`, with
members `mParam` (u32), `mProfName` (`ProfileName`), and
`mGroupType` (u8, "value is a `GROUP_TYPE_e`").  SM64DS's `fBase_c` stores the same
three at `+0x08`, `+0x0c`, and `+0x12`, seeded in that order by the same three
arguments.  The parameter list, its order, and the destination fields all agree.

That is strong lineage evidence for the *role* of `+0x12`.  It is not proof of the
SM64DS spelling, and this note does not rename `unk_012`: no SM64DS consumer of the
byte has been recovered yet, so its value set is still unknown.  The one recovered
writer is the create path; a reader would be needed to establish what the values
mean.

## Actor flags: what the profile authors and what the runtime writes

The `+0x08` word is a single storage location with two populations of bits in it.
`dActor_c::dActor_c` copies the descriptor word into `mFlags` verbatim, and the
per-frame framework then sets and clears bits in that same word.  Separating the
two is what makes the field readable.

Across the 353 soundly-parsed actor descriptors (see the defect section below),
exactly fifteen bits are ever authored:

| bit | profiles | proven core consumer |
|---|---:|---|
| `0x00000001` | 54 | `BeforeBehavior`: enables the clip test (`mFlags & 0x10003`), and `(f & 9) != 9` makes off-screen skip behaviour |
| `0x00000002` | 278 | `BeforeRender`: off screen plus this bit means do not draw; also enables the clip test |
| `0x00000004` | 25 | none recovered |
| `0x00000080` | 9 | none recovered |
| `0x00000200` | 5 | none recovered |
| `0x00008000` | 6 | none recovered |
| `0x00010000` | 3 | `BeforeBehavior`: defeats the global force-think override while the actor is invisible |
| `0x00800000` | 19 | none recovered in core; `Player::St_Talk_*` sets and clears it at runtime |
| `0x01000000` | 4 | none recovered |
| `0x02000000` | 8 | none recovered |
| `0x04000000` | 8 | none recovered in core; read by several unrelated actor `Behavior`s |
| `0x08000000` | 6 | `BeforeInitResources`: spawn even if the death table records this actor as already killed |
| `0x10000000` | 38 | none recovered in core; set at runtime by enemy code |
| `0x20000000` | 13 | none recovered |
| `0x40000000` | 3 | none recovered |

The runtime-state bits are the complement, and the population check is the point:
**no profile authors any of them.**

| bit | written by | meaning |
|---|---|---|
| `0x08` | `BeforeBehavior` (clipper), `AfterInitResources` | off screen |
| `0x10` | `BeforeBehavior` when the clip distance exceeds `mFarDistance` | far away; also suppresses the drop shadow in `DropShadowRadHeight` and `DropShadowScaleXYZ` |
| `0x20` | `BeforeBehavior` when `mAreaId` names an area that is not showing | wrong area |
| `0x20000`, `0x40000` | actor code | Yoshi-mouth states |

`AfterInitResources` seeds `0x38` -- all three at once -- so an actor starts fully
invisible and the first `BeforeBehavior` decides otherwise.

This replaces the speculative bit table in `include/dActor_c.h`, which listed
`0x08`, `0x10`, and `0x20` as though a profile might author them and described
`0x10` as "past shadow range".  `0x10` is the far-distance bit; shadow suppression
is one of its consumers, not its definition.  Seventeen bit positions are never set
by any profile and have no recovered consumer, so the word is nowhere near fully
attributed -- but the fifteen authored bits are now a closed set rather than an
open question, and eleven of them are still unexplained.

## Three registry rows are parsed with the wrong descriptor family

`CAMERA`, `MAP`, and `ENTRY_OBJECT` are recorded in
`symbols/profile_reconstruction_registry.tsv` as `actor_profile_0x1c`, and their
actor-only fields are visibly adjacent data rather than values:

```text
CAMERA        actor_flags 0x020084b0   clip_radius 0x020078c4   far_distance 0x02009e70
MAP           actor_flags 0x0209a764   clip_offset_y 0x0210c158  clip_radius 0x02086e78
ENTRY_OBJECT  actor_flags 0x0211478c   clip_radius 0x02114560   far_distance 0x02114010
```

Every one of those is a code or data pointer.  `CAMERA`'s descriptor at
`0x02086d78` reads `{factory 0x0200e444, 0x0000014c, 0x020084b0, 0, 0x020078c4, 0,
0x02009e70, 0}` -- an 0x08 base profile with execute order `0x14c` and draw order
zero, followed by three more 0x08 profiles packed behind it.  This is exactly the
failure `actor-profile-pilot.md` predicts when a short descriptor is read as a
long one; the family choice from RTTI ancestry picked wrong for these three.

The three rows are the sole source of the apparent profile-authored `0x08`, `0x10`,
and `0x20` bits, so the flag census above is self-consistent only once they are
removed.  That is a second, independent reason to believe they are mis-parsed.
`KI_HASIRA`, `CASKET`, `FM_BATTAN`, `RC_TIKUWA`, and `ONIMASU` also carry
pointer-shaped flag words (`0x02000002` and friends), but their range words are
ordinary clip values, so those are genuine `0x02000000`-bit profiles rather than
parse defects.

## WATERFALL with consumer-oriented labels

The direct bytes for `WATERFALL` can be represented experimentally as:

```cpp
// Reconstructed source shape; exact original names and declaration syntax do not survive.
daObjWaterfall_c* daObjWaterfall_c_classInit()
{
    return new daObjWaterfall_c();
}

const SpawnInfo g_profile_WATERFALL = {
    daObjWaterfall_c_classInit,
    197,        // +0x04: profile index and execute order
    21,         // +0x06: draw order
    0x00800003, // +0x08: actor flags/properties
    0x00060000, // +0x0c: clip-volume Y offset
    0x00300000, // +0x10: clip radius
    0x02000000, // +0x14: clip distance
    0x00000000, // +0x18: far distance
};
```

`SpawnInfo`, `g_profile_WATERFALL`, and the field comments are decompilation
vocabulary.  Only the bytes, pointer relationships, registry index, and consumer
behavior are Tier A.

## Lifecycle operations

The lifecycle comparison is stronger than a loose semantic analogy.  Both
frameworks expose four operations, and each operation has a do/pre/post virtual
triplet in the same order.

| SM64DS vtable slots | NSMBW operation | confidence | rationale |
|---|---|---|---|
| `InitResources`, `BeforeInitResources`, `AfterInitResources` (0-2) | `create`, `preCreate`, `postCreate` | high structural lineage | initialization may wait across frames; successful post-init schedules behavior and render nodes |
| `CleanupResources`, `BeforeCleanupResources`, `AfterCleanupResources` (3-5) | `doDelete`, `preDelete`, `postDelete` | high structural lineage | the pre-hook waits on child/auxiliary state and successful post-cleanup unlinks, destroys, destructs, and deallocates |
| `Behavior`, `BeforeBehavior`, `AfterBehavior` (6-8) | `execute`, `preExecute`, `postExecute` | high structural lineage | the pre-hook applies pause and actor culling gates before per-frame behavior |
| `Render`, `BeforeRender`, `AfterRender` (9-11) | `draw`, `preDraw`, `postDraw` | high structural lineage | the pre-hook applies pause, hidden-area, and off-screen gates before drawing |

The method *roles* and triplet structure are strongly supported.  Renaming the
SM64DS virtuals to the NSMBW spellings would be a separate source-naming decision.
The vtable slots, signatures, dispatch relationships, and bodies are direct
structural evidence; project spellings such as `InitResources`, `Behavior`, and
`Render` are not promoted to ROM-preserved names by this comparison.

The pinned public NSMBW framework header declares the corresponding
[`create/preCreate/postCreate`, `doDelete/preDelete/postDelete`,
`execute/preExecute/postExecute`, and `draw/preDraw/postDraw` triplets](https://github.com/NSMBW-Community/NSMBW-Decomp/blob/2e010f8708d8232c736b1ece507400dfd76aaa9c/include/game/framework/f_base.hpp).
Its profile header defines `{classInit, executeOrder, drawOrder}` base profiles,
adds actor properties for actor profiles, and defaults execute order to the profile
number in
[`BASE_PROFILE`/`ACTOR_PROFILE`](https://github.com/NSMBW-Community/NSMBW-Decomp/blob/2e010f8708d8232c736b1ece507400dfd76aaa9c/include/game/framework/f_profile.hpp).
Those files are lineage evidence, not proof that SM64DS used the same source names,
types, or macro syntax.

## Answered since the first pass

- **Troy's group flags.**  Not a profile field.  The `fBase_c +0x12` byte is the
  fourth argument of the create call, matching NSMBW's `mGroupType`.  Its producer
  is `func_02043180`, called from `func_02043098`, fed by
  `func_02042fe4`/`func_02042ffc`.  Its *values* remain unknown: no reader has been
  recovered.
- **Execute and draw order as explicit arguments.**  The question is moot for
  `+0x04`, which equals the registry index in all 391 entries.  `+0x06` is
  independent: 347 distinct draw orders over 353 sound actor rows, spanning 0..359
  with no relation to the index, so draw order was authored per profile.

## Open questions

- What are the group-type values, and who reads `fBase_c +0x12`?  A reader would
  turn the NSMBW `GROUP_TYPE_e` analogy into a checkable claim.
- Eleven of the fifteen profile-authored flag bits have no recovered consumer:
  `0x04`, `0x80`, `0x200`, `0x8000`, `0x800000`, `0x1000000`, `0x2000000`,
  `0x4000000`, `0x10000000`, `0x20000000`, `0x40000000`.  `0x800000`, `0x4000000`,
  and `0x10000000` have runtime writers in actor code, which is a starting thread.
- Did SM64DS's declaration mechanism accept culling values directly, or transform a
  smaller source-level parameter set into the four runtime range words?
- What is the correct descriptor family for `CAMERA`, `MAP`, and `ENTRY_OBJECT`,
  and does fixing the extractor's family choice change any other row?
