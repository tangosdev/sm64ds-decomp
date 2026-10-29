# OV007 portrait pointer tables

[PR #4119](https://github.com/tangosdev/sm64ds-decomp/pull/4119) adds 15 pointer arrays totaling 1,776 bytes for five portraits: Mario front and side, and Yoshi plain, heart and star. Each has X-coordinate, Y-coordinate, and cumulative progress-threshold pointers. Coordinates are bytes; thresholds are signed 32-bit integers. There are 444 exact pointer relocations and 324 distinct imported leaves.

The first version incorrectly declared the pointer elements const and routed their compiler-emitted `.rodata` into the original `.data` interval `020d7f0c..020d85fc`. That was a source reconstruction error, even though the bytes matched. The correction makes the pointer elements mutable, retains const pointees, and routes the compiler's `.data` to `.data.portraitPointers`. It removes the invented readonly slice inside data.

The three Luigi pointer arrays (444 bytes) and all 435 unique coordinate/threshold arrays remain ROM supplied. Luigi stores 37 records and dispatch uses 36. A conflicting shared-header declaration for `data_ov007_020cd72c` blocks the Luigi migration; the header belongs to another active task. The generic path engine remains separate from the portrait controller.

Both this PR and its count-array dependency remain drafts during source-layout correction. The original candidate was `b4a4b4baeda61a84d306079eb6fb482a21b1ee88`, based on `a0a42091e573db67d0f3ad1788f067c10ee40cb6`. Prior byte results are historical; they do not constitute acceptance of the corrected source. Findings remain in the adjacent experiment JSON, and detailed historical measurements remain in Git history.

See [the correction and reproduction instructions](ov007-portrait-correction1018.md).
