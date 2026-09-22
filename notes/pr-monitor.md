# PR monitor and repair guide

Run the read-only queue monitor from a checkout with GitHub CLI authentication:

```powershell
python tools/pr_monitor.py
```

It skips #1450 by default, puts conflicts and failed checks ahead of clean work,
and calls a PR *reviewable* only when it is not a draft, GitHub reports it clean,
every reported check has completed successfully, and it has been open at least
15 minutes. It never approves, rebases, pushes, or changes CI state.

## Repair loop

1. Work in a dedicated worktree. For decomp work, use
   `~/.codex/skills/decomp-worktree/wt-setup.ps1`; it wires the ROM/compiler
   inputs safely and prevents a repair from disturbing the primary checkout.
2. Take the first `conflict` or `failed` item. Read the failed job log and compare
   the PR diff with its merge base. Treat the check as evidence, not an obstacle.
3. Make the smallest semantic repair that makes the check true. Resolve a conflict
   by preserving both intended changes; fix an invalid path, broken source reference,
   byte/relocation mismatch, header ripple, or attribution row at its source.
4. If the failure is already fixed on current `main`, rebase the PR onto `main`
   and rerun its relevant checks. Do not copy unrelated tooling changes into a
   match PR. If the failure is shared but not on `main`, repair it in a separate
   tooling PR and leave matching PRs otherwise focused.
5. Run the same local gate named by CI, plus the relevant byte and relocation gates
   for changed `src/` files. Never suppress a check, add a broad ignore, or bank a
   newly broken result merely to turn CI green. If the correct repair is uncertain,
   leave the PR untouched and record the evidence in a comment.
6. Push only after the repair is clear. Return to the monitor and take the next
   item; do not assume a prior check result still describes the current head.

## Review and approval

For a `reviewable` PR, inspect the current diff and the complete check list. Approve
only if it remains conflict-free, all required checks are green, it has been open for
at least 15 minutes, and the change itself has no correctness or scope issue. A green
check is necessary evidence, not a substitute for review. Use `gh pr review <N>
--approve` only after those conditions are independently confirmed.
