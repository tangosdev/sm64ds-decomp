# Actor/profile declaration pattern pilot

Status: binary-pattern analysis only.  No original SM64DS macro name or syntax
has been recovered, and this report does not introduce one into canonical
source.

## Result

The sample supports a repeated **profile declaration architecture**, but not an
exact recovered macro.

Across 20 deliberately varied registry entries:

- all 20 descriptors are four-byte aligned;
- 14 actor-derived entries use the existing 0x1c `SpawnInfo` layout;
- six scene/process entries use a distinct 0x08 base-profile layout;
- 19 factories are ordinary construction wrappers;
- 18 directly store the most-derived vptr in the factory, while one calls a
  constructor; other field stores are not classified by this pilot;
- 13 factories are the last function in the currently recovered TU and seven
  are not;
- RTTI/profile/vtable ordering has several recurring forms rather than one
  fixed section order.

This is consistent with a source system that generated or standardized profile
objects and class-initializer wrappers.  It is also consistent with explicit,
repeated declarations governed by coding convention.  The ROM evidence cannot
choose between those explanations or recover the macro spelling.

## Two descriptor families

The pilot rejects a single undifferentiated `SpawnInfo` layout.

### Actor profile: 0x1c

For dActor-derived entries, the registry target has seven words:

```text
+0x00  factory
+0x04  s16 behavior priority
+0x06  s16 render priority
+0x08  flags
+0x0c  vertical range offset
+0x10  range
+0x14  draw distance
+0x18  trailing/unknown word
```

The existing project type `SpawnInfo` remains the evidence-bounded C++ name.
Architecturally this object functions as an actor profile descriptor.  The
pilot does not claim that Nintendo's SM64DS type was literally `ActorProfile`.

### Base/scene profile: 0x08

For dScene-derived entries such as `BOOT`, `STAGE`, `MINIGAME`, `MG_CUP`, and
the two Curling scenes, the registry target has only:

```text
+0x00  factory
+0x04  packed execute/draw priorities
```

Reading these as 0x1c actor profiles consumes adjacent unrelated data.  The
extractor therefore chooses the descriptor family from RTTI ancestry and emits
blank actor-only fields for 0x08 entries.

