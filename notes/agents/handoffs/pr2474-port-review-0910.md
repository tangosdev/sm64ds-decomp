# PR 2474 transcription gate repair, 2026-09-10

This handoff covers PORT-TRANS-01 only. The original port candidate is
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
diagnostic helper bodies are pinned by non-comment C token digests; changing
one requires review of that exception. The clock reads steady time, the
warning tracks its own printed flag, and the expiry reporter reads comms
counters/player count/statistics and updates only its diagnostic rate limit.
Those three comms accessors were separately read in the current source.

Fresh checks: `python port/tools/test_hostloop_transcription_check.py` passes
20 tests. The CLI also passes from outside the repository. The actual gate
reports 406 original body lines, 60 extern symbols and three pinned helpers.
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
