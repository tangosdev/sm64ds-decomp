# Handoff: port-split1015

This document describes this commit. The queue records its immutable output SHA.

## Identity and scope

- Issue: https://github.com/tangosdev/sm64ds-decomp/issues/3877.
- Task/stage: `port-split1015` / `split`; Codex session `codex-port-split1015-producer`.
- Branch/worktree: `cpp/port-split1015`, `C:/tmp/sm64ds-port-split1015`.
- Source base and workflow: `af21ea68d359d1aee18f3ff5a6ac51f7dfadc921`.
- Next owner: an independent verifier accepting the queue's exact `review` stage input; publication is authorized, merging is not.
- The user explicitly authorized disjoint edits to the IRQ/CP15 entries of `config/arm9/delinks.txt` despite the Decimal task's whole-file reservation. That task, its entries and its reservation were not changed.

## Change

Extract the ordinary C helper from IRQ3 and the entire ordinary C tail from CP15 into six separate compiler inputs. Keeping only the assembly prefixes in the original inputs preserves their contiguous ROM layout. The default build consumes all six new sources through complete delinks entries. These are deliberate portability boundaries, not a claim about original Nintendo filenames. The manifests retain the old verification separately as historical evidence and describe their reduced assembly ranges.

| Symbol | ARM9 range | Production source |
| --- | --- | --- |
| `func_02058c84` | `0x02058c84..0x02058cd0` | `src/unnamed/arm9/0205/func_02058c84.cpp` |
| `func_02058cd0` | `0x02058cd0..0x02058d58` | `src/unnamed/arm9/0205/func_02058cd0.cpp` |
| `func_02058d58` | `0x02058d58..0x02058d6c` | `src/unnamed/arm9/0205/func_02058cd0.cpp` |
| `func_02058d6c` | `0x02058d6c..0x02058d80` | `src/unnamed/arm9/0205/func_02058cd0.cpp` |
| `OS_GetInitArenaLo` | `0x02058d80..0x02058df4` | `src/OS_GetInitArenaLo.cpp` |
| `func_02059cb4` | `0x02059cb4..0x02059d08` | `src/unnamed/arm9/0205/func_02059cb4.cpp` |

The IRQ initializer includes `IRQ.h` and calls `IRQ::DisableIRQs(4)`, replacing a redundant local mangled declaration with the existing interface. Other function bodies and signatures are preserved. Remaining assembly, absolute arena addresses, address-based names and the startup helper's local declarations are unchanged. No shared headers, classes, layouts, lifecycle, vtables or owned data change. Function-address attribution stays unchanged; the existing CP15 path/symbol attribution overrides all refer to the retained assembly functions.

## Proof and limits

- All 18 functions across the eight resulting sources pass `linkcheck.linkcheck` using `2004/b56`: `VERIFIED`, `diffs: []`, `blind: 0`. Script and JSON: ignored `build/port-split/check_split.py` and `split-linkcheck.json`. The six production files equal their tested candidate files byte for byte.
- `python tools/tubuild.py verify arm9/CP15` and `... arm9/IRQ3`: 7/7 and 5/5, exact declared function sets, correct emission order, clean object isolation and relocation destinations. Current manifest verification records these reduced inputs.
- All six extracted files compile with x86 MSVC 14.51, `/W4 /std:c++17 /DSM64DS_PLATFORM_PC`, with no diagnostics. This is a compilation check, not a port executable link or runtime test.
- `python tools/port_refcheck.py`: all 259 references resolve.
- `python tools/check_decl_agreement.py --changed af21ea68d359d1aee18f3ff5a6ac51f7dfadc921`: no new declaration disagreements or header redeclarations; 151 files in scope.
- `python tools/source_coverage.py --check --base af21ea68d359d1aee18f3ff5a6ac51f7dfadc921`: unchanged 2,268,904 source-owned bytes, zero bytes handed back to the ROM.
- `python tools/rombuild.py -j16 --no-rom`: exit 0 on both the untouched base and candidate; 106/106 exact modules, 11,334/11,334 source-built functions reproducing, 332/332 source-owned data claims reproducing. Both retain the same nine baseline symbol diagnostics and the same aggregate ROM-data inventory (974 verified, 1,521 partial, 1 differ, 15 unnamed). This is module fidelity, not a claim that the full data inventory is complete. Logs: `build/port-split/baseline-rombuild.log` and `candidate-rombuild.log`; final report: `candidate-rombuild-report.json`.
- Commit-range prepush verification and private CI are recorded separately after this commit. Independent source review remains pending; this producer's checks are not independent acceptance.

The port still needs its hardware and arena-memory adaptations. In particular, the extracted arena setters retain fixed DS addresses; this PR does not remove `port/hal/os_arena.cpp` replacements or claim a host runtime improvement. The three small ARM match attempts and portable NitroSDK twins remain Tango's scope.

## Factory report investigation

At the pinned base, Cup and Roulette explicitly pass `scene` to `_ZN11dScMgBase_cC2Ev`. Trampoline2's six count/stride pairs agree with the ROM, and the runtime definition of `__cxa_vec_ctor` takes count before stride. All three factory functions were compiled with the pinned compiler and checked with their actual production object policies: `dScMgCup_c_classInit` at ov006 `0x020e0574`, `dScMgRoulette_c_classInit` at `0x0210a400`, and `dScMgTrampoline2_c_classInit` at `0x0212471c` each return `VERIFIED`, empty differences and zero blind relocations. Local reproducible check: `build/port-split/check_factories.py`; results: `factory-linkcheck.json`. The specific reported defects were not reproduced on this base, so no factory changes are included. This is not a claim that their manual lifecycle reconstruction or every declaration is complete.

Remote main was refreshed before publication to `1d3719e0ddad6d9238d288cdf0cf770b0bcb0249`. Its only intervening merge is #3874 (three unrelated actor sources and their evidence); it does not overlap this patch. Local full-build evidence above remains explicitly pinned to the source base, not asserted as a separate current-main composition test.
