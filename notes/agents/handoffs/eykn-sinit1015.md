# Mr. I native static initialization

Input/base: `53dc648eaf7d24847f461a193a3f81203cf4d098`. Production TU: `ov071/daEykn_c`, `src/game/actors/d_a_eykn.cpp`. Producer session/task: `codex-eykn-sinit1015` / `eykn-sinit1015`. Publication is PR-only; no merge requested or performed. Independent verifier and integration owner: queue verifier/integrator for this exact candidate; source acceptance remains pending.

## Source and ROM evidence

The compiler now constructs the model (0x2f7), close/open texture sequences (0x2f8/0x2fb), and death animation (0x2f9), then copies six existing ROM PMF descriptors into the three native `daEykn_c::State` rows. No handwritten constructor registration remains. The former C-linkage state-table and four resource imports become owned C++ definitions. Six descriptor imports use ordinary C++ linkage; their initialized data remains ROM-owned.

The local `EyknModelFile`, `EyknTextureSequenceFile`, `EyknAnimationFileHandle` names are inferred eight-byte resource-family ABI views, not recovered original class names. `LoadedFile` preserves the existing two-word layout while deriving from the existing empty SharedFilePtr interface. Exact manifest aliases target the same model (0x02017acc/0x02017ab4), texture-sequence (0x02017934/0x0201791c), animation (0x0201799c/0x02017984), and registration (0x020731dc) functions used by the original initializer. Alias spellings fit existing object string storage; no function bodies, relocation records, addends or storage bytes are patched.

The six descriptor words were read from retail: 0x02122cb8 -> St_Wait_Init 0x02121570; 0x02122cc0 -> St_Wait_Main 0x0212152c; 0x02122ca8 -> St_Attack_Init 0x021214f4; 0x02122cd0 -> St_Attack_Main 0x021211e0; 0x02122cb0 -> St_Die_Init 0x0212110c; 0x02122cc8 -> St_Die_Main 0x02120d30. Every second descriptor word is zero. This verifies row identity and order without claiming original English names.

Native ownership: original 25-function .text 0x02120668..0x02121a6c; .init 0x021228c8..0x02122a1c (340 bytes); .ctor 0x02122b34..0x02122b38 (4 bytes); .bss 0x02123038..0x021230b8 (128 bytes: four 8-byte resources, four 12-byte registration nodes, three 16-byte state rows). Existing configured BSS boundaries remain unchanged. Compiler node labels @534..@537 become unused configured labels @182..@185 at original addresses; local binding exposure and exact section ordering change no content. The old C shard is retired from source and complete delinks; it was absent from the CONVERTED baseline, so neither shared conversion ledger changes. Attribution stays keyed by ROM address.

## Validation

Pinned compiler: `2004/b56`. The untouched TU compiles in the wired worktree. All 25 original function byte arrays and raw relocation records are unchanged. Direct owned-section verification passes all three nontext ranges and 27/27 relocations with no missing, blind or wrong destination. Focused ROM-data inventory: 5 VERIFIED, 9 PARTIAL, zero DIFFERS, zero UNNAMED. Declaration agreement, CONVERTED ratchet, rename ledger and all 257 port references pass. No current port target references this source.

Raw per-function prepush: 26 checked, 25 VERIFIED, generated initializer BLIND-11, zero blocking. These are the explicit compiler import spellings; the alias-aware owned-section proof verifies every one of the 27 nontext destinations. Pinned source style passes with no deferrals. Formatting-only comment alignment leaves the entire 20,712-byte compiler object identical, SHA-256 6c65c45fa2c710ffa0cabaf098b6eff91c5ec943348c966f56c2f69707fe1756.

Full-ROM gate: **SCRATCH-DATA-VERIFIED**, all four owned ranges exact, 106/106 modules exact, 16 MiB ROM identical to the fresh source-independent strict stock control, SHA-256 d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8. The raw symbol audit has the same nine pre-existing errors and zero new errors; it is not claimed green. The control was regenerated after the final tracked symbol/delinks changes, so its content fingerprint is current.

Default production passes on the final source/configuration: 11,334/11,334 functions, 335/335 source-owned data claims, 242 BSS claims / 30,772 NOBITS bytes, and all 106 modules exact. Intact TU module gates, zero-new-symbol-error gate and storage aliases pass. The live interior label data_ov071_0212306c remains at its original address inside the generated registration-node BSS, using the existing section-owned LCF label support; no storage is fabricated. The full ROM-data inventory retains one unrelated pre-existing difference (_ZTV10dCcAcPos_c); focused Mr. I data has zero DIFFERS.

Stock full-ROM control: all 106 modules exact; 16 MiB ROM SHA-256 d1506e90efae5e2d2cf119926a4ac2a291bd5ca78349d09d5024e1a918c478e8. The raw symbol audit has nine pre-existing errors. This control does not certify the candidate; candidate and production results are recorded separately.

Ignored proof artifacts: build/eykn-canary.txt, eykn-original.o, eykn-native-compile.txt, eykn-owned-report.json, eykn-prepared.o, eykn-baseline.txt, eykn-fullrom.txt, eykn-romdata.json, eykn-decl.txt, eykn-tiers.txt, eykn-deadrefs.txt and the build/tu/ov071-daEykn_c/link report and linked ELF. Private queue receipts stay in build and are not publication artifacts.

## Remaining reconstruction

This is a native-initializer cleanup, not a complete class/interface reconstruction. Existing Fix12 bridges, scalar Particle/Player/Sound boundaries, layout casts and other measured leftovers in the source remain. Their source/ABI reconstruction belongs to a later producer and requires consumer byte proof. RTTI/vtable records retain their existing exact externalization policies and ROM ownership. Do not interpret producer byte proof as independent source acceptance. Next action: independent review of the exact published candidate and base, plus private validate before any separately authorized integration.

Commands (from the wired worktree):

```powershell
python tools/tubuild.py compile ov071/daEykn_c
# After final tracked config edits, regenerate the source-independent control:
python tools/tubuild.py linkcheck --baseline -j8
python tools/tubuild.py linkcheck ov071/daEykn_c -j8
python tools/rombuild.py -j16 --no-rom --report-json build/eykn-production.json
python tools/prepush_linkcheck.py --range 53dc648eaf7d24847f461a193a3f81203cf4d098..HEAD
python tools/romdata_check.py --files src/game/actors/d_a_eykn.cpp --json build/eykn-romdata.json
python tools/source_style.py --base 53dc648eaf7d24847f461a193a3f81203cf4d098
python tools/check_decl_agreement.py --changed 53dc648eaf7d24847f461a193a3f81203cf4d098
python tools/tiers_ratchet.py --check
python tools/check_rename_ledger.py --repo .
python tools/check_dead_references.py
python tools/port_refcheck.py
```
