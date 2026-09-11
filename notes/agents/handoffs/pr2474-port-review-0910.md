# PR 2474 transcription gate repair, 2026-09-10

This handoff covers PORT-TRANS-01 through PORT-TRANS-05. The original port candidate is
`91f525963d101265c3b88ef33ee5c19f996d08ac`; its ancestral source base is
`b99310a89af9c0e4737f81ebde00d5599dba7e31`. The PR target is separately
`31f3160466217cd906516d260dba0ac8f76aee26`, which is not an ancestor of
that candidate. The task continues `pr-2474-port-source-review-fixes`; that
completed historical task recorded no structured finding IDs. Its accepted
`404af352a70ebee5fef62af629bb8afb774a9691` is not an ancestor of this head,
so its adoption was checked in the current source rather than inferred.

PORT-TRANS-01: the existing transcription checker rejected the candidate's
six VS7 diagnostic additions. It now normalizes only the exact declarations,
counter increments, report arguments and surrounding game statements. The
three existing warning calls and local silencer also require their reviewed
sites. No variable-name or call-prefix blanket exclusion remains. The three
diagnostic helper bodies are pinned by exact source digests; changing
one requires review of that exception. The clock reads steady time, the
warning tracks its own printed flag, and the expiry reporter reads comms
counters/player count/statistics and updates only its diagnostic rate limit.
Those three comms accessors were separately read in the current source.

Fresh checks: `python port/tools/test_hostloop_transcription_check.py` passes
31 tests. The CLI also passes from outside the repository. The actual gate
reports 406 original body lines, 60 original extern declarations (including CpuCopy8) and three pinned helpers.
Negative controls cover altered wait conditions, state writes, callees,
extern widths, moved/duplicated counters, warning/silencer movement, report
arguments, logging-side mutations, continued comments and missing/duplicate
functions. `python port/tools/hostloop_transcription_check.py` passes on the
actual candidate. No C/C++ source or host build setting changed in this repair.

The earlier unnumbered review requests remain part of the full PR review:
integer address observations in boot/ARM7 diagnostics; an unchanged info
pointer described only as an observation; three actual info flags; removal
of the dead Enemy constructor fallback and unused Roulette profile extern;
shipping-cache validation; and unconditional trimming of the final raw VS4
frame. Those adopted corrections were read at the stated candidate. Fresh
existing tests passed 10 shipping-cache cases and seven VS4 proof cases, and
six changed cartridge functions passed relocated checks (300 bytes total).
These checks do not establish full host execution, command/reply behavior,
nonzero wireless work or multiplayer/rewind completion. Separate owned work
is repairing toolchain discovery, generated-owner reference checking and a
new cross-array pointer subtraction finding in wm_thread.cpp. The full port
PR and its 134-file target diff are not accepted by this small gate repair.

Independent rework, before acceptance: PORT-TRANS-02 removed the inherited
blanket `kRecs` replacement. Only the seven exact reviewed record walks can
normalize; the two fixed four-slot info loops and `sp18 = 4` remain fixed.
PORT-TRANS-03 pins reviewed code outside the compared bodies as well, covering
includes, preprocessor context, typedefs, linkage blocks, declarations and the
dispatch wrapper. An inactive old helper plus a new active definition, a callee
macro, or a changed record cannot preserve a passing body-only result.
PORT-TRANS-04 compares every original extern declaration by its actual name,
including CpuCopy8; the only two extra host declarations are exact outside-code
pins. The count remains 60, now meaning all 60 original declarations rather
than the former address-name filter plus its partial narrow-arm name match.
The 31-test suite includes all six independently reported false-pass fixtures,
all seven required widening sites, declaration additions/removals and both
source and wide context changes. These pins are source-review boundaries:
changing reviewed context requires a new review, not a claim of ROM proof.

PORT-TRANS-05: independent native probes showed that character-token hashes
confused `++n` with two unary plus operators and erased include-directive
line boundaries. Helper and outside-code pins now compare exact source with
only line-ending and leading-indentation normalization. Operators, comments,
logical directive boundaries and trailing whitespace are preserved; even a
harmless unrecognized helper comment edit requires fresh review. The three
new full-source negatives and all prior negatives are covered by 31 tests.
