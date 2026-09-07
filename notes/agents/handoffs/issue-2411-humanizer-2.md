# Handoff: issue-2411-humanizer-2

This is a committed producer WIP, not a published verifier input. Source is frozen
for the full affected-consumer and production proof. The expanded producer lease
remains active; a restarted coordinator must adopt this task and must not launch
replacement writers for these classes or release its resources.

## Identity and ownership

- Task `issue-2411-humanizer-2`, stage `revise`, class `da1up_c` / PR #2398,
  issue https://github.com/tangosdev/sm64ds-decomp/issues/2411.
- Producer `codex-humanizer-oneup-20260907-01`; scoped coordinator
  `codex-humanizer-fixes-20260907-01`; `fleet-0907` retains global coordination.
- Branch `cpp/humanizer-oneup-0907`, wired worktree
  `C:/tmp/sm64ds-humanizer-oneup-0907`.
- Adopted input `19df96cc21d5e2a87a97a22d4de67191319505bd`, task source base
  `26f54f8fc8faaf00568e86d33f37860a46470906`, workflow/tools
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- The scoped coordinator atomically amended and reclaimed 121 resources before
  the shared edits. The private receipt remains ignored and must not be published.
- No source push, ready publication, queue completion, or formal acceptance has
  occurred for this continuation. Next: finish producer proof, record exact gate
  limits, then request the scoped coordinator's independent review.

## Preserved outputs

The OneUp counter/Model/provenance checkpoint is
`2f0cfad6cc052a9152952e86aa5cf692667a3248`. Its timer member is a coined
`u16 mStateTimer` at 0x38c, preserving the 0x398-byte layout. Three functions now
use that live field and Render calls `mModel.Render(0)` instead of a fake six-slot
vtable. Those edits passed separately, together, and across all 36 OneUp functions.
The earlier handoff at that exact commit preserves the original probe evidence.

Independently accepted Moneybag `3a2db65652e77716dd663ac2f31c40c6aa5970f7` and
Ukiki `ac5af1055350c9467cd62b0d5e5c4aa8fe88a710` were adopted by merge, preserving
all history. Moneybag corrects provenance; Ukiki's EnterState0 uses `mState = 0`.
Their original evidence remains in [Moneybag's handoff](issue-2412-humanizer-2.md)
and [Ukiki's handoff](issue-2410-humanizer-2.md). Only their superseded task
envelopes were cancelled after independent passes. Their output is now owned
by this expanded continuation. The clean merged pre-contract control is
`07354fbb0c1b09d1c0968ca91034f29c7f455373`.

Attribution is unchanged: integration commit
`e3d7f6e6633078ace023d8478002c36526996933` already repaired the eight previously
reported reassignments. Earlier accepted OneUp source
`bb5bbfae682dccfc1cdb0aab0defdd702ad919c9` and its prior gates remain historical
in [the preceding handoff](issue-2411.md).

## Contract correction

The actor `OnTurnIntoEgg(Player&)` base and 24 named overrides now return void,
with matching headers, the host shim, and the three remaining Player `Obj::v19`
caller views. OneUp's `func_ov002_020af684` no longer has an int definition that
falls through after a void call. Its two PMF forwarding helpers also return void,
retaining the player lookup result, null test and actual argument. Key's helper
and destruction prototype now agree with the existing void destruction method;
Key and PowerStar forwarding declarations agree with their helper definitions.
Early `return void_expression;` control flow is retained. No arbitrary result,
assembly, warning suppression, altered address or symbol name was introduced.

This is a source-consistent, byte-proved reconstruction, not recovery of an
original type spelling. The pre-edit census found 21 nonempty fallthrough hook
bodies, one empty body, and three bodies forwarding calls (OneUp also falls
through), with zero constant/computed non-call return expressions. All 25 ROM
exit patterns support void without deleting meaningful register writes. Five
observed Player calls overwrite r0 before using an outgoing result. Unused r0
alone does not prove an original return type. The ARM immediate-slot scan does
not cover Thumb, computed slots or a complete indirect-call graph.

The separate minigame `OnTurnIntoEgg(int)` hierarchy remains int: 14 observed
ov004 calls consume its result. Goomba remains an unmigrated free hook; only its
header explanation changed. Its earlier int-versus-void 4-byte experiment is
preserved as historical evidence, not new acceptance of Goomba.

## Current proof and limits

- `build/humanizer-contract-targeted.json`: all **34 VERIFIED**, empty diffs,
  blind 0. This explicitly passes each source into normal linkcheck: 25 hooks,
  three OneUp helpers, Key's helper, and five Player callers.
- `build/humanizer-contract-tus.json`: all **251 functions MATCH** across the
  eight edited promoted TUs, clean relocation type/addend and destinations, with
  expected ROM emission order. Scratch manifests preserve tracked historical
  metadata. Seven runs exit 0. Propeller's run exits 1 for eight unlicensed
  inherited-chain RTTI records despite 27/27 clean text; pre-contract comparison
  is pending. This promotion audit is not green.
- Focused x86 host compile and full configured `smoke_actor` build both stop at
  `port/hal/actor_ctor_bridge.cpp:63`, fLiNdBa_c default-constructor error C2512.
  An immutable pre-contract source/header scratch control reproduces that same
  error. Logs: `build/humanizer-contract-host-compile.log`,
  `build/humanizer-contract-host-target.log`, and
  `build/humanizer-contract-host-control.log`. No host link/runtime pass claimed.
- Full closure is planned for **1,733 sources / 3,052 owned ROM slots**, including
  101 promoted TUs and the explicit extra caller/helper shards. It must verify
  complete manifest functions; basename NO-SYM is not proof. Execution pending.
- Full production module/ROM build, changed-range gate, final reference/data
  checks, and terminal private validation are pending for this source checkpoint.
  The prior PR's green checks are not assigned to changed source.

OneUp still has eight native definitions emitting nine method symbols and 27
free-function bodies. The opaque PMF holder C, local ModelCache view, raw offsets,
and other ABI parameter views remain reconstruction work. The live timer and
Model call do not count as new method conversions. Destructor/vtable/RTTI and
initializer ownership remain unchanged. Unknown original names do not end that
work. All private receipts, generated ROM data, objects and logs remain ignored.
