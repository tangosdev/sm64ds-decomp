# BookShot declaration repair for PR #2869

This commit preserves Andrew Boudreau's authored `348405c2e4640d1c48c45f1f0f0af6c27345a065`
and accepted BookShot composition `d160ad64587d3dceac98d5099e113d42de56b59c` as ancestors.
Producer `codex-bookshot-decls-0920` owns task `pr2869-source-repair-0920` in
`C:/tmp/sm64ds-book2869-repair0920`, branch `fix/pr2869-bookshot-decls0920`.
The two predecessor tasks are `pr2868-source-repair-0920` and
`pr2868-composition-0920`; both independent reviews completed before this claim.
The final composition base is actual BookShot-merged main
`a3776dc057779c652309ab5de764061c63a426ad`; the original local measurements used
`c04f97677c3c30b269354a36435c027bdd0afd5f`. Current-main acceptance must be refreshed
if a later source-bearing main lands.

The original PR's typed collision members are retained. `mShadowModel.InitCylinder()`
now uses the genuine method. Both accepted `SetFile` member calls and every earlier
BookShot contract repair remain intact. BOOK2869-01 fixes the author's reference
declaration of a C bridge whose actual definition takes `const Vector3*`; the final
bridge keeps concrete owner/actor/vector types and the actual pointer contract.

Two ordinary genuine `dCcAcPos_c::Init` forms were measured: aggregate fixed-point
locals and direct `.val` assignments. Both grow `BookShot::InitResources` from
0x284 to 0x294; the assignment form has zero blind relocations. Direct
`dBgCh_Actr::Init` compiles but the existing scalar-parameter header produces an
unmapped mangled target, so it fails strict linkage. That is an existing interface
boundary, not a compiler impossibility. The typed scalar bridges remain. Exact
patches, hashes, commands, driver and measured outputs are in
[the experiment](../../experiments/pr2869-source-repair-0920.json).

Fresh original-author, accepted-input and final complete objects are identical:
`05b5f1dc672fdd24cda95ebcd02e9a2327fe297bc948a889077b75a320620688`, 22376 bytes.
All 31 manifest functions (6040 bytes) strictly verify, with no differences or
blind relocations. Complete-TU verification still exits 1: 17 unlicensed outputs,
three missing compiler-only policies and destructor ordinal pairs [0,1]/[2,3].
Those are inherited failures, not accepted metadata or production enrollment.

BOOK2868-01 through BOOK2868-05 remain inherited without omission: named members
and the nine contract families stay repaired, reconstruction/object ownership
remain partial, and the accepted header comment is unchanged. Remaining work is
[issue #2872](https://github.com/tangosdev/sm64ds-decomp/issues/2872), next owner
`codex-integrator-0920`. The issue does not waive correctness or provenance.

Only the existing shadow TU and these two owned evidence documents are new work
by this producer. The later accepted manifest-note correction described below is
inherited through its original commit; headers, layouts, ledgers, enrollment and
every non-note manifest field remain unchanged.
No full-ROM result is claimed for an unenrolled shadow. Static checks and their
exact final commit are recorded in queue evidence after freezing this commit.
The declaration scanner excludes src_tu; a green ratchet cannot certify these
interfaces. The targeted definitions and measured calls provide that evidence.

Next: an independent verifier reviews the exact output, contracts, inherited
findings, complete-object limits and tested base. Root remains the sole original
PR publisher/merger. This producer handoff has `formal_acceptance: false` and does
not publish a source branch. No private inputs, object files or receipts are tracked.


## Accepted conflict-note continuation

The original source candidate `102d3df731a9cfc7e217a7042f7e761c6bb8d15a` is
preserved on `fix/pr2869-bookshot-decls0920`. This continuation is on
`fix/pr2869-bookshot-notes0920` and merges independently accepted metadata commit
`2de630659a82f027501fe66792068c97ff18f0b6`, task `pr2868-conflict-notes-0920`.
Root authored that metadata repair; `codex-bookmeta-review-0920` independently
accepted it before this merge. No receipt or source reservation was transferred.

BOOK2868-06 remains a provenance finding and is fixed by the six exact RESOLVED
prefixes. The original warning text remains; 13 unresolved notes still correspond
to 13 source markers. These six stale records made CI fail even though all byte
checks passed. `check_tubuild_conflicts.py --list` is now explicitly required in
the final producer gates. All source/header/compiler inputs and non-note manifest
fields are unchanged from the frozen source candidate; the full accepted finding
is retained in the experiment and will be carried into independent review.

The running task's original two predecessor IDs remain immutable. The later
accepted dependency above is explicitly recorded here and in producer evidence;
current v2 amendment supports resource additions, not predecessor edits. The final composition below incorporates the supplied main revision. No passing
independent source acceptance is claimed by this producer.


## Final composition for independent review

Branch `fix/pr2869-final-main0920` preserves both earlier branches: source repair
`102d3df731a9cfc7e217a7042f7e761c6bb8d15a` and metadata continuation
`a4b060303b55e0f34334a765c6a31d8764e0afc6`. Actual merged main is
`a3776dc057779c652309ab5de764061c63a426ad`, after PRs #2859, #2863 and #2868.
Only the owned declaration hunk conflicted. Main's incoming source blob exactly
equals accepted `2de63065`/`d160ad64`, so the preflighted follow-up source was
retained byte-for-byte. Every unrelated main path is preserved. Relative to main,
the delta is this existing shadow TU plus the two owned evidence documents.

Required final gates are fresh pinned31 strict functions, complete-object identity,
explicit complete-output limitations, conflicts, dead references, scoped declaration
agreement, port references and whitespace. Exact final commit/results are bound in
producer queue evidence; an independent reviewer must publish the source verdict.
BOOK2868-01 through BOOK2868-06 and BOOK2869-01 all remain in scope with issue2872.
No source branch, PR body or merge is published by this worker.
