# Exception unwinder signature repair

## Scope

`func_02071910` had three inconsistent interfaces. Its definition returned `void *` and accepted one `char *`, while both callers supplied two arguments and one caller declared an `int` return. Both callers use the returned saved link-register value as the PC passed to `func_02072dac`; the callee reads the first context pointer and returns that address.

The definition and both declarations now agree on `void *(char *, void *)`. The second parameter is left unnamed because the two callers pass different context views and the callee does not read it. `func_02072168` explicitly converts the returned address to the `int` PC expected by its existing `func_02072dac` declaration. No original name for the unused second parameter is claimed.

## Evidence

Before the change, `python tools/check_decl_agreement.py --symbol func_02071910` reported three disagreements: one return-type mismatch and two arity mismatches. Afterward it reports zero disagreements; `--changed origin/main` reports no new declaration disagreements.

On current `main` (`76e2aed46eb35ebd944d1bf14e668e73fc3bad0a`):

- Strict `linkcheck.py` verified each of the three affected functions before and after the change, with no diffs and no blind relocations.
- `prepush_linkcheck.py --files` checked the three changed sources: 3 verified, 0 warnings, 0 blockers.
- `rombuild.py -j16 --no-rom`: 11,395 functions, 468 data claims, 106/106 modules exact, zero mismatches.
- `source_style.py` checked all three files: zero findings and zero deferrals.
- `git diff --check origin/main`: clean.

## Follow-up

The separate local declarations for `func_02072dac` remain inconsistent across its callers; this patch intentionally limits itself to the independently evidenced `func_02071910` signature. No active queue reservation overlapped these three files when checked. The queue had no ready producer task on current `main`, so this PR remains a draft pending a fresh independent source-review task.
