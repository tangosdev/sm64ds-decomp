# Factory vtable [0] vs emitting-TU [2] spelling (BOO-2710-01, PR #2710)

- tested_commit: 8f5abbc11 (cleanup-datrs at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daTrs_c/d_a_trs_factories.cpp:41-43` keeps the
  importing-TU `&_ZTV7daTrs_c[0]` spelling because the emitting-TU [2] form
  lands +8 past the slot array.
- attempted_change (scratch-applied, then reverted):

```diff
-        *(int *)actor = (int)&_ZTV7daTrs_c[0];
+        *(int *)actor = (int)&_ZTV7daTrs_c[2];
 (both factories)
```

- method: every automated harness was tried and all four pass EITHER
  spelling, so the proof is reloc dump + ROM bytes (below).
- commands:

```text
.venv/bin/python tools/match.py --c src/game/actors/daTrs_c/d_a_trs_factories.cpp \
  --func daTrs_c_classInit_TERESA --addr 0x0211c590 --size 0x70 \
  --version 2004/b56 --module ov063 --strict-relocs --brief
.venv/bin/python tools/prepush_linkcheck.py --files src/game/actors/daTrs_c/d_a_trs_factories.cpp
.venv/bin/python tools/linkcheck.py --module ov063 --c src/game/actors/daTrs_c/d_a_trs_factories.cpp \
  --name daTrs_c_classInit_TERESA --addr 0x0211c590 --size 0x70
OI.plan(raw_object_bytes, symbol) per tools/objisolate.py + tools/test_objisolate.py pattern
```

- result:
  - match.py: baseline MATCH, variant MATCH (wildcards relocated words).
  - prepush_linkcheck: variant 2 checked, 2 VERIFIED.
  - linkcheck.py: baseline VERIFIED diffs:[] blind:0; variant VERIFIED
    diffs:[] blind:0. Root cause: ov063/relocs.txt (1186 rows) contains
    zero rows for _ZTV7daTrs_c, so no edge expectation exists for this
    import and either addend passes.
  - objisolate plan(): variant error None for both factories. By design:
    a _ZTV UNDEF addend >= VTABLE_PREAMBLE passes through (ModelAnim
    secondary-vtable precedent, objisolate.py:490-496); only non-_ZTV or
    sub-preamble addends are refused.
  - reloc dump (real 2004/b56 objects, R_ARM_ABS32 against _ZTV7daTrs_c):
    [0] addend 0; [2] addend 8. Instructions otherwise identical.
  - ROM ground truth (extracted/overlays/overlay_0063.bin, base 0x02115ee0):
    TERESA pool word @0x0211c5f4 = 0x0211e828; BOSS pool word @0x0211c584 =
    0x0211e828; symbols.txt _ZTV7daTrs_c = 0x0211e828; word at 0x0211e828 =
    0x0211b9bc (InitResources: the symbol IS the slot array).
  - therefore [0] links to exactly the ROM bytes; [2] links one word past
    (a vptr skipping into slot 1). The catching gate is the module
    byte-compare (tools/test_objisolate.py: "no gate earlier than the byte
    compare can see it" -- 76 functions across 34 modules historically).
- log (reloc dump, pristine then variant):

```text
  off=0x64 type=2 sym=_ZTV7daTrs_c addend=0
  off=0x64 type=2 sym=_ZTV7daTrs_c addend=0
  off=0x64 type=2 sym=_ZTV7daTrs_c addend=8
  off=0x64 type=2 sym=_ZTV7daTrs_c addend=8
```

- verdict: CONSTRAINT SUPPORTED
