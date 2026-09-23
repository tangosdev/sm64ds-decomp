# Picture Poker readability: first pass

This document describes this commit; the queue records its immutable SHA.

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2909
- Task: `minigame-card-readable-0922`, reconstruct stage.
- Producer: `codex-minigame-card-readable-0922` (Codex).
- Branch: `cleanup/minigame-card-readable-0922`.
- Source and workflow base: `0bc0d1c2df4fba865dd48841039d1446f2a105c4`.
- Status: locally byte-verified first-pass candidate; independent exact-commit
  source review and current-base integration are still required.

## Source changes

The production `src/minigames/d_s_mg_card.cpp` owns 37 functions in
ov006 `[0x020d95a4,0x020dbe40)` (10,396 bytes). The scene and both card classes
were reserved before editing. No header, symbol, manifest or enrollment changed.

- Replace unused `Obj`, `VObj`, `C`, `Src`, `Struct30`, `Slot6` and `Obj6`
  declarations and redundant scalar typedefs with the existing shared types.
- Deal directly through the modeled player/dealer arrays. ArrangeHand uses typed
  card indexing instead of reconstructing integer addresses and a 0x30 stride.
- Update cards through `dMgCardObj_c` instead of an unrelated Smartball class
  with a coincidentally compatible virtual slot. The recovered header proves
  Update occupies slot 1 in both player and dealer card classes.
- Name histogram counts, face ranks, touch distances and sprite placement locals;
  expand the dense hand-comparison blocks without changing statement order.
- Remove the redundant generated symbol inventory and ordinal separators. The
  manifest remains authoritative; all 35 explicit symbol markers stay in order.
- Correct comments: equal histogram counts prefer the higher face index, while
  singleton faces do not break ties. The actor-style scene callback names do
  not establish eating, swallowing or ground-pound events in Picture Poker.

Names introduced here describe existing bodies; they are not recovered original
identifiers. The header's field/class reconstruction is unchanged.

## Measured compiler constraints

All three direct-histogram-pointer experiments were rejected under `2004/b56`
with production flags (`-O4,p`, C++, exceptions off). Each changes only the
specified initializer in the pre-contract-repair snapshot, which emits the same
complete baseline object; exact sources, objects, hashes and
word offsets are preserved in the producer worktree's ignored
`build/card-readable/experiments.json` and its sibling files.

| Experiment | Exact rejected replacement | Result |
|---|---|---|
| CompareHands | `(short *)(int)countsA/B` -> `countsA/B` | 0x24c -> 0x240 bytes; 144 differing word positions; relocations unchanged |
| MarkSingles | `(short *)(int)counts` -> `counts` | 0x8c bytes retained; 9 differing words; relocations unchanged |
| ArrangeHand | `(s16 *)(u32)counts` -> `counts` | 0x2ac bytes retained; 16 differing words; relocations unchanged |

The shorter integer casts remain. Removing redundant parentheses and the
ArrangeHand intermediate u64 conversion preserves the original object. Keep
DealIn's separately documented reload alias, Render's measured branch shape,
and the header's factory/lifecycle constraints for later targeted experiments.

## Local proof

`python tools/tubuild.py compile ov006/dScMgCard_c` emits exactly the same raw
27,880-byte ELF as the unmodified source at the pinned base:
`e289b5b9cc3bf32742022e2b63cfff1a04692077485ac3272b6fadab2675a3b4`.
This equality includes all code, data, symbol tables and relocations. It is
separate from the explicit cartridge checks below.

- `tubuild.py --manifest build/card-readable/manifest.json verify
  ov006/dScMgCard_c`: 37/37 TEXT-VERIFIED; correct section order, no unlicensed
  outputs, isolation and relocation-destination checks clean. The private
  manifest copy prevents verification from modifying tracked metadata.
- `linkcheck.linkcheck` for every manifest function, after
  `objisolate.plan` and `reloc_audit._as_the_build_links_it` (the same preparation
  as the CLI): 37 VERIFIED, 10,396 bytes, every `diffs: []`, every `blind: 0`.
  Report: `build/card-readable/linkcheck.json`. The initial direct raw-object
  probe omitted vtable-preamble preparation and reported seven wrong addends;
  `linkcheck-raw-preisolation.json` retains that harness result separately.
