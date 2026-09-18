# Sound::PlaySub mangled scalar vs method form (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_sld_mng.cpp:13-14` keeps PlaySub mangled
  (Fix12<int> by value, wall 6az; the file comment at lines 28-31 says the
  method form grows Behavior 0xa4 -> 0xb8).
- attempted_change (scratch-applied, then reverted):

```diff
 #include "daSldMng_c.h"
+#include "math/Fix12.h"
+
+namespace Sound {
+int PlaySub(u32 soundID, u32 volume, u32 pan, Fix12<int> distance, bool loop);
+}
 
 extern "C" {
 extern u16 DecIfAbove0_Short(u16 *timer);
-/* ABI wall: spelling this as Sound::PlaySub(..., Fix12<int>, bool) makes
- * mwccarm home the by-value Fix12 in an 8-byte stack slot, growing Behavior
- * from 0xa4 to 0xb8. The cartridge passes the same raw fixed-point word in
- * r3, so the measured scalar call view is retained. */
-extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(
-    u32 soundID, u32 volume, u32 pan, Fix12i distance, bool loop);
 }
@@ Behavior:
+    Fix12<int> dist;
+    dist.val = 0x15666;
     switch (mState) {
-            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
-                0x20, 0x14, 0x7f, 0x15666, false);
+            Sound::PlaySub(0x20, 0x14, 0x7f, dist, false);
-            _ZN5Sound7PlaySubEjjj5Fix12IiEb(
-                0x20, 0x7f, 0, 0x15666, false);
+            Sound::PlaySub(0x20, 0x7f, 0, dist, false);
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_sld_mng.cpp \
  --func _ZN10daSldMng_c8BehaviorEv --addr 0x02112678 --size 0xa4 \
  --version 2004/b56 --module ov019 --strict-relocs --brief
```

- result: baseline MATCH; variant 999 word(s) differ, size 0xa4 vs 0xb4
  (164 -> 180 bytes). The method form still un-matches, but the current
  tree measures 0xb4, not the 0xb8 the file comment records -- a 4-byte
  drift from intervening cleanups, not a refutation. (A per-case-local
  spelling measured 0xbc; the mangled-name-with-class-param spelling
  measured the same 0xb4.)
- log (variant):

```text
TARGET _ZN10daSldMng_c8BehaviorEv @ 0x02112678 size 0xa4  bytes: 10402de908d04de20040a0e1d610d4e5000051e30200000a010051e30f00000a1a0000ead0f8fbeb060750e3170000aa68309fe500c0a0e32000a0e31410a0e3... (164 bytes)
  2004/b56: 999 word(s) differ

--- closest: 2004/b56 (999 differ) ---
  size differs: target 0xa4 vs candidate 0xb4

========================================
MATCHING VERSIONS: none
```

- verdict: CONSTRAINT SUPPORTED
