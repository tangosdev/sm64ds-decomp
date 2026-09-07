# Handoff: issue-2414

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- **Issue URL, task ID, stage, session and harness:**
  https://github.com/tangosdev/sm64ds-decomp/issues/2414 — task `issue-2414`,
  stage `reconstruct` (role `producer`), session `prod-2414b-0907`, harness
  Claude Code (Opus 5). Claimed against the exact SHA the queue offered.
- **This is a text-only continuation stage.** The preceding `reconstruct`
  attempt (session `prod-2414-0907`) produced
  `d004de771d55ddf2ff4699250a57b940d27c4f97`, which an independent verifier
  (session `vfy-2414-0907`) then confirmed substantively — including
  reproducing the langmode red on the accepted input, and a forced clean
  recompile in each declaration position that produced a byte-identical
  27,752-byte object with the same sha256, against a deliberate control that
  did change the hash. That verification stands. **This stage changes no
  source whatsoever**; it corrects and completes the prose of this document,
  which is why the byte proof carries over unchanged rather than needing to be
  re-established. Every measurement below was re-checked against the tree, not
  copied forward.
- **Source branch and previous accepted input SHA:** branch `cpp/daMip_c-v2b`,
  branched at `d004de771d55ddf2ff4699250a57b940d27c4f97` — the accepted input
  for this stage. That commit sits on `cpp/daMip_c-v2`, which in turn branched
  at `f10902c14b91ceb4618062c119c3075852292519`, the tip of the v1 branch
  `cpp/Rabbit-tu`. Nothing was rebased, reset or restarted, and no branch in
  that chain was moved; `origin/main` was deliberately **not** merged in (see
  the integrator precondition).
