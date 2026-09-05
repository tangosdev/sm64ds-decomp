# Launch commands

Paste one of these into a fresh Claude Code or Codex instance. Each is
self-contained: the instance reads its role file from the repo, claims work, and
keeps going. Nothing here depends on a Claude skill or memory, so a Codex
instance and a Claude instance follow the identical protocol.

Repo: `C:\Users\andre\source\tangosdev\sm64ds-decomp`

## The generic launcher

> Read `notes/agents/PIPELINE.md` and `notes/agents/roles/<ROLE>.md` in
> `C:\Users\andre\source\tangosdev\sm64ds-decomp`. Execute that role for the
> next unclaimed class. Claim it first with `python tools/classqueue.py claim
> <class> --role <ROLE> --worktree <your worktree>`; if the claim is denied,
> take the next row instead. Work in your own worktree created with
> `~\.claude\skills\decomp-worktree\wt-setup.ps1` (Codex: read that script and
> follow it — it junctions `extracted/`, `tools/mwccarm/`, `tools/bin/`, which
> the byte gates need and which `git worktree add` does not copy). When your
> stage is committed and pushed, release the claim and take the next class. Do
> not stop after one.

Substitute `<ROLE>`: `scout`, `writer`, `humanizer`, `builder`, `reviewer`.

## How many of each

The validator is the bottleneck — one box, one job, about 20 minutes per PR — so
builders queue no matter how many you run. A sensible standing crew:

| role | count | why |
|---|---|---|
| scout | 2 | read-only, cheap, keeps the facts queue ahead of the writers |
| writer | 3 | the slow creative step; independent classes never collide |
| humanizer | 1 | fast, and one consistent voice keeps the style uniform |
| builder | 2 | more than 2 just queue behind the validator |
| reviewer | 1 | must be a different instance from the builder that opened the PR |

Give **Codex the scout and humanizer roles** — both are read-and-judge work with
no compiler dependency. Give **Claude the writer and builder roles**, which need
the mwccarm behaviours and the gate traps.

## Safety rails to state in every launch

- Never `git worktree remove` — it deletes through the junctions and empties the
  real `extracted/` ROM dump, which cannot be re-downloaded. Use
  `~\.claude\skills\decomp-worktree\wt-remove.ps1`.
- Never hand-edit `symbols/actor_renames.tsv` or
  `notes/data/c-cpp-classification.tsv`. Row order is load-bearing.
- Never force-push over a claim ref.
- A near-miss never lands in `src/`.

## Watching it

    python tools/classqueue.py list          # who holds what
    gh pr list --state open --limit 40       # what is waiting on the validator