This two-family result has a close conceptual analogue in later EAD code, where
the framework distinguishes base and actor profile declarations.  The public
NSMBW decomp's pinned
[`f_profile.hpp`](https://github.com/NSMBW-Community/NSMBW-Decomp/blob/2e010f8708d8232c736b1ece507400dfd76aaa9c/include/game/framework/f_profile.hpp)
materializes a `className##_classInit` wrapper and `g_profile_##profName` object
from separate base/actor profile macros.  That is Tier B lineage evidence only;
it is not proof that SM64DS used those macro names or token concatenations.

## Repeated data placement

Observed relative orders in the 20-row sample are:

| Relative address order | Count | Interpretation |
|---|---:|---|
| RTTI, RTTI name, profile, vtable | 8 | most common compact actor cluster |
| profile, RTTI, RTTI name, vtable | 5 | common scene/process arrangement |
| RTTI name, RTTI, profile, vtable | 5 | alternate RTTI emission order |
| profile, RTTI name, RTTI, vtable | 1 | arm9 `BOOT` variant |
| base vtable, profile; no most-derived RTTI | 1 | unresolved `STAR_CAMERA` exception |

Six actor profiles place the vtable exactly `0x24` bytes after the 0x1c
descriptor (`CANNON_SHUTTER`, `C1_TRAP`, `WATERFALL`, `OBJ_MIP_KEY`,
`PROPELLER_HEYHO`, and `KINOKO_CREATE_TAG`).  Other valid actors use offsets
from `0x5c` through `0x138`, so `+0x24` is a recurring packed-data pattern, not a
layout invariant.

The profile can precede or follow RTTI.  A source macro might expand into data
whose final section placement is affected by compiler/linker emission, but the
observed ordering does not justify reconstructing a single declaration order.

## Factory placement

Thirteen sampled factories are the final function in their recovered text TU;
seven are not.  The non-final set includes arm9 under-segmentation, shared-profile
TUs, and class families with other code following the factory.  Therefore
“factory last” is useful supporting evidence in compact TUs, not an acceptance
rule.

The repeated construction shapes are stronger evidence than text placement:
19 of 20 allocate, establish the recovered class identity, and return the
registered object with no gameplay logic.  The full operator-new inventory
independently attributes 391 sites: 388 directly store the class vptr and three
route through a constructor.  These population counts support a systematic
source convention beyond the 20-row sample.

Directional static-initializer analysis assigns an initializer to ten distinct
sampled recovered TUs: seven at high confidence and three at medium confidence.
These assignments strengthen TU grouping for cases such as Cannon Shutter,
Teresa, Wanwan, Mip Key, Propeller Heyho, and the shared Book TU.  Initializer
presence is not uniform, however, and its `.init` address is deliberately not
treated as adjacent to the owning `.text`.

## Profile/class namespace separation

The declaration model needs two independent parameters.  Direct ROM examples
include:

```text
WATERFALL       -> daObjWaterfall_c
KURIBO          -> daKrb_c
MG_CURLING      -> dScMgCurling_c
MG_CURLING_J    -> dScMgCurling2_c
```

Deriving a global from the class would erase genuine registry identity.  A
source generator, if present, therefore plausibly accepted both a profile ID
and a class, like the structural relationship visible in later EAD code.  A
public NSMBW example is
[`d_a_obj_fruit_tree.cpp`](https://github.com/NSMBW-Community/NSMBW-Decomp/blob/2e010f8708d8232c736b1ece507400dfd76aaa9c/source/dol/d_basesNP/bases/d_a_obj_fruit_tree.cpp),
which supplies a profile token and a C++ class separately.  Again, this only
provides a reconstruction prior.

## Why the exact `classInit` macro remains unresolved

The full registry creates a contradiction for naive global token concatenation:
42 of 310 distinct `<Class>_classInit` proposals have multiple factory
addresses.  The pilot includes same-class multi-profile cases in the same TU,
notably `TERESA` and `BOSS_TERESA`, where two functions would both become
`daTrs_c_classInit`.

Possible historical explanations include static/local linkage, unique generated
suffixes, profile-qualified names, or a different source organization.  None is
selected by the ROM evidence.  Inventing a compensating macro would hide this
open question.

## Safest experimental source shape

For an unambiguous case, the first experiment should keep declarations explicit:

```cpp
// Reconstructed source-style names; exact original spellings are not preserved.
daObjWaterfall_c* daObjWaterfall_c_classInit()
{
    return new daObjWaterfall_c();
}

const SpawnInfo g_profile_WATERFALL = {
    daObjWaterfall_c_classInit,
    // Existing ROM-verified fields, explicitly listed.
};
```

This keeps compiler behavior, relocations, layout, and field values visible.  A
project-local helper such as `SM64DS_PROFILE_RECONSTRUCTED` should be considered
only after several explicit declarations pass strict byte and relocation gates;
it must be documented as a decomp abstraction, not recovered Nintendo syntax.

## Confidence and unresolved questions

High-confidence architectural findings:

- the registry has profile objects whose first word is a construction factory;
- actor and base/scene descriptors have distinct layouts;
- the ROM profile ID and RTTI class are separate namespaces;
- construction-wrapper behavior is systematic.

Lineage-supported findings:

- `g_profile_<ROM_ID>` is a coherent reconstructed global-name scheme;
- `<Class>_classInit` is coherent only where it is unique;
- later `d_a*` / `d_sc*` organization is a useful filename prior.

Unresolved:

- whether SM64DS used macros at all;
- exact macro/type/global/factory spellings;
- how same-class multi-profile factories were named or scoped;
- whether profile data and RTTI were deliberately adjacent in source or merely
  co-located by section/link behavior;
- whether current TU boundaries split some minigame factories from their true
  source units.

The binary supports a macro-like architecture.  It does not yet support adding
a purported original macro to canonical source.
