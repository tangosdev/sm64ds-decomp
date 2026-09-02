# PAL16 - sixteen Yoshis, sixteen colours

Lane: the colour half of the wide VS match. Three field reports from a
seven-player online game came in as three bugs and were two:

  (a) "player 5 looked like a copy of player 1"
  (b) "a player who picked black came out brown"
  (c) "pixels around the edges of the Yoshi texture did not get the colour
      treatment"

(a) and (b) are one defect with two visible faces and are fixed. (c) was
measured and is ROM-faithful behaviour, not a defect; the measurement is
below and the recolour set is unchanged.

Worktree `C:\tmp\pal16`, branch `port/pal16`, base `ad09cd9f5`
(port-mount-noseat-cluster). Private TEMP `C:\tmp\pal16tmp`, evidence
`C:\tmp\pal16-out` and `C:\tmp\pal16bld`. Nothing pushed, nothing merged.

`git diff --stat ad09cd9f5 -- src/` is empty: no byte-matched source moved.

---

## THE DEFECT, end to end

VS forces every seat to Yoshi. That is the cartridge's rule, four lines long,
at `hal/level_boot.cpp` (`data_0209f2d8 == 1` makes `f2` and `f1` both 3), so
seats are told apart by **palette row alone**. Two things stood between a wide
seat and its own row.

### 1. The row followed a slot number that had been truncated to two bits

`src/_ZN6Player13InitResourcesEv.cpp:72` unpacks the player number out of the
spawn flag word as `(a >> 6) & 3` -- **two bits**, boxed between the character
sub-index at bits 3..5 and the sublevel at bit 8. The port cannot express slot
4 in a spawn, so `hal/level_boot.cpp` packs a value that **cycles 1,2,3** (a
plain `i << 6` would set bit 8 at `i == 4` and corrupt the sublevel) and then
repairs `mPlayerNo` at `Player+0x6d8` to the true slot the moment
`Actor::Spawn` returns.

That repaired the **identity** and left the **colour** behind, because the
colour does not come from `mPlayerNo`. Six lines earlier, inside the same
`InitResources`, `port/unmatched/TexSeq_Caller_ov002_020e5948.cpp:406-408`
computes

    *(s32 *)(c + 0x61C) = material[0]->paletteBase + (mPlayerNo << 1);

from the value it had just unpacked, and `0x61C` is what every renderer
actually spends: `hal/player_bridges.cpp`'s `hal_player_vs_palette` stamps that
word into `+0x20` of every material the body and the head draw with. Nothing
recomputed it afterwards.

So the reuse was **not** "slot n wears slot (n mod 4)" -- which is what the
warning in `fs_mods.cpp` said, and it was wrong. It was slot n wearing slot
`((n-1) mod 3) + 1`:

| seat | packed | row drawn |
|---|---|---|
| 0 | 0 | 0 |
| 1 | 1 | 1 |
| 2 | 2 | 2 |
| 3 | 3 | 3 |
| 4 | 1 | **1** |
| 5 | 2 | **2** |
| 6 | 3 | **3** |

Seat 4 drew seat 1's row. That is report (a) exactly, at seven players: seat 5
and seat 2, seat 4 and seat 1. And it is report (b): a black pick in seat 5
was written into **row 5 of the colour table**, which nothing ever read, while
seat 5 drew whatever recoloured row seat 2 had been given. Another player's
recoloured brown is precisely what comes out.

**The fix.** `port_vs_spawn_extra_players` in `hal/level_boot.cpp` corrects
`0x61C` by the same delta that corrects `mPlayerNo`. One step of `mPlayerNo` is
exactly one sixteen-colour row (the base is in eight-byte units and ntr binds
at `base * 16`, so `<< 1` is +32 bytes), so the correction needs nothing the
actor does not already hold:

    *(int *)(a + 0x61c) += ((int)i - (int)packed) << 1;

It runs in two places for one reason each. A pass at the top of the function
repairs any slot the level's **own entrance table** seated with a truncated
number, keyed on the disagreement rather than on which function spawned it; the
main loop repairs its own slots using `wire_slot`, which is exactly what
`InitResources` unpacked. **For slots 0..3 the delta is zero in both**, so a
two- or four-player match repairs nothing and behaves exactly as it always has.

