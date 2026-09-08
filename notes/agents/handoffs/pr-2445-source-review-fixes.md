# Handoff: pr-2445-source-review-fixes

This document describes this continuation commit. The queue records its immutable
output SHA. Independent verification and integration are separate stages.

## Identity and resumption

- PR: https://github.com/tangosdev/sm64ds-decomp/pull/2445. Task
  `pr-2445-source-review-fixes`, producer stage `fix`, session
  `codex-humanizer-snmbdy-20260908-01` (Codex); supporting coordinator
  `codex-humanizer-integrator-20260908`.
- Branch `cpp/humanizer-snmbdy-0908`, wired worktree
  `C:/tmp/sm64ds-humanizer-snmbdy-0908`; adopted PR input
  `19921838ecf6acaf689a4dbc3bb4fd05e4d0effc`, actual merge base
  `690637e41a7a302eace631f6065ab1c4fe37326e`. This resumes the existing promotion.
  The original worktrees, branches and handoff are preserved.
- Predecessor task `snmbdy-ov072-0907b`; known source contributors
  `prod-snmbdy-0907` and `int-snmbdy-0907b-b303` are recorded with inherited queue
  authors/findings. Workflow remains
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`; enqueue/claim use upgraded queue tools
  `6ef1dfb896e6074e83d2aa1a03cc3b701d07dcae`. Source verification tools are unchanged
  from the adopted input. No protocol activation is performed by this task.
- Required artifacts are this handoff, the original
  [producer handoff](prod-snmbdy-0907.md), `src/actors/daBgSnmBdy_c.cpp`,
  `include/daBgSnmBdy_c.h`, and `config/tu_manifest.d/ov072/daBgSnmBdy_c.json`.
- Status: byte-verified candidate for the owned class-local correction; the two
  ledger explanations remain pending legitimate ownership. Different-session
  independent verification is next;
  this producer cannot accept its own candidate. No normal source push, PR edit,
  review submission or merge was performed here.
- Local proof lives in this worktree's ignored `build/pr2445-proof-20260908/`
  and `build/pr2445-proof-20260908-control/`. The primary checkout's ignored
  `build/fleet-restart-20260907/review-watch/` preserves the source review,
  task spec, input census, revised PR-body draft, exact pending ledger patch and
  its before/after row evidence. Private claim receipts stay ignored locally.

The public source review of adopted input `19921838` is recorded at
https://github.com/tangosdev/sm64ds-decomp/pull/2445#pullrequestreview-5145352638.
This continuation addresses its class-local provenance/evidence wording while
retaining the pending ledger finding explicitly.

## What changed and why

`SetState(int)` is retained byte-for-byte as executable C++, but its declaration,
definition comment and manifest now disclose the unproved `Ei` spelling. The ROM
establishes table indexing and call-site behavior; those observations do not
recover the private English identifier or distinguish original `int` from an enum.
No replacement signature or claimed enum declaration is invented.

The old handoff is explicitly historical. Its gate results and pinned bases remain
intact. Its census now distinguishes 26 out-of-line member definitions from the
separate factory, and discloses the offset checker's partial coverage. The manifest
records the fresh census at the exact adopted input, keeping the earlier reported
line count as historical context. Production mode, function symbols, addresses,
section ownership, all executable tokens, signatures, field layout and attribution
are unchanged by this continuation.

Scope is `daBgSnmBdy_c`, ov072 `.text [0x0211f000,0x0211ff34)` (29 functions,
3,892 bytes) plus `.data [0x02122780,0x0212283c)` (188 bytes). Only the five
reserved class-local source/header/manifest/handoff files change.

The third provenance surface is still pending. `symbols/actor_renames.tsv` is
reserved by the preserved, paused `issue-2411-humanizer-2` task and was not edited.
The local patch appends the same inference caveat only to these two `why` cells,
retaining all other columns, rows and ordering:

| Module/address | From | To |
| --- | --- | --- |
| ov072 / 0x0211fcb0 | `func_ov072_0211fcb0` | `_ZN12daBgSnmBdy_c8SetStateEi` |
| ov072 / 0x0211fcb0 | `_ZN11SnowmanBody8SetStateEi` | `_ZN12daBgSnmBdy_c8SetStateEi` |

The prepared suffix for each existing explanation is: `; SetState and the Ei int
parameter spelling are inferred; table indexing and call sites do not establish
the original type or rule out an enum` (one line in each TSV cell).

The coordinator must arrange legitimate ownership for those two explanations
before treating the three-surface provenance finding as fully resolved. No global
generator refresh, symbol rename, baseline update or unrelated ledger edit is part
of this repair.

## Reconstruction dimensions

- ROM identity remains directly evidenced by the `12daBgSnmBdy_c` RTTI payload at
  ov072 `0x0212278c`, its typeinfo at `0x02122780`, the base pointer to `dActor_c`
  at arm9 `0x0208e390`, and the matching vtable header. The original English
  helper names, factory/profile identifiers and filename remain inferred.
- The 29 functions comprise 26 out-of-line native members, a free registry factory
  and the inline destructor's D1/D0 pair. The 28 member symbols already had native
  member definitions in the adopted legacy shards; this repair adds none. Existing
  genuine progress is compiler-owned construction, the factory's ordinary `new`,
  typed subobjects and intact-object metadata ownership.
- The adopted input census is 18 of 26 member bodies with `tiers.py`'s RAW_OFFSET
  pattern, 154 matches on 117 lines. The input had 642 source lines and 126 header
  lines. This is a defined regex measurement, not complete raw-access accounting.
  The local Player view, matrix word-copy adapters, external mangled ABI calls,
  one unknown field and padding remain. No compiler wall is newly asserted here.
- Size remains `0x3a8`. Model, ShadowModel, dCcAc_c, dBgCh_Actr and PathPtr are typed
  at `0xd4`, `0x124`, `0x14c`, `0x180` and `0x380`. The header-offset helper reads
  15 fields through `0x38c`, then stops at the StateFunc typedef; it does not prove
  the PMF/scalar tail. Exact byte proof and the existing size assertion have their
  own scopes and do not turn that partial checker result into full field coverage.
- The TU owns the full class vtable storage including its eight-byte ABI preamble,
  RTTI/name and profile through the 188-byte intact data claim. The compiler emits
  the destructor pair and vptr; the class has no emitted static initializer.
  Compiler-only and externalized outputs retain their existing manifest policies.
- All 29 per-symbol attribution overrides and the retired-stem credit mapping are
  retained without modification. Remaining broader reconstruction opportunities
  stay separate from this bounded provenance/evidence correction.

## Proof and limits

All checks below used the frozen class source/header/manifest in this commit.
The runner's `source-identity.json` confirms unchanged noncomment executable tokens
against `19921838` and byte-identical manifest content outside `notes`; the gates
also confirm that their execution did not change those inputs. No fields,
signatures, production policies, ownership records or compiler settings changed.

- `python tools/linkcheck.py --module ov072 --name <symbol> --addr <address>
  --size <size> --c src/actors/daBgSnmBdy_c.cpp`, all 29 manifest functions:
  **29 VERIFIED, zero blind bytes and empty diffs**. Process exits are zero.
  The source is passed explicitly for each symbol, including D1/D0 and the factory;
  this is not a filename-only `NO-SYM` result. Logs and structured results are in
  `build/pr2445-proof-20260908/symbols.json` and `symbol-*.log`.
- `python tools/affected_src.py include/daBgSnmBdy_c.h --json` finds exactly one
  consumer, `src/actors/daBgSnmBdy_c.cpp`; all its 29 functions are covered above.
  `consumers.json` retains the exact inventory.
- `python tools/rombuild.py -j8` exits 0, builds the production ROM, and reports
  11,192/11,192 source functions reproducing, 26/26 owned data claims reproducing,
  three BSS claims (172 NOBITS bytes), and **106/106 modules exact**. Final SHA256:
  `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
  Logs: `build/pr2445-proof-20260908/rombuild.log` and
  `build/rombuild-report.json`. This command generated a genuine fresh strict
  stock control in `build/tu/_baseline/link/`, excluding all 23 enrolled intact
  candidates from that control. The final production build then includes them
  under the normal manifest policies; it is distinct from control packing.
