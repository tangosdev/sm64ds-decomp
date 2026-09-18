# BATCH2-2685-02: retained-dependency dispositions (PR #2685)

`completion: partial`

This is the review-decision record that review finding BATCH2-2685-02 asked for.
It is not a source-review pass, and it does not replace one: the disposition
vocabulary and the acceptance requirements come from
`notes/agents/SOURCE-REVIEW-CUTOVER.md`, and only an independent verifier
publishes `source_review`. What this file does is make the batch's retained
dependencies enumerable, so each one carries an id, a location, a kind, an
explicit disposition, a reason, a tracking issue and a next owner instead of
living only as `Leftover:` prose spread over 39 source files.

Scope: the classes in PR #2685 ("Cleanup leftover overlay TUs (batch 2)").
Measured compiler claims are pinned by the 20 artifacts in this directory and
indexed by `notes/experiments/batch2-2685-not-experimentable.md`; that index
stays the authoritative per-block map, and this file keys to it by location.

## Disposition vocabulary

From `notes/agents/SOURCE-REVIEW-CUTOVER.md`:

- `fixed` - corrected in this candidate; changed source receives fresh byte proof.
- `compiler_constraint` - reconstruction finding only; a measured cleaner
  alternative under pinned `2004/b56`, with pinned experiment source and a
  durable committed artifact.
- `deferred` - reconstruction finding only; the reviewer explicitly accepts
  partial scope, **names the issue and next owner**, and sets `completion: partial`.
- `open` - cannot publish pass; return through rework.

Correctness and provenance findings cannot be relabelled as reconstruction to
defer them. That rule is why section A below is `fixed` rather than `deferred`.

## A. Provenance findings - corrected in this candidate (`fixed`)

BATCH2-2685-01 was raised against `src/game/actors/d_a_pg_mthr.cpp`. The same
claim - that an existing `func_ovNNN_*` label is a preserved original symbol, and
therefore a barrier to implementing a real member - recurred verbatim in two more
files in this batch and in softer form in a third. `func_ovNNN_*` names are
address-derived analysis labels; the image carries no original mangled symbol
table, so an existing label is neither evidence that a helper was originally free
nor a compiler barrier. A migration renames source and config together.

| id | location | summary | disposition |
| --- | --- | --- | --- |
| B2-PROV-01 | `src/game/actors/d_a_pg_mthr.cpp:10` | Rationale corrected earlier; its ownership evidence was still miscounted ("all four" for a 15-helper range, "one extern decl" for three declaration sites). Counts and migration scope corrected. | `fixed` |
| B2-PROV-02 | `src/game/actors/d_a_bakubaku.cpp:9` | Carried the pre-correction wording verbatim ("the ROM symbols are still the func_ov labels ... would ... miss those labels"). Rewritten; ownership evidence and migration scope recorded. | `fixed` |
| B2-PROV-03 | `src/game/actors/d_a_wanwan.cpp:11` | Same verbatim wording. Rewritten; the weaker evidence here (untyped `char*`/`void*`/`u8*` receivers) is stated as weaker. | `fixed` |
| B2-PROV-04 | `src/actors/daMky_c.cpp:12` | "stay those ROM labels" - same conflation, no barrier claim. Retitled to address-derived repository labels; notes that `0211124c` is owned by another TU. | `fixed` |
| B2-PROV-05 | `src/actors/daMky_c.cpp:1566` | "ROM symbol is Vector3_16" - that spelling is a `config/arm9/symbols.txt` row, not ROM content. Reworded. | `fixed` |
| B2-PROV-06 | `src/game/actors/d_a_s_cre.cpp:13` | Cited `#2513` for the shared `dActor_c::Pos()` boundary. `#2513` is a closed PR ("Cleanup ov002/daObjLava_c"). The boundary is issue **#2566**. Corrected here and in the index note. | `fixed` |

These are prose and rationale only, with no emitted byte changes; the commit that
carries this file records its build validation.

## B. Retained raw-field dependencies (`deferred`)

Fields this batch reads through an offset or a cast because the owning class does
not present them yet. Each stays as written; none is claimed to be impossible.

