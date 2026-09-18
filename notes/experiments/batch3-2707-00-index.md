# BATCH3-2707-01 pinned experiments: verdicts + non-experimentable inventory

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- Every row below is one `batch3-2707-<slug>.md` in this directory, in the
  pilot artifact format (claim, attempted_change, command, verbatim log).

## Verdicts (25 experiments: 24 SUPPORTED + 1 VERIFIED-CURRENT, 0 DISPROVED)

Mandatory claims:

- tree-render-vector3-out: DEFERRED, completion: partial, issue #2748 (body
  MATCHes; +_ZN7Vector3D1Ev refused by production _isolate -- an
  ownership/manifest dependency, NOT a codegen constraint)
- sldmng-playsub-method: SUPPORTED (0xa4 -> 0xb4 re-measured; file text says 0xb8)
- warpkun-isplayer-fold: SUPPORTED (0xb4 -> 0xa8)
- floatboard-bobphase-collapse: SUPPORTED (0x20c -> 0x208)
- soundobj-loopinvariants: SUPPORTED (0x14c -> 0x15c without the pragma)
- tree-dccpos-init-6az: SUPPORTED (0x100 -> 0x118)
- tree-applyopacity-current: VERIFIED-CURRENT (Render MATCHes, no attempted_change)
- tree-applyopacity-census: PARAMETER CONFIRMED (25/25 ROM call sites set r2
  against a 33.4% base rate; the two register-copy sites carry a constant 0,
  not a live value -- answers BATCH3-2707-01)

Remaining compiler-testable Leftovers:

- floatboard-riderpos-ptr: SUPPORTED (0x20c -> 0x210, +1 insn)
- floatboard-5ab4-staged-locals: SUPPORTED (8 words differ, same size)
- floatboard-atan2-6az: SUPPORTED (0x20c -> 0x224)
- floatboard-isclsn-6az: SUPPORTED (0x20c -> 0x220)
- warpkun-radius-cse: SUPPORTED (7 words differ, same size)
- warpkun-dccac-init-6az: SUPPORTED (0x70 -> 0x84)
- setse-int-casts: SUPPORTED (0xd0 -> 0xb8, exactly six words short)
- setse-volatile-rmw: SUPPORTED (0xd0 -> 0xd4)
- soundobj-dup-0x21: SUPPORTED (0x14c -> 0x144)
- soundobj-loop-shape: SUPPORTED (for and assign-in-cond forms both 0x14c -> 0x16c)
- soundobj-ret-goto: SUPPORTED (17 words differ, same size)
- soundobj-loop-int: SUPPORTED (0x94 -> 0xac with bool)
- billboard-defaultless-switch: SUPPORTED (0x9c -> 0xa8 with default)
- tree-clipper-6az: SUPPORTED (0x134 -> 0x14c)
- tree-znwj-new: SUPPORTED (body MATCHes; emits homeless _Znwm import, unlinkable)
- abuku-dccac-init-6az: SUPPORTED (0x50 -> 0x64)
- abuku-vector3-pod: SUPPORTED (body MATCHes; +_ZN7Vector3D1Ev refused by production _isolate)
- abuku-sway-actorid-forms: SUPPORTED (0x13c -> 0x130, joint)

## NOT-EXPERIMENTABLE (one line why each; not forced)

- S14 profile/table placements (cam_tag, sld_mng, sound_obj, set_se,
  warpkun, tree g_profile_*, abuku spawn info, billboard/set_se tables):
  .data/gap ownership outside .text-only TUs, not function codegen.
- "Naming belongs with callee" + data_* link names (floatboard
  func_020393a4/dBgW, sine table, sld_mng spawn Vector3, set_se/sound_obj
  data_* externs, func_*/table labels): symbol naming; renaming is
  symbols.txt, match.py compares bytes not names.
- decl_common/shared-signature agreement (floatboard (char *) sigs,
  abuku (daObjAbuku_c *) decls): cross-TU decl identity, ABI-identical,
  no codegen delta asserted.
- No alternate spelling exists: abuku Particle New/NewSimple,
  billboard Matrix4x3_FromRotationY (no header declares them); tree
  *(Vector3 *)&mPosX pun (no Pos() accessor); set_se PlayLong vector
  (no shared overlay, no breaking alternate asserted).
- No legal alternate exists: tree dCcPos_c::C1 (qualified ctor call is
  illegal C++); billboard member/static func form (explicitly
  "unmeasured", linkage change alters symbol identity).
- Spelling-only / untouched bytes: cam_tag + billboard pad_0d0, sld_mng
  unk_0d0/pad_0d7; abuku Player::Heal (include choice, same mangling).
- Non-codegen rationale: warpkun SetRanges S13 half (decl placement);
  warpkun height-local (aliasing correctness); sound_obj
  SoundObjectCallbackOwner shadow (type naming); sound_obj PlaySub 6az
  wall (measurement explicitly deferred to d_a_sld_mng.cpp, tested as
  sldmng-playsub-method).
