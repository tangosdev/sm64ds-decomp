# Handoff: decl-overload-repair-0921

This document describes this commit. The queue records its immutable output SHA.

## Identity and scope

- Task: `decl-overload-repair-0921`, repair stage; producer session
  `codex-overload-producer-0921`, Codex, delegated implementation by
  `review_rnk_final`. Root owns publication and integration.
- Branch: `fix/native-overload-review-0921`; accepted input and tested source base:
  `17273b5add8ac4a7590437bdc2831a926c030498`.
- Workflow pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Historical motivating review: [PR #2877](https://github.com/tangosdev/sm64ds-decomp/pull/2877).
  This candidate targets current main as a separate tooling change.
- Status: locally verified candidate; independent exact-commit review and
  publication remain pending. `review_mip_final` is the assigned verifier.

The user explicitly authorized the exact overload proposal and a separate upstream
PR, including the scoped exception for `tools/check_decl_agreement.py` and
`tools/test_check_decl_agreement.py`. Its raw patch SHA256 is
`eb70d70eaa92a2df0b6fdaad62dd8b4e55f94fd510c9271106b606e6a68fafa3`.
The existing `pr-2471-source-review-fixes` reservation and branch remain intact.
The only additional changed file is this reserved handoff. No source, shared
headers, declaration baseline, native-destructor proposal, or other tooling
changes are included.

## Behavior

Plain native C++ free functions can share one source name while having distinct
parameter signatures. Previously the checker compared all declarations with the
first selected definition, so a valid `ApproachLinear(short&, short, short)`
declaration could be compared with the `int&` overload.

The checker now selects an existing native definition with the declaration's
normalized parameter tuple when there are multiple distinct native signatures.
Return types do not choose the overload; the selected definition still receives
the existing return, arity, parameter and linkage checks. Explicit linker names,
C linkage, members, unspecified signatures, unmatched types and conflicting
definitions retain the previous comparison path. No parser, scope or baseline
fingerprint rules change.

## Validation and limits

- `python tools/test_check_decl_agreement.py`: exit 0, 157 tests passed, no skips.
  Nine new tests include reversed definition order, typedef equivalence, wrong
  return/parameters/arity/linkage, explicit linker identity and pointer/reference
  distinctions. Running those nine against the unchanged base checker produces
  three expected failures and no errors, establishing the regression control.
- Whole current-main population, before and after: 8,649 files, 38,267
  declarations, 10,638 definitions, two unparsed records and 16,606 diagnostic
  rows. All diagnostic rows and keys are unchanged. This is a preservation check,
  not a claim that the source tree has no declaration disagreements.
- The historical immutable source snapshot
  `7fba8b3ad772616c57f8e35c3c11e4d3e10e6d7d` demonstrated the motivating defect:
  exactly the three Hanachan short-overload parameter findings disappeared,
  with no introduced or otherwise changed diagnostic rows. That historical
  source repair is not part of this tooling candidate.
- The two edited tool files exactly reproduce the frozen approved proposal,
  allowing only Git's CRLF checkout conversion. The raw approved patch hash was
  rechecked with `Get-FileHash -Algorithm SHA256`.
- `git diff --check`: exit 0. `python tools/port_refcheck.py`: exit 0,
  418 references resolve. Relative handoff/tool documentation targets exist.
- No ROM build, compiler matching or private validator is claimed: this patch
  changes only Python tooling, tests and this handoff. Native-destructor parsing
  and native-to-mangled identity reconstruction remain outside its scope.

Ignored local evidence is retained under `build/reviewer-0919/decl-overload-0921/`
in the primary checkout: full before/after findings, population proof and the
old-tool regression control. The complete suite log is the sibling
`decl-overload-0921-tests.log`. Frozen proposal and earlier independent review
remain under `decl-overload-proposal/` and
`decl-overload-independent-review.json`. No private receipt is committed.

Next: the independent verifier reviews this exact candidate and base; root then
publishes the separate upstream tooling PR using the authorized task. Follow
[the protocol](../PIPELINE.md) and [review policy](../SOURCE-REVIEW-CUTOVER.md).