- `python tools/rombuild.py -j16`: exit 0, ROM built; 106/106 modules exact;
  11,208 source-built functions reproduce; all 26 source-owned data claims
  reproduce; zero new symbol errors against the scratch baseline's nine known
  symbol-table errors. Report: `build/card-readable/rombuild.log`.
- `python tools/romdata_check.py --files src/minigames/d_s_mg_card.cpp
  --json build/card-readable/romdata.json`: 19 emitted data records: 12 VERIFIED
  (276 bytes), 7 PARTIAL (80 covered bytes), zero DIFFERS or UNNAMED. PARTIAL
  remains partial coverage, not new source ownership. The whole-tree report's
  three differing emitted-data records are outside this unchanged TU object;
  the whole-tree report is not a blanket data-completion claim.
- `python tools/port_refcheck.py`: all 418 references resolve.
- `git diff --check`: clean. No shared header changed, so no additional header
  consumer expansion is claimed. Commit-range checks follow this commit.

The independent preliminary reviewer confirmed raw ELF equality, all 35 marker
identities/order and the typed substitutions, and requested the prose corrections
now included here. That review is not formal queue acceptance.

## Remaining scope and next action

This is the first source-quality pass, not completed class reconstruction. The
constructor/destructor callbacks, unnamed shared helpers/data, unknown fields,
remaining byte-stride hand loops, and measured control-flow/alias constraints
remain. The source was examined in full; the second pass follows the first pass
across the full minigame inventory and can reuse these measurements.

The verifier must inspect this exact commit and run its required gates; the
integrator must separately prove its composition with current main and the local
all-open-PR worktree. This focused branch contains no aggregate PR merges.
Attribution remains unchanged because no symbol, source owner or path moved.
Private validation and GitHub Source review are pending; no merge is authorized
by this handoff.

## Independent review rework

The first formal review returned this task to the producer with three open
finding IDs. All are repaired in this commit and require fresh independent
acceptance:

- **CARD-READ-01:** use `Sound.h`'s `u32 Sound::PlayBank2_2D(u32)` declaration
  instead of a local void return. The native definition agrees with the header.
- **CARD-READ-02:** declare `RenderOamMainScreen` as void, matching its definition.
- **CARD-READ-03:** give `DecompressLZ16` a pointer destination and use explicit
  VRAM pointers at the two calls. Give `__cxa_vec_ctor` unsigned counts and real
  element-callback function pointers; remove the callback-to-data-pointer casts.
  The primitive and runtime definitions establish these contracts.
- **CARD-READ-04:** callback-event and singleton-tie prose corrections remain fixed.
- **CARD-READ-05:** the measured ABI/alias constraints and remaining reconstruction
  above remain deferred to issue #2909 and the producer's second pass.

`LoadFile` keeps its buffer-pointer return: the underlying helper returns allocated
buffers that this TU decompresses and deallocates. Copying the raw legacy C
wrapper's integer spelling would not improve that evidenced pointer contract.

The repaired source still emits the identical 27,880-byte raw object with the
SHA256 above. Producer rework uses the new private receipt in ignored build state;
no verifier edited the candidate. Formal acceptance of the rejected predecessor
is not claimed.

## Review follow-up CARD-READ-06

The source cleanup removed redundant local s16/s32 typedef conflict markers,
but their manifest notes still called them open. Those exact two notes now say
RESOLVED, retaining the historical disagreement. The source uses the existing
shared scalar types. All other conflict notes and markers remain unchanged.

This follow-up changes only those note labels and this handoff. Source, headers,
manifest enrollment/function/compiler/data policies and all build tools are
identical to candidate b5c61e1401ccd4970b9f4b9fa09109c198054ccc, independently
verified against base b93e467b9b7b6a75a32a547fcd1158399a3525dd. Its complete
object, strict37 and full ROM proofs are retained by checked input identity;
no additional full ROM execution is claimed for the note-only repair.
