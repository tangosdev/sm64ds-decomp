# Dossunbar native static initialization

Producer: `codex-native-inits-three1015`; task `native-inits-three1015`.
Class issue: https://github.com/tangosdev/sm64ds-decomp/issues/2438.
Original main: `e209dacfaaa95cb5490637ff053d62182689bebe`. Final predecessor: Arrow Sign
`9e431f31d3123779ae170365420423adfdbb83a7`, including Snowman Breath PR #3887.

`src/game/actors/d_a_obj_bk_dossunbar.cpp` defines its four file handles and
two seven-entry dispatch tables as ordinary C++ globals. The compiler emits
the original 540-byte initializer and constructor-table word, replacing the
handwritten C shard. Fourteen ROM-owned PMF descriptors and both tables use
ordinary C++ linkage. Existing state bodies and descriptor/data ownership
remain partial; the fourteenth state-selection helper is not reconstructed
as a new class method by this slice.

The local DossunbarModelFile/DossunbarCollisionFile type names are inferred
eight-byte ABI views of existing resource families. Exact manifest aliases
bind their C++ constructor/destructor imports to the original arm9 functions.
Construction order remains small-model, small-collision, large-model,
large-collision, followed by enter-table and update-table initialization.
No new helper implementation or shared header is added.

Owned ov015 ranges (half-open): .text 0x02111ba0..0x02112290;
.init 0x02113048..0x02113264; .ctor 0x02113418..0x0211341c;
.bss 0x0211499c..0x02114a5c (four eight-byte handles, four 12-byte
registration nodes and two 56-byte tables). The exact compiler-node renames
@745/@746/@747/@748 -> @939/@940/@943/@963 retain all original addresses
under unique labels. Manifest-licensed complete-section ordering preserves
the original BSS object order without modifying contents or relocations.

Early proof: pinned 2004/b56; all 25 original function bodies unchanged;
all three nontext ranges and all 35 relocations verify. Full candidate gate: SCRATCH-DATA-VERIFIED; all four owned ranges, all 35
nontext relocations and all 106 modules pass. The complete three-cleanup stack
builds the exact 16 MiB retail ROM, SHA-256
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
The raw symbol audit retains the same nine existing failures, zero new; it is
not green. The predecessor's final proof/handoff refresh changes no runtime
source/config/manifest contract relative to the full-ROM-tested stack.
Per-function prepush: 25 VERIFIED, initializer BLIND-10 on generated C++
imports, zero blocking. Exact owned-section alias checks close those ten sites
and prove every nontext destination. Source style, declaration agreement,
rename ledger, dead references and the conversion ratchet pass. Focused metadata:
6 VERIFIED, 11 PARTIAL, zero DIFFERS.
Default production command: `python tools/rombuild.py -j16 --no-rom
--report-json build/dossunbar-production.json`; the terminal result is recorded
in the PR description. Reports stay under ignored build/dossunbar-baseline.txt,
build/dossunbar-fullrom.txt, build/dossunbar-production.json and
build/dossunbar-prepush.txt.
The composed conversion ledger removes exactly the Arrow Sign and Dossunbar
handwritten initializer identities (3485 -> 3483). Every unrelated identity
and its order, and all original exception rows, are preserved. Registry profiles, RTTI/vtable
data, resource-table column relocation identities and Fix12 ABI bridges remain
under their existing policies. Attribution remains by original ROM address.
Independent source acceptance and private CI validation are pending.
