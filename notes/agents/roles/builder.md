# Role: builder — prove the bytes, then open the PR

You are the only stage that decides whether the reconstruction is correct. The
writer's and humanizer's green output is evidence, not proof: re-run everything
yourself, in your own worktree, at a named ref.

## Gate order

Run these in order and stop at the first failure. Each is a real invocation.

    python tools/tubuild.py verify    <ov>/<Class>
    python tools/tubuild.py linkcheck <ov>/<Class>
    python tools/check_src_tu_compiles.py --quiet
    python tools/eligible.py -j 16
    python tools/check_references.py --against origin/main
    python tools/rombuild.py -j 16 --no-rom
    python tools/check_rename_ledger.py --repo .
    python tools/check_profile_campaign.py --repo .
    python tools/langmode_audit.py --check
    python tools/check_header_offsets.py --changed origin/main
    python tools/port_refcheck.py
    python tools/check_duplicate_sources.py
    python tools/check_dead_references.py
    python tools/check_tubuild_conflicts.py --list
    python tools/tiers_ratchet.py --check

PASS signals:

- `tubuild verify` gives `N/N MATCH, objisolate clean, reloc-destinations clean`
- `rombuild` gives `module fidelity: 106/106 exact, 100.000000% of compared
  bytes` and `ROM-build analysis: PASS`
- everything else exits 0 with no increase in any backlog count

**Byte match alone is never enough.** Every relocated word is a wildcard in
`match.compare`, so require all three: byte compare, `objisolate` (relocation
type and addend), and `reloc_audit` (destination identity).

`eligible.py` must run fresh on the exact commit before `check_references.py` —
a stale report makes that gate print "skipping", which reads like a pass and is
not one.

## Traps that make a green run a lie

- `check_src_tu_compiles.py` prints `NOT CHECKED` and exits 0 when
  `tools/mwccarm/2004/b56/mwccarm.exe` is missing. In a fresh worktree that is
  the default. Wire the junctions (`decomp-worktree` skill) or the gate is blind.
- `linkcheck --baseline` builds the **working tree**, so a baseline taken after
  your header edits proves nothing. Take it before.
- Never run two consumers of `build/` at once — a backgrounded `rombuild` beside
  `eligible.py` invents link errors.
- `check_references.py` soft-skips when no eligibility report is present.

## The PR

Title: `<ovNN>: promote <Class> into a single C++ TU`.

The body must carry the proof block verbatim:

    tubuild verify  10/10 MATCH, objisolate clean, reloc-destinations clean -> TEXT-VERIFIED
    rombuild -j16   106/106 exact, 100.000000% of compared bytes, mismatching 0
    romdata_check   6 VERIFIED, 5 PARTIAL, 0 DIFFERS

and a class-identity write-up: cite the `_ZTS` / `_ZTI` (with base) / `_ZTV` ROM
addresses and the factory's `new`-size literal; diff the vtable slot-by-slot
against the base class's vtable to enumerate the overrides; state where you put
the destructor and why.

Add a plain-English TL;DR a CS generalist can read, above the evidence.

Real validation is off-box: `pr-validate.yml` posts the PR SHA to a relay whose
self-hosted worker does the stock-ROM build. It is one box, one job at a time,
about 20 minutes per PR, and it validates **the final SHA** — so do not push
again after it goes green.

## Done when

The PR is open, the `PR validation` check is green, and you have released your
claim. Do not merge your own PR.
