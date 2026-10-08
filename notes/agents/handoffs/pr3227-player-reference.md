# PR #3227: Player reference parameter

Input: `dbd94c7c483db1b6c5603c7ec2a680b46461643e`.
Task: `pr3227-player-reference`. Producer/integration owner: `codex-pr3227-reference`.
Independent reviewer: `kpa3_review`. Endpoint: push to existing PR #3227; no merge.

## Scope and authorization

Change [func_ov002_020d7430](../../../src/unnamed/ov002/func_ov002_020d7430.cpp) to take `Player&`, update its two source callers, and
remove its obsolete char* declaration from include/decl_common.h. The user
explicitly confirmed this exact prepared patch, including the scoped header edit
while `jump-contract-repair-0918` reserves that header. That task's branch and
reservation remain untouched. No other shared-header declarations change.

The existing Player method passes *this directly. The remaining legacy free
function caller converts its char* at that call boundary. The callee retains a
char* view solely for its existing Hurt, state and address-named helper bridges.
C linkage preserves the enrolled symbol name. No layout, symbol/config identity,
attribution, enrollment, or baseline exception changes are needed.

Completion is partial: this exposes the evidenced Player object in the signature;
it does not complete the legacy caller, shared Player fields, or engine bridges.
The prior handoff pr3227-player-caller.md records those inherited limitations.

## Verification

The prepared patch's three functions independently passed the pinned compiler's
linked-byte probe before application: VERIFIED, diffs [], blind 0 for each:

- [ov002](../../../config/arm9/overlays/ov002/symbols.txt) [func_ov002_020d6790](../../../src/unnamed/ov002/func_ov002_020d6790.cpp): 0x020d6790, size 0x208.
- [ov002](../../../config/arm9/overlays/ov002/symbols.txt) [func_ov002_020d7430](../../../src/unnamed/ov002/func_ov002_020d7430.cpp): 0x020d7430, size 0xd4.
- [ov002](../../../config/arm9/overlays/ov002/symbols.txt) [Player::St_YoshiPower_Main](../../../src/_ZN6Player18St_YoshiPower_MainEv.cpp): 0x020d7504, size 0x9cc.

Applied production declaration check:
`python tools/check_decl_agreement.py --changed dbd94c7c483db1b6c5603c7ec2a680b46461643e`
passes, with no new declaration disagreements. affected_src.py reports 535 source
consumers of decl_common.h; the header change only removes the obsolete unused
prototype. Final production command:
`python tools/rombuild.py -j16 --no-rom --report-json build/player-reference-rombuild.json`
returned exit 0: 106/106 modules exact, 11,215 source-built functions reproduce,
0 mismatches, 27/27 source-owned data claims reproduce, and 0 new symbol errors.
523 source files were recompiled after the header change. The whole-tree report-only
ROM-data inventory still has 3 differing symbols; this patch does not repair them.

The range consumer gate expanded to 538 files / 2,244 functions. Its serial run was
stopped for runtime reasons, not counted as a completed gate. The unchanged
`prepush_linkcheck.py --files ... --json ...` checker was then run in 12 disjoint
parallel batches, covering exactly `changed_src_files(input..HEAD)`. A Counter of
(file, symbol) pairs from the aggregate reports equals the expected complete scope,
including multiplicities. All 12 exits were 0: 2,242 VERIFIED, 2 warnings, 0 blocking.
Ignored local reports: build/player-reference-parallel/{scope,summary}.json,
batch-0.json through batch-11.json, and build/player-reference-range.json.
No tracked verification tools or acceptance criteria were changed.

Both warnings independently reproduce at input and candidate:

- `_ZN9dScDSMT_c8BehaviorEv`: BLIND-2, diffs []; unresolved overlay_64 at +0x184
  and overlay_66 at +0x188, R_ARM_ABS32, addend 0.
- [func_ov089_0213162c](../../../src/actors/daObjKey_c.cpp): BLIND-1, diffs []; unresolved data_02111b68 at +0x4e0,
  R_ARM_ABS32, addend 0.

All three edited functions independently return VERIFIED, diffs [], blind 0.
The reviewer checked source commit `85fc2ffe75c6564884e634b9191ea7ab2f361b10`
against the exact input base in an isolated wired worktree. The reviewer also ran
declaration and attribution checks, and found no actionable source defects. Root
attribution check: 0 changed, 0 lost. This proof-note update changes no reviewed code.
Source acceptance is partial and scoped to this signature change; whole-PR hosted
validation and Source review remain separate. Next action: publish this exact
candidate to PR #3227 after the final metadata review.
