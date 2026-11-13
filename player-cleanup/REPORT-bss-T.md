# BSS batch T

Branch `bss/player-T`. Claimed `.bss` end: `0x02110154`. Symbols moved in `0x0210e14c..0x0210f914`: 0. The first symbol that does not match is `data_ov002_02110154`.

The continuation on `bss/player-wait` claims through `0x02110924`. `ST_WAIT` and `ST_OWL` are the other names of `data_ov002_02110154` and `data_ov002_02110244`; the link assigns those names. The five tables are in the same span. The notes below are the record of where this batch stopped.

## What landed

`src/game/player/d_a_ply_methods.cpp` already owned `.bss` `0x0210e14c..0x0210f914`. This batch adds the contiguous run from `0x0210f914` up to, and not including, `data_ov002_02110154`.

161 new zero-filled objects, inserted in address order immediately after `data_ov002_0210f908`. `data_ov002_0210e158` and `data_ov002_0210e7b8` stay after the new objects, in that order.

- 146 objects of 12 bytes, `0x0210f914` through `0x0210ffe0`. Each is `void *[3]`, except `data_ov002_0210f92c`, which is `int[3]`.
- 15 `daPly_c::State` objects of `0x18` bytes, `0x0210ffec` through `data_ov002_0211013c`. No initializer. `PlayerInitStates` writes the slots.
- Interior labels are not objects: `data_ov002_0210ffbf` is inside `data_ov002_0210ffbc`, and `data_ov002_02110101` is inside `data_ov002_021100f4`.

`config/arm9/overlays/ov002/delinks.txt` and the manifest `.bss` section both end at `0x02110154`. The manifest gained those 161 `.bss` rows only. `.data`, `.ctor`, the pinit pointer, and `defined_symbol_renames` are unchanged. No compiler temp name in the old range shifted, so the rename `from` values were left as they were.

## Why the claim stops at `0x02110154`

`data_ov002_02110154` and `_ZN7daPly_c7ST_WAITE` (`daPly_c::ST_WAIT`) are one address. `data_ov002_02110244` and `_ZN7daPly_c6ST_OWLE` (`daPly_c::ST_OWL`) are the same shape further on. One object each. The header already declares both static members.

Defining `daPly_c::ST_WAIT` emits one `.bss` object of `0x18` bytes under the mangled name. The call sites that still spell `data_ov002_02110154` stay an undefined import. `undefined_symbol_aliases` cannot point that import at the local definition: the isolate step refuses with `canonical alias destination _ZN7daPly_c7ST_WAITE already exists in object`. A second `daPly_c::State` under the `data_ov002` name is a second object. The BSS alias path only emits a label when its address is strictly inside the object, so a second name at the object's own start is not written into the link.

The five tables at `0x0211073c` through `data_ov002_02110884` sit past that address. A span that included them would contain the hole at `0x02110154`, so they were not claimed. The next claimed BSS, the star at `0x02110924`, is untouched.

## Proof

`python tools/tubuild.py linkcheck ov002/daPly_c` on this prefix:

- `.bss` `0x0210e14c..0x02110154` verified, emitted `0x2008`, 0 relocations
- `.text`, `.init`, and `.data` identical to the previous claim
- `dsd check symbols --fail`: 0 errors, 0 new against the baseline
- full ROM identical to stock (`ddab9300b24aeae254192c2a30d812b07ece3adff83230e75c865ed915c9de40`)
- result: scratch-data-verified

A linkcheck of the longer span, with `ST_WAIT`, `ST_OWL`, and the five zero-filled tables defined, stopped in isolate on the alias refusal above. It did not reach a byte compare.
