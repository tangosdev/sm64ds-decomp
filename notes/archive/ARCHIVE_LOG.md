# notes/ archive log

Generated 2026-08-14 by an automated triage pass (4 parallel haiku agents, one
per ~14-file slice of `notes/`, each checking currency, supersession, inbound
references, and whether described work has already landed). Results were
independently verified against `git status` / file content before being
trusted — see the corruption note at the bottom.

Nothing was deleted. Archiving is a plain filesystem move; git still has full
history for every file under its original path.

## Archived (1 of 57 reviewed)

| File | Reason |
|---|---|
| `n64-decomp-cross-reference.md` | Self-marked `Status: investigation notes, incomplete. Parked 2026-08-06 for later revisit.` Not linked from any current handoff/plan doc. **Mislabeled, corrected 2026-09-06:** the file has kept taking substantive rename-cascade commits every week since this archiving (it is not dead, just physically parked under `notes/archive/`, which stays frozen either way). `notes/data/notes-registry.tsv`'s `status` column for it was changed from `historical` to `live` to match; the file itself was not moved. |

## Kept (56 of 57)

All other files were judged current/load-bearing: either actively linked from
other notes or `AGENTS.md`/`CONTRIBUTING.md`/`README.md`, describe still-open
work, or are the canonical reference for an idiom/process still in use
(`matching-style.md`, `mwccarm-codegen.md`, `pret-idioms.md`, etc.). No note
was found to state a claim about compiler idioms that is now contradicted
elsewhere — the "idiom notes going stale" concern didn't turn up a concrete
case in this pass, but see the two flagged items below for the closest
candidates.

## Needs a human decision (left in place, not archived)

| File | Why it's borderline |
|---|---|
| `overlay-ambiguous-references.md` | Content opens with "Status: mostly closed... of the 137 references that were really overlay-ambiguous, 10 remain." Mostly-resolved but not fully — could go either way. (Also: this file got mangled by a tool-execution glitch during the automated pass and was restored verbatim from git HEAD — see below.) Resolved 2026-09-06: zero inbound citers were the only gate-visible surface left pointing at it after `notes/overlay-residency.md` closed 127 of its 137 cases, so its remaining unique content (the original disagreement table and the three wrong-callee bug names) was folded into `overlay-residency.md` §0 and this file was deleted; see the row below. |
| `plan-scalar-markers.md` | Opens with "Status: done for the 71 mechanical cases. A handful remain for a person -- `gen_header.py --report` is the live count; it was 1 when this was written." One review agent recommended archiving it; I left it for a human call since "1 remains" means it isn't fully closed. |

## Deleted 2026-09-06 (notes/ audit, slice 5)

Per `CONVENTIONS.md` §3, zero-inbound retirement is delete-outright-plus-ledger-row,
never a move — `notes/archive/` stays frozen (no new entries by this rule or by
hand). All nine rows below were verified zero-inbound across the three §3
searches (the dead-reference gate's own scan, every other note's relative links,
and `git grep` by basename) before deletion; the six with real citers were
MERGE-disposition (every citer gate-visible, every citation rewritten in the same
commit as the deletion), not zero-inbound.

| File | Last commit | Reason |
|---|---|---|
| `handoff-validator-and-tests.md` | `bf534e80b` | Zero inbound. All three of its asks (test coverage, `check_header_offsets.py` gate, the 200-file cap doc) are resolved; `tool-tests.yml`'s own header now explains the rationale better and more currently. |
| `class-init-pilot.md` | `3d4377d61` | Zero inbound. Strict subset of `actor-profile-pilot.md` (same 20-row pilot, same WATERFALL/PLAYER/STAR_CAMERA/collision findings), born and never updated since the originating PR while `actor-profile-pilot.md` was extended to the full 391-row registry. |
| `enemy-leaf-provenance.md` | `8659c0029` | Nonzero inbound (6 `include/*.h` field-provenance comments), all gate-visible. MERGE: content folded into `enemy-provenance.md` (the "set A"/"set B" split was a session-order artifact, not semantic); all 6 citers repointed in the same commit. |
| `overlay-ambiguous-references.md` | `2d5657139` | Nonzero inbound (`include/Player.h`, `notes/rtti-reconciliation.md`, `tools/rtti_symbols.py`, this ledger's own incident note), all gate-visible. MERGE: unique content (original 169-reference disagreement table, the three wrong-callee bug names, reproduction commands) folded into `overlay-residency.md` §0; all citers repointed in the same commit. |
| `profile-macro-patterns.md` | `f23240bb1` | Nonzero inbound (`notes/profile-lifecycle-crosswalk.md`, twice), gate-visible. MERGE: the two tables not already covered by `actor-profile-pilot.md` (data-placement order, factory-placement/vptr-population stats) plus the 0x08-misread mechanism folded into `actor-profile-pilot.md`; both citers repointed in the same commit. |
| `sinit-ownership-pilot-ov002.md` | `f7314e903` | Zero inbound. Case-log content (dEnemyBase_c) folded into new `sinit-ownership-cases.md` alongside the other three case files below; `sinit-ownership.md` (the methodology README) never cited any of the four and is unchanged. |
| `sinit-ownership-wave-edges.md` | `145719519` | Zero inbound. Case-log content (CutsceneObject+daDemo_c, Painting) folded into `sinit-ownership-cases.md`. |
| `sinit-ownership-wave-resource-handles.md` | `d40735380` | Zero inbound. Case-log content (Bird, daObjBkBillboard_c, daObjMarioCap_c) folded into `sinit-ownership-cases.md`; the promotion-status prose was stale in the good direction (Bird and daObjBkBillboard_c are now `text-verified`/`promoted`) and was corrected in the merge. |
| `sinit-wave-a-ownership.md` | `e4b63e380` | Zero inbound. Case-log content (OneUpMushroom, daObjKurumajiku_c, MadPiano) folded into `sinit-ownership-cases.md`. |

## Incident: one agent's move command malfunctioned

While archiving `n64-decomp-cross-reference.md`, one agent's shell also
somehow renamed `overlay-ambiguous-references.md` to a garbage filename
(`archive"` + an embedded newline + literal leftover command text) sitting
directly in `notes/`, instead of moving it into `notes/archive/`. Content was
unaffected (verified byte-identical against the git-tracked HEAD copy, modulo
CRLF/LF). I found it via `git status`, confirmed the content, and renamed it
back to its original path under `notes/`. No data was lost,
but it's the reason I independently re-verified every claimed move in this
pass rather than trusting agent self-reports at face value.
