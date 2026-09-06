# Tool-gate findings, 2026-09-05

This pass was deliberately limited to `tools/` plus this findings note. It did
not edit any live agent documentation, promotion queues, source files, TU or
delink manifests, attribution data, or converted-tier baselines. It did not run
`tiers_ratchet --update`.

## Frozen reconstruction censuses

The harvesters were current and the two tests were stale. Reproducing the
frozen census now finds 805 destructor symbols, 2,715 destructor-call sites,
1,208 owner/member pairs, and 374 owners. The 1,208 pairs comprise 832 members,
369 base-at-zero pairs, and 7 secondary-base pairs, with no owner/offset type
conflicts.

The sized-operator-new census now has no genuinely headerless live class.
`RecRoomCupboard` gained a header in #1830, and the former `BigBooIcon` result
resolves through its vtable alias to the `daTrsIcon_c` header added in #1895.
The expectations and diagnostics were refreshed to reflect that landed state.

## Header offsets and Allman inline bodies

`check_header_offsets.py` counted braces only on an inline virtual method's
declaration line. With an Allman opening brace, it treated the following `{` as
an unknown declaration and the method's closing `}` as the end of the outer
class. That could report a green-looking zero-field check.

The parser now waits for the body opener, tracks the complete inline body, and
ignores braces inside ordinary comments and quoted literals. It still handles
one-line bodies and declarations ending in `;`, and it fails closed when a
promised body never appears. Regression tests cover good and bad offsets after
Allman bodies.

## Dead references in C and C++ comments

`check_dead_references.py` now scans comments in `.c`, `.cc`, `.cpp`, `.h`, and
`.hpp` files while ignoring include directives, string and character literals,
raw strings, and executable code. A minimum code-file census prevents a silent
zero-file pass. Existing code-comment debt is stored separately as exact
`(citing file, referenced path)` pairs in
`tools/dead-code-reference-baseline.json`; deletions heal naturally, while any
new pair fails the gate. The existing `--update` mode cannot bank this debt;
refreshing it requires the explicit `--update-code` option.

On the first sealed base the scan covered 10,809 C/C++ files and found 1,106
existing dead comment-reference pairs in 240 citing files, including 1,025 dead
`src/` references in 170 files. No cleanup was attempted. After rebasing onto a
main that included the `dScMgSound_c` promotion, the new gate rejected exactly
82 newly stale shard references in that promoted source. Because those refs
were already on main and source cleanup is intentionally out of scope, the
initial code baseline was aligned to current main at 1,188 pairs. Future
additions remain failures.

## Verification

- Frozen census tests: 39 passed; both census scripts pass their `--check`
  modes.
- Header-offset tests: 39 passed; related header-generation tests: 21 passed.
- Representative live headers checked all documented fields, including
  `dScMgBase_c.h` (40 fields) and `dActor_c.h` (33 fields).
- Dead-reference suites: 60 passed. The repository scan covers 10,728 C/C++
  files on the integration base and reports no new dead references or broken
  Markdown links.
