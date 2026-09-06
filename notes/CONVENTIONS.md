# notes/ Conventions

`notes/` is append-only today: +92,348 lines added against -6,534 deleted, lifetime
(14:1); 76 of 113 files were added in a single month; 337 notes-commits landed in the
last 7 days touching 117 distinct files in 14. This doctrine has to survive that
velocity without requiring a mass edit nobody has time for, or discipline nobody
enforces. It stays short on purpose — a norms doc nobody rereads is not a norms doc.

## 1. Note types

| Type | Pattern | Purpose | Lives how long |
|---|---|---|---|
| Reference | `<topic>.md`, `<topic>-provenance.md` | durable fact/procedure other work cites | until superseded |
| Plan | `plan-*.md` | a proposal for work not yet landed | until the work lands or is dropped |
| Runbook | `runbook-*.md` | a repeatable operational procedure | indefinite, revised in place |
| Handoff | `handoff-*.md` | one session's state for the next reader | 2-4 weeks, then retire (§3) — never a long-term reference |
| Data | `data/*.tsv`, `data/*.json` | machine-read/written facts, usually tool-regenerable | as long as a tool reads it |
| Agent process | `agents/**` | pipeline/role definitions | indefinite, revised in place |
| Archive | `archive/*.md` | frozen historical record | frozen — see §3 |

A session's tracking doc, a per-agent work queue, or a personal fork of a committed
note is none of these. It does not go in `notes/` at all — see §4.

## 2. Lifecycle

`created -> live -> superseded -> retired`. No note has an owner; whoever is touching
it moves it between states:

- **created -> live**: the commit that adds real content. A stub with an unfilled
  table never reaches "live" and should not have been committed — see the
  `goal-promote-agent-suite.md` example in §4.
- **live -> superseded**: whoever writes the replacement adds an in-place banner to
  the OLD note in the SAME commit that adds the new content. Never a separate
  cleanup commit, never a silent fork. `tu-promotion-conventions.local-draft.md` is a
  fork done wrong: it stalls the committed 537-line doc at 274 lines, shares zero
  unique headings with it, and its unique content — pre-rename file-path conventions,
  a PR marked "open draft" that closed three revisions ago — is actively wrong, not
  merely stale.
- **superseded -> retired**: mechanical, §3. No judgment call, no approval needed.

## 3. Retire rule

A note is a retire candidate at zero inbound citations, the union of three searches
(any nonzero hit disqualifies it):

