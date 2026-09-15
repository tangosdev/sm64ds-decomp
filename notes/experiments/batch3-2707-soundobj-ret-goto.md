# SoundObj starter ret/goto shape vs early returns (BATCH3-2707-01, PR #2707)

- tested_commit: 9d3c80dd2 (cleanup-leftover-overlay-batch-3 at experiment time)
- compiler: mwccarm 2004/b56 (tools/mwccarm/2004/b56/mwccarm.exe)
- claim: `src/game/actors/d_a_sound_obj.cpp:153-166` keeps the ret/goto
  done shape because early returns un-match func_ov002_020f9468.
- attempted_change (scratch-applied, then reverted):

```diff
-    /* ret/goto done is this starter family's shape -- PlaySecretSound,
-       PlaySmallSecretSound and func_0200f7f0 all share it -- and early
-       returns un-match this function. */
-    int ret = 0;
     if (*counter < self->mCounterLimit - 0xf) {
         int vol = self->mVolume;
         _ZN5Sound7PlaySubEjjj5Fix12IiEb(self->mSoundID, vol, 0x7f, (0x7f - vol) << 0xc, self->mLoop);
         *counter += 1;
-        goto done;
+        return 0;
     }
     if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(self->mSoundID, 0x7f, 0, kPoolDist, self->mLoop))
         return 1;
-done:
-    return ret;
+    return 0;
```

- command:

```text
.venv/bin/python tools/match.py --c src/game/actors/d_a_sound_obj.cpp \
  --func func_ov002_020f9468 --addr 0x020f9468 --size 0x94 \
  --version 2004/b56 --module ov002 --strict-relocs --brief
```

- result: baseline MATCH; variant 17 word(s) differ at the same size
  0x94 (148 bytes). Early returns re-predicate the tail.
- log (variant):

```text
TARGET func_ov002_020f9468 @ 0x020f9468 size 0x94  bytes: 10402de908d04de20140a0e1be1dd0e1b020d4e10f1041e2010052e10b0000aae030d0e5d81090e57f20a0e300308de57f3061e2d40090e50336a0e1cd65fceb... (148 bytes)
  2004/b56: 17 word(s) differ

--- closest: 2004/b56 (17 differ) ---
```

- verdict: CONSTRAINT SUPPORTED