- **Original source base SHA and installed workflow/tool SHA:** both
  `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
- **Separate evidence commits and required artifacts in this commit:** the task
  pins no separate evidence commits. The scout facts for this class are
  committed on this branch at `notes/data/class-facts/Rabbit.json` (still under
  the retired coined name — see remaining work). The stage's required artifacts,
  `src/actors/daMip_c.cpp` and `config/tu_manifest.d/ov085/daMip_c.json`, are
  both present.
- **Next action, responsible role and blockers:** independent verification of
  this commit by a session that is none of `prod-2414-0907`, `prod-2414b-0907`,
  `vfy-2414-0907`, or the v1 producer recorded in this branch's history. Because
  this stage changed only prose, that verification is a document review against
  the tree plus whatever re-confirmation the verifier wants of the unchanged
  byte proof — the source content is bit-identical to the already-verified
  `d004de771`. No blocker inside this scope.
- **Status:** verified candidate for the checks a producer can run, with the
  source unchanged from an independently verified commit. The full-ROM build,
  the TU byte/relocation pass and the composed static gates passed at this
  content when `d004de771` was produced and were reproduced independently; the
  private validator has not been run and no PR exists yet.
- **Gates re-run at this stage, and the ones deliberately not re-run:** this
  stage edits one Markdown file under `notes/` and nothing else, so only the
  gates a prose change can affect were re-run: `check_dead_references.py` and
  `check_python_names.py`, both exit 0. The byte and link gates —
  `rombuild.py`, `tubuild.py verify`, `romdata_check.py`,
  `prepush_linkcheck.py`, `premerge_check.py` and the rest recorded under
  **Proof** — were **not** re-run here, because the source tree they measure is
  byte-identical to `d004de771`; their recorded results carry over unchanged and
  should be read as evidence about that commit's content, which is this
  commit's content. Nothing in this document should be read as a fresh full-gate
  pass at this SHA.
- **Remaining uncommitted/local-only material and where it is preserved:** none.
  Working tree clean. The private queue receipt and every build log stay in the
  worktree's gitignored `build/` and are not committed.

## What changed and why

- **Class/TU/symbol and module-qualified ROM scope:** `daMip_c` in ov085.
  `.text` `0x0212a6d4..0x0212cc88`, 32 functions, one translation unit at
  `src/actors/daMip_c.cpp`. Class data in ov085 at `0x021300bc` (typeinfo name),
  `0x021300c8` (`__si_class_type_info`) and `0x021300f8` (31-slot vtable address
  point).
- **Reserved source/header/config surfaces actually touched by *this* stage:**
  `src/actors/daMip_c.cpp` and `include/daMip_c.h`. Nothing else. The reserved
  ov085 `symbols.txt`/`delinks.txt`, the TU manifest and the retired coined
  header were already in their final state at the accepted input.
- **ROM observations:** unchanged from the accepted input and independently
  re-measured here, not re-asserted: 32 of 32 functions reproduce the cartridge
  byte for byte with correct relocation destinations, and the class's own
  vtable and typeinfo record compare equal to the ROM.
- **Lineage evidence or structural inference:** the class name is read from the
  image, not coined. Member-ness is ROM-proven for `SetState` and the sixteen
  `State*` functions, each of which is the target of an eight-byte
  pointer-to-member-function constant in `0x0213003c..0x021300bc`.
- **Hypothesized names/filenames, explicitly not recovered facts:** the file
  name `src/actors/daMip_c.cpp`; the factory spelling `daMip_c_classInit`; the
  six coined helper names `TestWaterBelow`, `UpdateGrab`,
  `UpdateMatrixAndShadow`, `UpdateCarriedMatrix`, `UpdateMirrorShadow` and
  `RenderMirrorImage`, whose member-versus-file-local form the cartridge does
  not distinguish; the field names in `include/daMip_c.h`.
- **Compiler experiments and measured barriers:** one change was made at this
  stage, and it was made because a gate that is green on the base goes red on
  the merge result, not for style. `tools/premerge_check.py` reported the
  langmode ratchet failing with `codegen_hacks.extern_vtable` 165 -> 166 and
  `extern_vtable_classes` 168 -> 169. `tools/langmode_audit.py` counts a file
  under `src/` that spells `extern ... _ZTV...;` itself, because that idiom
  normally marks a vptr stamped by hand for a class with no vtable at all.
  daMip_c is the opposite case — this TU owns the key function, the compiler
  emits the real table, and it compares equal to the cartridge — so what the
  gate caught was the declaration site. The declaration moved to
  `include/daMip_c.h` beside the class, which is what the landed sibling
  `include/Scuttlebug.h` does for the identical factory seam, and the type
  changed to `int[]` so it agrees with the spelling `include/decl_common.h`
  already carries. The counts return to 165/168 and the bytes do not move: the
  TU still verifies 32/32.
  The hand-written store itself stays, and the barrier is *which* allocator a
  natural `new` would select, not the absence of one.
  **Correction, recorded rather than quietly dropped:** an earlier draft of this
  document, and the immutable commit message of `33e458302`, both assert that a
  natural `new daMip_c()` would select "the global `operator new`, which this
  image does not contain". That is false. `_Znwj` is present at arm9
  `0x0203cbe4` in `config/arm9/symbols.txt`, and `src/_Znwj.cpp` defines it as a
  tail-call veneer to `func_0203cc0c`. The commit message cannot be edited, so
  the error stays in this branch's history; this paragraph is the correction.
  The real, measured barrier is the one `src/actors/Scuttlebug.cpp` states for
  its own factory: natural `new` selects the **wrong allocator**. The cartridge
  factory allocates through `fBase_c::operator new`, and mwccarm 2004/b56
  rejects an in-class `operator new` declaration — `include/fBase_c.h:150`
  records that restriction, and is cited correctly above — so `fBase_c` cannot
  expose that allocator under the natural spelling at all. The mangled call is
  therefore the only way to reach the allocator the ROM actually used.
  Scuttlebug retains the same seam for the same reason, but it records this
  different and correct measurement; it does **not** record the "no global
  `operator new`" claim this document previously attributed to it.

  Noted in passing, not repaired here because it is source outside this
  stage's text-only scope: the adjacent comment in `include/fBase_c.h` says the
  global `_ZdlPv` "exists nowhere in this image", while
  `config/arm9/symbols.txt` lists `_ZdlPv` at `0x0203cbf0`. The in-class
  `operator delete` requirement it argues for may still hold on other grounds,
  but that particular justification needs re-measuring by whoever next owns
  `fBase_c`.

## Reconstruction dimensions

- **Exact function/byte and relocation coverage:** 32/32 functions byte-exact,
  relocation-aware, with the emitted object's relocation types and addends
  checked as well; all 32 emit in ROM-ascending section order. Coverage is the
  whole licensed run and nothing in it is excluded or masked.
- **Genuine methods; remaining free-function/ABI bridges:** 31 of the 32 are
  real `daMip_c::` member functions. The one remaining free function is
  `daMip_c_classInit`, and that is correct rather than debt: the cartridge
  carries it unmangled as the registry factory. Inside it, seven calls stay
  spelled as mangled `extern "C"` declarations — `fBase_c::operator new`, the
  `dEnemyBase_c` base constructor and five member constructors — because the
  cartridge folded the constructor into the factory and emitted no
  `_ZN7daMip_cC1Ev`/`C2Ev` of its own.
- **Recovered layout/fields; remaining shadow structs/raw offsets:** the class
  is a real `daMip_c : dEnemyBase_c` and the five sub-objects at `0x110`,
  `0x144`, `0x300`, `0x368` and `0x3c0` are typed and close on each other; the
  `0x474` size assert is the ROM's own allocation literal.

  **This is the weakest dimension of the candidate, and a previous draft of
  this section understated it badly** by naming only seven pads, `mState`,
  `daMip_cSelf`, one `V3Blk` and "the ov085 file statics". Measured over
  `src/actors/daMip_c.cpp` at this commit, with comments stripped first, the
  actual remaining surface is:

  - **18 shadow-struct definitions naming 11 distinct types.** Sixteen are
    written as `struct NAME { ... }`: fifteen block-scope (`RG`, `V3` ×2, `G`,
    `Vector3` ×3, `V3Blk`, `Mtx43` ×4, `Vector3_local`, `Obj`, `VObj` — 9
    names) and one file-scope, `daMip_cSelf`, forward-declared, used for the
    `daMip_cStateFn` pointer-to-member typedef, then defined as
    `{ char pad[0x364]; ... }`. The remaining two are anonymous structs reached
    only through a typedef — `typedef struct { s32 x, y, z; } Vector3;` and
    `typedef volatile struct { Fix12i x, y, z; } Vec3Scratch;` — and they count,
    because the rule here is *definitions*, with no exclusion for the typedef
    spelling. Several are the *same* geometry retyped per member: four separate
    `Mtx43`, and `Vector3` defined four times over (three named plus the
    anonymous typedef), so folding them is a real consolidation, not cosmetic.

    *Counting rule, stated so the next census does not miss the same two
    things:* strip `/* */` and `//` comments first, then match the keyword
    `struct`, **an optional name**, and then `{`. Both halves of that matter.
    Making the name optional is what catches `typedef struct { ... } NAME;`,
    and allowing anything between `typedef` and `struct` is what catches
    `typedef volatile struct { ... } NAME;` — a pattern anchored on the literal
    text `typedef struct` silently drops it. Count *definitions*, not distinct
    names: the same geometry redeclared per member is exactly the debt this
    figure is meant to size, so collapsing repeats understates it. Report the
    distinct-name count beside the definition count rather than instead of it.

    *Correction, recorded rather than quietly replaced:* this section has now
    been miscounted twice, in the same direction, which is why the rule above is
    written out. An earlier draft said 15 definitions across 10 names, counting
    only the named form and only two of the three named `Vector3` definitions.
    An independent review then put the total at 17, finding the anonymous
    `Vector3` typedef but not `Vec3Scratch`, because `volatile` sits between
    `typedef` and `struct`. Applying the rule above gives 18 definitions naming
    11 distinct types, and every earlier figure was an undercount — the error
    has always understated the debt and strengthened the document's own
    consolidation argument, so treat a low number here with suspicion.
  - **63 block-scope `extern` declarations naming 26 distinct objects.** Nine
    are arm9 `data_0209xxxx`/`data_020a0e68` objects and seventeen are ov085
    `data_ov085_0213xxxx` objects. The most-repeated single object is declared
    seven times in seven different members.
  - **19 `char *` aliases of `this`** (`char *c = (char *)this;` and two
    `self`/one `p` variants), plus two further `((char*)this)` casts passed
    directly as call arguments at the `dCcAc_c::Init`/`dBgCh_Actr::Init` seam.
    Counting `<alias> + <constant>` inside the function body that declares each
    alias, those drive **252 raw-offset accesses on 220 lines over 49 distinct
    offsets**. **33 of those offsets are at or above `0x110`**, which is the
    boundary above which the layout is this class's own rather than inherited —
    and the header already names or types many of them: three of the five typed
    sub-objects (`0x144`, `0x300` and `0x368`; `0x110` and `0x3c0` are typed in
    the header but are *not* among the 33, so no raw access to them survives),
    and the named fields `0x426`, `0x427`, `0x429`, `0x42a`, `0x438`, `0x43c`,
    `0x440`, `0x444`, `0x448` and `0x45c`. Every one of those ten is an access
    that could be spelled as the member it already is. `0x424` is in the 33 too
    but does not belong on that list: the header declares it `u8 pad_424[0x2]`,
    a padding span rather than a member, so recovering it means naming the field
    first, not just respelling the access.

    *Method, so this is reproducible rather than asserted:* strip `/* */` and
    `//` comments; match `char *NAME = (char *)this`; split the file into
    top-level brace regions and, within each region declaring such an alias,
    count `NAME` followed by `+` and an integer literal. A narrower count that
    only accepts a dereference form, or that excludes aliases passed on as
    arguments, lands lower — an independent measurement of the same file
    reported 18 aliases, 187 accesses, 165 lines, 39 offsets and 28 at or above
    `0x110`. Both measurements support the same conclusion; treat the shape,
    not the exact integer, as the finding.

  Individually still unrecovered, and named so the next owner can plan:
  seven padding spans (`pad_390`, `pad_3e8`, `pad_424`, `pad_42b`, `pad_44c`,
  `pad_460`, `pad_46c`); `mState` at `0x364`, which is really a pointer to a
  state record but is kept an opaque `s32` because `Behavior` compares it by
  address against four ov085 objects; **`mTalkingPlayer` at `0x45c`, which the
  header's own comment documents as "A Player *" while declaring it `s32`, and
  which the body then round-trips through `*(void **)&mTalkingPlayer` at three
  sites** — it was missing from the previous list entirely and is the cheapest
  correct field recovery left in the class; the `daMip_cSelf` shadow struct the
  pointer-to-member call goes through; the `V3Blk { s32 w[3]; }` block in
  `StateFleeMain`, which is load-bearing — the scalar spelling costs eight
  bytes; and the ov085 file statics, which stay block-scope inside the member
  that recovered each one, because the legacy shards disagreed about their
  types and canonicalising them is a measured codegen hazard rather than a
  tidy-up.

  None of this weakens the byte proof, which is exact and independently
  reproduced. It is the gap between "the bytes are right" and "the class is
  reconstructed". Anyone sizing this class's remaining work from the previous
  wording would have underestimated it by a wide margin; "promoted" is a
  packaging state, not a claim that reconstruction is complete.
