# Handoff: minigame-jump3d-readable-0922

This document describes this commit. The queue records its immutable output SHA.

## Identity and scope

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/2722; scope comment:
  https://github.com/tangosdev/sm64ds-decomp/issues/2722#issuecomment-5772369820.
- Producer: `codex-jump3d-readable-0922`, Codex; stage `produce`.
- Branch: `cpp/minigame-jump3d-readable-0922`; wired worktree:
  `C:/tmp/sm64ds-mg-jump3dread0922`.
- Accepted input, tested source base and workflow tools:
  `d033c646262e878ce234bafd37d6b5b3abf40179`.
- Active independent-review policy observed at claim:
  `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- Owned ROM interval: ov006 `[0x020c762c,0x020c8a30)`, all 28 existing functions,
  5,124 bytes. Changed surfaces are only `src/actors/dMgJump3DMario_c.cpp` and this
  handoff. No header, manifest, symbol, delink, attribution or ledger edit.
- Status: producer-verified candidate; independent verification and integration remain separate.
  Private producer logs, baseline object and receipts remain under ignored
  `build/jump3d-readable/`; receipts are credentials and must not be published.
- Next role: independently verify this exact candidate against the recorded base,
  carry the partial reconstruction scope below, then offer integration. Do not
  infer current-main composition acceptance from this producer proof.

## Source changes and contract evidence

The existing promoted production TU remains the build input. This follow-up adds
no shadow source and makes no new TU-boundary or class-layout claim.

- **JUMP-DECL-01 fixed:** use native `ApproachLinear(int&,int,int)` returning int,
  `ApproachLinear2(short&,short,short)` returning int, and
  `UpdateAngle(short&,short,int,short)` returning void. Their actual production
  definitions are `src/_Z14ApproachLinearRiii.cpp`,
  `src/_Z15ApproachLinear2Rsss.cpp` and `src/_Z11UpdateAngleRssis.cpp`. The facing
  target/max-step constants fit short; parameter evaluation order is preserved.
- **JUMP-DECL-02 fixed:** the scalar `Particle::System::NewSimple` ABI bridge now
  returns `void*`, matching
  `src/_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_.c`. Its ignored result and
  coordinate shifts are preserved; no speculative Particle method was introduced.
- **JUMP-DECL-03 fixed within local scope:** the scalar SetAnim bridge now takes
  `ModelAnim*`, `BCA_File*`, `int`, `int`, `u16`, matching the existing C definition
  in `src/_ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj.c`. The shared native header
  still uses u32 for the final parameter and Fix12 by value for speed. That
  disagreement is recorded, not represented as recovered original truth or fixed
  globally. Every final argument in this TU is zero.
- **JUMP-SOUND-01 fixed:** include `Sound.h` and call `Sound::PlayBank2_2D`; its u32
  return agrees with `src/_ZN5Sound12PlayBank2_2DEj.cpp`. No duplicate local Sound
  declaration remains.
- **JUMP-READ-01 fixed:** name projected screen coordinates, shadow phase/scales,
  touch X/Y and distances, particle coordinates, hold direction, respawn random
  samples/speed scale, and the material comparison result. These are descriptions
  inferred from calculations, not recovered identifier spellings. The two random
  calls, repeated shadow-phase read, integer widths, fixed-point rounding,
  volatile store and comparison temporary remain in their original order.
- **JUMP-PROSE-01 fixed:** the historical matrix/flag tests changed eleven/five
  words at unchanged sizes; they did not add that many words. Source comments now
  state the measured facts and avoid an unproved spill/relocation explanation.

## Preserved constraints and remaining reconstruction

Completion is **partial**, under issue #2722. This is a first source-quality pass,
not completion of the class or its wider original TU.

- **JUMP-ABI-01 deferred:** retain two arguments to `func_02016a14` and
  `func_02016a04`, including 0x7fff and 0x210. Their one-argument C wrappers omit
  the forwarded r1 contract. The next calls, `func_02045f80` and `func_02045ef8`,
  accept and forward a second value; `func_02045fd4` writes that value to material
  flag bits 0..14, and `func_02045f4c` shifts it into bits 16..31. Blindly matching
  the incomplete wrapper declarations would discard intentional material values.
  Reserve that definition/caller chain separately before repairing it.
- Eight C entry bridges and twelve calls in seven neighboring shards remain;
  the possible wider original TU boundary is unresolved. This pass preserves the
  15 native state/entry methods, three native virtual accessors and native C1/D1.
  It converts helper call syntax, not those eight entry points.
- Keep `common.h` first, the incomplete-class Jump3DState PMF typedef, plain
  Jump3DVec members/scratch, existing member layout, PMF records and lifecycle
  ownership. Address-derived global/virtual/bridge names and local helper ABI
  views remain. Existing Sound_PlayBank1Panned and sound-thunk definition debt is
  cross-owner work; this pass does not claim their interfaces are reconstructed.
- The manifest's ten compiler-only dispositions are unchanged: six metadata
  objects plus four extra constructor/destructor functions. Production routing
  and gap ownership remain unchanged. No symbol or path move changes credit.

The following are **historical** 2004/b56 experiments, mostly pinned to
`2546f0ae4` in `notes/experiments/`; they were read and preserved, not rerun as new
negative experiments by this producer:

| Existing experiment | Actual measured limit |
| --- | --- |
| `jump3d-2711-mtx-intptr-zeroing.md` | Named zeroing: 11 differing words, same 0x12c size |
| `jump3d-2711-statehold-flag-reuse.md` | Direct touch local: 5 differing words, same 0x174 size |
| `jump3d-2711-common-first-matrix.md` | Swapping includes: copy grows 0x54 to 0x78 |
| `jump3d-2711-setanim-member-form.md` | Tested native Fix12 call: EnterDamp grows 0x88 to 0x94 |
| `jump3d-2711-statemove-volatile-store.md` | Plain zero store: StateMove shrinks 0x3c4 to 0x3a8; no relocation-count measurement |
| `jump3d-2711-87d0-int-temp.md` | Folded comparison: loader shrinks 0x16c to 0x160 |
| `jump3d-2711-scratch-vector3.md`, `jump3d-2711-members-vector3.md` | Matching function bodies can still add an unlicensed four-byte Vector3 destructor |

EnterHit's plain PMF assignment already landed before this task; it is not a new
improvement here. These negative results concern the recorded substitutions,
not every possible future spelling. Parent/root's independent preliminary review
of the complete source/header/diff and changed callee contracts found no introduced
blocker; formal exact-commit acceptance remains the verifier's responsibility.

## Producer proof

Pinned compiler: mwccarm `2004/b56`, production flags from `build_pin.flags_for`.
Logs named below are private, relative to `build/jump3d-readable/`.

- Compile baseline and candidate with `match.compile_c` using production flags:
  complete raw ELF identical, **18,896 bytes**, SHA-256
  `852cfffac8ebb9a97cbd80517c9f55f60c808839df72a4fe923dc2792cd3b56a`.
  `baseline.json`, `object-comparison.json`, `before.o`, `candidate.o`.
  This includes all sections, symbol/relocation tables and compiler passengers.
- `python tools/tubuild.py verify ov006/dMgJump3DMario_c`: exit 0,
  **28/28 MATCH**, expected emission order, clean objisolate and relocation
  destinations; `tu-verify.log`.
- For every manifest function, require a successful `objisolate.plan`, apply
  `reloc_audit._as_the_build_links_it`, then call `linkcheck.linkcheck` with exact
  manifest address/size/module: **28/28 VERIFIED**, 5,124 bytes,
  `diffs: []`, `blind: 0`; `strict.json`.
- Raw ELF inventory: **32 nonlocal functions** = 28 licensed plus four existing
  compiler-only functions, and six data objects. No unexpected nonlocal symbol;
  all 28 `@symbol` markers unchanged and equal the manifest function set;
  `object-inventory.json`.
- `python tools/romdata_check.py --files src/actors/dMgJump3DMario_c.cpp --json
  build/jump3d-readable/romdata.json`: exit 0, four VERIFIED / two PARTIAL / zero
  DIFFERS, no blind words. Standard comparison verifies 44 full and 40 partial
  bytes. RTTI names emit 19/25 bytes into estimated extents 20/28; the standard
  word loop compares 16/24. `romdata.log`, `metadata-records.json`.
- Independent complete emitted-storage comparison using the gate's relocation
  resolution also checks vtable preambles and string tails: all **104 emitted
  metadata bytes** match retail, zero blind words. This does not claim the
  neighboring alignment padding; `complete-metadata.json`.
- `python tools/check_decl_agreement.py --changed
  d033c646262e878ce234bafd37d6b5b3abf40179`: no new declaration disagreements;
  existing scoped debt remains. `declarations.log`.
- `python tools/check_tubuild_conflicts.py ov006/dMgJump3DMario_c`: exit 0,
  no conflict note/marker inconsistency. `python tools/port_refcheck.py`: exit 0,
  all 408 references resolve. `git diff --check`: clean.
- Shared-header consumer expansion: no shared header changed. Full ROM and the
  exact committed-range gate cover this source and its current build route.
- `python tools/rombuild.py -j16 --report-json build/jump3d-readable/rombuild.json
  --data-json build/jump3d-readable/romdata-all.json`: exit 0. All **11,208/11,208**
  source-built functions reproduce; **106/106** modules exact; **26/26** owned
  data claims and three BSS claims (172 bytes) pass. Actual stock ROM packaged at
  `build/sm64ds.nds`. Intact-TU gates pass modules, zero new symbol errors and
  storage aliases. Logs: `rombuild.log`, `rombuild-command.json`, `rombuild.json`.
  Its separate no-substitution baseline retains the tree's existing dsd symbol
  errors; no new symbol errors were introduced. The all-source metadata census
  remains 773 verified, 272 partial, three differing, 274 unnamed; those inherited
  corpus counts are not claimed as fully reconstructed metadata. This TU has
  zero differing/unnamed records as separately measured above.
- Committed-range relocation and attribution gates: run after this commit and
  retain their exact output SHA in the producer evidence instead of amending
  a proved commit to embed its own SHA.
- Private validator, PR CI, independent verifier and current-main integration:
  not claimed by this local producer handoff.