### 2. Under that, the palette only had four rows

`yoshi_all_16p_pl` in `data/player/yoshi_model.bmd` (ARCHIVE/arc0.narc member
196) is **128 bytes**: four stacked sixteen-colour rows. Fixing the selector
alone would have pointed seats 4..15 past the end of the palette.

`status/VS16HOST.md` recorded sixteen rows as owed and `fs_mods.cpp` said they
could not be authored from there, "because growing the palette means growing an
asset the game allocates from its own data rather than a host array the port
hosts". That was true about a host array and wrong about the seam: the fs
filter signature is `u32 (*)(unsigned fileID, u8 **data, u32 size)`, so a filter
may **replace the buffer** and return a new size. `lovesme_filter` has always
done it.

`yoshi_rows16_filter` (`hal/fs_mods.cpp`) appends a 512-byte palette at the end
of the served file and repoints the record (`+4` offset, `+8` size). Five
hundred and twelve appended, not three hundred and eighty-four: the cartridge's
own 128 bytes cannot serve as the first quarter of the new palette because the
bytes after them belong to other records, so the four ROM rows are copied up
with the twelve new ones. **Nothing else in the file moves**, which is the
whole reason for appending rather than rewriting in place.

Rows 0..3 with no custom colour are the cartridge's bytes verbatim. Rows 4..15
come from twelve authored default colours (`kVsDefaultBody` / `kVsDefaultShoes`
in `hal/vs_palette_gen.h`) through the **same family transform** as a player's
own pick, so a defaulted seat is shaded like every other seat rather than being
a flat colour. Every generated row, custom or default, is anchored on ROM row
`s % 4`, which keeps slots 0..3 doing precisely what they already did and means
seats 0, 4, 8 and 12 give byte-identical output for the same pick.

**The 512 bytes reach VRAM whole, and this was traced rather than assumed.**
The upload walk is `src/_ZN5Model13LoadTexAndPalER8BMD_File.cpp`, reached once
per unique BMD from `Model::DoSetFile`, and one `sz` read out of the record
drives the bounds test, the reservation and the copy length:

    sz = p->size;
    if (data_020a4bcc + sz > data_020a4bd8) Crash();
    ...
    data_020a4bd8 -= ((sz + 0xf) & 0xfff0);
    GX::LoadTexPltt(p->data, data_020a4bd8, sz);
    p->vramOffset = data_020a4bd8;

`Model::UpdateFileOffsets` rebases the record's pointers on load and leaves the
size at +0x08 alone, so the grown size survives the rebase. 512 is over the
`sz <= 8` threshold so it takes the falling arena from the 0x18000 ceiling, and
512 is already 16-aligned. **There is no 128, no 0x80, and no assumption of 16
or 64 colours anywhere between the BMD record and the host memcpy**, and no
fixed-length palette copy in `port/ntr/` or `port/hal/`.

The row then falls out of the units. `src/func_020462d0.c` sets a runtime
material's +0x20 to `vramOffset >> 4` for a sixteen-colour format, and ntr binds
at `PLTT_SLOT_BASE + base*16`, so a material base is a palette address in
sixteen-byte units. `Player+0x61C` is `base + (mPlayerNo << 1)`, and +2 in those
units is +32 bytes -- exactly one row. **Slot 15 lands at +480 of 512, inside.**

(An earlier draft of the `fs_mods.cpp` note cited `src/func_0204a028.c` as the
upload walk. That is the PARTICLE archive's palette walk -- its only callers are
`Particle::SysTracker::Initialise` and one ov007 site -- and no BMD goes through
it. It is size-driven in the same way, which is how the wrong citation survived
reading. Corrected in the source.)

It runs **last** in `mod_filter`, after `PaletteYoshi`, `CustomPalette` and the
lobby's own four rows, so whatever those wrote is the baseline the wide rows are
generated from rather than something they get to overwrite. And it is
**unconditional** rather than gated on session width or on `SM64DS_VS_COLORS`:
every peer has to agree about what everyone looks like, and a gate is one more
thing that can be true on one machine and false on another. The cost is 384
bytes of extra palette VRAM out of the 0x18000-byte window; rows 4..15 are
unreachable unless a player actually has `mPlayerNo >= 4`.