| id | location | field / access | owning surface | issue | next owner |
| --- | --- | --- | --- | --- | --- |
| B2-RF-01 | `src/actors/daBombking_c.cpp:37` | `Player+8`, `+0x6ce`, `+0xc8`; `Camera+0x114`/`+0x154` | Player, Camera | #2479 | unassigned |
| B2-RF-02 | `src/actors/daMip_c.cpp:75` | `Player+8`, `+0x6d9`, `+0x6ce` | Player | #2414 | unassigned |
| B2-RF-03 | `src/actors/daMky_c.cpp:22` | `Player+8`, cap `+0xc8` | Player, cap class | #2410 | unassigned |
| B2-RF-04 | `src/actors/daSanbo_c.cpp:65` | `Player+8` | Player | #2477, #2558 | unassigned |
| B2-RF-05 | `src/game/actors/d_a_wanwan.cpp:31` | `ClosestPlayer()+0x6fb` byte | Player | none filed | none |
| B2-RF-06 | `src/game/actors/d_a_obj_bk_dossunbar.cpp:19` | `unk_0a4` is `dActor_c` X speed, not this class's field | dActor_c | none filed | none |
| B2-RF-07 | `src/game/actors/d_a_wanwan.cpp:34` | `(Vector3 *)&mPosX` / `&mScaleX`; `Pos()` not on this branch's `dActor_c` | dActor_c | #2566 | @andrewboudreau |
| B2-RF-08 | `src/game/actors/d_a_s_cre.cpp:13` | `Pos()` written as a leaf overlay of `mPosX/Y/Z` | dActor_c | #2566 | @andrewboudreau |
| B2-RF-09 | `src/game/actors/d_a_bakubaku.cpp:37` | `data_0209f32c` read as water height | the owner of that global | none filed | none |

**B2-RF-05, B2-RF-06 and B2-RF-09 do not yet satisfy `deferred`.** That
disposition requires a named issue and next owner. Until one exists for the
Player byte-field surface, the `dActor_c` `unk_0a4` speed field and the
`data_0209f32c` global, those three are `deferred` in intent and `open` by the
letter of the rule. The Player offsets recur across B2-RF-01..05, so one issue
covering the Player field surface would close four of them at once.

## C. Retained interface dependencies (`deferred`)

Call shapes and typed views retained because the shared surface has not been
recovered, not because a cleaner spelling was measured and rejected. Where a
cleaner spelling *was* measured and rejected, the row is in section D instead.

| id | location | dependency | reason retained | issue | next owner |
| --- | --- | --- | --- | --- | --- |
| B2-IF-01 | `src/game/actors/d_a_wanwan.cpp:27` | `SharedFilePtr` reached through `decl_common.h`'s `char` view | no recovered layout for `SharedFilePtr` | none filed | none |
| B2-IF-02 | `src/game/actors/d_a_bakubaku.cpp:32` | `decl_common.h` `extern int` handles punned to `SharedFilePtr` | typing them cleanly is a shared-header change | none filed | none |
| B2-IF-03 | `src/game/actors/d_a_pg_mthr.cpp:34` | `data_ov018_*` handles + `decl_common` `int[]` spelling of `02112c0c` | same shared-header change, plus S14 packaging | none filed | none |
| B2-IF-04 | `src/actors/daObjLava_c.cpp:13` | wrapper plants the default callback; direct `New` needs callback wiring | a wiring change, not a spelling | none filed | none |
| B2-IF-05 | `src/game/actors/d_a_obj_bk_dossunbar.cpp:12` | extern-C state bodies, PMF records filled by sinit | the tables are not this TU's | none filed | none |
| B2-IF-06 | `src/game/actors/d_a_obj_ki_ita.cpp:13` | `func_ov002_020b5e58` shared float-board setup; slot 0 pure virtual on the base | naming belongs in ov002 | #2563 | unassigned |
| B2-IF-07 | `src/game/actors/d_a_obj_wc_obj01.cpp:13`, `d_a_obj_wc_obj06.cpp:14`, `d_a_obj_wc_obj07.cpp:15` | the same shared ov002 float-board / Kaitendai helpers | naming belongs in ov002 | none filed | none |
| B2-IF-08 | the twelve ov002/ov098 `Init`/`Cleanup` rows listed under "Naming belongs in ov002/ov098/base" in `batch2-2685-not-experimentable.md` | base-class linker names, base leaves the slots pure virtual | naming belongs with the base | none filed | none |

