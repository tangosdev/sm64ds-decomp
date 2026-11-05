# Handoff: issue-4570-dactor-newsimple-1105

This document describes this commit. The queue records its immutable output SHA.

## Identity and resumption

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/4570; task `issue-4570-dactor-newsimple-1105`; producer session `codex-dactor-newsimple-1105` (Codex).
- Branch: `audit/source-audit-current1105`; queue input/original source base: `fe190dc93a4125144024138d3b8f6ba4d193c7d2`. After #4848 merged, this candidate was rebased onto current `main` at `682321c64de078383e3c66cea08b0a3cd451a5ef`; the final gates below were rerun on that base.
- Workflow/tool pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`; predecessor task: `particle-actors-cont1101`.
- This commit contains the source, Actor TU manifest and this handoff. Build logs are local under ignored `build/`.
- Next action: independent verifier reviews source plausibility and reruns exact TU, relocation and full-ROM proof on this commit; independent source review and integration remain pending.
- Status: verified byte candidate. The TU verifier still reports the pre-existing unlicensed `__sinit_d_actor.cpp` section, so this is not a clean TU-promotion verdict.
- No source edits remain uncommitted; private build outputs stay in `build/`.

## What changed and why

- Scope: `Particle::System::NewSimple` calls in `src/runtime/actor/d_actor.cpp`, ARM9 range `0x0200f658..0x02011654`.
- The file-local `Particle::System` declaration said `NewSimple` returned `int`; the shared declaration in `include/Particle__System.h` and its definition in `src/engine/particle/Particle.cpp` return `void *`. The TU now includes the shared header. Its two int-returning C helpers cast the pointer result explicitly; the remaining NewSimple calls discard it.
- Importing the full header renumbered four compiler-generated anonymous-union destructors by +56. The Actor TU manifest deadstrip rows and generated verification record were refreshed for the observed names. No ROM symbol or attribution rows changed.
- This is a type/layout cleanup only; no original identifier is newly claimed.

## Reconstruction dimensions

- Byte coverage: all 97 licensed Actor functions match. Relocation destinations and object-isolation checks are clean.
- The two C helper functions keep their existing int declarations; the explicit casts preserve the 32-bit return value. Other declarations of those C helpers are unchanged because callers discard the result.
- The local Particle::System shadow is removed from this TU. Other #4570 shadow work remains open.
- The four anonymous-union destructor sections remain compiler-only and deadstripped. `__sinit_d_actor.cpp` remains the one unlicensed emitted section in the TU manifest verdict; the independent linkcheck verified it.
- Attribution is unchanged; the source path and function ownership remain as before.

## Proof

- `python tools/tubuild.py verify arm9/Actor --no-write --verdict` — 97/97 MATCH; object isolation clean; relocation destinations clean; expected emission order. Exit 1 only for the existing unlicensed `__sinit_d_actor.cpp` section (1 unlicensed, 0 compiler-only policy errors). The default verify refreshed the manifest record with the same limitation.
- `python tools/tu_object_diff.py --base origin/main --numbered src/runtime/actor/d_actor.cpp` — only four compiler-numbered destructor names changed: `$98->$154`, `$101->$157`, `$104->$160`, `$107->$163`; the section bodies and relocation destinations are otherwise unchanged. The manifest licenses those four names.
- `python tools/prepush_linkcheck.py --files src/runtime/actor/d_actor.cpp` — exit 0; 98 checked, 98 verified, 0 warnings, 0 blockers.
- `python tools/rombuild.py -j16 --no-rom` — exit 0; 11,395 source-built functions, 0 mismatches; 106/106 modules exact; 470/470 source-owned data claims reproduce; 0 differing ROM data symbols.
- `python tools/check_decl_agreement.py --changed origin/main` — no new declaration disagreements or header-declaration redeclarations.
- `python tools/port_refcheck.py` — 217 references resolve. `git diff --check` — pass.
- Private PR validation and independent source review: pending.
