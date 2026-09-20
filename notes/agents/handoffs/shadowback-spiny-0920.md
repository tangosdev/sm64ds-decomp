# shadowback-spiny-0920

Producer: `codex-shadowback-0920` (Codex). Source and workflow base:
`077483245976d72477eba0d2a7ccca4390da2b1b`. Branch: `fix/shadow-backports-0920`.
This describes the local candidate containing this handoff. The producer lease is
held in the v2 task named above. No source commit or review evidence has been
published through the queue; `formal_acceptance: false`.

The scope is one already-enrolled production function. Shared headers, symbols,
delinks, attribution, lifecycle definitions and the shadow TU are unchanged.
No whole-TU promotion or completion of the parent reconstruction is claimed.

Backport named members and call-contract repairs into
`src/_ZN5Spiny13InitResourcesEv.cpp`. Six resource/model/shadow calls now use their
existing real C++ interfaces. Collision and animation calls use the recovered
members. The state helper now matches its actual `void(char*, int)` definition;
the spurious third argument is removed. The animation bridge matches the actual
scalar definition's `u16` final argument. Its shared native declaration still
uses `u32`; this caller does not assert that broader interface is reconciled.

The mesh bridge explicitly follows its actual production C definition's integer
address parameters. This deliberately differs from the shadow pointer view;
restoring a coherent native mesh interface requires header/callee/caller work.
The native mesh header currently spells a different, unmapped symbol.

A fresh caller experiment substituted:
```cpp
Fix12<int> radius = {0x2d000};
Fix12<int> height = {0x3c000};
mdCcAc_c.Init(this, radius, height, 0x200000, 0x4a3d0);
```
With production C++ flags and `2004/b56`, it emits 332 bytes against 316 expected.
Strict linkcheck reports `NO-REPRO` / `requested-size-mismatch`. The failed form
was not adopted. Its source and machine-readable result are preserved locally in
`build/shadow-backport/spiny-native-cylinder.cpp` and `.json`. This is one bounded
experiment, not a universal claim about every possible native form.

SP2863-01 through SP2863-06 retain their historical dispositions. The three-arg
state-helper requirement in the old manifest is contradicted by the later
#2863 experiment and this production whole-object identity check. Raw matrix,
rotation and resource-handle views, retail class identity and complete-object
ownership remain under issue #2870. No shared-interface completion is claimed.
Source lineage: merged PR #2863, commit `aba8efc28`.

## Local proof

- Scope: `ov077:0x2126058`, 316 bytes,
  `_ZN5Spiny13InitResourcesEv`.
- Before and after explicit `tools/linkcheck.py --c <source> --name <symbol>
  --addr <address> --size <size> --module <module>`: `VERIFIED`, `diffs: []`,
  `blind: 0`.
- Complete unisolated compiler object: 1688 bytes, byte-for-byte
  identical to the source-base object. Both SHA-256 values:
  `6268d43c620aa0caff5045207734aa0c16f0e61f0ed0f8ad767a734c2f2e660a`.
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