- **Lifecycle, vtable/RTTI, initializer and data ownership:** `~daMip_c` is
  declared first among the virtuals and defined out of line, so it is the key
  function and this TU emits `_ZTV7daMip_c`, `_ZTI7daMip_c` and `_ZTS7daMip_c`.
  `tools/romdata_check.py` reports 11 emitted data records for this source: 6
  VERIFIED (180 bytes equal to the cartridge, including this class's own vtable
  and typeinfo record and the four ancestor typeinfo records), 5 PARTIAL, and
  **0 DIFFERS**. The PARTIAL rows are typeinfo-name strings whose emitted
  coverage is shorter than the ROM extent the config names; they are not
  disagreements. Two compiler-only symbols deadstrip exactly and are recorded in
  the manifest: `_ZN7daMip_cD2Ev` (the cartridge runs D1 straight into D0 with
  no third variant) and `_ZN7Vector3D1Ev`. The TU claims `.text` only; it owns
  no `.data` or `.bss`.

  **Coverage limit on the vtable evidence, stated rather than left implicit:**
  because this TU claims `.text` only, `ov085/daMip_c` is not an intact TU —
  the same condition that makes `tubuild.py linkcheck` refuse it below. It is
  therefore absent from the build report's `intactTus` list, objisolate
  discards the vtable and RTTI this TU emits, and the ROM gap supplies those
  bytes at link time instead. The consequence is that `tools/romdata_check.py`
  is the **only** evidence for this class's vtable. The full-ROM build does not
  independently corroborate it, because the vtable bytes it linked came from
  the cartridge gap rather than from this source. Read the vtable claim as one
  gate's verdict, not as two agreeing ones.