Same caveat as section B: every row whose issue column reads "none filed" is
`deferred` in intent only. The ov002/ov098 shared-base naming surface
(B2-IF-06..08) is one surface, not twenty, and one issue would carry it.

## D. Compiler-constraint claims, pinned (`compiler_constraint`)

Pinned by the artifacts in this directory, each recording `tested_commit`,
`compiler`, `attempted_change`, `command`, `result` and `log` against
`2004/b56`. The verdicts are the artifacts' own.

SUPPORTED: `batch2-2685-pgmthr-setanim`, `-pgmthr-dropshadow`, `-pgmthr-init-1ac`,
`-pgmthr-matrix`, `-bakubaku-matrix`, `-bakubaku-deadstore`, `-dossunbar-resdesc`,
`-dossunbar-setfile`, `-waterfall-levelid`, `-waterfall-vector3`,
`-waterfall-particle`, `-wcobj01-getclsn`, `-wcobj06-getclsn`, `-wanwan-factory`,
`-wanwan-matrix`.

DISPROVED, and the clean spelling adopted in commit `57e14aa5a`:
`batch2-2685-bakubaku-klass-ice`, `-wanwan-stump`, `-damky-matrix`, `-damip-matrix`.

A DISPROVED artifact is the system working: the claim was tested, the cleaner
spelling won, and the source moved. Those four are `fixed`, not deferred.

## E. Compiler-barrier claims NOT yet measured

Claims of the form "the member form will not link" that no artifact pins. A link
verdict needs a full `mwldarm` link of the range; `tools/match.py` compares bytes
and cannot decide it, and `tools/linkcheck.py` resolves relocations rather than
performing that link. These are downgraded in the source to an honest "not yet
measured" rather than being asserted as fact.

| id | location | claim | status |
| --- | --- | --- | --- |
| B2-NM-01 | `src/game/actors/d_a_pg_mthr.cpp:23` | `dBgCh_Actr::Init` member form "links Undefined" | **not yet measured.** The mechanism that *is* provable is recorded instead: `include/types.h:45` makes `Fix12i` a plain `s32`, so a member-form call mangles that argument `i`, while `config/arm9/symbols.txt:1294` spells the row `5Fix12IiE`. Whether `mwldarm` then reports Undefined is untested. |
| B2-NM-02 | `src/game/actors/d_a_bakubaku.cpp:24` | the same claim | **not yet measured**, same rewrite. |
| B2-NM-03 | `src/game/actors/d_a_pg_mthr.cpp:23` | `TextureSequence::SetFile` method-form size cost | **not yet measured** - no size predicted, no artifact. Retained as a wall reference only. |
| B2-NM-04 | `src/game/actors/d_a_obj_bk_dossunbar.cpp:16` | `IsClsn` / `NewSimple` wall | **not yet measured** - no sizes. `SetFile` in the same block *is* pinned (`-dossunbar-setfile`). |
| B2-NM-05 | `src/actors/daMky_c.cpp:19`, `daMip_c.cpp:75`, `daSanbo_c.cpp:65`, `daBombking_c.cpp:37`, `d_a_wanwan.cpp:25` | generic "wall 6az" references | **not yet measured per TU.** The wall is pinned for pg_mthr and dossunbar; these five cite it without a per-TU size. Honest status: inherited, not independently measured. |

## F. Packaging deferrals carried forward

Overlay `.data`/`.rodata`/file-table words, sinit file IDs and S14 text-only
`g_profile` placement, enumerated per file in
`batch2-2685-not-experimentable.md`. These are ownership and packaging
statements, not interface dependencies: the words exist, this TU does not own
them, and nothing in this batch claims otherwise. Disposition `deferred`, tracked
with the owning overlay's promotion work. No issue is filed for any of them.

## What this record does not establish

- It is not a `source_review` pass, and it does not assert one.
- It does not claim the batch has had complete independent source acceptance.
  PR #2685 spans 111 files across many classes and shared headers; the two named
  review findings are addressed here, and the remainder of the diff is not
  re-reviewed by this record.
- Every "none filed" in the issue column is a real gap under
  `notes/agents/SOURCE-REVIEW-CUTOVER.md`, not a formality.