1. `python tools/check_dead_references.py --list` reverse lookup — literal
   `notes/<path>` text anywhere in the repo (the real gate's own scan).
2. Every other `notes/**` file's own relative links, resolved against **its**
   directory (`os.path.relpath` arithmetic) — the bare-filename note-to-note link
   style, invisible to (1). Measured: of 31 notes with zero hits from (1), 2
   (`actor-profile-pilot.md`, `llm-assisted-global-data-migration.md`) are cited
   exactly this way and are not retire candidates.
3. `git grep -n "<basename>"` across the whole tree — a fallback for file types still
   outside (1)'s surface. As of the 2026-09 gate extension, `check_dead_references.py`
   itself now scans `.c`/`.h`/`.cpp`/`.hpp` comments, `.js` comments and string/template
   literals, and every `.json`/`.jsonl` (config prose plus `tu_manifest.d`'s
   `boundary_evidence`/`notes` fields), so a hit here that falls inside those surfaces
   should already have surfaced in (1) — this step is now needed mainly for what (1)
   still can't reach. The historical "48 of 75" count predates that extension and
   should be re-measured before relying on it.

**Zero on all three:** delete the file outright in one commit and add a row to
`notes/archive/ARCHIVE_LOG.md` (path, date, one-line topic, last commit sha). Do not
move it to `archive/` — history plus the ledger row is enough.

**Nonzero on any:** never delete, never move. Add an in-place status banner
(`STATUS: SUPERSEDED — see <link>`, `STATUS: STALE — verify before use`, or
`STATUS: LIVE`) and leave the file exactly where it is. A banner is free — the path
doesn't change, so nothing that cites it, and none of the 785 depth-sensitive relative
links elsewhere in `notes/`, needs to change. A move never is: 11 files are `PINNED`
(read at runtime from `config/tu_manifest.d/**`, not CI-checked), 14 more are `HIGH`.
The last archive-by-move pass produced exactly this failure: `pret-idioms.md` moved
from `notes/` to `notes/archive/pret-idioms.md`, and agent-prompt files (`tools/refine_run.js`,
`tools/sched_run.js`, `tools/archive/crack_pr104.js`) kept sending agents to the dead
path — invisible to (1) because nothing scanned `.js` string/template literals. Both
gaps are now closed: `check_dead_references.py` scans `tools/**/*.js` (comments and
string/template literals) among the other newly-added surfaces in (3) below, and every
citer was repointed — the four live agent-prompt sites (`tangos.json`,
`tools/chaosviewer.config.json`, `tools/refine_run.js`, `tools/sched_run.js`) to
`notes/matching-style.md`, since pret-idioms.md's guidance is superseded there rather
than merely relocated, and the remaining non-prompt citers to the real
`notes/archive/pret-idioms.md` path. `notes/archive/` remains **frozen** regardless: no
new entries, by this rule (retirement deletes, never relocates) or by hand — the 8
files already there stay put, since moving them back costs the exact citation-rewrite
this rule exists to avoid, for files that all still carry live citations.

## 4. Where new findings go

**Session scratch, tracking docs, per-agent work queues** never touch `notes/` — they
live in the session's own scratchpad, or a gitignored path if they must survive a
worktree boundary. `.gitignore` bans the naming habits that produce this, as a
backstop, not the primary control: `*.local-draft.md`, `goal-*.md` (see the
`goal-promote-agent-suite.md` example in §2), `*_agent[0-9]*.tsv`/`.md`, and a dated
filename (`recon-2026-06-13.md` style).

**A durable new fact that belongs to an existing note:** append to that note, in the
commit that found it, and update its freshness stamp (§5) in the same edit — never a
`-v2` file, never a `.local-draft` copy.

**A genuinely new topic:** cited by more than the session that produced it, with no
existing note covering the area — otherwise it's an append. Nothing enforces this
automatically; a new `notes/*.md` file in a PR is a review flag, like a new dependency.

## 5. Freshness contract

A point-in-time number needs a **freshness contract** (what invalidates it) or a
**`Measured <date> on <sha>`** stamp naming the exact command that produced it.
Neither is a permanent fix: `handoff-marker-typing.md` did the stamp correctly
(`Written 2026-08-06`, "the command that produces it appears with it"), and its
**767 bare object markers remaining** was still **20x stale a month later**
(`tools/marker_census.py` reports 37, measured 2026-09-05). Prefer citing the command
over citing its output; if you must cite the output, expect it to rot at this
project's velocity and say so.

Same failure at tool scale: `python tools/cpp_tu_state.py --check-note` is documented
as the authority in 6 places (the role docs, `cpp-tu-current-state.md`, and
`real-cpp-migration-runbook.md` twice) and wired into exactly 0 CI workflows.
Documenting a check is not running it.

## 6. Citation debt

Every rename or move pays its citation debt in the **same commit**: update every
citer (§3's three searches), rewrite every depth-sensitive relative link the move
touches — its own outbound links, and every other note's inbound link whose depth
changed — recomputed with `os.path.relpath`, never hand-counted `../`. Confirm with
`python tools/check_dead_references.py --list`.

**Never bank a break you just caused into `config/dead-reference-baseline.json`.**
`--update` is for pre-existing history you are not touching, not a way to make your
own diff pass. Five entries in that file today are exactly this pattern from the last
archive move — still green, still wrong.
