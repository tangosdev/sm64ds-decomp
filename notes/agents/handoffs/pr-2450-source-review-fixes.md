# Handoff: pr-2450-source-review-fixes

This is a continuation of the existing Pkn promotion, not a new class campaign.
The queue records this commit's immutable output SHA.

## PKN-06 correction, 2026-09-10

Producer `codex-pkn-review-fix-0910` resumes the same task and `fix` stage from
`24260cc326e3124f37772e4c89256e9c40efdc60`, on branch `review/pr2450-0910` in
`C:/tmp/sm64ds-r2450-0910`. The active source-review policy is pinned to workflow
`18af52b49b9fb5663d417bdfa86109b7b157aaa3`; queue commands use the current
coordinator checkout. The original source/base/workflow pins below remain
historical records. The queue records this correction's output.

The previous independent review rejected PKN-06 because the active manifest
still said the vtable ran to the end of ov084 `.data`. Its ownership explanation
now agrees with the source and dossier: the address point is `0x02130c28`, its
31 four-byte slots occupy `0x7c` bytes and end at `0x02130ca4`, and seven zero
words follow before the section ends at `0x02130cc0`. Storage including the
eight-byte header remains `0x84`. No ownership, address, size or isolation policy
changes.

The endpoint was reread from `extracted/overlays/overlay_0084.bin`, using module
base `0x02129020`: `0x02130c28 + 31 * 4 == 0x02130ca4`, and unpacking the seven
32-bit words from that endpoint to `0x02130cc0` returns seven zeros. This confirms
the padding; it does not derive the slot count from the section boundary.

All prior findings remain recorded:

