# Draft classification repair

- Task: `draft-verdict-repair-0918`
- Producer session: `codex-draft-verdict-producer-0918`
- Base: `1732d1588ae9cb20b853c40c5899fe9a3eaf3efe`
- Scope: `tools/reloc_audit.py`, `tools/linkcheck.py`, `tools/pr_linkcheck.py`,
  `tools/test_linkcheck.py`, and this handoff. No source/header, matched-count,
  manifest, enrollment, compiler-failure exemption or workflow changes.

`winning_object` previously returned `len-mismatch` for three different outcomes:
no compiler output, no requested symbol, and a real requested function whose size
differs from the ROM range. This made an honest non-counted draft a hard `NO-SYM`
failure even after its compile error was corrected. The repair separates these
outcomes without changing the `(obj, sym, err, offset)` return contract.

A size near miss requires a defined, positive-sized `STT_FUNC` in an executable
`SHT_PROGBITS` section, with its complete extent inside the section. Missing,
undefined, absolute, data, zero-extent, malformed and truncated-symbol cases do
not establish that evidence. Only an actual requested-function near miss becomes
`NO-REPRO`; compile/missing-output errors stay `NO-SYM`. Unknown error reasons
fail closed. The existing exact, any-symbol, zero-size-alias, address-resolved
nested-entry and full-span carrier successes retain their paths. A wrongly
supplied containing-symbol window is still a hard failure.

The PR policy still follows `asm_policy.counts_as_matched`, and only an eligible
`NO-REPRO` becomes `DRAFT`. Hard sibling failures now outrank a near miss when
aggregating a multi-symbol file. Unbannered transcription is rejected before the
non-counted draft downgrade. Missing source/ROM and unexpected failure statuses
cannot be hidden by a sibling draft. JSON retains failure reasons and actual
sizes. Markdown reports drafts, blind/unresolved coverage and passenger results
truthfully instead of asserting every non-failing file is byte-identical.

## Findings retained

- `JUMP-CONTRACT-01`: separate source task owns the scalar contract correction;
  this tooling change does not apply or accept it.
- `JUMP-DRAFT-02`: Jump remains unmatched. A compiled draft is not byte or
  relocation proof and gains no matched credit or production enrollment.
- `JUMP-GATE-03`: distinguish positive emitted-function size mismatch from
  compilation/missing-symbol failure.
- `JUMP-GATE-04`: prevent mixed `NO-SYM`/`NO-REPRO` from becoming `DRAFT`.
- `JUMP-GATE-05`: remove blanket byte-identical claims for draft/unverified rows.
- `JUMP-GATE-06`: keep unbannered `dcd` transcription hard even on `NO-REPRO`.

## Validation

The focused suite passed 125 tests:

```text
python -m unittest tools.test_linkcheck tools.test_pr_linkcheck   tools.test_pr_linkcheck_verdict tools.test_reloc_audit_flags   tools.test_prepush_linkcheck tools.test_asm_policy tools.test_bytegate
```

After adding the executable-section evidence guard, the directly affected first
four modules passed 52 tests. These include the real compiler alias/nested
fixtures, wrong callee/addend checks, and 16 new `DraftClassificationIntegration`
tests with synthetic ELF objects and CLI positive/negative controls. The existing
`tools.test_linkcheck` module is already enabled in the public workflow; no
workflow edit or additional dependency is needed.

A clean tracked export without `extracted/` or `tools/mwccarm/`, overlaid with the
four final tool/test files, ran `python -m unittest tools.test_linkcheck -v`:
30 tests collected, 28 passed, two named real-compiler fixtures skipped. New
integration tests execute without private inputs and reject accidental ROM access.

The private real controls used the unchanged base above and separate source
producer commit `9f6508003f92787fb5a530c38e62aa0d92eb829f`:

- Unchanged base Jump fails compilation, returns `NO-SYM / compile-failed`, and
  actual `pr_linkcheck --files src/_ZN11dScMgJump_c13OnYoshiTryEatEi.cpp --fail -j1`
  exits **1**, despite its `NONMATCHING` banner.
- The source producer's scalar header correction emits the requested function at
  **356 bytes against 360** with pinned `2004/b56` and actual build flags. The
  existing complete version sweep emits sizes **348, 352, 356**; none match.
  Both direct-object and source-search classification return `NO-REPRO` with an
  explicit size reason. The actual PR checker, run with only the immutable source
  producer's include path overlaid on compilation, reports `DRAFT` and exits **0**.
- Jump's `counts_as_matched` stays false and no complete production entry covers
  `ov006:0x020ee994`. No matching or relocation-success claim is made for it.

Local evidence is under the primary checkout's `build/reviewer-0918/`:
`draft-verdict-private-validation.json`, `draft-verdict-final-tests.log`,
`draft-verdict-no-rom-tests.log`, positive `draft-verdict-real-jump-pr.json/.md`,
and negative `draft-verdict-negative-pr.json/.md`. The design-only predecessor is
`jump-draft-classification-design.json`; it does not assert implementation success.

No full ROM rebuild is claimed for this tooling-only change. Full common-header
consumer validation, the helper/Jump2 exact checks and full ROM proof belong to
the separate source task after the tools are independently reviewed and deployed.
This handoff is producer evidence, not independent acceptance or authorization to
publish the source change.
