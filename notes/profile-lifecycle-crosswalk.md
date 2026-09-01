# SM64DS profile fields and lifecycle crosswalk

Status: evidence crosswalk for the profile-reconstruction pilot.  It records
Troy's NSMBW comparison as a hypothesis, then separates the portions confirmed by
SM64DS consumers from fields and spellings that remain unresolved.

## Runtime descriptor fields

The recovered constructors give the following offset-level view.  The member names
below describe observed runtime use; they are not claims about Nintendo's original
SM64DS declarations.

| offset | width | observed SM64DS use | evidence status |
|---|---:|---|---|
| `+0x00` | 4 | class-construction factory | Tier A: the registry descriptor points to an allocating wrapper |
| `+0x04` | 2 | registry/profile index **and** behavior/execute-list order | Tier A: all 391 registry entries have a valid descriptor interpretation whose value equals the registry index; `fBase_c::fBase_c` copies the same halfword into `manager.behaviorNode` priority |
| `+0x06` | 2 | render/draw-list order | Tier A: `fBase_c::fBase_c` copies it into `manager.renderNode` priority |
| `+0x08` | 4 | actor flags/properties | Tier A for storage: `dActor_c::dActor_c` copies it to `dActor_c::mFlags`; individual bit meanings are only partly understood |
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
2. A distinct group-flags member has not yet been located in either runtime
   descriptor.  `fBase_c` does carry a byte at `+0x12`, seeded from construction
   context rather than read from the profile by the recovered constructor.  That is
   a lead, not enough evidence to identify Troy's proposed group flags or their
   source-declaration position.

The four actor range words are not merely adjacent values inferred from magnitude.
They flow through the matched `dActor_c` constructors and `SetRanges`, and their
later culling consumers establish offset-Y, radius, clip-distance, and far-distance
roles.  A source-level declaration macro could still have accepted conceptually
different or differently ordered arguments, but the emitted runtime object has the
layout above.

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

## Open questions

- What source concept and bit meanings correspond to Troy's group flags?
- Is the `fBase_c +0x12` construction-context byte related, and where is its
  producer?
- Did SM64DS's declaration mechanism accept culling values directly, or transform a
  smaller source-level parameter set into the four runtime range words?
- Were execute and draw order explicit arguments in exceptional profiles even though
  `+0x04` equals the profile index throughout the registry?
- What are the complete actor-flags/property bit definitions?
