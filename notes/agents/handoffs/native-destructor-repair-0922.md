# Lifecycle receiver contract repair

PR #2908 fixed fifteen missing-receiver arity findings. This continuation fixes
the related direct destructor results, constructor contracts and runtime callback
interfaces. The declaration checker landed separately in #3143; #3168 banked its
remaining diagnostics without repairing the source. This commit supplies the
source repair and removes only the newly banked keys that it actually heals.

The composition starts from `f5681d31b5a6c100e65da5ef33879c05e1c54a6f`.
It preserves the newer native methods, factories, fields and TU promotions.
Eykn and Hanachan now construct native dBgCh_Gnd objects; Cannon uses native new.
Those owners' replacements supersede the earlier proposed raw declaration edits.
No retired source or removed local declaration is restored.

Direct constructor and non-deleting destructor ABI views retain their receiver
and pointer result. Array counts and strides use the runtime's unsigned types;
result-discarding callback casts remain explicit at the measured array-runtime
boundary. Static-destructor registration and Player's callers retain object and
callback addresses as pointers. Roulette passes its allocated scene to its base
constructor. The existing source credit and build enrollment are unchanged.

The raw callable census covers 392 constructor/Vector prototypes. It follows the
earlier population through native replacements and includes block-local imports
outside the checker's general parsing coverage. Existing object-shaped Vector
symbol-address views remain outside this callable repair. This is a bounded
interface correction; complete native class reconstruction is not claimed.

The exact candidate/base identities and current proof belong to the PR and queue.
Required checks include the checker unit suite, full and scoped declaration
agreement, complete affected
consumer objects and relocations, full ROM/module verification, and independent
source acceptance. Previous source/checker branches and older measurements remain
historical evidence; they do not replace the applied candidate's acceptance.

Carry NDT0922-01..03, RCV2908-01..04, RCV-PACKET-01..04 and
STALE-ARRAY-RUNTIME-TEST into the independent handoff. The original NDT findings
were top-level records and require explicit carry-forward. Preserve their kinds
and evidence, including the resolved UTF-8 BOM harness failure.

Integration owner: codex-receiver-publication-0925. Active owners' unrelated
work, branches and reservations remain intact. No checker change or new baseline
allowance is part of this source continuation.