- **Attribution preserved through each move/rename:** every one of the 32
  functions carries its originating shard path in the manifest's per-function
  `legacy_source` field, and each member carries an `// @symbol` marker. The
  ledger rows recording the retired coined spelling were written at the accepted
  input; `tools/check_rename_ledger.py` passes over 1932 mangled/vtable rows.
- **Remaining agreed issue scope:** the padding spans and `mState`'s real type
  are the next concrete reconstruction improvements and belong on the issue,
  not to this stage.

## Proof

Every command below was run in this worktree at exactly this source content,
against base `origin/main` `88dbe66db2cb3f0cd1dc704e9f2775eb37aea646`.

- **Full-ROM build:** `python tools/rombuild.py -j16 --no-rom` — exit 0. 11,191
  of 11,191 source-built functions reproducing, 0 mismatching; module fidelity
  106/106 exact at 100.000000% of compared bytes; intact-TU gates report dsd
  modules PASS, zero new symbol errors and exact storage aliases. `--no-rom`
  means the linked modules were compared but no `.nds` image was assembled, so
  no ROM checksum is claimed here. The run's own baseline control reports a
  pre-existing `dsd check symbols --fail` failure with no TU substituted, which
  belongs to the tree and not to this change.
- **Explicit function/consumer relocation checks:**
  `python tools/prepush_linkcheck.py --range origin/main..HEAD` — exit 0. The
  three changed headers fan out to 932 sources; 929 checked, 632 verified, 297
  warnings, **0 blocking**, 3 NONMATCHING drafts skipped. **Coverage limit,
  named:** this tool derives one symbol from each file's stem, so a promoted
  multi-symbol TU cannot be checked by it — `daMip_c` appears in that run as
  `NO-SYM`. The relocation destinations of the 32 functions are covered instead
  by the objisolate pass below and by the full-ROM link above, not by this tool.
