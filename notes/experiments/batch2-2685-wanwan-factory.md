# Factory hand-rolled C2/vec_ctor vs return new (BATCH2-2685-02, PR #2685)

- tested_commit: 019a7cc06 (cleanup-leftover-overlay-batch-2 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_wanwan.cpp:29-31` keeps
  the hand-rolled C2 / vec_ctor walk because `return new`
  size-DIFFs (Vector3[7] ctor is func_0203d384, not the implicit default).
- attempted_change (scratch-applied, then reverted):

```diff
-extern "C" daWanwan_c *daWanwan_c_classInit()
-{
-    char *c = (char *)_ZN7fBase_cnwEj(0x620);
-    if (c) {
-        _ZN12dEnemyBase_cC2Ev(c);
-        *(int **)c = &_ZTV10daWanwan_c[2];
-        _ZN10dCcAcPos_cC1Ev(c + 0x110);
-        _ZN9ModelAnimC1Ev(c + 0x150);
-        _ZN11ShadowModelC1Ev(c + 0x1b4);
-        __cxa_vec_ctor(c + 0x1dc, 7, 0x50, (void *)_ZN5ModelC1Ev, (void *)_ZN5ModelD1Ev);
-        __cxa_vec_ctor(c + 0x40c, 7, 0x28, (void *)_ZN11ShadowModelC1Ev, (void *)_ZN11ShadowModelD1Ev);
-        __cxa_vec_ctor(c + 0x524, 7, 0xc, (void *)func_0203d384, (void *)_ZN7Vector3D1Ev);
-        __cxa_vec_ctor(c + 0x578, 7, 0xc, (void *)func_0203d384, (void *)_ZN7Vector3D1Ev);
-    }
-    return (daWanwan_c *)c;
-}
+extern "C" daWanwan_c *daWanwan_c_classInit()
+{
+    return new daWanwan_c();
+}
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_wanwan.cpp \
  --func daWanwan_c_classInit --addr 0x02112d1c --size 0xf0 \
  --version 2004/b56 --module ov014 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0xf0 vs 0xa0
  (240 -> 160 bytes). `return new` DIFFs as claimed.
- log (variant):

```text
TARGET daWanwan_c_classInit @ 0x02112d1c size 0xf0  bytes: 10402de908d04de2620ea0e3c5c1fceb0040b0e12700000a1770feeba4109fe5110e84e2001084e54e07fceb150e84e2010ffceb6d0f84e2c30cfceb88109fe5... (240 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0xf0 vs candidate 0xa0

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
