# shadowback-kinopio-0920

Producer: `codex-shadowback-0920` (Codex). Source and workflow base:
`077483245976d72477eba0d2a7ccca4390da2b1b`. Branch: `fix/shadow-backports-0920`.
This describes the local candidate containing this handoff. The producer lease is
held in the v2 task named above. No source commit or review evidence has been
published through the queue; `formal_acceptance: false`.

The scope is one already-enrolled production function. Shared headers, symbols,
delinks, attribution, lifecycle definitions and the shadow TU are unchanged.
No whole-TU promotion or completion of the parent reconstruction is claimed.

Backport the star-ID argument correction from shadow PR #2829 into
`src/_ZN11daKinopio_c12St_Talk_MainEv.cpp`. Both the declaration and call now say
that `IsStarCollectedInCurLevel` receives `int starID`, agreeing with its actual
production definition in `src/IsStarCollectedInCurLevel.c` and the existing
`daKinopio_c::InitResources` caller.

The old zero-argument declaration happened to reproduce the retail instructions
because the preceding comparison already left the star ID in the argument
register. The source now expresses that real dependency. The function's existing
`opt_propagation off` pragma and other reconstructed interfaces are unchanged.
This two-line repair does not claim whole-TU promotion or repair the remaining
Toad declarations, fields or lifecycle ownership.
Source lineage: merged PR #2829, commit `2c8493b80`.

## Local proof

- Scope: `ov085:0x21291ac`, 656 bytes,
  `_ZN11daKinopio_c12St_Talk_MainEv`.
- Before and after explicit `tools/linkcheck.py --c <source> --name <symbol>
  --addr <address> --size <size> --module <module>`: `VERIFIED`, `diffs: []`,
  `blind: 0`.
- Complete unisolated compiler object: 2320 bytes, byte-for-byte
  identical to the source-base object. Both SHA-256 values:
  `97dfe933cced8e873450bedbc417b1143ef9deb19fb45fc96111f9774120f7a1`.
  Only the target function is defined; no lifecycle/RTTI/vtable output was added.
- Compilation uses `2004/b56` and `rombuild.CFLAGS` with `-lang c++`.
- Three-file prepush linkcheck: 3 checked, 3 verified, no warnings or blocking.
- `check_decl_agreement.py --changed 077483245976d72477eba0d2a7ccca4390da2b1b`:
  no new declaration disagreements; no baseline was edited.
- `port_refcheck.py`: 418 references, all resolve. `git diff --check`: pass.
- `rombuild.py -j 16 --no-rom`: PASS. 11,208/11,208 source-built functions
  reproduce; 106/106 modules exact; all 26 owned initialized-data claims exact,
  plus three BSS claims. 8,021 object-cache hits and exactly these three fresh
  compiles. Final candidate ROM packaging was skipped. The stock control retains
  nine symbol errors, and intact-TU comparison introduces zero new errors.
  The broad emitted-data report still has 3 differing and 298 unnamed records;
  none is emitted by these three code-only, object-identical changes.
  Log: `build/shadow-backport/rombuild.log`; structured report:
  `build/shadow-backport/rombuild-report.json`.
- Language-mode check against chaos-data `c2a172f73a18dc02b8f416f8fe598341cbc28b7f`:
  reports `extern_vtable` 104 -> 105 and classes 105 -> 106. The independent
  clean worktree at the exact source base reproduces both failures. This is an
  unchanged main/baseline failure, not a passing gate. The CONVERTED ratchet
  passes at 2989 against 2973, identically on the base and this candidate.
- Independent exact-commit review and private CI: not yet run for this candidate.

Next owner is the independent verifier for this local candidate. Remaining full
class reconstruction stays with its existing follow-up issue and ownership.
Local reports are preserved under `build/shadow-backport/`; compiler objects are
private ignored output and must not be committed or published.