### Heads follow bodies for free

`hal_player_vs_palette` is called for the body model **and** the head model, and
both times it writes the **body's** `0x61C`. The head files carry their own
32-byte copy of body row 0 which is never sampled in a match; the head's
materials are re-pointed at the body palette, so a head samples body row
`slot` for whatever slot it is. That was already true and it keeps working
unchanged at sixteen rows, because the head's texture indices outside the body
and shoe groups are the ones the generator never touches.

`vsc_build`'s two 32-byte head records stay, still carrying slot 0's row, for
the case where a head is ever drawn from its own palette. Its **body** record
is retired: writing four rows there first would have applied the family
transform twice to rows 0..3.

---

## (c) THE EDGE PIXELS - measured, and it is the cartridge

The recolour set is body indices {0,1,2,3} and shoes {7,8}, derived by diffing
the ROM's four rows rather than from texel coverage, and the header said so and
said the coverage count had not been done. It has been done now.

Every 16-colour texture in the three Yoshi BMDs (arc0 members 194 `yoshi_head`,
195 `yoshi_head_fill`, 196 `yoshi_model`) was decoded and each palette index
counted, together with the fraction of its texels having a four-neighbour that
uses a **body** index 0..3 -- which is what an outline sitting on the body
boundary looks like. The parse was validated against `kVsRomRows` before any
number was believed.

| idx | texels | adj to body | ROM rows | | idx | texels | adj to body | ROM rows |
|---|---|---|---|---|---|---|---|---|
| 0 | 305 | 1.000 | MOVES | | 8 | 441 | 0.000 | MOVES |
| 1 | 1349 | 1.000 | MOVES | | 9 | 890 | 0.072 | SAME |
| 2 | 11060 | 1.000 | MOVES | | 10 | 921 | 0.000 | SAME |
| 3 | 356 | 1.000 | MOVES | | 11 | 504 | 0.006 | MOVES |
| 4 | 252 | 0.000 | SAME | | 12 | 631 | 0.258 | SAME |
| 5 | 372 | 0.000 | SAME | | 13 | 858 | 0.326 | SAME |
| 6 | 423 | 0.000 | SAME | | 14 | 987 | 0.226 | SAME |
| 7 | 558 | 0.048 | MOVES | | 15 | 8765 | 0.054 | SAME |

**The body ramp is its own anti-alias.** Index 0, the darkest green, has a body
adjacency of 1.000 and a neighbour profile of 67% itself and 25% index 1: it IS
the dark shading edge, and it is already in the group. There is no separate
outline index between the body and the background.

**Index 4 is the eye pupil.** All 252 of its texels are inside the eye sprites
in the two head files, none at all in the 128x64 body atlas, and its body
adjacency is 0 of 252. Its neighbours are 81% itself and 11% index 12: a solid
pupil blob surrounded by grey, not a one-texel outline.

**Index 11 is the saddle rim.** 3 of 504 texels border the body; its neighbours
are the saddle cluster (5, 6, 10). It DOES move between rows, so the artists
treated it as per-player, but it is not a body edge -- putting it in the body
group would tint the saddle rim to the body hue. That is the one judgement call
the header already documented, and this measurement supports the choice made.

**And index 11 has a second, independent answer.** `status/VSCOLORUI.md` (an
untracked local document in the main checkout, since `status/` is gitignored)
had already flashed index 11 magenta in all four rows through the shipped
PaletteYoshi mod and re-run a two-window match: **zero pixels of 393216
differed**, in both windows, against a control that flashed index 0 and moved
54 and 75 pixels. So index 11 is not merely off the body edge -- the drawn
Yoshi geometry does not sample it at all. That is exactly the evidence a texel
histogram cannot produce, because a histogram reads the ATLAS and not the
display lists, and it is why the two measurements are cited together rather
than either being taken as sufficient on its own.

**The belly ramp decision stands, and it is a decision, not a finding.**
Indices 12..15 are what actually borders the body, and they are identical in
all four ROM rows. Leaving them alone keeps every VS Yoshi's belly the white
the cartridge gives it. Tinting them is a look change the owner should make
knowingly; it is a two-line edit to `kBodyIdx` in `hal/vs_palette_gen.h` plus a
re-run of the identity test, which would then no longer hold against the ROM's
own rows. Recorded here per the coordinator's instruction to keep the call as
made and say so.

