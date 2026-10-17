# Amp native static initialization

Input/base: `53dc648eaf7d24847f461a193a3f81203cf4d098`. Production TU: `ov070/daBrq_c`, `src/game/actors/d_a_brq.cpp`. Producer: `codex-brq-sinit1015`, task `brq-sinit1015`. Endpoint is a scoped PR; no merge requested or performed. Independent source acceptance remains pending for the exact published candidate/base.

## Source and ROM evidence

Five native resource objects reproduce construction order: body model 0x2b1, electric model 0x2b3, electric animation 0x2b2, stopped animation 0x2b5, electric texture sequence 0x2b4. Their family constructors/destructors map exactly to the original arm9 model (0x02017acc / 0x02017ab4), animation (0x0201799c / 0x02017984), texture sequence (0x02017934 / 0x0201791c), and registration helper (0x020731dc) destinations. The local BrqModelResource, BrqAnimationFileResource and BrqTextureSequenceResource names are inferred eight-byte ABI views, not recovered original class names. The shared SharedFilePtr layout is unchanged. This deliberately differs from the earlier notes/sinit-dabrq-ownership.md boundary: exact import aliases bind local inferred ABI views without renaming canonical engine symbols; it does not claim original resource wrapper types have been recovered. The earlier neutral probe and its measurements remain historical evidence, with immutable links to the original initializer transcription.

Six ordinary C++ PMF descriptor imports copy into the native three-row State table. Retail words identify cooldown enter/update (0x0212320c -> 0x02120ce4; 0x02123224 -> 0x02120cac), active enter/update (0x02123214 -> 0x02120bf8; 0x02123234 -> 0x021209e4), and defeated enter/update (0x0212321c -> 0x02120910; 0x0212322c -> 0x021208a4). Each second descriptor word is zero. These descriptors remain ROM-owned. A final Vector3(0, -0x28000, 0) supplies the cylinder offset and native destructor registration.

Existing .text ownership 0x02120570..0x02121118 and class metadata .data 0x0212323c..0x021232f4 remain unchanged. Added native ownership: .init 0x02122d80..0x02122f30 (432 bytes), .ctor 0x021230a8..0x021230ac (4 bytes), .bss 0x021235ec..0x02123698 (172 bytes: five 8-byte handles, six 12-byte registration nodes, one 12-byte vector, three 16-byte state rows). The old handwritten C++ shard is retired from source and complete delinks; it was absent from CONVERTED, so no conversion ledger changes. Credit remains by ROM address.

Six compiler registration nodes are exposed under unused short configured labels @228, @229, @230, @232, @233, @234 at the original addresses. Their renames, binding exposure, exact import aliases and licensed section order change no bytes, storage, relocation records or addends. Existing metadata externalization and vtable address-point policies are retained. The state comment in include/daBrq_c.h now correctly identifies BSS/native initialization; no declaration or layout changes. affected_src reports only this production source consumes that header.

The typed resource definitions also replace three raw loaded-file layout casts and two redundant animation casts with field access. The complete pinned 17,120-byte compiler object is identical before/after that cleanup and final formatting: SHA-256 4567d371b8a6709718bed43cf8fd30ef1c683af721b916325311ed953b1b9181. All 19 original method byte arrays and relocation records are unchanged from the untouched input object.

## Validation

Pinned compiler 2004/b56 runs in the wired worktree. Direct owned-section proof passes all four nontext ranges and 71/71 relocations: 36 existing metadata relocations plus 35 initializer/ctor relocations. Focused ROM data: 6 VERIFIED, 11 PARTIAL, zero DIFFERS, zero UNNAMED. Source style passes with no deferrals; declaration agreement introduces no new disagreements or header redeclarations. All 257 port references pass. Conversion and rename ratchets pass. Raw per-function prepush verifies all 19 existing methods and reports BLIND-14 for compiler import spellings in the generated initializer, zero blocking. Exact manifest aliases and the strict 71-relocation whole-object proof close all fourteen sites. Historical path references are updated to immutable source links; their earlier measurements remain intact.

Full-ROM result: **SCRATCH-DATA-VERIFIED**. All five owned ranges and 106/106 modules exact; full 16 MiB ROM identical to the fresh source-independent stock control, SHA-256 d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8. Raw symbol audit: same nine pre-existing errors, zero new errors; it is not claimed green. All 71 owned relocations are verified.

Default production passes on the final source/configuration: 11,334/11,334 functions, 335/335 source-owned data claims, 242 BSS claims / 30,816 NOBITS bytes, and 106/106 modules exact. Intact module, zero-new-symbol-error and storage-alias gates pass. The full data inventory retains the unrelated pre-existing _ZTV10dCcAcPos_c difference; focused Amp data has zero DIFFERS. Main advanced independently to d6edd59a6a476acfdf8325980be709d129313923 during proof with no Amp-file overlap; this producer proof is pinned to the original source base above, and current-main composition/private validation remains the independent reviewer/integrator gate.

The source-independent control is regenerated after final tracked symbol/delinks changes. Its 106 modules are exact and its 16 MiB ROM SHA-256 is d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8. The raw symbol audit has nine pre-existing errors; it is not claimed green.

Ignored evidence: build/brq-original.o, brq-final-raw.o, brq-original-compile.txt, brq-native-compile.txt, brq-owned-report.json, brq-field-access-proof.json, brq-baseline.txt, brq-fullrom.txt, brq-romdata.json, brq-decl.txt, brq-tiers.txt, and build/tu/ov070-daBrq_c/link. Private receipts stay under build and are not publication artifacts.

## Remaining reconstruction and handoff

This completes the native initializer slice, not complete class/interface reconstruction. Existing Fix12/scalar ABI imports, vector field overlays, matrix-copy view and other recorded source leftovers remain. No new compiler-wall claim is made for them here; future Amp/interface producers under issue #2438 must reconstruct those consumers with fresh byte proof. Producer byte correctness does not substitute for independent source acceptance. Next owner/action: independent queue verifier reviews every changed/deleted path and the explicit inferred ABI views at the exact PR head/base; integration remains separately authorized.

Commands:

```powershell
python tools/tubuild.py compile ov070/daBrq_c
# After final tracked configuration edits:
python tools/tubuild.py linkcheck --baseline -j16
python tools/tubuild.py linkcheck ov070/daBrq_c -j16
python tools/rombuild.py -j16 --no-rom --report-json build/brq-production.json
python tools/prepush_linkcheck.py --range 53dc648eaf7d24847f461a193a3f81203cf4d098..HEAD
python tools/romdata_check.py --files src/game/actors/d_a_brq.cpp --json build/brq-romdata.json
python tools/affected_src.py include/daBrq_c.h
python tools/source_style.py --base 53dc648eaf7d24847f461a193a3f81203cf4d098
python tools/check_decl_agreement.py --changed 53dc648eaf7d24847f461a193a3f81203cf4d098
python tools/tiers_ratchet.py --check
python tools/check_rename_ledger.py --repo .
python tools/check_dead_references.py
python tools/port_refcheck.py
```
