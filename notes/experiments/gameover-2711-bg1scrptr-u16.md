# GetBG1ScrPtr u16* vs void* spelling (GAMEOVER-2711-01, PR #2711)

- tested_commit: 2546f0ae4 (cleanup-mgjump3-gameover at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/actors/dScGameOver_c.cpp:28-29,80-82` keeps the `unsigned short *`
  spelling of `_ZN3G2S12GetBG1ScrPtrEv` over a `void *` one.
- attempted_change (scratch-applied, then reverted):

```diff
-extern unsigned short *_ZN3G2S12GetBG1ScrPtrEv(void);
+extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
```

```diff
-    unsigned short *p = _ZN3G2S12GetBG1ScrPtrEv();
+    unsigned short *p = (unsigned short *)_ZN3G2S12GetBG1ScrPtrEv();
```

- command:

```text
.venv/bin/python tools/match.py --c src/actors/dScGameOver_c.cpp \
  --func func_ov003_020b0730 --addr 0x020b0730 --size 0xe0 \
  --version 2004/b56 --module ov003 --strict-relocs --brief
```

- result: baseline MATCH; variant MATCH (identical bytes, same size 0xe0).
  The other consumer (InitResources, 0x5dc) also still MATCHes under the
  void* spelling. The u16* choice is typing ergonomics (it types `p += ...`
  and the 16-bit stores without a cast), not a codegen constraint.
- log (variant):

```text
TARGET func_ov003_020b0730 @ 0x020b0730 size 0xe0  bytes: f04f2de904d04de200a0a0e10190a0e10080a0e3bcb09fe50850a0e1017aa0e3026aa0e3b0409fe57a92feeb8810a0e1b1109be1000059e30720a011810080e0... (224 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- verdict: DISPROVED
