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
- Originating branch `cpp/humanizer-oneup-0907`, wired worktree
  `C:/tmp/sm64ds-humanizer-oneup-0907`. The reworked candidate continues on
  branch `cpp/humanizer-oneup-0908-repair` in worktree
  `C:/tmp/sm64ds-oneup-prod-0908`.
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
across the headers, the host shim, and the three remaining Player `Obj::v19`
caller views.

An earlier revision of this section claimed those overrides returned void "with
matching headers". That was wrong, and a whole-tree compile falsified it. The
header change moved a shared contract in `include/dActor_c.h` and its sibling
actor headers but touched no file under `src_tu/`, so four byte-verified
promoted translation units still defined the method as `int` and the compiler
rejected them as redeclarations. Measured: at candidate `848da2bb8`,
`tools/check_src_tu_compiles.py` exited 1 with 156/160; at base `26f54f8fc` it
exited 0 with 159/159 (the denominator moves because the candidate enrolls
`ov002/da1up_c`). The four definitions — `src_tu/actors/Actor.cpp:1140`,
`src_tu/actors/Koopa+KoopaSmall.cpp:347`, `src_tu/actors/Lakitu.cpp:88` and
`src_tu/actors/Spiny.cpp:88` — are now void, and the evidence bullet below
records the re-run.

That repair is a source-consistency fix; it is not new evidence about the
original return type, and the caveats stated later in this section still apply
unchanged. It rests on the same reasoning as the rest of the contract: all four
bodies already fell off the end with no return statement, and the only slot-19
dispatch site, `Player::St_YoshiPower_Main`, models the callee as
`virtual void v19(char *)` and invokes it as a bare statement, so no caller
consumes a value from them.

OneUp's `func_ov002_020af684` no longer has an int definition that
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

## Repairs after independent integration review

An independent integrator failed candidate `6bf546d78` and reworked the task.
The repairs landed on branch `cpp/humanizer-oneup-0908-repair` in worktree
`C:/tmp/sm64ds-oneup-prod-0908`.

An earlier revision of this section narrated three repairs followed by a merge
of `main`. That ordering is inverted. The merge is `587f156e3`, its parents are
`6bf546d78` and `c52f63ca5`, and it is the oldest of this branch's commits, so
the merge came first and every repair below was made on the merged tree. The
bullets are in that order. This is immaterial to the tree, which is the same
either way; only the narration was wrong.

- **The merge.** Merged `origin/main` at `c52f63ca5`, 94 commits ahead of the
  task base. The only conflict was `notes/cpp-tu-current-state.md`, which is
  machine-generated. Regenerated it with `tools/cpp_tu_state.py --write-note`
  rather than hand-resolving, and `--check-note` exits **0**. The live counts
  (170 manifest entries, 9,097 physical production source files) match neither
  side of the conflict, so a hand-merge would have shipped a wrong number.
- **The `dead-references` gate** was exit 1 on this branch while exit 0 on
  `origin/main` and at base `26f54f8fc`. `PATH_RE` in
  `tools/check_dead_references.py` excludes `+`, so the Koopa translation unit
  cited in the section above is truncated at the plus and the truncation is
  reported as a path that is not in the tree. Eight tracked files carry `+` in
  the name and that regex can express none of them, so the citation is right.
  Banked the exact (citing file, referenced path) pair in
  `config/dead-reference-baseline.json`, the remedy that tool's own docstring
  names, with direct precedent one entry above for the identical truncation of
  the `EnemySpawner` translation unit. The prose was not reworded and the tool
  was not changed. `python tools/check_dead_references.py` now exits **0**.
- **The queue rows.** This branch's `da1up_c` row in
  `notes/data/tu-promotion-queue.tsv` wrote `total_lines 1295`;
  `tools/queue_audit.py` derives **1284** from the three source files covering
  ov002 `0x20aee40`-`0x20b05d0`. Correcting that cell left five rows still
  disagreeing, and an earlier revision of this bullet called all five
  pre-existing on `origin/main` and left them alone. That was wrong for three of
  them. The sentence it rested on was true as far as it went — those cells are
  unchanged from `main` — but the conclusion drawn from it was not, because
  three of the rows are stale precisely as a result of this branch's own edits
  to the files they measure. Measured directly at `c52f63ca5`, base has **2**
  disagreeing rows; this candidate had **5**. The three extra:

  | row | old cell | refreshed | source file | lines base → here |
  |---|---|---|---|---|
  | `Crate` | 1431 | **1427** | `src/_ZN5Crate13OnTurnIntoEggER6Player.cpp` | 42 → 38 |
  | `Coin` | 1148 | **1147** | `src/_ZN4Coin13OnTurnIntoEggER6Player.cpp` | 24 → 23 |
  | `daGmch_c` | 1119 | **1120** | `src/actors/daGmch_c.cpp` | 1119 → 1120 |

  Each row delta matches its file delta exactly, so these three are this
  branch's to move. `ChiefChilly` (1992 → **1993**) and `Player`
  (26798 → **26799**) genuinely are pre-existing: `main`'s own `9b8cb8add`
  edited `include/ChiefChilly.h` and `include/Player.h` without refreshing the
  queue file, and they are the two rows base already disagrees on. All five
  were refreshed by one `python tools/queue_audit.py --write` pass, which
  rewrote only the derived columns of `notes/data/tu-promotion-queue.tsv` and
  no other file. After it, `python tools/queue_audit.py` reports "queue agrees
  with the tree" — **0 rows disagreeing** — and `--check` exits **0**. Take
  those two readings, not the bare exit status: the plain invocation exits 0
  whether or not rows disagree, and it exited 0 while five rows were stale.

