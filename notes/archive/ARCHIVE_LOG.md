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
| `n64-decomp-cross-reference.md` | Self-marked `Status: investigation notes, incomplete. Parked 2026-08-06 for later revisit.` Not linked from any current handoff/plan doc. |

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
| `overlay-ambiguous-references.md` | Content opens with "Status: mostly closed... of the 137 references that were really overlay-ambiguous, 10 remain." Mostly-resolved but not fully — could go either way. (Also: this file got mangled by a tool-execution glitch during the automated pass and was restored verbatim from git HEAD — see below.) |
| `plan-scalar-markers.md` | Opens with "Status: done for the 71 mechanical cases. A handful remain for a person -- `gen_header.py --report` is the live count; it was 1 when this was written." One review agent recommended archiving it; I left it for a human call since "1 remains" means it isn't fully closed. |

## Incident: one agent's move command malfunctioned

While archiving `n64-decomp-cross-reference.md`, one agent's shell also
somehow renamed `overlay-ambiguous-references.md` to a garbage filename
(`archive"` + an embedded newline + literal leftover command text) sitting
directly in `notes/`, instead of moving it into `notes/archive/`. Content was
unaffected (verified byte-identical to `git show HEAD:notes/overlay-ambiguous-references.md`
modulo CRLF/LF). I found it via `git status`, confirmed the content, and
renamed it back to `notes/overlay-ambiguous-references.md`. No data was lost,
but it's the reason I independently re-verified every claimed move in this
pass rather than trusting agent self-reports at face value.