- The first `tubuild linkcheck` attempt refused compilation because the new local
  worktree lacked RcCarpet/Propeller vtable partition control evidence. That raw
  exit 1 is preserved in `build/pr2445-proof-20260908/tubuild-linkcheck.log`.
  After the production build generated the required control, rerunning
  `python tools/tubuild.py --manifest
  build/pr2445-proof-20260908-control/tu_manifest.gate.json linkcheck
  ov072/daBgSnmBdy_c -j8` exits 0, **SCRATCH-DATA-VERIFIED**: all 3,892 text bytes
  and 188 owned data bytes reproduce with zero differences; the object audit is
  33 LICENSED, zero unlicensed symbols/sections. Every module is exact and the
  full scratch ROM has the same stock SHA256 above. The full manifest copy starts
  with the production entries and keeps automatic verification updates ignored;
  the tracked manifest's historical verification object remains unchanged.
  Log: `build/pr2445-proof-20260908-control/tubuild-linkcheck.log`; complete
  report: `build/tu/ov072-daBgSnmBdy_c/link/linkcheck.json`.
  **The nested `dsd check symbols --fail` is still red** on nine errors also
  present in the fresh stock control: zero new, zero resolved. The exact control
  and candidate errors are retained in that report; the nested check is not
  represented as passing.
