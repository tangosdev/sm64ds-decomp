# Tree ApplyOpacity 2-word extern current state (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_tree.cpp:94-109` spells the measured
  `(Model *, u32, int)` extern (`line 109`) for ApplyOpacity's second
  parameter; already fixed on this branch, no attempted_change.
- attempted_change: none (current-state confirmation only).
- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_tree.cpp \
  --func _ZN8daTree_c6RenderEv --addr 0x020ec0a4 --size 0x134 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH (308 bytes, 0x134). The affected function is
  green with the 2-word extern in place.
- log (baseline):

```text
TARGET _ZN8daTree_c6RenderEv @ 0x020ec0a4 size 0x134  bytes: f04f2de91cd04de210119fe510219fe508108de5001092e5d4a080e20050a0e10040a0e11f60a0e31e7aa0e304009de5... (308 bytes)
  2004/b56: MATCH

========================================
MATCHING VERSIONS: 2004/b56
```

- note: this file only confirms the function is green with the extern in
  place. The evidence that the third word IS a parameter -- the 25-site
  call-site census, the ROM-wide base-rate control and the two
  register-copy sites the review asked about -- is in
  `batch3-2707-tree-applyopacity-census.md`.

- verdict: VERIFIED-CURRENT
