# Particle::System::NewSimple member form vs mangled scalar (FEATHER-2713-01, PR #2713)

- tested_commit: f9c1e4609a52783e8a611ba2608225d6e7419aac (cleanup-wingfeather at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/daFeather_c/daFeather_c.cpp:171,208` keeps
  Particle::System::NewSimple mangled scalar because the member form
  with Fix12<int> by value changes codegen (6az).
- attempted_change (scratch-applied, then reverted):

```diff
+namespace Particle { struct System { static void *NewSimple(unsigned, Fix12<int>, Fix12<int>, Fix12<int>); }; }
 // @symbol _ZN11daFeather_c8BehaviorEv
@@ first call site
-            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY, mPosZ);
+            Fix12<int> ps_x1; ps_x1.val = mPosX;
+            Fix12<int> ps_y1; ps_y1.val = mPosY;
+            Fix12<int> ps_z1; ps_z1.val = mPosZ;
+            Particle::System::NewSimple(0xd2, ps_x1, ps_y1, ps_z1);
@@ second call site
-                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, mPosY, mPosZ);
+                Fix12<int> ps_x2; ps_x2.val = mPosX;
+                Fix12<int> ps_y2; ps_y2.val = mPosY;
+                Fix12<int> ps_z2; ps_z2.val = mPosZ;
+                Particle::System::NewSimple(0xd2, ps_x2, ps_y2, ps_z2);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/daFeather_c/daFeather_c.cpp \
  --func _ZN11daFeather_c8BehaviorEv --addr 0x020b2e9c --size 0x280 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0x280 vs 0x2c8
  (640 -> 712 bytes, +0x48).
- log (variant):

```text
TARGET _ZN11daFeather_c8BehaviorEv @ 0x020b2e9c size 0x280  bytes: 10402de910d04de25c129fe50040a0e10000d1e5010050e30100a0030000a013000050e30f00001a0400a0e10277fdeb081090e5000051e30200001aed2e00eb... (640 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0x280 vs candidate 0x2c8

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
