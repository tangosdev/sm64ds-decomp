# Handoff: issue-2411-humanizer-2

This is a producer candidate for scoped coordinator review. It has not been
published as ready or independently accepted. The expanded producer lease remains
active; a restarted coordinator must adopt this task and must not launch
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
  occurred for this continuation. Next: scoped coordinator reviews the exact
  candidate and recorded limits, publishes the producer queue handoff, and then
  arranges independent verification before PR/integration publication. Terminal
  private validation remains pending.

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

## Source and host proof

The ROM source/header checkpoint is `b9339491fc6c4153715f6e26b596cd5478be5ffa`.
Subsequent edits only remove the obsolete HAL constructor wrapper, mark its
abort-only trap `[[noreturn]]`, and update this handoff/manifest prose. The ROM
source/header trees and production ownership declarations are unchanged from the
proved checkpoint. Independent verification must still pin the final candidate.

- `build/humanizer-contract-targeted.json`: **34 VERIFIED**, empty diffs, blind 0.
  This passes each source explicitly to normal linkcheck: 25 hooks, three OneUp
  helpers, Key's helper, and five observed Player callers.
- `build/humanizer-contract-tus.json`: all **251 functions MATCH** across eight
  edited promoted TUs, clean relocation type/addend and destinations, with expected
  ROM emission order. Seven runs exit 0. Propeller's promotion audit exits 1 for
  eight unlicensed inherited-chain RTTI records despite 27/27 clean text. The
  exact wired pre-contract control produces the same eight records and exit 1;
  this audit remains a documented baseline limit, not a green promotion gate.
- `build/humanizer-contract-header-proof/summary.json`: immutable checkpoint,
  **3,052/3,052 source-qualified slots visited**, no missing or unexpected slots, plus one
  VERIFIED emitted passenger. **3,046 source-qualified slots VERIFIED**, five BLIND verdicts and
  one NO-REPRO: strict runner exit **1**, not all-source acceptance. The inventory
  spans 1,733 sources and 101 promoted TUs, with all 1,417 manifest functions and
  three additional enrolled initializers explicitly covered.
- The separate `production-enrollment.json` uses unmodified
  `rombuild.enrolled()`: **3,036 production slots**, of which **3,033 VERIFIED**
  and three initializers BLIND. Of 16 non-production draft slots, 13 verify and
  the other three retain their baseline gaps. No config was changed to improve
  this classification. The full production module proof below covers the actual
  linked initializer ranges; it does not change these raw linkcheck verdicts.

| Source slot | Module | Raw result | Production source? |
|---|---|---|---|
| RollingIronBall InitResources | ov100 | BLIND-14 | No: incomplete delink entry |
| daObjMarioCap_c InitResources shard | ov002 | BLIND-33 | No: incomplete delink entry |
| MrI InitResources | ov071 | NO-REPRO | No: not enrolled |
| `__sinit_d_a_obj_fm_battan.cpp` | ov023 | BLIND-6 | Yes |
| `__sinit_d_a_obj_kb1_billboard.cpp` | ov044 | BLIND-3 | Yes |
| `__sinit_d_a_star_gate.cpp` | ov100 | BLIND-3 | Yes |

The coordinator's independent read-only inspection of prepared production
objects, final ELF/map ownership, and actual linked module images separately
verifies **508 initializer bytes and 12 ctor-table bytes**, zero wildcards or
differences. The three initializers resolve 10, 5 and 19 relocations respectively.
The standalone helper omits the production manifest alias policy, explaining its
BLIND-6/3/3 counts; those raw diagnostics remain unchanged. Evidence is in the
coordinator's ignored `build/fleet-restart-20260907/review.oneup.initializer-contributions.json`.
This inspection used the producer's existing build and is not a fresh independent
build or formal queue acceptance; the final verifier repeats it after its own
build. Together with source-qualified function verification, it establishes
coverage of the 3,036 complete production slots without relabeling draft results.

All six raw gaps reproduce at exact control
`07354fbb0c1b09d1c0968ca91034f29c7f455373`, using the same explicit canonical
source paths. Control artifacts are under
`C:/tmp/sm64ds-humanizer-control-oneup-0907/build/humanizer-contract-baseline/`:
`gaps.final.json`, `propeller.log`, and individual raw logs. Earlier scratch
Propeller attempts selected the wrong include path and are not baseline proof.

