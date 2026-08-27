# The walk_window selftest BMP gate

`SM64DS_WINDOW_SELFTEST=N` runs the port's interactive binary for N frames with W
held, dumps the last framebuffer to `walk_window_selftest.bmp` next to the exe, and
exits. It is the port's only CI-checkable rendering gate: no window, no user, one
file to compare.

The gate is real, but it is not a pure function of the source, and reading it as one
produces false regressions. This note states the comparison rule, and records the
defect that made the rule necessary.

## The rule

**Compare selftest BMPs byte-exact only between builds whose `.dsstate` section
lands at the same base address with the same interior layout.** An equal base is
necessary and not sufficient (the doctrine block in `port/tools/battery.py`,
measured 2026-08-16): an insertion inside `.dsstate` shifts every hosted global
past it while leaving the section base exactly where it was, and the frame follows
the addresses. If base or span differ, the comparison says nothing.

`.dsstate` is the PE section holding the hosted DS globals (`hal/dsstate_seg.cpp`
brackets it with the `dsstate_lo` / `dsstate_hi` sentinels). Any change that grows
port code or data past a 4 KB page boundary moves that base, and a naive byte
compare then reports a difference with nothing wrong in the change. The difference
looks exactly like a rendering regression: a handful of pixels, plausible colours,
in a plausible place. A change that adds or grows a hosted global moves the
interior instead, to the same effect, with no base movement to flag it.

To compare across a layout change, pad the smaller build with an inert file-scope
`.data` blob in 4 KB steps, outside any DSSTATE bracket, until base and span both
agree, then compare. Read both from the `dsstate_guard` line the link prints, or
from the layout line the selftest prints beside the BMP. Verify any pad is actually
present in the .obj or the map before trusting what it measures; the retired row
below is what happens otherwise.

Do not answer a base- or span-shift failure by adopting a pixel budget. A tolerance
would hide the class of defect described below, which is what the gate exists to
catch.

## The measurement that established it

Four builds, base logic plus inert padding appended to `port/hal/host_settings.cpp`,
all reaching an identical final player position (-4915200, 2805556, 9342995):

| build | `.dsstate` base | BMP md5 |
|---|---|---|
| base as-is | 0x9ee000 | eb32dcab491562c27348aad32273cd8a |
| base + 16B inert `.bss` (retired) | 0x9ee000 | eb32dcab491562c27348aad32273cd8a |
| base + 4KB inert `.data` | 0x9ef000 | 518ba22ae2604117409491e0c4f38056 |
| base + 8KB inert `.data` | 0x9f0000 | 15fde8a893d010d8d46e3c7dc284ac47 |

The `.bss` row is retired as a null result (measured 2026-08-16, recorded in
`battery.py`): a volatile `.bss` pad is dropped by the compiler, absent from the
recompiled .obj and not merely from the map, so that build was the base build under
another name and its row measures nothing. Check any pad for presence in the .obj
or the map before believing what it measures.

Padding used: `extern "C" __declspec(dllexport) unsigned char rev_pad_data[4096] = {1};`

The delta is three pixels, max channel delta 12, all water blue, at (31,217),
(81,222) and (32,224) in the 512x384 frame. Same source, same position, three
different images. At 1000 frames the BMPs were byte-identical across the same three
bases, so the dependence is state and frame specific rather than universal.

## Root cause: a truncated hosted data block, not the render path

The address dependence was not a blend step or a draw-order step keyed off a pointer.
It was an array running off the end of the block that hosted it, into memory whose
contents depend on the image base.

The castle moat water is a texgen mode 1 material whose S/T translation comes from a
91 frame BTA track: `src/unnamed/arm9/0204/func_020469e8.c` reads `tableC[idx+frame]`,
`TextureTransformer::Update` supplies the frame, and `Animation::Advance` wraps modulo
the BTA header's 91. The track is one contiguous run of 91 Fix12 words at ov009 DS
address 0x021122ec.

Nothing named that track, so the only thing hosting it was the synthetic gap block
`port_ov009_gap_0211222c`. A gap block is sized by the next entry in dsd's
`symbols.txt`, and dsd guessed eleven `ambiguous` boundaries inside the track's span.
The first, at 0x02112320, ended the block 0xf4 bytes in, so the hosted copy was
thirteen of the ninety one words.

From animation frame 13 the read ran past the block into whatever the linker placed
next, which was the neighbouring gap block: the ASCII `"water_mat"` followed by a row
of rebased host pointers. Those pointer values travelled into the geometry stream as
the water's texture matrix translation, because `func_02044b30` emits `-(value << 9)`
as that translation. A host address was deciding pixels.

The delta stayed at three pixels only by coincidence of scale: a 4 KB shift is
exactly one period of the 32x32 water texture, so the UV moved by a whole texel
period and most of the surface landed back on itself.

The fix is one line in `port/ov009_syms.txt` naming the array with its ROM extent:

    data_ov009_021122ec:0x16c

Sizing the symbol reshapes the gap runs around it (`port_ov009_gap_02112228` grows
24 to 96 bytes, `port_ov009_gap_0211222c` shrinks 244 to 40, and a zeroed pad appears
up to `data_ov009_02112bc4`). Net hosted coverage is +312 bytes of real track and
-100 bytes of unreferenced neighbour span. The track also moves out of ordinary
`.data` into `.dsstate`, so save states now capture it.

## The general class

This is the same shape as the dust particle defect: a truncated romdata blob, not the
render code it appeared to implicate. Wherever a real array is hosted only by a
synthetic gap block, a dsd `ambiguous` boundary guess inside its span silently clips
it, and the overrun reads whatever the linker parked next.

A sweep of the 44 gap blocks found five more proven truncations (two ov009 path
tables, ov016 CLPS, an ov021 class name string, and an ov070 curve cut at its apex),
all byte-verified against the raw ndspy overlay images in `extracted/overlays/`
rather than the dsd export copies, which are stale or compressed for ov021.

Known leftovers, none of them fixed:

- ov009 0x02113104, a path array never hosted at all (contested window).
- ov002 plain mount named sizing, unaudited (383 targets, no `--pack` protection).
- Four spurious `kind:load` relocs inside `data_ov009_021133d4` in
  `config/arm9/overlays/ov009/relocs.txt`. They are dsd misreading s16 path
  coordinates that parse as ov006/ov007/ov089 addresses. Harmless while those
  targets are unhosted; the ov089 leg is one symbol line away from arming a cross
  pass overwrite. The banner in `port/ov009_syms.txt` documents it. Suppressing
  entries in shared dsd config is a call for the repo owner, so they are left in
  place deliberately.

## Status

The rule above is the durable part and applies whether or not the fix lands.

The fix itself is not on `main` and not on any remote. It sits on the local branch
`bmp-hunt` (six commits, based on the wave 6 port lane commit `daeeb9b29`, fix at
`95154b8a5`, tip `e4f25f862`), which is the only branch that carries it. The
selftest's `.dsstate` layout print landed on the same branch. `main`'s port tree
predates `walk_window` entirely, so none of this infrastructure exists there yet.
Post fix, the three layouts above were reported to produce a single md5.
