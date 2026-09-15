# Tree _Znwj spelling vs ::operator new (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_tree.cpp:90-93` spells `_Znwj` because
  `::operator new` mangles `_Znwm` (sizeof is unsigned long), which has
  no ROM home; the cartridge calls `_Znwj`.
- attempted_change (scratch-applied, then reverted):

```diff
-    node = (TreeNode *)_Znwj(sizeof(TreeNode));
+    node = (TreeNode *)::operator new(sizeof(TreeNode));
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_tree.cpp \
  --func _ZN8daTree_c13InitResourcesEv --addr 0x020ec22c --size 0x100 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: variant function body MATCHes (bytes identical; the `bl` is a
  wildcarded reloc slot). The claim is about symbol identity, not bytes,
  so the emitted import was inspected directly: the variant object
  references undefined `_Znwm` (STB_GLOBAL, SHN_UNDEF), exactly as the
  comment says. `_Znwm` occurs in NO symbols file in the tree
  (`grep -rln _Znwm config/arm9/overlays/ config/arm9/symbols.txt`
  prints nothing), while config ground truth
  (`config/arm9/overlays/ov002/relocs.txt:7763`) requires that slot
  (`0x020ec288`) to target `0x0203cbe4` = `_Znwj`
  (`config/arm9/symbols.txt:1633`). match.py still reports MATCH
  because `reloc_audit.classify` returns UNRESOLVED -- not WRONG-DEST
  -- for a homeless import, and only WRONG-DEST fails the strict gate.
  The variant is unlinkable in production (undefined import, no home).
- log (variant):

```text
TARGET _ZN8daTree_c13InitResourcesEv @ 0x020ec22c size 0x100  bytes: 70402de908d04de20060a0e1080096e5d8109fe52002a0e1073000e2040053e30430a0a3030191e7035181e0000050e30800001ad41086e25000a0e3931020e0... (256 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- log (variant object import + home search):

```text
_Znwm shndx= SHN_UNDEF bind= STB_GLOBAL
```

```text
$ grep -rln "_Znwm" config/arm9/overlays/ config/arm9/symbols.txt
$   (no output: no ROM home anywhere)
```

- verdict: CONSTRAINT SUPPORTED
