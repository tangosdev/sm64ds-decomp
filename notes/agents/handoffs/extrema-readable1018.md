# Readable ov007 extrema helper

Base: `d343f7fd1688d33ae54848eda3a6d72ab17ad37f`.
Production scope: `src/unnamed/ov007/func_ov007_020c2f14.cpp`, ov007
`0x020c2f14..0x020c3054` (320 bytes).

## Change and review boundary

The C++ conversion in #4022 replaced named coordinates with an array and used
an intermediate copy to obtain matching output. This correction restores `x`,
`y`, and `z`, initializes `maximum` and `minimum` directly from `points[0]`, and
names the axis indices. Strict comparisons retain the first index on ties;
nullable output writes and their ordering are unchanged. The original function
requires at least one input point.

The private plain three-int record and configured C-linkage export remain.
C linkage preserves the existing symbol; it does not prevent using C++ in the
body. This is a readability correction, not recovery of an original class or
translation-unit identity. No header, caller, symbol, enrollment or filename is
changed. The sole source caller, `func_ov007_020c5c14`, supplies three-int records
and requests X/Y bounds; its existing declaration debt is outside this change.

## Producer checks

- Pinned 2004/b56 focused linkcheck: VERIFIED, 320 bytes, no differences or blind relocations.
- Changed-source formatting: zero findings or deferrals.
- Declaration-agreement ratchet: no new disagreements or copied header declarations.
- Whole-ROM build: 106/106 modules exact, 11,334 source-built functions;
  347/347 owned data claims reproduce. Committed-range linkcheck: 1/1 VERIFIED.
  Baseline control retains nine pre-existing symbol diagnostics; the broad
  ROM-data inventory reports one difference outside the owned-data claims.
- Independent source and byte verification is required before marking the PR ready.

## Bounded shared-type experiment

Replacing only the private plain record with the existing `Vector3` from
`types.h`, using the same two direct initializations, does not match under the
pinned compiler. Its nontrivial construction/destruction changes the generated
copies. This experiment supports retaining the plain record for this candidate;
it does not prove that every possible shared-type reconstruction is impossible.

Compiler: `2004/b56`; normal production flags selected by `tools/linkcheck.py`.
Extract the following C++ block verbatim to `build/extrema/vector3.cpp`, then run:

```powershell
python tools/linkcheck.py --module ov007 --name func_ov007_020c2f14 --c build/extrema/vector3.cpp --addr 0x020c2f14 --size 320
```

Experiment SHA-256 (UTF-8, LF): `f25a184717bdf3bbb709903b5c120bb64872a43d2c556dca7f494563ffaf96ba`.

```cpp
//cpp
// Find the first point at each axis minimum and maximum.
// points must contain at least one point; each output index is optional.
#include "types.h"

extern "C" {
void func_ov007_020c2f14(Vector3 *points, int pointCount, int *outMinX, int *outMaxX,
                         int *outMinY, int *outMaxY, int *outMinZ, int *outMaxZ) {
    Vector3 maximum = points[0];
    Vector3 minimum = points[0];
    int i;
    int minXIndex, maxXIndex, minYIndex, maxYIndex, minZIndex, maxZIndex;
    maxZIndex = 0;
    minZIndex = 0;
    maxYIndex = 0;
    minYIndex = 0;
    maxXIndex = 0;
    minXIndex = 0;
    for (i = 1; i < pointCount; i++) {
        if (points[i].x < minimum.x) {
            minXIndex = i;
            minimum.x = points[i].x;
        } else if (points[i].x > maximum.x) {
            maxXIndex = i;
            maximum.x = points[i].x;
        }
        if (points[i].y < minimum.y) {
            minYIndex = i;
            minimum.y = points[i].y;
        } else if (points[i].y > maximum.y) {
            maxYIndex = i;
            maximum.y = points[i].y;
        }
        if (points[i].z < minimum.z) {
            minZIndex = i;
            minimum.z = points[i].z;
        } else if (points[i].z > maximum.z) {
            maxZIndex = i;
            maximum.z = points[i].z;
        }
    }

    if (outMinX) {
        *outMinX = minXIndex;
    }
    if (outMinY) {
        *outMinY = minYIndex;
    }
    if (outMinZ) {
        *outMinZ = minZIndex;
    }
    if (outMaxX) {
        *outMaxX = maxXIndex;
    }
    if (outMaxY) {
        *outMaxY = maxYIndex;
    }
    if (outMaxZ) {
        *outMaxZ = maxZIndex;
    }
}
}
```

Observed result:

```json
{
 "name": "func_ov007_020c2f14",
 "module": "ov007",
 "addr": "0x020c2f14",
 "verdict": "NO-REPRO",
 "reason": "requested-size-mismatch",
 "diffs": [],
 "blind": 0,
 "expected_size": 320,
 "emitted_sizes": [
  276,
  280
 ]
}
```
