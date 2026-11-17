# Merging & coordination

The playbook for anyone reviewing, merging, or coordinating work on this repo —
human maintainers and AI sessions alike. If you are landing a PR or working a
function or class, follow this. See [`AGENTS.md`](AGENTS.md) for what a change
looks like and what the merge gate checks.

## 1. Claims (before you touch a function or class)

For the coordinated v2 class/TU fleet, follow
[notes/agents/PIPELINE.md](notes/agents/PIPELINE.md): a successful queue claim is
mandatory before editing, and the designated integrator controls landing order.
An unavailable service is not permission to bypass v2 ownership. The separate
range service below is not unified with v2; reconcile it during cutover and keep
its work disjoint. The following best-effort fallback applies only outside the
coordinated v2 fleet.

- `claims_check` the span first. `claims_lock` (module / start / end) to reserve it,
  `claims_release` when it is banked.
- Claims are **best-effort**. If they return `401` / "missing key", the claims service
  just is not configured on this machine — note it once and proceed. Each agent already
  gets a distinct batch, so an unclaimed target is fine to work.
- Locks live on the claims service.

## 2. What may be merged

- **Match PRs** must pass the **`validate`** CI check. It compiles every changed
  `src/*.c|*.cpp` on a private build box and compares the relocated bytes to the ROM.
  A passing byte check is required alongside independent source acceptance.
- **Source reconstruction PRs** are also reviewed as source; do not describe a
  green byte check as finished humanization. There is no CI check for this. PRs
  coordinated through the fleet queue carry an independent queue review: run
  `python tools/check_pr_source_review.py --pr NUMBER` immediately before landing
  one, and treat missing evidence, stale review, incomplete coverage and unresolved
  findings as blockers ([the review procedure](notes/agents/SOURCE-REVIEW-CUTOVER.md)).
- **WRONG / blind files.** If `validate` flags any file as `wrong-dest` (a reloc links to
  the wrong symbol) or `blind` (a reloc slot could not be resolved), **drop those files and
  land only the verified subset.** Never merge a file that does not reproduce the ROM — it
  puts a false "match" in the ledger that someone later has to hunt down.
- **NONMATCHING advances.** A PR that improves an in-progress `// NONMATCHING` file (e.g.
  `Stage::PS_Update`) **fails `validate` by design** — a non-matching file cannot byte-reproduce,
  and `main`'s current copy fails the same way. Review it and merge with an admin override
  (`gh pr merge <n> --admin --squash`). These are progress, not byte-matches.
- **Header PRs** (anything touching `include/`) are byte-checked like match PRs, but on the
  *consumers*: `validate` expands each changed header through the reverse include graph
  (`tools/affected_src.py`) and compiles every source that includes it. Green means the header
  edit is codegen-neutral for everything that uses it. Two verdicts to read carefully:
  *"changed header(s) have no source consumer yet"* is a real pass (a header landing ahead of
  its users); *"the validation clone has no tools/affected_src.py"* is a **build-box problem,
  not a PR problem** — pull on the box and re-run, never override it.
- **Near-miss DB / notes / tooling PRs** have no `src` match to byte-check; review for sanity
  and merge.
- **`port/` reference breaks.** A rename, `.c`-to-`.cpp` migration, or file move in `src/`/
  `include/` can strand a `port/` reference (`slice_gate*.txt`, `CMakeLists.txt` hostgen
  symbol lists, `port/hal/*.cpp` linkage bridges) — nothing in `validate` catches this since
  `port/`'s MSVC build isn't part of the decomp toolchain. `python tools/port_refcheck.py`
  is the check (also runs in `tools/hooks/pre-push`); treat a report from it like any other
  bug and fix the `port/` side before merging.
- **Drafts:** never merge someone else's draft. That is the author saying "not ready."

## 3. How to merge

- Merge the contributor's **own PR** with `--squash`.
- **Never re-create someone's work as a new PR under your own name.** Who matched a
  function is the row already in [`function-authors.json`](function-authors.json).
  Moving or rewriting the source does not change that row.

## 4. Conflicts

- For a fork PR with *maintainer edits allowed*, resolve on their branch and push the fix; keep
  their commit so authorship survives. Otherwise ask them to rebase.

## 5. Who matched a function

- The record is [`function-authors.json`](function-authors.json). One row per matched
  function. The key is the module and the address.
- The chart in `contributions.json` is regenerated from that file. Do not hand-edit
  the chart.
- A new match needs a new row. Do not change or delete an existing author.
- Moving a source file does not change the row, so a fold does not need a credit edit.

## 6. After merging

Nothing to do by hand — pushing to `main` auto-runs the workflows that refresh the README
progress bar, the treemap, `chaos-db.json`, and `contributions.json`.
