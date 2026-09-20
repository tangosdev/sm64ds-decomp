# Handoff: pr2868-source-repair-0920

This commit repairs the nine call-contract families and factual header comment
identified by an independent whole-source review of PR #2868. It remains a partial,
unenrolled shadow; local byte proof is not formal source acceptance or promotion.

- Producer: `codex-bookshot-producer-0920` (Codex child `/root/review_mip_final`).
- Input: `47ee06b41539d307052a7af1b1c1e602bb386f6a`.
- Base: `9022c77b3834c5ee77c31b4fb4af587418137d6c`.
- Workflow pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Branch: `review/pr2868-repair0920`; own wired worktree
  `C:/tmp/sm64ds-book2868-review0920`.
- Integrator: `codex-integrator-0920`; source review by a different agent remains
  required. The producer's earlier read-only compiler work is historical evidence.
- Owned writes: existing BookShot shadow, one factory-order comment in
  `include/BookShot.h`, this handoff and the
  [durable experiment](../../experiments/pr2868-source-repair-0920.json).
  Live v2 ownership was checked; all paths and ov020:0x021111a0..0x02112938 were
  reserved before editing. Other worktrees, ledgers and the follow-up draft remain
  untouched. Producer lease stays running; no output or source branch published.

## Repairs and retained findings

BOOK2868-01 remains the useful original member cleanup, with its original author
and commit ancestry preserved. BOOK2868-04 is fixed locally across all nine
reviewed families. Real `ClosestPlayer`, `UpdatePos`, `ModelBase::SetFile`,
`dActor_c::Spawn` and both `ApproachLinear` overloads replace false or conflicting
local interfaces. The state helper now returns void; its sole recorded caller
discards the result. A `short` angle local and actual `GetSubtraction` member call
avoid the redundant argument sign extension while preserving the ROM comparison.
The other corrected declarations use the actual pinned scalar returns, pointer
views and widths for sound, Player Hurt/Bounce, SetAnim and DropShadow.

The genuine fixed-point SetAnim and DropShadow member alternatives were compiled
as valid full-TU probes. They grow the affected functions; exact attempted source
patches, commands/flags and results are retained in the experiment. These specific
failures justify retaining the corrected existing scalar bridges for this bounded
repair; they do not prove every cleaner form impossible. Shared member signatures
were not changed to match scalar definitions. The former zero-argument
ClosestPlayer and incompatible helper-return comments are disproved by matching
ordinary interfaces and removed.

BOOK2868-05 is fixed by changing only `before` to `after` in the factory-order
comment. The factory stores its vptr after the base constructor and before its
five members. Header non-comment text is identical; layout evidence and the
explicit retail daBook_c identity remain intact.

BOOK2868-02 and BOOK2868-03 remain open: raw matrix/vector views, address macros,
remaining helper reconstruction, community-named metadata and complete-output
ownership have not received blanket acceptance. Their existing follow-up draft is
unchanged and not yet posted. Correctness/provenance findings are repaired locally,
not moved into a deferred reconstruction bucket. No formal pass is asserted.

## Proof and limits

Pinned `2004/b56` full-TU compile of the applied source: **31/31 strict VERIFIED**,
6,040 function bytes, empty differences and zero blind checks. Its complete
22,376-byte object is byte-for-byte identical to both the original input and actual
base; SHA256 `05b5f1dc672fdd24cda95ebcd02e9a2327fe297bc948a889077b75a320620688`.

`tubuild.py --manifest build/book2868-producer/manifest.json verify
ov020/BookShot+BookShotSpawner+Bookend` exits **1**. The inherited 17 unlicensed
outputs, three missing compiler policies (BookShot D2, BookShotSpawner D2, Vector3
D1), and destructor ordinal pairs [0,1] / [2,3] remain unchanged. Per-function
isolation is diagnostic proof, not authorization to drop metadata in production.

`affected_src.py include/BookShot.h` identifies eight enrolled consumers; all eight
pass `prepush_linkcheck.py --files` with empty differences and zero blind checks.
The explicit shadow-versus-definition audit has no diagnostics for the reviewed
nine contract families. The normal declaration ratchet excludes src_tu and is not
used as their proof. Full-ROM validation is not claimed: production still builds
the unchanged legacy sources, so it would not exercise this shadow edit.

The durable replay and final static/reference checks are run before the local
commit. ROM/compiler inputs, object binaries and private receipts stay ignored.
Next: independent exact-commit review, followed by issue approval/publication and
the integrator's current-base process. This producer does not publish or merge.
