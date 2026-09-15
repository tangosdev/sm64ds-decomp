# Render `out` as int[3] vs stack Vector3 (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_tree.cpp:196-199` keeps `out` as `int[3]`
  because a stack `Vector3`'s inline dtor emits `Vector3D1`, which the
  production isolate refuses as unlicensed content.
- attempted_change (scratch-applied, then reverted):

```diff
-            /* Leftover: out is int[3], not Vector3: a stack Vector3's inline
-             * dtor emits Vector3D1, which production _isolate refuses as
-             * unlicensed content (this TU has no compiler_only_output row). */
-            int out[3];
+            Vector3 out;
             int dist =
                 _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(
                     data_0209f43c, &data_0209b3ec, &node->pos, kCanopyLift,
-                    (Vector3 *)out);
+                    &out);
```

- command (function body):

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_tree.cpp \
  --func _ZN8daTree_c6RenderEv --addr 0x020ec0a4 --size 0x134 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant Render body still MATCHes (0x134, no
  word differs). The claim is about an EXTRA emitted function, so the TU
  was also compiled with the production flags (`rombuild.CFLAGS`, C++
  form) and its defined symbols plus the production `rombuild._isolate`
  verdict (8 enrolled keeps, 9 deadstrip-data rows) were recorded.
- log (variant, function body):

```text
TARGET _ZN8daTree_c6RenderEv @ 0x020ec0a4 size 0x134  bytes: f04f2de91cd04de210119fe510219fe508108de5001092e5d4a080e20000a0e304108de50c008de510b08de20050a0e10040a0e11f60a0e31e7aa0e304009de5... (308 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- log (variant, TU emitted defined STT_FUNC symbols):

```text
DEFINED FUNC SYMBOLS:
  _ZN7Vector3D1Ev size=0x4
  _ZN8daTree_c13InitResourcesEv size=0x100
  _ZN8daTree_c16CleanupResourcesEv size=0x80
  _ZN8daTree_c16OnPendingDestroyEv size=0x4
  _ZN8daTree_c6RenderEv size=0x134
  _ZN8daTree_c8BehaviorEv size=0x54
  _ZN8daTree_cD0Ev size=0x54
  _ZN8daTree_cD1Ev size=0x40
  daTree_c_classInit size=0x5c
```

- log (variant, production isolate):

```text
PRODUCTION _isolate verdict: unlicensed content in text-only multi-symbol object: section[28] .text size 0x4 defines ['_ZN7Vector3D1Ev']
```

- log (baseline, same TU compile + isolate):

```text
DEFINED FUNC SYMBOLS:
  _ZN8daTree_c13InitResourcesEv size=0x100
  _ZN8daTree_c16CleanupResourcesEv size=0x80
  _ZN8daTree_c16OnPendingDestroyEv size=0x4
  _ZN8daTree_c6RenderEv size=0x134
  _ZN8daTree_c8BehaviorEv size=0x54
  _ZN8daTree_cD0Ev size=0x54
  _ZN8daTree_cD1Ev size=0x40
  daTree_c_classInit size=0x5c
PRODUCTION _isolate verdict: OK (no error)
```

- verdict: CONSTRAINT SUPPORTED