- **Complete emitted TU and data/metadata checks:**
  `python tools/tubuild.py verify ov085/daMip_c` — exit 0: 32/32 MATCH,
  objisolate clean (relocation type and addend), reloc-destinations clean,
  emission order all 32 in ROM-ascending section order, compiler-only symbols an
  exact deadstrip — TEXT-VERIFIED.
  `python tools/romdata_check.py --files src/actors/daMip_c.cpp` — exit 0: 6
  VERIFIED / 5 PARTIAL / 0 DIFFERS / 0 UNNAMED, as detailed above.
  **Not available, and why:** `python tools/tubuild.py linkcheck ov085/daMip_c`
  exits 1 and does not run — `intact TU preparation refused: ov085/daMip_c:
  intact production requires one .text claim and at least one non-text claim`.
  This TU claims `.text` only. That refusal was recorded at the accepted input
  and is reproduced here rather than taken on trust.
- **Shared-header consumer expansion:**
  `python tools/affected_src.py include/daMip_c.h` reports exactly one consumer,
  `src/actors/daMip_c.cpp`; the only other file naming that header names it in
  prose. The header edit is therefore fully covered by the TU verification and
  by the full-ROM build, and the 932-source fan-out above is the wider expansion
  the branch's other header changes carry.
- **Port/path/reference and other applicable static gates:**
  `python tools/premerge_check.py HEAD --base origin/main` — exit 0, and this is
  the check that matters most here because it evaluates the **merge result**:
  converted-ratchet, dead-references, duplicate-sources, header-offsets,
  langmode-ratchet, layout-check, src-tu-refs and source-coverage are all `ok`,
  nothing goes green to red. (On the accepted input the same command exited 1
  with langmode-ratchet as a REGRESSION; that is what this stage fixed.)
  Also run, each exit 0: `python tools/port_refcheck.py` (402 references, all
  resolve), `python tools/check_rename_ledger.py`,
  `python tools/check_tubuild_conflicts.py` (154 manifest entries; every
  recorded conflict matches its source and vice versa),
  `python tools/check_profile_campaign.py` (786 live claims, 0 diverge),
  `python tools/check_python_names.py`, `python tools/check_dead_references.py`
  (no new dead references, no broken links) and
  `python tools/cpp_tu_state.py --check-note`.
