# PR #2487: London source review and repair

This continues the accepted eight-function tilting-bridge TU. The factory uses
ordinary C++ construction through the actor allocator. Behavior uses the existing
named fields, while resource loading, collision cleanup, sound and transform
updates use matching class interfaces. Local declarations follow the actual
remaining raw helper definitions.

## Identity and ownership

- Public input: `a0e810a06017ed6782af57eb369002094200f0ad`, branch
  `integrate/london-humanizer-0908`.
- Actual Git source base: `224e660ea0219d0999c58483eb2dd9b38991f5ef`.
- Predecessor: `issue-2480-london-source-review`; its accepted source and five
  findings are preserved, with the remaining scope narrowed by this repair.
- Task: `pr2487-source-review-0910`; producer
  `codex-r2398-london-producer-0910`; policy
  `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Resources: London TU, class header, ov022 manifest and this handoff. No shared
  header/helper, enrollment, data ownership, attribution or global ledger changes.
- Follow-up: [#2480](https://github.com/tangosdev/sm64ds-decomp/issues/2480),
  humanizer/integrator support through @andrewboudreau.

## Findings for independent review

| ID | Kind | Producer disposition |
| --- | --- | --- |
| PR2444-01 | reconstruction | Fixed and preserved: Render uses the real inherited mModel.Render(0), without a fabricated local Model receiver. |
| PR2444-02 | reconstruction | Fixed and preserved: the matrix helper uses a typed local London receiver, real actor fields and mModel.mat4x3. Its shared external char* contract and address-derived name remain unchanged; Clock work belongs to #2481. |
| PR2444-07 | provenance | Fixed and preserved: field uses and lifecycle behavior are stated directly, and promoted status remains distinct from complete reconstruction. |
| LON-PROFILE-01 | provenance | Fixed with new consumer evidence: +4/+6 are behavior/render priorities, read by ldrh in the actual constructor. Current u16 declarations follow that contract. The original signedness is not inferred from positive constants. |
| LON-REMAINING | reconstruction | Partial under #2480: shared collision/callback/helper interfaces and raw camera/resource/CLPS storage remain coordinated work. The manual factory and raw behavior/cleanup aliases listed in the prior handoff are now repaired. |
| LON-CONTRACT-02 | correctness | Fixed: local IsClsnInRange, collider SetFile and func_020393d4 declarations now agree with their actual definitions, including receiver, const matrix and callback integer storage types. |
| LON-CPP-01 | reconstruction | Fixed: adopt the measured ordinary-new factory, resource loaders, collider cleanup, named behavior fields, sound call and callback reference. The exact unreferenced compiler-generated base D2 is explicitly classified. |
| LON-PROVENANCE-02 | provenance | Fixed: current manifest notes describe the repaired source and measured limits. Removed process/codegen banners no longer serve as evidence; dated prior reports remain historical. |

## Source and data evidence

RTTI identifies daObjFl_London_c and direct base dBgActor_c. The factory's
allocation instruction supplies the 0x320 size; RTTI does not encode that size.
The two byte fields remain at 0x31e/0x31f. InitResources and Behavior use the
cooldown and direction flag; Behavior changes mPrevAngleZ between -0x2000 and 0,
then copies it to mAngleZ and refreshes the model transform. This does not claim
recovered original names for the class factory, profile or matrix helper.

The actual fBase_c constructor obtains actorID from data_020a4b54 separately.
Its instructions at arm9:0x02043eac and 0x02043ebc use ldrh on descriptor +4/+6
before calling the priority setters. The descriptor retains its 0x1c layout,
initializers and complete relocated bytes. Current u16 declarations align with
those unsigned consumers; they do not recover original source signedness.

The local IsClsnInRange declaration now uses dBgActor_c* and two scalar integers,
matching src/actors/dBgActor_c.cpp. The collider SetFile definition in
src/_ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block.c takes
void*, void*, const Matrix4x3*, int, short and void*. func_020393d4.c takes int*
and int and stores the value in p[6]. The leaf follows these actual definitions;
it does not substitute a fabricated typed shared ABI. Native callback spelling
&dBgW::UpdatePosWithTransform retains the existing integer storage boundary.

Resource loader/release calls use the existing SharedFilePtr methods through
raw resource anchors. That method-only type does not establish complete shared
storage layout or ownership. The matrix helper still accepts char*, and the
sound vector still crosses the existing scalar camera-field boundary. These,
the raw collision interfaces, callback storage and CLPS anchor remain in #2480.
No universal compiler limitation or completed shared reconstruction is claimed.

## Measured alternatives and proof

The public control and candidate each verify all eight complete functions:
812 text bytes, 39 module-correct text references, zero blind references and zero
differences. Every candidate raw function body is also identical to its public
counterpart. The existing text interval remains [0x02111980, 0x02111cac).

All 196 owned data bytes in [0x02113f2c, 0x02113ff0) and all 37 configured data
relocations match. The range contains RTTI, the type-name string, profile and
vtable storage; its 32-slot address point starts at 0x02113f70 and ends at
0x02113ff0. Metadata reports seven VERIFIED and five PARTIAL rows, without any
DIFFERS. The five partial type-name extents belong to fBase_c, dBase_c, dActor_c,
dBgActor_c and London. The complete licensed data-range check covers London
storage; individual metadata extents are not relabeled as fully verified.

Isolated ordinary-new and value-initialized-new factories, native cleanup,
named behavior fields, resource loaders, unsigned priorities, native sound and
native callback references each preserve all eight functions. The tested native
collider SetFile call, using the current Fix12<int> type, emits a 168-byte
InitResources instead of 160 and fails its byte comparison. The other seven
functions still match in that experiment. This one failed form is recorded
without claiming that all alternatives are impossible.

Ordinary construction emits a 56-byte _ZN10dBgActor_cD2Ev absent from the manual
factory control. It has no configured ROM home and no incoming relocation in the
complete raw object. The exact manifest compiler_only_output entry permits that
unowned helper to be dead stripped; it does not license a new ROM function.
Consequently the complete raw objects differ, while all eight licensed function
bodies are identical. Whole-TU checks retain eight clean isolation plans,
ROM-ascending emission and no unlicensed output after the explicit policy.

Fresh verification also covers four actual dependency bodies: the 352-byte
fBase constructor, 196-byte IsClsnInRange, 576-byte collider SetFile and eight-byte
callback setter. All are VERIFIED with zero blind references or differences.
The changed London header's complete consumer closure is only the London TU,
covered by the eight-function proof above.

Local reproducible proof and experiments are retained under
C:/tmp/sm64ds-r2487-0910/build: baseline/, candidate/,
contract-and-equivalence.json, constructor-extra-proof.json,
interface-probes.json, additional-probes.json and header-consumers.json.
Producer evidence pins the immutable output, contributor-credit check and
terminal production/reference gates. The old manifest build report and original
handoff are historical. Independent source acceptance, current-main composition
and required terminal private validation remain separate obligations.

The fresh complete production build passes: 106/106 modules are byte-identical,
all 11,199 source-built functions reproduce, all 25 owned-data claims reproduce,
and three BSS claims pass their object/symbol gates. Intact-TU verification has
zero new symbol errors against its control. The broad emitted-metadata inventory
still contains four distinct DIFFERS symbols across ten object records; all ten
were freshly reproduced from source definitions unchanged since the actual base
and outside the London header closure. They are retained in
build/inherited-metadata-control.json and are not counted as verified metadata.
The local port check resolves 423 references; the prose/code reference check
reports no new dead references or broken Markdown links.
