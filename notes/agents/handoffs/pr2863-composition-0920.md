# Handoff: pr2863-composition-0920

This commit composes the accepted Spiny shadow repair with current main.
Producer evidence is partial and makes no formal source-acceptance claim for this
new base. The original repair branch is preserved unchanged.

- Composer: `codex-shadow-composer-0920` (Codex child `/root/review_mip_final`).
- Predecessor task: `pr2863-source-repair-0920`; independently accepted source
  `51683149abf9a2da651321adcb016f00b1cf045c` at base
  `9022c77b3834c5ee77c31b4fb4af587418137d6c`.
- Current main and policy stable target: `c04f97677c3c30b269354a36435c027bdd0afd5f`.
  There are no skipped bot commits in this recorded base window.
- Clean merge commit: `faa5aeabdd1e6a9a6754eb7601a39ef84fa7a712`; accepted source and
  current main are both ancestors. The final candidate adds only this handoff and
  the [composition report](../../experiments/pr2863-composition-0920.json).
- Workflow pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- New branch `review/pr2863-composition0920` in own wired worktree
  `C:/tmp/sm64ds-spiny2863-review0920`; `review/pr2863-repair0920` remains at the accepted source.
- Task inherits the predecessor authors and all finding IDs. It reserves only
  the composition report and handoff; source/header/config paths are read-only
  requirements. No shared ledger/source conflict occurred and no manual conflict
  resolution was performed.

The clean main merge preserves every accepted source/header/manifest/experiment
blob and every unrelated main blob, including the already accepted Mip ledgers.
No additional source repair, source/header declaration edit, enrollment or metadata
policy change is included in this composition.

Fresh pinned `2004/b56` full-shadow compile: **34/34 strict VERIFIED**,
5,864 function bytes, empty differences and zero blind checks. The complete
21,528-byte object is identical to the accepted repair;
SHA256 `c052dd9b101ed0bc57abb9226bbe5dcc2389964d06bb60519c6adfbe3b61f659`.

The full-TU command still exits **1** on inherited complete-output limitations:
9 unlicensed outputs, 0 compiler-only policy errors, and destructor
ordinal exceptions [[0, 1]]. Strict function proof does not license discarded
metadata or establish promotion readiness. A full-ROM build is not claimed: this
source is still unenrolled and production would use its unchanged legacy shards.

The scoped declaration ratchet passes at the current base; it excludes src_tu and
does not independently validate every shadow contract. Reference/whitespace checks
are completed before the final local commit, with their actual logs retained in
ignored build. The report records exact commands, blob identities, per-function
results and inherited independent finding dispositions without claiming the
predecessor reviewer's checks as this composer's work.

Remaining reconstruction stays explicitly owned by
[issue #2870](https://github.com/tangosdev/sm64ds-decomp/issues/2870), next owner
`codex-integrator-0920`. No finding is erased, reclassified or silently closed.
The compose stage may be published with this producer's own receipt; a different
session must claim VERIFY and publish an actual current-base source judgment.
Root remains the sole original-PR publisher/merger. No PR branch push or merge is
authorized by this handoff.
