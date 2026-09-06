# CLAUDE.md

This is `sm64ds-decomp`: a from-scratch decompilation of Super Mario 64 DS. The
goal is source that reads like it shipped from Nintendo EAD in 2004 *and*
compiles byte-identical to the retail cartridge under the pinned mwccarm
compiler. Both halves are required; when they conflict, byte accuracy wins,
because it's the only half a machine can check.

## Before you touch anything

- **Read [`AGENTS.md`](AGENTS.md) first.** It states the one rule that gates
  every PR (`src/` must byte-reproduce the ROM) and what a change looks like.
- **No ROM or Nintendo asset lives in this repo.** Everything byte-verified
  runs against a cartridge dump you or the human supplies locally; it is
  git-ignored and never committed.
- **A worktree needs gitignored inputs wired in** (`extracted/`, the pinned
  `tools/mwccarm/`, `tools/bin/dsd.exe`) before any byte gate will run — see
  the `decomp-worktree` skill if one is installed, or
  [`notes/worktree-inputs.md`](notes/worktree-inputs.md).
- **Verify locally before claiming something works:**
  `python tools/rombuild.py -j16 --no-rom`.

## Where things live

- [`AGENTS.md`](AGENTS.md) — PR shape and the merge gate, written for an AI
  agent's human to read first.
- [`notes/README.md`](notes/README.md) — the index into ~140 reference notes
  (matching craft, C++ conversion, TU promotion, provenance), grouped by task.
  Start there instead of grepping `notes/` blind.
- [`notes/CONVENTIONS.md`](notes/CONVENTIONS.md) — the lifecycle policy for
  `notes/` itself: what kind of note to write, when to retire one, and why a
  session's scratch work does not belong here (it belongs in your own
  scratchpad or a gitignored path).
- [`CONTRIBUTING.md`](CONTRIBUTING.md) — environment setup. [`MERGE.md`](MERGE.md)
  — the review/merge playbook, if you are landing or coordinating PRs.

## The one thing that surprises people

A byte-match alone does not prove a relocation calls the right symbol —
`tools/match.py` wildcards relocated words. If your function calls anything or
touches globals, also run `python tools/prepush_linkcheck.py --range
origin/main..HEAD` before opening a PR.