The full command `python tools/rombuild.py -j16 --report-json
build/humanizer-contract-rombuild.json --data-json
build/humanizer-contract-romdata-all.json` exits **0** with all standard gates:
**11,192 source-built functions reproduce, zero mismatch; 23/23 source-owned data
claims reproduce; three BSS claims; 106/106 modules exact**. It actually packages
`build/sm64ds.nds`, SHA-256
`d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`, identical to
the independently regenerated ROM-gap stock control. No `--no-rom` or check
suppression flags were used. Intact-TU module/storage gates pass with zero new
symbol errors. The nine baseline `dsd check symbols --fail` errors remain:
overlay_100, overlay_102, data_020ad524, data_020ad560, _deq and four ITCM func_*
symbols. That standalone check is not reported green.

Scoped emitted-data measurement compiles **1,733/1,733** consumer sources on both
candidate and exact control. It retains 448 VERIFIED identities (35,632 bytes),
130 PARTIAL, one DIFFERS (Goomba's `_ZTV7daKrb_c`), and 340 UNNAMED, with zero
lost or gained verified identities and identical differing identities. Raw
records and `ratchet.json` are in `build/humanizer-contract-metadata/`; the control
has a sibling `metadata/` directory. Partial extent coverage is not exact-data
proof. Whole-tree build data separately reports 696 VERIFIED, 220 PARTIAL,
four DIFFERS and 397 UNNAMED; it is not an all-tree metadata clearance.

The host `port/slice_gate9.txt` already includes the native dActor constructor.
The obsolete HAL constructor wrapper duplicated that owner and called a raw C2
bridge no longer defined by the source. Removing only that wrapper resolves both
the duplicate dActor constructor and invented raw-C2 dependency. The native
constructor retains its real `mActorListNode(this)` initialization. `actor_trap`
is truthfully `[[noreturn]]` because it always aborts; no result or suppressed
warning was added. Both focused x86 `/Od` and actual Release `/O2 /Ob2 /DNDEBUG`
compiles pass, in `build/humanizer-contract-host-native-owner.json` and logs.

The full `smoke_actor` target compiles the corrected shim but still fails to link:
four other duplicate definitions and 22 unresolved externals, recorded verbatim
in `build/humanizer-contract-host-native-owner-target.log`. Duplicates are Model
C1/C2, ModelAnim C1/C2, dBgW_Kc C1/C2, and dBgActor::Kill. No host link or runtime
pass is claimed. The original control's C2512 constructor error is preserved in
`host.log`; the intermediate initializer probe and its exposed linker errors are
historical logs, not the final implementation.

Reference and port-reference checks pass (423 port references, none stale), and
OneUp's 14 commented field offsets match with struct span 0x398. Contributor
attribution against task base passes: 28 consolidated credits intact, zero changed
or lost. Logs and exact commands are in `build/humanizer-contract-checks.json`.
Supplemental `prepush_linkcheck.py --range
19df96cc21d5e2a87a97a22d4de67191319505bd..HEAD` exits 0: 1,732 checked,
1,609 VERIFIED, 123 warnings, zero blocking; one NONMATCHING draft skipped.
The raw log/JSON are `build/humanizer-contract-prepush.log` and `.json`.
Promoted-TU basename NO-SYM warnings do not provide function coverage; the complete
source-qualified checks and initializer contribution proof above supply it.

Independent verification and terminal private validation remain pending. Earlier
PR green checks are not assigned to this changed source, and the strict raw
consumer gaps/full host link failure must remain visible in any publication.

OneUp still has eight native definitions emitting nine method symbols and 27
free-function bodies. The opaque PMF holder C, local ModelCache view, raw offsets,
and other ABI parameter views remain reconstruction work. The live timer and
Model call do not count as new method conversions. Destructor/vtable/RTTI and
initializer ownership remain unchanged. Unknown original names do not end that
work. All private receipts, generated ROM data, objects and logs remain ignored.
