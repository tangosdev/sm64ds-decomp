# Handoff: the validator runs no tests

> **PARTLY OVERTAKEN BY EVENTS -- re-measured 2026-08-21. Read this box before quoting
> any number below.**
>
> - **Section 1's headline is no longer true.** Three workflows invoke tests now:
>   `.github/workflows/python-names.yml` runs `python -m unittest tools.test_check_python_names -v`,
>   `.github/workflows/src-tu-refs.yml` runs `python tools/test_check_src_tu.py`, and
>   `.github/workflows/dead-references.yml` runs
>   `python -m unittest tools.test_check_dead_references -v`.
>   The count is **39** test files in `tools/`, not 21. What is still true: those three
>   workflows cover three test files, the other 36 are run by nothing, and
>   `tools/hooks/pre-push` still invokes no tests at all.
> - **Section 2's ask has landed.** `.github/workflows/header-offsets.yml` exists and runs
>   `check_header_offsets.py` on PRs touching `include/`, and `tools/hooks/pre-push` gained
>   `check_duplicate_sources.py`.
> - **Section 2's tree-wide numbers are stale.** Re-run on 2026-08-21:
>   `python tools/check_header_offsets.py include/*.h` checks 475 headers and reports
>   **0 mismatched, 0 unparsed**, 8 skipped as polymorphic C++ structs, exit 0 -- not the
>   "19 pre-existing mismatches and 25 unparsed lines ... not clean tree-wide yet" recorded
>   below. (PR #1669 rewrites that tool, so re-measure again after it lands.)

**For:** whoever owns the validator / CI configuration.
**Ask:** three changes, in priority order. Nothing here is urgent-broken; all of it is
a gate that exists but is never pulled.

---

## 1. Almost all of `tools/test_*.py` is executed by nothing

*(Originally written as "never executed by anything"; corrected 2026-08-21 -- three of
the 39 now run in CI. The numbers in this section are the 2026-08-21 re-measurement.)*

There are **39** test files in `tools/`. Exactly three are invoked by a GitHub workflow,
and `tools/hooks/pre-push` still invokes none:

```sh
ls tools/test_*.py | wc -l                       # 39
grep -rlE "test_|pytest|unittest" .github/workflows/*.yml
#   .github/workflows/dead-references.yml -> python -m unittest tools.test_check_dead_references -v
#   .github/workflows/python-names.yml    -> python -m unittest tools.test_check_python_names -v
#   .github/workflows/src-tu-refs.yml     -> python tools/test_check_src_tu.py
grep -oE "python tools/[a-z_]+\.py" tools/hooks/pre-push | sort -u
#   check_duplicate_sources.py  check_references.py  eligible.py
#   port_refcheck.py  prepush_attribution.py  prepush_linkcheck.py
```

So 36 of the 39 are documentation that happens to be executable. They pass today -- I ran
all of them -- but nothing would notice when one stops.

Each file is self-running and exits non-zero on failure, so no test framework is
needed:

```sh
for t in tools/test_*.py; do python "$t" || exit 1; done
```

**Where it belongs:** CI, not the pre-push hook. They are fast and need no ROM, but
the hook is already slow (`eligible.py` compiles ~11,100 files) and a contributor who
has not run `git config core.hooksPath tools/hooks` never gets it anyway. CI is the
backstop that actually applies to everyone.

**Caveat worth checking before wiring it up:** a few tests skip themselves when the
toolchain is absent (`test_build_pin.py` says so in its docstring). `extracted/` and
`tools/mwccarm/` are git-ignored, so a CI runner has neither. Confirm the suite is
green on a bare checkout before making it blocking, or the first result will be a
false alarm and the gate will get switched off again.

## 2. `tools/check_header_offsets.py` should run on any PR touching `include/`

*(**Done.** `.github/workflows/header-offsets.yml` now does exactly this. Kept for the
rationale; the tree-wide counts at the end of this section are stale -- see the banner.)*

A generated struct header encodes its layout twice -- once as a sequence of
declarations and pads, once as a `/* 0x0a4 */` comment per field. Retyping a field
silently breaks the agreement unless the following pad is shrunk to match.

**Nothing else in the build can see that.** A header is not compiled on its own, and
the byte gate is blind to a field no source file happens to read. `rombuild.py` will
report `106/106 exact, PASS` over a header whose comments have drifted from its
layout.

```sh
python tools/check_header_offsets.py $(git diff --name-only origin/main -- include/)
```

Exits non-zero on a mismatch or an unparsed declaration. It carried the entire
weight of verification for #1129 (562 fields) and #1138 (2,447 fields).

**Known limitation, stated so it is not mistaken for coverage:** it declines to model
polymorphic C++ structs, because an implicit vptr is not in the text. It prints
`skipped -- polymorphic C++ struct` for those (7 headers today). It also reports 19
pre-existing mismatches and 25 unparsed lines across headers this programme has not
touched -- **so it is not clean tree-wide yet**, and would need either those fixed or
a scoped invocation (changed files only, as above) before it can be blocking.

## 3. Consider raising or documenting the 200-file cap

`notes/runbook-reference-repair.md` §7 says the validator caps 200 in-scope files
across `src include config mods attribution.json`, enforced server-side, with nothing
in-repo to warn you. `tools/split_prs.py` exists to split batches and its docstring
puts the working size at "roughly 150-200 files".

Two small asks:

- **Emit the real limit somewhere in-repo**, even as a constant, so a contributor can
  check before pushing rather than after a server-side rejection.
- The header work naturally produces wide-but-shallow diffs -- #1138 touches 107
  in-scope files while changing only declarations. If the cap is about review effort
  rather than validation cost, a header-only batch may deserve a different number.
  That is a policy call, not a request.

---

## What I did NOT change

I have not touched `.github/workflows/`, `tools/hooks/`, or any validator
configuration. Everything above is a proposal with the command that implements it.

The one change I did make is additive: `tools/test_gen_header.py`, 12 regression
tests, one per bug that shipped in this programme and was caught by review rather
than by a gate. Three of them were verified by reintroducing the original bug and
confirming the test goes red -- a test that has only ever passed proves nothing about
whether it can fail.