- **Private validation, if run, and the exact PR head/base it tested:** not run.
  No PR is open for this branch. Nothing above should be read as a validator
  pass, and the terminal private validation the issue requires for acceptance
  has not happened.

### Integrator precondition: compose forward before validating

**Read this before running the private validation. Skipping it produces a false
failure that looks exactly like a real ROM-data regression.**

This class is a `#2425` case. On `origin/main` the class is still the coined
`Rabbit`, spread across per-function shards, and the ov085 `symbols.txt` names
its vtable `_ZTV6Rabbit` at `0x021300f8`. On main, the `Rabbit.h` header
declares `virtual ~Rabbit()` first among the virtuals (line 100 there), so the
`_ZN6RabbitD1Ev` shard is the key function and emits that vtable. Both of those
files exist only on `origin/main` — this branch is what retires them, folding
the shards into one TU and the header into `include/daMip_c.h` — so they are
named here without repo-rooted paths on purpose: those paths are dead in *this*
tree by design, and spelling them out would trip the dead-reference gate over
the very rename the branch exists to make. On this
branch the same address `0x021300f8` carries `_ZTV7daMip_c`. Nothing about the
cartridge data changed — only the source-side name — so a name-keyed
comparison reads the whole vtable as a **loss** that never happened.

`origin/main` already fixes this: commit `1c93d2663` ("Anchor the ROM-data diff
on the cartridge address, not the symbol name (#2425)") makes
`validate_merge._data_anchor` key on `module`, `addr` and `bytes`. But that
function returns `None` for any report row missing those keys, and falls back to
name comparison when it does — **and this branch's own
`tools/romdata_check.py:361-364` still emits `{"module", "symbol"}` only.** It
predates the fix. So running the validation with *this branch's* tools produces
anchorless rows and reproduces the phantom loss even when `validate_merge` on
main is new enough. Both halves have to be forward of the fix, not just one.

Therefore, for the integrator:

1. **Merge `origin/main` at or beyond `1c93d2663` into the composition before
   running the private validation.** Do not validate this branch standing alone.
2. **Regenerate the base ROM report; do not reuse a cached one.** A report
   produced by the older `romdata_check` carries no `addr`/`bytes` keys, so
   feeding it to the new `validate_merge` silently re-enters the name-only path
   and re-creates the same false failure.

Composing forward is also why this stage did **not** merge `origin/main` into
the branch itself: the byte proof recorded above — and the independent
verification of it — is pinned to exactly this content, and re-cutting the
commit to absorb main would discard that proof for no gain. Compose at
integration time instead.

### Left to the integration lane, deliberately

Neither is a defect in this candidate and neither is reconciled here, because
both are generated bookkeeping shared by every branch in this wave and neither
is reserved to this task.

1. `python tools/queue_audit.py --check` exits 1 on one row —
   `total_lines` for the daMip_c row, 1890 against a tree that now holds 1891,
   the net one line this stage's comment added. Regenerating it rewrites a
   shared generated TSV outside this task's reservation, so it was left alone.
   No workflow under `.github/workflows/` runs this gate.
2. The branch already carried edits to `include/decl_common.h`,
   `symbols/actor_renames.tsv`, `config/converted-baseline.json` and
   `config/converted-backslide-exceptions.jsonl` at the accepted input. This
   stage did not touch any of them.

Separately, `notes/data/class-facts/Rabbit.json` still carries the retired
coined name in its path. Renaming it needs a reservation this task does not
hold; it is recorded here so it is not lost.

A log generated after this commit belongs in separately recorded evidence.
