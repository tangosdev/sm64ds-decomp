# OV007 path construction readability

Task `ov007-path-readability1018` continues [issue #4112](https://github.com/tangosdev/sm64ds-decomp/issues/4112) and `data-ov007-pathsets-native1018`, on base `3d76ff61c15e9e00d6d9ea317f4400aa1649764b`.

Three existing builders now use one shared `Ov007StrokePath` layout and common declarations:

| Entry point | Operation | Code bytes |
|---|---|---:|
| `020c2acc` | Expand byte X/Y coordinates to Q12 and update segment geometry | 108 |
| `020c2b38` | Copy existing Q12 X/Y coordinates and update segment geometry | 100 |
| `020c2b9c` | Allocate a planar path, copy optional progress thresholds, set point count | 92 |

Named fields replace the local `struct O` and raw accesses at offsets 8, 0x20, 0x24 and 0x28. Function symbols, opaque pointer interfaces, data ownership and delink configuration are unchanged. The header's builder declarations retain C linkage for C++ callers.

## Layout evidence and limits

Allocator `020c2c68` allocates 0x30 bytes, stores representation and flags at 0/4, capacity at 0x0a, and allocates the arrays at 0x18 through 0x2c. Reset `020c28ac` clears the active count at 8 and uses capacity to initialize those arrays. `020c2d44` computes segment lengths and angles from the planar coordinates. `020c5014` reads the optional progress thresholds during interpolation; literal timing units remain unknown.

`020c26fc` stores the indices returned by `020c3054` at 0x0c/0x0e and 0x12/0x14. These are indices of coordinate extrema, not bounding coordinates. The third slots are only known to be cleared. Signed 16-bit index slots follow the existing short stores; the store instructions alone do not recover signedness uniquely. Alternate point storage is left opaque: only its 12-byte stride is needed here.

The size assertion is for the 32-bit target ABI. It does not establish 64-bit host compatibility or independently prove every field offset. The allocator/consumer evidence establishes the layout; relocated byte checks establish the offsets actually accessed by the three edited functions. No current `port/` reference names these three sources.

This is a bounded source improvement, not recovery of an original class or TU. The other engine consumers retain their existing views. The lower allocator's integer-address return and differing pointer declarations require a separate consumer-wide repair; this change preserves that existing interface. The portrait-controller reconstruction and its draft PRs are separate work.

## Reproduction

With the pinned compiler and extracted ROM installed, from the repository root:

```sh
python tools/affected_src.py include/Ov007StrokePath.h
python tools/prepush_linkcheck.py --range 3d76ff61c15e9e00d6d9ea317f4400aa1649764b..HEAD
python tools/check_decl_agreement.py --changed 3d76ff61c15e9e00d6d9ea317f4400aa1649764b
python tools/tubuild.py linkcheck --baseline --module ov007 -j16
python tools/rombuild.py -j16
python tools/port_refcheck.py
```

The header has exactly three source consumers. Initial strict verification reproduces all three functions (300 bytes) and their relocation destinations, with zero warnings; the declaration ratchet introduces no new disagreements. Final runtime and independent review results belong to the exact commit's queue receipt and PR checks. Existing baseline symbol diagnostics are distinct from new errors. Publication does not authorize a merge.
