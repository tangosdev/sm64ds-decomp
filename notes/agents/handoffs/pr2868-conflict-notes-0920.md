# BookShot conflict-note reconciliation

This continuation repairs BOOK2868-06, the six stale manifest conflict notes
reported by the actual `tubuild conflicts` check on PR #2868. The accepted input is
`d160ad64587d3dceac98d5099e113d42de56b59c`, tested against source base
`c04f97677c3c30b269354a36435c027bdd0afd5f`.

Root producer `codex-integrator-0920` owns task `pr2868-conflict-notes-0920` and only
the existing BookShot TU manifest plus this handoff. The primary checkout and the
parallel #2869 source repair remain untouched.

Six original warning strings are retained and explicitly marked RESOLVED:
the two separate ApproachLinear(int&,int,int) legacy sites, ClosestPlayer, Spawn,
UpdatePos and the sound function func_0201267c. The accepted shadow already uses
the genuine ordinary methods/overload and the truthful void sound declaration.
The removed contradictory declarations and markers therefore no longer describe
open conflicts. All thirteen remaining open notes and source markers stay intact.

Only these note prefixes and this handoff change. Every source, header, compiler
input and non-note manifest field is identical to the independently accepted
input. Its strict 31-function and eight-consumer proofs and complete-object hash
`05b5f1dc672fdd24cda95ebcd02e9a2327fe297bc948a889077b75a320620688` remain historical
proof bound by that identity; this correction makes no new compiler-pass claim.
The earlier source review omitted the inexpensive conflict-accounting gate. The
new candidate requires a fresh successful check and independent manifest review.

BOOK2868-01 through BOOK2868-05 retain their dispositions. Remaining reconstruction
is partial under [issue #2872](https://github.com/tangosdev/sm64ds-decomp/issues/2872),
owned by `codex-integrator-0920`. Seventeen unlicensed outputs, three missing
compiler policies and two destructor-order pairs still prevent promotion.

The original [source repair](pr2868-source-repair-0920.md) and
[composition](pr2868-composition-0920.md) contain the accepted source proof.
An independent verifier must accept this exact successor before integration.