- `python tools/romdata_check.py --files src/actors/daBgSnmBdy_c.cpp --json
  build/pr2445-proof-20260908/romdata.json` exits 0: six VERIFIED data symbols
  (196 compared bytes), four PARTIAL type-name records (36 equal bytes),
  zero DIFFERS and zero UNNAMED. The partial extents remain partial; the complete
  188-byte owned class data has the separate exact intact-object proof above.
- The global informational emitted-data report retains four distinct DIFFERS:
  arm9 `_ZTV10dCcAcPos_c`, ov002 `_ZTIN8daDemo_c10anmModel_cE` and
  `_ZTIN8daDemo_c13simpleModel_cE`, and ov084 `_ZTV7daKrb_c`. Their sources are
  outside this header's consumer closure and unchanged by this continuation.
  This global helper result is not called all green; production owned-data claims
  and modules have their separately stated exact coverage.
- `python tools/check_header_offsets.py include/daBgSnmBdy_c.h` exits 0:
  15 fields, zero mismatched/unparsed, span `0x38c`, with the tail limitation above.
  `python tools/port_refcheck.py` exits 0: 423 references, zero stale.
  `python tools/check_rename_ledger.py` exits 0: 2,055 mangled/vtable rows agree;
  1,532 coined rows are explicitly outside that check. This does not resolve the
  two pending prose disclosures. Logs are in `build/pr2445-proof-20260908/`.

Post-commit attribution/prepush evidence and the exact commit-to-tested-input
identity are recorded separately in the ignored review bundle. They cannot turn
an unresolved provenance finding into an independent pass.


The historical `tubuild.py verify` refusal for six inherited externalized RTTI
records is preserved in the original handoff; this limited helper was not rerun
because source semantics and those policies are unchanged. The fresh intact-object
production gate below is the relevant complete text/data evidence, with its own
actual result and limits recorded. No blanket success is inferred from historical
checks or a bare promoted-TU filename producing `NO-SYM`.

The private validator previously passed PR head
`19921838ecf6acaf689a4dbc3bb4fd05e4d0effc` on 2026-09-08; it has not tested this
continuation or a future merge composition. Different-session independent review
and proof must precede PR/integration publication. Producer queue handoff, if
approved, comes before that independent stage and is not acceptance.