**What actually borders the body is 12, 13, 14 and 15** -- the white belly and
highlight ramp, at adjacencies of 0.23 to 0.33 -- and all four are
**byte-identical in every one of the cartridge's four rows**. All four retail VS
Yoshis have the same white belly, which is exactly what identical rows predict.
Recolouring them is what report (c) asks for on its face, and it is a deliberate
departure from the cartridge, so **it is not taken here**. If the owner wants
tinted bellies that is a look decision he should make knowingly, and it is a
two-line change to `kBodyIdx`.

**There are no format-5 textures anywhere in the Yoshi set.** All nine are
format 3, plain 16-colour 4bpp. The scouting note's theory that the
4x4-compressed decoder's two-word blend at a 14-bit palette offset
(`port/ntr/texture.cpp` case 5) could straddle a row boundary is inapplicable:
that code never runs on a Yoshi.

**One stated limit.** Adjacency is measured in texture space, so two UV islands
packed next to each other in the atlas count as adjacent. That bias inflates
adjacency and never deflates it, and the case against 4 and 11 rests on their
adjacency being near zero, so the bias runs against them. The probe counts
texels, not display-list references, so it cannot see an index used only by
geometry that is never drawn.

Raw numbers: `C:\tmp\pal16_probe\coverage.txt` and `neighbours.txt`.

---

## Two masks that were wrong at slot 4 and up

Both found by reading rather than by a failure, both in the same family as the
main defect.

- `hal/player_bridges.cpp` read `mPlayerNo & 3` to save and restore health
  across the door character swap. `data_02092144` has been sixteen wide since
  0.3.2 (`hal/cxx_aliases.cpp:70`), so a swap in slot 5 saved slot 1's health
  and then wrote slot 1's health back over whatever slot 1 had done in between.
  Read whole, bounded rather than masked.
- `hal/editor_channel.cpp`'s INFO row reported `Player+0x6d9 & 3` as the live
  character. `InitResources:76` seats that byte as `b & 7` and
  `hal/comms_conductor.cpp:828` already reports the ROM's three-bit width;
  two bits folded anything above 3 onto a different character and reported it
  as fact. Now `& 7`. The save-side array beside it is a different source and
  keeps its own width.

---

## PROOFS

### Unit: `port/tools/vs_palette_test.cpp`

    cl /nologo /std:c++14 /EHsc /O2 /I C:\tmp\pal16\port\hal ^
       C:\tmp\pal16\port\tools\vs_palette_test.cpp /Fe:vs_palette_test.exe
    vs_palette_test.exe

Verbatim, rc=0. The first six blocks are the pre-existing suite including the
identity; `rows16` is this lane's:

<!-- PROOF: vs_palette_test -->

### File shape: `port/tools/vs_rows16_bmd_check.py`

Reproduces the append-and-repoint against the cartridge's own member 196 and
asserts that only the palette record's own offset and size words change below
the old file length, that every other record still points where it pointed and
lies inside the grown file, and that `parse_bmd` still reads it. This is the
middle link between the unit test (colour arithmetic) and a real run (the
upload).

    python port/tools/vs_rows16_bmd_check.py

<!-- PROOF: bmd_check -->

### Build

<!-- PROOF: build -->

### Battery

<!-- PROOF: battery -->

### The wide session, with colours read back from inside the game

`port/tools/vs_pal16_proof.sh` runs N windows in one loopback VS session, muted
and minimized-never-activated per the standing rule, with
`SM64DS_VS_COLOR_PROBE` armed. `port/tools/vs_pal16_check.py` turns the probe
lines into a verdict on five separate claims. `SM64DS_VS_STATE_HASH=2` stays on,
so the same run also answers "and nothing desynced".

**Why a separate rig.** `vs16_ladder.sh` proves the simulation agrees and is
blind to colour by construction: sixteen perfectly-agreeing peers can all draw
the same green Yoshi, and the ladder calls that clean. It did, for the whole of
0.3.2.

<!-- PROOF: wide -->

### Screenshots

<!-- PROOF: shots -->

---

## GAPS, said plainly

<!-- GAPS -->