### Re-measured after the merge

The build readings recorded in the next section were taken before that merge.
They do not transfer across 94 commits of `main`, so they were taken again on
the merged tree. Where a figure below supersedes one in the next section, the
figure below is the one measured on this tree.

- `tools/check_src_tu_compiles.py`: exit **0**, **170/170** translation units
  compile. Set-differencing the compiled-unit lists against the base control
  (exit 0, 159/159) shows **no unit dropped**: the eleven added are the ten
  `main` enrolled between `26f54f8fc` and `c52f63ca5`, plus this branch's own
  `ov002/da1up_c`. Comparing totals alone could not have shown that, because a
  rising total can hide a dropped unit.
- `tubuild verify` on the four repaired translation units, re-run separately
  after the merge: `arm9/Actor` **97/97 MATCH**, `ov062/Koopa+KoopaSmall`
  **39/39 MATCH**, `ov077/Lakitu` **32/32 MATCH**, `ov077/Spiny` **34/34
  MATCH**, all four objisolate clean, reloc-destinations clean and
  TEXT-VERIFIED, and all four exit 1 on the same unlicensed compiler-only
  records. The base control at `26f54f8fc` reproduces every one of those
  counts, the same unlicensed records and the same exit 1, so the promotion
  refusal is inherited and not a regression from this branch. `tubuild verify`
  rewrites the manifests it checks; both worktrees were restored afterwards and
  `git status --porcelain` confirmed empty before committing.
- `tools/rombuild.py`: exit **0**, **106/106 modules exact**, 100.000000% of
  compared bytes, ROM-build analysis PASS, sha256
  `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`. The ROM
  was deleted first and confirmed gone before the run, and the rebuilt file's
  mtime falls inside the build window, so the hash is not read off a failed run
  that left the previous ROM in place. As `main`'s work landed, source-built
  functions rose to **11,199** with zero mismatching, source-owned data claims
  to **25** reproducing with zero mismatching, and whole-tree ROM data to
  **705** verified, 224 partial, four differing and 387 unnamed by config.

## Source and host proof

Unless a figure is restated under "Re-measured after the merge" above,
every reading in this section was measured at pre-merge candidate
`6bf546d78` on task base `26f54f8fc`, and describes that tree.

The ROM source/header checkpoint is `b9339491fc6c4153715f6e26b596cd5478be5ffa`.
Subsequent edits remove the obsolete HAL constructor wrapper, mark its
abort-only trap `[[noreturn]]`, update this handoff/manifest prose, and — added
in the repair described above — change the return type of four
`OnTurnIntoEgg(Player&)` definitions under `src_tu/` from `int` to `void`. That
last edit is the only change to the ROM source tree since the checkpoint; the
header tree and the production ownership declarations are unchanged from it, and
each of the four affected TUs was re-verified byte-for-byte (see below).
Independent verification must still pin the final candidate.

- `build/humanizer-contract-targeted.json`: **34 VERIFIED**, empty diffs, blind 0.
  This passes each source explicitly to normal linkcheck: 25 hooks, three OneUp
  helpers, Key's helper, and five observed Player callers.
- `build/humanizer-contract-tus.json`: all **251 functions MATCH** across eight
  edited promoted TUs, clean relocation type/addend and destinations, with expected
  ROM emission order. Seven runs exit 0. Propeller's promotion audit exits 1 for
  eight unlicensed inherited-chain RTTI records despite 27/27 clean text. The
  exact wired pre-contract control produces the same eight records and exit 1;
  this audit remains a documented baseline limit, not a green promotion gate.
- `tools/check_src_tu_compiles.py`: **exit 0, 160/160 translation units compile**
  after the four `src_tu/` definitions were changed to void — superseded by the
  170/170 reading above, which is the same gate on the merged tree. The same command
  exits **1 at 156/160** on candidate `848da2bb8` and **0 at 159/159** on base
  `26f54f8fc`, so the four failures were introduced by the header change and are
  now cleared. This gate proves compilation only; it says nothing about bytes.
- `tubuild verify` on the four repaired TUs, each run separately, after the edit:
  `arm9/Actor` **97/97 MATCH**, `ov062/Koopa+KoopaSmall` **39/39 MATCH**,
  `ov077/Lakitu` **32/32 MATCH**, `ov077/Spiny` **34/34 MATCH** — all four
  objisolate clean and reloc-destinations clean, all four TEXT-VERIFIED. The
  return type is therefore not load-bearing in the emitted code for any of them.
  All four also exit 1 on the same unlicensed compiler-only records (`_ZTV`/
  `_ZTI`/`_ZTS`, a `D2` variant, `_ZN7Vector3D1Ev`) that make the promotion
  audit refuse. Run at base `26f54f8fc` as a control, the same four produce
  byte-for-byte the same counts, the same unlicensed records and the same
  exit 1, so that refusal is a pre-existing baseline limit, not a regression
  from this change.
- Whole-ROM check after the edit, with `build/sm64ds.nds` deleted first so a
  failed run could not leave a stale artifact behind: `tools/rombuild.py -j 16`
  exit 0, ROM rewritten (fresh mtime), **106/106 modules exact**,
  100.000000% of compared bytes, ROM-build analysis PASS, sha256
  `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`.
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
claims reproduce; three BSS claims; 106/106 modules exact**. The first two of
those counts moved with `main` and read 11,199 and 25 on the merged tree; the
106/106 and the sha256 below are unchanged there. It actually packages
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
four DIFFERS and 397 UNNAMED — 705, 224, four and 387 on the merged tree; it
is not an all-tree metadata clearance on either.

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
