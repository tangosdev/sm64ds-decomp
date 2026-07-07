# Merging & coordination

The playbook for anyone reviewing, merging, or coordinating work on this repo —
human maintainers and AI sessions alike. If you are landing a PR or working a
function, follow this.

## 1. Claims (before you touch a function)

- `claims_check` the span first. `claims_lock` (module / start / end) to reserve it,
  `claims_release` when it is banked.
- Claims are **best-effort**. If they return `401` / "missing key", the claims service
  just is not configured on this machine — note it once and proceed. Each agent already
  gets a distinct batch, so an unclaimed target is fine to work.
- If you edited [`CLAIMS.md`](CLAIMS.md) to reserve a span, remove your line (or
  `claims_release`) once the work lands.

## 2. What may be merged

- **Match PRs** must pass the **`validate`** CI check. It compiles every changed
  `src/*.c|*.cpp` on a private build box and compares the relocated bytes to the ROM.
  **Green = byte-verified = merge.**
- **WRONG / blind files.** If `validate` flags any file as `wrong-dest` (a reloc links to
  the wrong symbol) or `blind` (a reloc slot could not be resolved), **drop those files and
  land only the verified subset.** Never merge a file that does not reproduce the ROM — it
  puts a false "match" in the ledger that someone later has to hunt down.
- **NONMATCHING advances.** A PR that improves an in-progress `// NONMATCHING` file (e.g.
  `Stage::PS_Update`) **fails `validate` by design** — a non-matching file cannot byte-reproduce,
  and `main`'s current copy fails the same way. Review it and merge with an admin override
  (`gh pr merge <n> --admin --squash`). These are progress, not byte-matches.
- **Near-miss DB / notes / tooling PRs** have no `src` match to byte-check; review for sanity
  and merge.
- **Drafts:** never merge someone else's draft. That is the author saying "not ready."

## 3. How to merge (preserve attribution)

- Merge the contributor's **own PR** with `--squash`. GitHub keeps the PR author as the
  commit author, so the contributor chart credits **them**.
- **Never re-create someone's work as a new PR under your own name** — the squash then records
  *you* as the author and strips their credit (this is exactly how a batch of matches once got
  mis-attributed). If you must consolidate — e.g. drop bad files from a fork you cannot push to —
  add a `src → login` entry under `overrides` in [`attribution.json`](attribution.json).

## 4. Conflicts

- `CLAIMS.md` conflicts are the common case (everyone edits it). Resolve by taking **main's**
  version — the claim is moot once the work lands — and keep the `src` file.
- For a fork PR with *maintainer edits allowed*, resolve on their branch and push the fix; keep
  their commit so authorship survives. Otherwise ask them to rebase.

## 5. Attribution & the contributor chart

- The chart reads **`contributions.json`** (`{login: matched count}`), regenerated and committed
  to `main` on **every merge**. **Do not hand-edit it** — fix names in `attribution.json`.
- One person shows up under several names? Git identity varies per machine and per squash. Add a
  `git-handle → canonical GitHub login` entry under `aliases` in `attribution.json`.
- A match landed under the wrong author (e.g. squashed under the maintainer)? Add a `src → login`
  entry under `overrides` in `attribution.json`.

## 6. After merging

Nothing to do by hand — pushing to `main` auto-runs the workflows that refresh the README
progress bar, the treemap, `chaos-db.json`, and `contributions.json`.
