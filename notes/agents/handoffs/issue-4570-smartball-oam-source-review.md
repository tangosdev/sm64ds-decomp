# Source review handoff: Smartball OAM declaration

- Task: `issue-4570-smartball-oam-shadow-1103`, issue [#4570](https://github.com/tangosdev/sm64ds-decomp/issues/4570).
- Producer: `codex-smartball-oam-writer-1103b`.
- Tested base: `76e2aed46eb35ebd944d1bf14e668e73fc3bad0a` (`origin/main`).
- Scope: `src/minigames/d_s_mg_smartball.cpp` only. No object layout, call site, or shared-header definition changed.

## Source finding

The file declared a local method-only `struct OAM` with the static ten-argument
`Render` overload. `include/OAM.h` already declares that same static overload,
with unnamed parameters to avoid shifting compiler-local symbol numbering in
its consumers. The local declaration was removed and the shared header included.
The call sites remain unchanged. The shared method is static, so the calls do
not pass an `OAM` object.

`tools/pad_layout_audit.py` on the tested base reported 28 header shadows; on
this candidate it reports 27. This removes one confirmed same-class method
declaration. It does not name any unknown fields or claim a recovered source
boundary.

## Verification

- Before-edit control: `tubuild.py verify ov006/dScMgSmartball_c --no-write` —
  132/132 functions matched; object isolation, relocation destinations, and
  emission order clean.
- Candidate: the same command — 132/132 matched; object isolation,
  relocation destinations, and emission order clean. The TU manifest needed no
  compiler-local alias changes.
- `prepush_linkcheck.py --files src/minigames/d_s_mg_smartball.cpp` — 132
  verified, zero warnings, zero blockers.
- `check_decl_agreement.py --changed origin/main` — no new local redeclarations
  or declaration disagreements.
- `check_dead_references.py` — no new dead references or broken Markdown links.
- `source_style.py --base origin/main --files src/minigames/d_s_mg_smartball.cpp`
  — one file, zero findings, zero formatting deferrals.
- `port_refcheck.py` — 217 references resolve.
- `git diff --check` — clean.

The full `rombuild.py -j16 --no-rom` was attempted. It stopped in existing
repository-wide object-isolation/compiler-only and TU-manifest errors in other
files (including `src/Vec3_ApproachHorz.cpp` and `src/Timer.cpp`); the report
does not list the Smartball TU as a failure. Therefore this handoff claims the
focused exact TU and relocation proof, not a passing whole-repository build.

## Review boundary

The bytes establish this source substitution's code and relocation identity.
They do not prove that this source fragment matches the original EAD file
boundary. Independent source review and hosted PR validation remain pending.
