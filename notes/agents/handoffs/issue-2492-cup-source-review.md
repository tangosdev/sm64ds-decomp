# Handoff: issue-2492-cup-source-review

This is a continuation of promoted `ov006/dScMgCup_c` from [PR #2348](https://github.com/tangosdev/sm64ds-decomp/pull/2348) and [scout #2343](https://github.com/tangosdev/sm64ds-decomp/pull/2343). The queue records this commit's immutable SHA. [Issue #2492](https://github.com/tangosdev/sm64ds-decomp/issues/2492) owns the remaining class work.

- Producer: `codex-cup-humanizer-producer-20260908-01`, Codex; branch `cpp/humanizer-cup-0908`.
- Accepted source input/base: `51e74d3ea9b8f1a2342e944be6d7fa9d3c57e5ea`; installed workflow: `6ef1dfb896e6074e83d2aa1a03cc3b701d07dcae`. Cup source/header/manifest at the earlier reviewed `224e660ea0219d0999c58483eb2dd9b38991f5ef` are identical to this input.
- Status: byte-verified bounded checkpoint; source reconstruction is partial and **CUP-05 remains unresolved correctness**. This is not whole-source acceptance or an integration approval.
- Next owner: independent verifier, with a fresh claim for this exact offered commit. The CUP-04 research contributor (`inventory_remote`, Codex) must not verify their own contribution. Preserve the candidate as held/rework if the source-review requirements remain unsatisfied.

The only executable changes remove the twenty-slot fake `Obj` receiver, use real virtual `OnYoshiTryEat(-1)` / `OnYoshiTryEat(3)` self-calls, and initialize `cup = i` before both jumps to the shared epilogue in `func_ov006_020def80`. Header declarations, field widths/order and method contracts are unchanged. The manifest gains a provenance/scope note; its claims and old measurements are preserved. Historical scout facts, all 32 `path#symbol` credits, symbols and enrollment are unchanged.

## Findings and remaining scope

| ID | Disposition at this checkpoint |
| --- | --- |
| CUP-01 | Fixed: the two receivers are this scene, whose real slot-18 override is already declared. Calls remain virtual. |
| CUP-02 | Fixed: current comments describe real `dScMgCup_c::*` PMFs and `this->*`, and the seven own slots 0, 6, 9, 16, 17, 18 and 20. Original member spellings remain inferred. |
| CUP-03 | False naming prohibition and “no matched access” at `pad_5400` corrected. Typed-field work remains: `Obj6e`, raw inherited `mHudScore` access, and the live words at 0x5400/0x5404/0x5408 need individually measured replacements. Issue #2492, humanizer/integrator support through @andrewboudreau, retains ownership. |
| CUP-04 | Fixed locally: every incoming epilogue path now assigns `cup`. The prior claim that a retail register observation closed uninitialized C++ behavior is removed. See experiments below. |
| CUP-05 | **Open correctness finding:** `Virtual50()` is declared `int` and falls through after a void-declared call. The shared minigame slot-20 base/callers/overrides require a separately reserved coherent audit and correction; the current byte match does not settle the return contract. No fabricated return or actor-hook analogy was introduced. Issue #2492 retains the dependency. |
| CUP-06 | Partial reconstruction: the manual factory, component/array byte storage, runtime array helpers, mangled constructor calls and address-named external interfaces remain. Measure natural lifecycle/component alternatives in an owned continuation under issue #2492. |

The class retains eight genuine state methods, five non-destructor overrides, the generated D1/D0 pair, fourteen address-named helper bodies, two empty callbacks and the factory. The TU still owns only `ov006` text `0x020de988..0x020e0638`; it acquires no data or BSS. Its compiler-only D2 and 13 RTTI/vtable passengers keep their existing dispositions. The RTTI class name is ROM evidence; original State/slot/factory names and many field names are not recovered strings. The scout's old pre-promotion status and field-naming policy are historical, not the continuation's instructions.

## Measured alternatives

- CUP-01's earlier ignored probe at `224e660e` removed `Obj` and replaced only the two calls; all 32 functions and typed relocations matched. The combined producer source repeats this proof against a fresh `51e74d3e` control.
- CUP-04 was researched at source-identical `224e660e` by `inventory_remote`, under `2004/b56` and production flags. Moving `cup = i` to its declaration and deleting the late assignment emits 0xa0 bytes instead of the retail 0xa4. Using `i` directly in the epilogue or assigning it at the epilogue also emits 0xa0 bytes; these are failing controls, not verified alternatives.
- The successful form adds `cup = i;` immediately before each of the two `goto epilogue;` statements and retains the final-arm assignment. Every source path initializes the local. The researcher proved all 32 functions and complete TU; this producer separately proved the combined final source, including the exact 0xa4-byte function at `ov006:0x020def80`. The unchanged historical constraints in the other four source locations were not counterprobed again.

The successful CUP-04 form is durable in this commit. Research artifacts `initialize-every-arm.patch`, `cup04-probes.json`, `cup04-fulltu.json` and `defined-local-whole-tu.log` are retained in the ignored review bundle; the failed transformations above specify their reproducible source changes.

## Proof

Byte tooling is from source base `51e74d3e`; the compiler is `2004/b56`, with `-O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc -Cpp_exceptions off`.

- Complete TU: `python tools/tubuild.py --manifest build/cup-proof-final/candidate-manifest.d verify ov006/dScMgCup_c` exits 0, **32/32 TEXT-VERIFIED**, with clean isolation/destinations and ROM order. An ignored manifest copy receives the verification output; the production manifest's historical evidence is not overwritten.
- Strict proof: the recorded producer runner compiles this entire TU once with the pin, applies the production isolation policy, then invokes `linkcheck` with each manifest function's exact name/address/size and module. **32/32 VERIFIED, 7,344 text bytes, zero blind checks and zero differences.** A fresh control compiled from accepted input has identical function bytes and typed relocation records. `candidate.strict.json`, `control.strict.json`, `comparison.json` record the results.
- Header closure: `affected_sources(["include/dScMgCup_c.h"])` returns only this complete TU; all 32 functions above cover it. `python tools/check_header_offsets.py include/dScMgCup_c.h` exits 0: **25 commented fields, zero mismatch/UNPARSED, span 0x5470**. Non-comment header tokens are identical to input, and the actual class compile enforces its size assertion.
- Raw metadata: `python tools/romdata_check.py --files src/actors/dScMgCup_c.cpp --json build/cup-proof-final/metadata-cli.json` exits 0: **7 VERIFIED (212 bytes), 6 PARTIAL (68 equal bytes), zero DIFFER/UNNAMED**. All 13 dispositions equal the fresh control. This diagnostic does not transfer ownership of the ROM-gap metadata to the TU.
- Port references: `python tools/port_refcheck.py --json build/cup-proof-final/port.json` exits 0, **423 checked, zero stale**.
- Full production packing: `python tools/rombuild.py -j8 --report-json build/cup-proof-final/packed-rom.json --data-json build/cup-proof-final/all-metadata.json --rom-out build/cup-proof-final/candidate.nds` exits 0: **106/106 modules exact, 11,199 reproducing functions, zero mismatches; 25/25 production data claims and 3 BSS claims (172 bytes) pass their gates**. The actual 16,777,216-byte packed artifact has SHA-256 `d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8`, identical to the stock control. The global raw metadata report retains 704 VERIFIED, 223 PARTIAL, 4 DIFFER and 389 UNNAMED records; the 9 inherited baseline symbol diagnostics remain, with zero new symbol errors. Neither is described as a universal metadata/symbol pass.
- Attribution: all 32 original symbol override values and the entire attribution file are unchanged. The exact-commit attribution and supplemental prepush checks are recorded separately after commit; they must not replace the full 32-function proof above.
- Private CI and final integration: not run here. No source branch push, PR or merge is included in this producer stage. The independent verifier must judge exact source correctness and the unresolved CUP-05 finding before any source acceptance.

Local gate logs and hashes are retained under ignored `build/cup-proof-final/`; source/header/manifest proof identities are recorded in `identity.json`. Producer queue publication carries only the reviewed commit and a public-safe next action. Private receipts, ROMs, compiler inputs and local machine paths are excluded from tracked content and public handoff evidence.