| Finding | Current disposition |
| --- | --- |
| PKN-01 | Fixed in the preserved input: state 8 returns void. |
| PKN-02 | Fixed in the preserved input: state-1 PMF provenance. |
| PKN-03 | Fixed in the preserved input: scoped declaration and ABI claims. |
| PKN-04 | Fixed in the preserved input: real class PMF receiver. |
| PKN-05 | Previously accepted as deferred, partial reconstruction; continue under [issue #2473](https://github.com/tangosdev/sm64ds-decomp/issues/2473), owned by the humanizer integrator through @andrewboudreau. |
| PKN-06 | Manifest endpoint explanation corrected here; fresh independent review required. |

Only this handoff and one manifest explanation changed. Source, headers,
configuration addresses, the parsed manifest policy and contributor attribution
are identical to the accepted repair input. Earlier compile, relocation and ROM
measurements below describe that input and are retained as historical evidence;
no fresh whole-ROM build or source acceptance is claimed for this prose repair.
The independent verifier must review the correction and retained findings before
publication. A later composition against current main requires its own review
and validation.

## Preserved input identity and measurements

The remaining sections describe `24260cc326e3124f37772e4c89256e9c40efdc60` and
its earlier source input. They preserve the original experiments and handoff;
the durable continuation issue and current PKN-06 disposition are above.

### Identity and resumption

- Task: `pr-2450-source-review-fixes`, producer stage `fix`.
- Durable thread: [PR #2450](https://github.com/tangosdev/sm64ds-decomp/pull/2450),
  temporarily used until a class issue is available.
- Producer: `codex-pkn-source-review-fixes-20260908-01`, Codex.
- Coordinator: `codex-humanizer-integrator-20260908`; predecessor task
  `pkn-ov084-0907`, prior integration session `int-pkn-0907`.
- Branch: `cpp/pkn-source-review-fixes-0908`.
- Accepted source input: `ffd6d8dd436827d06fcb79e1a0cf5171eb7ee231`.
- Source base: `b0d348e536fb3304b6ab12d88fe95d780cbeb91d`.
- Workflow pin: `f327f7b6460e157153eb7fc0749dbbe60dd854f1`.
  Compiler/source tools are the input commit's unchanged tracked tools; the
  compiler is `2004/b56`.
- Status: locally verified partial reconstruction candidate. Independent source
  acceptance and verification remain pending. No normal branch push, PR update,
  private validation of this correction, or merge is included in this handoff.
- Next role: an independent verifier reviews the complete candidate and every
  finding below, repeats the source-backed byte gates, and explicitly decides
  whether the remaining reconstruction scope is acceptable.

## Changes and finding dispositions

- **PKN-01 corrected:** state 8 now returns `void`. The only configured incoming
  reference to ov084:0x0212f298 is the PMF record at ov084:0x02130be4, containing
  that function pointer and zero adjustment. `__sinit_ov084_02130654` installs it
  at table index 8; `Behavior` discards the state call's result. Its former C
  shard had `int` and no return; folding it into C++ made that fallthrough invalid.
  The corrected 0x44-byte body has no fabricated result.
- **PKN-02 corrected:** state 1's old `daFPkn_c::Kill` / vtable attribution was
  false. The existing dossier and PMF record ov084:0x02130ba4 identify
  ov084:0x0212fa7c as Pkn state 1. Its original identifier remains unknown.
- **PKN-03 corrected:** separate `extern "C"` blocks do not create independent
  declaration scopes. The broad caller-ABI assertion has been replaced with the
  actual compiler alternatives and limits below. Unused declarations for the
  five entry points now reached through real methods were removed.
- **PKN-04 corrected within this scope:** `Behavior` uses
  `void (daPkn_c::*)()` directly; the invented `PknStateCls::dummy` type is gone.
  This does not rename the nine PMF target symbols or claim to finish their
  method conversion.
- **PKN-05 improved, remaining scope requires reviewer disposition:** nine calls
  in `Behavior` use existing class methods; `mFlags` is accessed directly; state
  7 and state 8 use the real actor fields. These remain C-linkage bridges to
  existing address identities, not claims of original free-function declarations.
  The small unsigned timer cast in state 8 preserves the ROM's `ldrh`; the shared
  field is signed, and its uncast form emits `ldrsh`.
- **Additional provenance correction:** the vtable ends at ov084:0x02130ca4,
  followed by seven zero words to the `.data` end at 0x02130cc0. The old claim
  that the section boundary alone proved 31 slots was false. The existing
  dossier already records the base/peer slot-count and inherited-final-slot
  evidence. The original handoff now points to this correction while preserving
  its earlier proof's commit scope.

The owned code span remains ov084:0x0212eaf0..0x02130174: 24 functions, 5,764
bytes. No symbol identities, source paths, delink ranges, PMF records, or data
ownership changed. The class header is text-identical to the input. Attribution
is semantically identical, including all 24 promoted path#symbol overrides.

## Reconstruction limits

Six ordinary method definitions and the compiler-generated destructor pair are
retained. All fifteen address-named helpers remain; two now use typed actor
fields. `PknVec3`, `PknMtx43`, the two-word `PknSharedFile` view, remaining raw
helpers, and the manual factory remain partial reconstruction. The nine PMF
handlers need a coordinated future method/symbol migration if renamed; their
config and data surfaces were not reserved here.

The bounded next producer slice is to examine the remaining helper/factory
interfaces and existing fixed-point declarations without manufacturing symbol
aliases. Keep it tracked on PR #2450 or the eventual class issue. This producer
does not approve its own broader deferral or claim source acceptance.

Production remains text-only. The compiler's vtable/RTTI copies are discarded;
their canonical data stays ROM-supplied. Partial metadata comparisons below are
not complete source-owned-data proof.

## Proof

All commands used this candidate's source/header/config blobs and the input
commit's unchanged tools. The new handoff does not affect compilation. Logs and
structured details remain in ignored `build/pkn-proof/`; later exact-commit
verification belongs in separately recorded evidence, not a self-SHA amendment.

| Check | Result | Local evidence |
| --- | --- | --- |
| Worktree compiler canary | `(True, 2004/b56)` | setup output |
| `python tools/tubuild.py verify ov084/daPkn_c` | exit 0; 24/24 MATCH; clean objisolate, destinations, and ROM order | `final.tu.log` |
| Exact-symbol, single-object linked check described below | 24/24 VERIFIED; exact names/sizes; zero differences and blind slots | `final.strict.json` |
| `python tools/affected_src.py include/daPkn_c.h` | exit 0; sole consumer is this complete TU; header unchanged | `final.affected.log` |
| `python tools/prepush_linkcheck.py --files src/actors/daPkn_c.cpp include/daPkn_c.h` | exit 0 but two basename NO-SYM warnings; **not counted as verification** | `final.prepush-files.log/json` |
| `python tools/romdata_check.py --files src/actors/daPkn_c.cpp --json build/pkn-proof/final.romdata.json` | exit 0; 5 VERIFIED / 6 PARTIAL / 0 DIFFERS / 0 UNNAMED | `final.romdata.log/json` |
| `python tools/port_refcheck.py` | exit 0; 423 references, no stale references | `final.port.log` |
| `python tools/rombuild.py -j16 --no-rom --report-json build/pkn-proof/final.rom.json --data-json build/pkn-proof/final.data.json` | exit 0; 106/106 modules exact; production analysis PASS | `final.rom.log/json` |

The unmodified input was first checked with the same TU and full production
commands (`baseline.tu.log`, `baseline.rom.log/json`): both exit 0, 24/24 MATCH and
106/106 exact. Baseline and final emitted-data records are equal: overall
705 VERIFIED / 224 PARTIAL / 4 DIFFERS / 387 UNNAMED. The four inherited
differences are neither hidden nor attributed to this repair. The final build
reused 8,943 cached objects and compiled the changed TU once.

For the stricter check, compile the complete TU once with
`match.compile_c(Path("src/actors/daPkn_c.cpp"), "2004/b56", swarm.CPP_FLAGS)`.
For each exact manifest symbol, apply existing
`reloc_audit._as_the_build_links_it(raw, symbol)` and call
`linkcheck.linkcheck(symbol, address, size, "ov084", name_index,
obj=isolated, sym=symbol)`. Require `VERIFIED`, empty differences, zero blind
slots, and exact compiled size. This avoids the stock CLI's compiler sweep and
any-symbol fallback. The complete 5,764-byte source range and the unchanged
72-byte PMF band were additionally compared directly from the final production
module to retail; both are exact. The vtable/padding band is also unchanged.

## Compiler experiments

Each trial was applied to the preceding accepted source and kept only after a
strict linked-byte pass. The final whole-TU proof separately fixes both compiler
version and requested symbol. A rejected trial was restored immediately; its
source and patch were retained in `build/pkn-proof/probes/`. The relevant minimal
alternatives are included below so the constraints can be tested again from
committed source. A failed convenient spelling is not proof that every possible
genuine C++ spelling fails.

| Trial | Alternative | Measured result |
| --- | --- | --- |
| p01 | state 8 returns void | VERIFIED; retained |
| p02 | real daPkn_c PMF receiver | VERIFIED; retained |
| p03 | nine Behavior calls use existing methods | VERIFIED; retained |
| p04 | state 8 uses fields with a signed timer read | one differing word: `ldrh` becomes `ldrsh` at +0x0c; rejected |
| p05 | direct flags and ordinary timer increment | one differing word: `ldrh` becomes `ldrsh` at Behavior +0xbc; rejected as a group |
| p06 | SetAnim with a function-entry Fix12 value | InitResources 604 -> 624 bytes; rejected |
| p07 | both dCcAc Init calls with Fix12 values | 604 -> 672 bytes; rejected |
| p08 | dCcAcPos Init with Fix12 values | 604 -> 644 bytes; rejected |
| p09 | current scalar dBgCh_Actr::Init declaration | 604 bytes, BLIND-1 on `_ZN10dBgCh_Actr4InitEP8dActor_ciiP10Vector3_16S3_`; rejected |
| p10 | `mHomePos = mSpawnPos` | 604 -> 600 bytes; rejected |
| p11 | state 7 uses actor fields | VERIFIED; retained |
| p12 | state 8 fields with explicit unsigned timer conversion | VERIFIED; retained |
| p13 | direct flags independently of timer | VERIFIED; retained |
| p14 | `mStateTimer = (u16)mStateTimer + 1` | 360 bytes, four differing words; rejected |
| p15 | SetAnim with the Fix12 value declared at the call | 604 -> 612 bytes; rejected |

The p06/p07/p08/p15 measurements compile the saved candidates explicitly with
`2004/b56` and `swarm.CPP_FLAGS`. Newly emitted literal data names in those
objects are not declared as original symbols or added to config.

### Reproducible alternatives for the retained seams

These patches are local compiler experiments, not changes to apply to production
without repeating the exact gates. Function context is retained; surrounding
comments may have changed in this continuation.

#### p06-real-init-setanim

Candidate source SHA256: `ee6888954297d06561fc850a52732bd6856db5cc5609b13fd7c0cb59013a4f99`.

```diff
--- a/src/actors/daPkn_c.cpp
+++ b/src/actors/daPkn_c.cpp
@@ -281,6 +281,7 @@
  */
 int daPkn_c::InitResources()
 {
+    Fix12<int> animSpeed = { 0x1000 };
     int i;
     Vector3 v;
     for (i = 0; i < 6; i++)
@@ -336,7 +337,7 @@
     unk_474 = 0;
     mParticleHandle = unk_474;
     unk_478 = 0;
-    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130df4.file, 0, 0x1000, 0);
+    mModelAnim.SetAnim((BCA_File *)data_ov084_02130df4.file, 0, animSpeed, 0);
     return 1;
 }
 
```

#### p07-real-init-dcc

Candidate source SHA256: `8b07052c9c8154b2e65e98bc9991c7b9c8b4033a89a177792a852980f67a3d5b`.

```diff
--- a/src/actors/daPkn_c.cpp
+++ b/src/actors/daPkn_c.cpp
@@ -281,6 +281,9 @@
  */
 int daPkn_c::InitResources()
 {
+    Fix12<int> smallRadius = { 0x32000 };
+    Fix12<int> largeRadius = { 0x82000 };
+    Fix12<int> collisionHeight = { 0x64000 };
     int i;
     Vector3 v;
     for (i = 0; i < 6; i++)
@@ -292,8 +295,8 @@
         return 0;
     if (mModel.SetFile((BMD_File *)data_ov002_0210da38.file, 1, -1) == 0)
         return 0;
-    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c1, this, 0x32000, 0x64000, 0x200004, 0x423e0);
-    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c2, this, 0x82000, 0x64000, 2, 0x423e0);
+    mdCcAc_c1.Init(this, smallRadius, collisionHeight, 0x200004, 0x423e0);
+    mdCcAc_c2.Init(this, largeRadius, collisionHeight, 2, 0x423e0);
     v.x = 0; v.y = 0; v.z = 0;
     _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0x46000, 0x64000, 0x200002, 0);
     mScaleX = 0x1000;
```

#### p08-real-init-ccpos

Candidate source SHA256: `efdedb6e717dcceb130699aac161f5c3f05a6ca8c9db77fd359107ecb0607cdd`.

```diff
--- a/src/actors/daPkn_c.cpp
+++ b/src/actors/daPkn_c.cpp
@@ -281,6 +281,8 @@
  */
 int daPkn_c::InitResources()
 {
+    Fix12<int> offsetRadius = { 0x46000 };
+    Fix12<int> offsetHeight = { 0x64000 };
     int i;
     Vector3 v;
     for (i = 0; i < 6; i++)
@@ -295,7 +297,7 @@
     _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c1, this, 0x32000, 0x64000, 0x200004, 0x423e0);
     _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c2, this, 0x82000, 0x64000, 2, 0x423e0);
     v.x = 0; v.y = 0; v.z = 0;
-    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &v, 0x46000, 0x64000, 0x200002, 0);
+    mdCcAcPos_c.Init(this, v, offsetRadius, offsetHeight, 0x200002, 0);
     mScaleX = 0x1000;
     mScaleY = 0x1000;
     mScaleZ = 0x1000;
```

#### p09-real-init-mesh

Candidate source SHA256: `5245412646d895f58edd11872c68ccf71068e35d6425058de1aacc8f7a18e80c`.

```diff
--- a/src/actors/daPkn_c.cpp
+++ b/src/actors/daPkn_c.cpp
@@ -308,7 +308,7 @@
     mStateTimer = 0;
     unk_108 = 3;
     unk_46c = 0;
-    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x64000, 0x64000, 0, 0);
+    mWithMeshClsn.Init(this, 0x64000, 0x64000, 0, 0);
     mSpawnPos.x = mPosX;
     mSpawnPos.y = mPosY;
     mSpawnPos.z = mPosZ;
```

#### p10-init-home-copy

Candidate source SHA256: `6bd5e7d25e78609bfda574071f0e438d6ec8724c5bf592772b91f981e18ba555`.

```diff
--- a/src/actors/daPkn_c.cpp
+++ b/src/actors/daPkn_c.cpp
@@ -316,8 +316,7 @@
         s16 *tbl = data_02082214;
         /* The add sits INSIDE the integer cast, which is load-bearing here:
            not interchangeable with `&mSpawnPos`. */
-        Vector3* home = (Vector3*)(((int)this + 0x440));
-        mHomePos = *home;
+        mHomePos = mSpawnPos;
         /* The shift must be LOGICAL so the angle wraps -- on the signed s16 it
            would index the wrong table entry for negative angles. */
         unsigned short angh = *(unsigned short*)&mAngleY;
```

#### p14-behavior-unsigned-timer

Candidate source SHA256: `95b0e3f61fe9d3e0139a85873e0fb658eb44853fe803bbb05649398c7a066524`.

```diff
--- a/src/actors/daPkn_c.cpp
+++ b/src/actors/daPkn_c.cpp
@@ -370,10 +370,7 @@
     func_ov084_0212f204(((char*)this));
     old = mState;
     (this->*data_ov084_02130e80[old])();
-    {
-        unsigned short* p100 = (unsigned short*)((char*)&mStateTimer);
-        *p100 = (unsigned short)(*p100 + 1);
-    }
+    mStateTimer = (u16)mStateTimer + 1;
     cur = mState;
     if (old != cur) {
         if (cur == 5) {
```

#### p15-init-setanim-localized

Candidate source SHA256: `a6f491c85c12a3a1a760eb0a5e3c0d1f6dcb8b49d2a458cf0b634211a015eb21`.

```diff
--- a/src/actors/daPkn_c.cpp
+++ b/src/actors/daPkn_c.cpp
@@ -336,7 +336,8 @@
     unk_474 = 0;
     mParticleHandle = unk_474;
     unk_478 = 0;
-    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130df4.file, 0, 0x1000, 0);
+    Fix12<int> animSpeed = { 0x1000 };
+    mModelAnim.SetAnim((BCA_File *)data_ov084_02130df4.file, 0, animSpeed, 0);
     return 1;
 }
 
```

Final source SHA256: `9d742b2063f2b3c7b2e9b9fb24faeb0bbe662ac494c6d8b6a9bfda1c079e1c03`.

Pinned-compiler raw object SHA256: `c116b50d545de7120049d8465355c4bd56eac21a82fb9088243bc9712d417b79`.
