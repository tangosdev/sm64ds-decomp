# Handoff: pr-2474-port-source-review-fixes

This document describes this commit. The queue records its immutable output SHA.

## Identity and continuation

- Existing PR: https://github.com/tangosdev/sm64ds-decomp/pull/2474
- Task/stage: `pr-2474-port-source-review-fixes` / `fix` (producer).
- Session/harness: `codex-port-boot-diagnostics-20260908` / Codex.
- Branch: `fix/port-boot-diagnostics-0908`.
- Preserved input: `86132871880bb29c82954570bd9dd69e76d5c477`, from `land/cons-861328718`.
- Original source base: `b99310a89af9c0e4737f81ebde00d5599dba7e31`.
- Installed workflow/tools: `6ef1dfb896e6074e83d2aa1a03cc3b701d07dcae`, read from the separate workflow checkout. The older source input was not upgraded or rebased.
- Status: locally verified host diagnostic correction; independent verification and final PR gates pending. Next owner: independent verifier, then the existing PR integrator.
- The original tangosdev commit history remains the ancestry of this continuation. No source author, attribution record, or existing PR branch was rewritten.

## Narrow correction

Owned files changed: `port/hal/boot_arms.cpp`, comment-only `port/slice_r2abc.txt`, and this handoff.

The save-block layout guard now subtracts `uintptr_t` address representations. Its separately declared arrays do not form one C++ array merely because the host linker places them together. The expected offsets and refusal-before-clear behavior are unchanged.

The device diagnostic reports an offset only when the observed row address is inside the hosted 0xa0-byte table. A retained external row is identified by its host address instead. Null and unchanged pointers are reported as observations; neither is described as proof that a particular helper did or did not run. Selecting the same row again can leave the pointer unchanged.

This is host behavior and diagnostics work, with no new ROM reconstruction or naming claim. The card-bringup body is unchanged byte for byte as source text. All seven slice source entries are unchanged. The R2b refusal, the partial host `port_rom_card_bringup` bridge, the two uncalled card-driver edges, storage definitions, and boot call order remain the existing implementation. Linkage coverage is not a claim that these remaining host behaviors were removed.

## Local proof

Ignored evidence: `build/port-boot-review/proof.json`, with expanded commands, file hashes, exit codes, scenario logs, and limits. `boot_arms_harness.cpp` in that directory is a controlled test fixture; it links the actual changed `boot_arms.cpp` object without editing that source.

- **MSVC x86 compile:** `cl /nologo /O2 /W4 /EHsc /c port/hal/boot_arms.cpp /Fo<ignored-object>`; exit 0, no warnings. Installed VS18 x86 tools were used.
- **Focused host execution:** two harness executables, normal grouped storage and a deliberately displaced save-block section; six scenarios each, **12/12 exit 0**. The displaced layout refuses the clear while the valid layout performs it. Device cases cover re-writing the same table pointer, changing to the table row, retained external rows below the table and exactly at its upper boundary, retained null, and clearing the pointer. Both original card-bringup guard branches also pass. The harness asserts the clear/identify order and call counts.
- **Existing host-loop gate:** `python port/tools/hostloop_transcription_check.py`; exit 0, 406 normalized lines and 60 externs exact. Log: `build/port-boot-review/hostloop-transcription.log`.
- **Scope checks:** `git diff --check`; exit 0. The seven non-comment slice entries and complete `port_rom_card_bringup` body equal the preserved input. No `src/`, `include/`, or `config/` change.

The harness supplies controlled external calls and storage. It proves these guard/diagnostic branches, not the complete game boot, actual production linker placement, save persistence, ROM matching, or linkage count. No full host executable is configured in this owned checkout, so the asset-backed battery, save proof and thread-create proof were not run. The input's build script hardcodes an unavailable VS2022 BuildTools installation; direct x86 compilation used the installed VS18 toolchain. The wired setup's ARM canary could not import `build_pin.py`, which this old input does not contain; no ARM success is claimed or required for this host-only change.

Full-ROM, class/TU relocation, metadata and shared-header consumer gates are outside this correction: no cartridge source or shared header changed. Existing PR CI is historical input evidence, not a pass for this commit. The integrator must obtain fresh terminal checks after the independent verifier accepts this exact candidate. No actual PR branch push or merge was performed by this producer.

The private producer receipt and all local harness/build files remain ignored. No source work remains uncommitted; the only outstanding work is independent review and the existing PR's integration gates.
