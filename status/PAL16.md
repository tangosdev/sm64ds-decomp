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
exactly one sixteen-colour row (the base is in sixteen-byte units and ntr binds
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

## Three places that were wrong at slot 4 and up

Both found by reading rather than by a failure, both in the same family as the
main defect.

- `hal/player_bridges.cpp` read `mPlayerNo & 3` to save and restore health
  across the door character swap. `data_02092144` has been sixteen wide since
  0.3.2 (`hal/cxx_aliases.cpp:70`), so a swap in slot 5 saved slot 1's health
  and then wrote slot 1's health back over whatever slot 1 had done in between.
  Read whole, bounded rather than masked.
- `hal/player_bridges.cpp`'s LEGACY SWAP (`SM64DS_SWAP_LEGACY=1`) repacked the
  spawn word as `mPlayerNo << 6` and re-ran `Player::InitResources`, which
  unpacks `(a >> 6) & 3` and recomputes `+0x61C` from it. So a legacy swap in
  slot 5 came back as player 1 wearing row 1 -- the very defect this lane fixed
  at the spawn, reverted by the one other place a live Player is rebuilt
  mid-level. Caught in review, not by a run. It was wrong twice: `5 << 6` is
  0x140, so bit 8 also went in SET, and the function's own note promises bits
  8+ go in as 0 because they feed `func_ov002_020c7dd0`'s entrance type. The
  field is now masked going in and both halves of the identity are put back
  after, by the same delta the spawn uses. For slots 0..3 it is a no-op, which
  is every legacy swap ever taken.

  The port's other two callers of `InitResources` do not need this and are not
  changed: `level_boot.cpp`'s `ps_init` is the vtable slot, so it runs inside
  `Actor::Spawn` and is exactly what the spawn-side repair already corrects;
  `hal_player_init_resources` is the gate-10 smoke's direct entry, one Mario at
  slot 0 where the delta is zero by construction.

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

    identity: picking the ROM's own colours changes nothing
      ok   ROM row 0 regenerates itself
      ok   ROM row 1 regenerates itself
      ok   ROM row 2 regenerates itself
      ok   ROM row 3 regenerates itself
    determinism: same input, same bytes, twice
      ok   64 colour pairs, forwards then backwards, byte-identical
    fixed slots: 4,5,6,9,10,11,12,13,14,15 never move
      ok   240 colour pairs leave every fixed slot alone
    shading: the body ramp keeps its order and stays distinct
           208 picks tried
      ok   no pick inverts the ramp
      ok   no pick flattens it below three distinct shades
    hex parsing
      ok   lower case
      ok   upper case
      ok   a non-hex byte is refused
      ok   a space is refused
    golden: body 8a2be2 shoes ffd700 -> 4c8c 6cb1 7573 69d3 0022 075d 084e 12da 035f 107c 1093 1a99 56d7 675a 77bd 7fff
      ok   golden row
    rows16: the ROM's four rows survive a build with no picks
      ok   row 0 is the cartridge's, byte for byte
      ok   row 1 is the cartridge's, byte for byte
      ok   row 2 is the cartridge's, byte for byte
      ok   row 3 is the cartridge's, byte for byte
    rows16: all sixteen body ramps are distinct
      ok   no two seats wear the same body
    rows16: the shoe pairs are distinct as well
      ok   no two seats wear the same boots
    rows16: the fixed slots are fixed in all sixteen rows
      ok   ten fixed slots x sixteen rows all untouched
    rows16: a pick reaches its own seat and only its own seat
      ok   seat 9 wears its pick, anchored on ROM row 1
      ok   no other seat moved
    rows16: one pick, two seats on the same base row, same bytes
      ok   seats 0, 4, 8 and 12 are byte-identical

    all green

(rc 0. The identity block at the top is the pre-existing suite: picking each
ROM row's own colours reproduces that row exactly, which is what keeps a
no-custom-colour match byte-identical to the cartridge.)

### File shape: `port/tools/vs_rows16_bmd_check.py`

Reproduces the append-and-repoint against the cartridge's own member 196 and
asserts that only the palette record's own offset and size words change below
the old file length, that every other record still points where it pointed and
lies inside the grown file, and that `parse_bmd` still reads it. This is the
middle link between the unit test (colour arithmetic) and a real run (the
upload).

    python port/tools/vs_rows16_bmd_check.py

    source: C:\tmp\pal16\extracted\dsd\files\ARCHIVE\arc0.narc member 196, 14296 bytes
    the cartridge's own shape
      ok   yoshi_all_16p_pl is 128 bytes
      ok   its four rows are kVsRomRows
    the grown file
      ok   grew by exactly 512 bytes plus alignment padding
      ok   the appended block starts at or after the old end
      ok   nothing below the old length changed except the palette record's own offset and size words: ['0x26f8', '0x26fc', '0x26fd']
      ok   the record still has the same name and place
      ok   it now says 512 bytes
      ok   at the appended offset
      ok   and 512 bytes are really there
      ok   the palette is four-byte aligned
      ok   rows 0..3 are the cartridge's, byte for byte
      ok   rows 4..15 are all present and all distinct
    every other record still points where it pointed
      ok   the same set of records
      ok   nothing but the Yoshi palette moved or resized: -
      ok   every record lies inside the grown file: -
    it still parses
      ok   parse_bmd accepts the grown file
      ok   and reads the palette back as 512 bytes
      ok   with the same texture, material and bone counts as before

    all green

(rc 0.)

### Build

`port/build-port.cmd`, 32-bit MSVC + ninja, into `build/port`, private TEMP
`C:\tmp\pal16tmp`. **BUILD_OK**, all post-link guards green:

    alternatename_guard ... OK
    gxband_guard: gxbank 3/24 maps (floor 3), 13 members, 36 deltas; dtcm 3/24
      maps (floor 3), 1 members, 0 deltas; vsrank 3/24 maps (floor 3), 2
      members, 3 deltas; vsstar 3/24 maps (floor 3), 2 members, 3 deltas;
      ready 3/24 maps (floor 3), 2 members, 3 deltas -- layout OK
    tailjump_guard: 35 frames (7 A, 8 C, 22 veneer derived, 2 overlap), 97
      assertions over 24 map(s) -- forms OK

    walk_window.exe  5,470,720 bytes
      sha256 6ec49672af9569ed37263108d74d862c9883284cbde187d704545cf480d44c58
    smoke_player.exe 4,701,696 bytes

THE ONE EDIT MADE AFTER THIS BINARY WAS BUILT is a comment: a note in
hal/level_boot.cpp said the palette base was in eight-byte units when it is in
sixteen (func_020462d0 sets a material's +0x20 to vramOffset >> 4). The
conclusion it supports was right either way -- `<< 1` is +32 bytes, one row --
and no instruction changed, so every proof below still describes this tree.
Said out loud because "the artifact is older than the tree" is the failure
shape that hides behind a green report.

TWO BUILDS WERE KILLED BEFORE THIS ONE AND NEITHER WAS A COMPILE FAILURE, said
plainly because their logs look like failures and are not. `FAILED:` lines with
no diagnostic under them are processes being killed, not code being rejected.
The first died when a ten-minute foreground wait loop timed out and took the
build's process tree with it; the second when `start /b` left the build on a
console that then went away. The third ran under `start` with its own console
and completed. Nothing in the tree changed between them.

### Battery

`python port/tools/battery.py --skip-build` (the build above is the one it
would otherwise make), log `C:\tmp\pal16bld\battery.log`.

    smoke_fs.exe: ok        smoke_gx.exe: ok        smoke_heap.exe: ok
    smoke_model.exe: ok     smoke_modelanim.exe: ok smoke_oam.exe: ok
    smoke_objwin.exe: ok    smoke_persist.exe: ok   smoke_player.exe: ok
    smoke_roots.exe: ok     smoke_savestate.exe: ok smoke_sdat.exe: ok
    smoke_soak.exe: ok  soak: 455 models, 455 rendered (89840 tris total),
                            0 empty, 0 load-fail, 0 faulted
    smoke_soak_anim.exe: ok  anim soak: 473 compatible pairs, 473
                            animated+rendered, 0 empty, 0 load-fail, 0 faulted
    levels: 50 mounted, from hal/level_boot.cpp
    scenes: 34 hosted, from hal/scene_boot.cpp
    default boot: ok -- a bare launch reaches the TITLE, 300 frames clean
    linkage: 9139 (80.7%)
    ptr_audit: 0 unhosted code pointers
    battery: ALL GREEN
    BATTERY_RC=0

Linkage 9139 (80.7%) is **unchanged** from the base, which is the honest
reading rather than a win: nothing in this lane adds or removes a linked
matched function. Pre-existing skips, re-probed bare on every run and not this
lane's: level 27 without TTC_MOVING_BEAM, level 45 without GOOMBOSS.

### The legacy-swap fix, rebuilt and exercised

The review fix above landed after the battery, so it was rebuilt incrementally
and the path it touches was driven live rather than re-argued.

    ninja -C build/port
    [6/9] Building CXX object ...\hal\player_bridges.cpp.obj
    [7/9] Linking CXX executable smoke_player.exe
    [8/9] Linking CXX executable walk_window_hires.exe
    [9/9] Linking CXX executable walk_window.exe
    dsstate_guard: OK -- 14233 hosted DS symbols all inside .dsstate
    NINJA_RC=0

One TU recompiled and three executables relinked, which is exactly the blast
radius. `vs_palette_test` re-run: all green (unchanged -- it does not touch this
file, and re-running it is the cheap way to say so rather than assume it).
`smoke_player.exe`: `Mario walks on the castle grounds (gates 10+11 GREEN)`,
rc 0.

**And the changed path itself**, which no proof in this lane had exercised:

    SM64DS_SWAP_LEGACY=1 SM64DS_SELFTEST_SWAP=1,2,3,0
    SM64DS_WINDOW_SELFTEST=600 SM64DS_FAULTS_FATAL=1

    [chr] f30 selftest swap -> 1    [chr] f30 swap PASS: param1=1 want=1
    [chr] f50 selftest swap -> 2    [chr] f50 swap PASS: param1=2 want=2
    [chr] f70 selftest swap -> 3    [chr] f70 swap PASS: param1=3 want=3
    [chr] f90 selftest swap -> 0    [chr] f90 swap PASS: param1=0 want=0
    rc=0, no faults

Four legacy swaps, all passing, on the rebuilt binary. THIS IS THE SLOT-0 ARM
ONLY: a single-player level has one Player at slot 0, so it proves the masking
did not break the path everyone actually uses, and it does NOT exercise the
slot-5 case the fix is for. That case has no harness -- the legacy swap is a
debug fallback and nothing wires it to a wide VS session -- so it rests on the
delta arithmetic, which is the same arithmetic the seven-window run proved at
the spawn. Stated rather than glossed.

### Palette VRAM headroom

`sh port/tools/vs_pal16_headroom.sh`, a solo VS arena boot with
`SM64DS_TEX_LOG=1`, 400 frames. Large palettes fall from `data_020a4bd8` =
0x18000 and small ones rise from `data_020a4bcc` = 0, and each bind's `paloff`
is a palette's own VRAM byte offset, so the widest gap in the sorted offsets IS
the free space between the two arenas.

    distinct palette offsets: 46, from 0x00000 to 0x17ed0
    rising arena (small palettes, up from 0)            tops out at 0x00000
    falling arena (large palettes, down from 0x18000)   floors at 0x113e0
    FREE SPACE BETWEEN THEM: 0x113e0 bytes (70624)
    the sixteen-row palette costs 0x180 (384) more than the ROM's four,
    which is 0.54% of that gap

So the change spends 384 bytes out of roughly 70 KB of daylight. And the growth
was confirmed to actually happen, from the same run:

    [mods] yoshi_all_16p_pl grown to 16 rows (128 -> 512 bytes at +14296);
           every VS seat has its own colours

Measured on a SOLO boot. Seven players do not multiply it: the Yoshi model is
loaded once and shared, and every seat indexes rows inside that one palette,
which is the entire design.

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

**The run.** `sh port/tools/vs_pal16_proof.sh 7 900 0`, seven windows, one
loopback session, arena 0 (level 51), ports 42940..42946, 900 frames per child
and 1800 for the parent. Every window muted (`SM64DS_VOLUME=0`), minimized and
never activated (`SM64DS_MINIMIZED=1`, `SM64DS_NO_FOCUS=1`). Output
`C:\tmp\pal16-out\pal7`.

Sixteen-field `SM64DS_VS_COLORS`, **slot 5 = `000000:000000`**, with seats 3,
7, 11 and 15 left empty so a defaulted seat is exercised in the same session:

    ff0000:0000ff,00ff00:ff00ff,0000ff:ffff00,,ff8800:004488,000000:000000,
    00ffff:880000,,ff00ff:00ff88,8800ff:ffcc00,ffffff:cc0000,,00ff88:660066,
    ff99cc:330066,999999:0044cc,

**The colour verdict**, `python port/tools/vs_pal16_check.py
C:/tmp/pal16-out/pal7 --expect "5=0000,0000,0000,0000"`, rc 0:

    windows: p0, p1, p2, p3, p4, p5, p6
      p0 sees 7 seats: 0=0457/001f/2dbf/425c, 1=1ec7/2362/03e0/37b4, 2=1c43/2845/3066/7c00, 3=167c/22de/033f/579e, 4=09b8/023f/3ebf/531d, 5=0000/0000/0000/0000, 6=41c7/5228/5e89/7fe0
      p1 sees 7 seats: 0=0457/001f/2dbf/425c, 1=1ec7/2362/03e0/37b4, 2=1c43/2845/3066/7c00, 3=167c/22de/033f/579e, 4=09b8/023f/3ebf/531d, 5=0000/0000/0000/0000, 6=41c7/5228/5e89/7fe0
      p2 sees 7 seats: 0=0457/001f/2dbf/425c, 1=1ec7/2362/03e0/37b4, 2=1c43/2845/3066/7c00, 3=167c/22de/033f/579e, 4=09b8/023f/3ebf/531d, 5=0000/0000/0000/0000, 6=41c7/5228/5e89/7fe0
      p3 sees 7 seats: 0=0457/001f/2dbf/425c, 1=1ec7/2362/03e0/37b4, 2=1c43/2845/3066/7c00, 3=167c/22de/033f/579e, 4=09b8/023f/3ebf/531d, 5=0000/0000/0000/0000, 6=41c7/5228/5e89/7fe0
      p4 sees 7 seats: 0=0457/001f/2dbf/425c, 1=1ec7/2362/03e0/37b4, 2=1c43/2845/3066/7c00, 3=167c/22de/033f/579e, 4=09b8/023f/3ebf/531d, 5=0000/0000/0000/0000, 6=41c7/5228/5e89/7fe0
      p5 sees 7 seats: 0=0457/001f/2dbf/425c, 1=1ec7/2362/03e0/37b4, 2=1c43/2845/3066/7c00, 3=167c/22de/033f/579e, 4=09b8/023f/3ebf/531d, 5=0000/0000/0000/0000, 6=41c7/5228/5e89/7fe0
      p6 sees 7 seats: 0=0457/001f/2dbf/425c, 1=1ec7/2362/03e0/37b4, 2=1c43/2845/3066/7c00, 3=167c/22de/033f/579e, 4=09b8/023f/3ebf/531d, 5=0000/0000/0000/0000, 6=41c7/5228/5e89/7fe0

    1. every seat wears a different body ramp
      ok   seat 0 never changed colour mid-run
      ok   seat 1 never changed colour mid-run
      ok   seat 2 never changed colour mid-run
      ok   seat 3 never changed colour mid-run
      ok   seat 4 never changed colour mid-run
      ok   seat 5 never changed colour mid-run
      ok   seat 6 never changed colour mid-run
      ok   no two seats share a ramp: -

    2. the row index is the TRUE slot
      ok   seat 0 has one stable palette base
      ok   seat 1 has one stable palette base
      ok   seat 2 has one stable palette base
      ok   seat 3 has one stable palette base
      ok   seat 4 has one stable palette base
      ok   seat 5 has one stable palette base
      ok   seat 6 has one stable palette base
      ok   base(slot k) - base(slot 0) == 2k for every seat: -

    3. the latch agrees with the selector, and geometry was drawn
      ok   seat 0 TEXPLTT_BASE == Player+0x61C ([5515] vs [5515])
      ok   seat 1 TEXPLTT_BASE == Player+0x61C ([5517] vs [5517])
      ok   seat 2 TEXPLTT_BASE == Player+0x61C ([5519] vs [5519])
      ok   seat 3 TEXPLTT_BASE == Player+0x61C ([5521] vs [5521])
      ok   seat 4 TEXPLTT_BASE == Player+0x61C ([5523] vs [5523])
      ok   seat 5 TEXPLTT_BASE == Player+0x61C ([5525] vs [5525])
      ok   seat 6 TEXPLTT_BASE == Player+0x61C ([5527] vs [5527])
      ok   every seat submitted triangles behind its palette: -

    4. every window agrees about every seat
      ok   p1 drew all 7 seats: missing -
      ok   p1 paints every seat the same colour p0 does: -
      ok   p2 drew all 7 seats: missing -
      ok   p2 paints every seat the same colour p0 does: -
      ok   p3 drew all 7 seats: missing -
      ok   p3 paints every seat the same colour p0 does: -
      ok   p4 drew all 7 seats: missing -
      ok   p4 paints every seat the same colour p0 does: -
      ok   p5 drew all 7 seats: missing -
      ok   p5 paints every seat the same colour p0 does: -
      ok   p6 drew all 7 seats: missing -
      ok   p6 paints every seat the same colour p0 does: -

    5. a named pick lands on its own seat
      ok   seat 5 wears 0000,0000,0000,0000 (got 0000,0000,0000,0000)

    all green

**The bases are the whole fix in one line:** 5515, 5517, 5519, 5521, 5523,
5525, 5527 -- exactly +2 per seat, all the way through seats 4, 5 and 6, which
before this lane collapsed back onto 5517, 5519 and 5521. Seat 3 comes out
`167c 22de 033f 579e`, which is ROM row 3 verbatim: the defaulted path below
four is the cartridge's own bytes.

**No divergence.** `port/tools/dhdiff.py` over all 21 pairings of the seven
windows, on both the walking run and the still capture run:

    C:/tmp/pal16-out/pal7        pairings NO DIVERGENCE: 21   other: 0
    C:/tmp/pal16-out/still/pal7  pairings NO DIVERGENCE: 21   other: 0

**And the lockstep result is not vacuous**, which is the trap the standing rule
names: lockstep is blind to motionless players. Per-actor state-hash churn over
the walking run, actorID 191 (PLAYER), from p0's log:

    uid 1=1739  2=1739  3=1733  4=1699  6=1739  7=1739  8=1514
    MINIMUM across the seven player actors: 1514 distinct states

Every seat moved through 1500+ distinct states. Seven agreeing statues would
have produced exactly the same NO DIVERGENCE and this number would have been 1.

**Pixels, not just palettes.** `port/tools/vs_pal16_census.py` counts, against
the exact words the game reported binding, how many pixels each seat painted in
the captured frame. From the still run's parent window:

    seat 0  556 px   seat 1  980 px   seat 2  294 px   seat 3  318 px
    seat 4  484 px   seat 5  (below)  seat 6  617 px
    seats with pixels on screen: [0, 1, 2, 3, 4, 6]

SEAT 5 COUNTS ZERO AND IS NOT MISSING. Its whole body ramp is
`0000,0000,0000,0000`, and 0000 is also the background and the outline in a
great many places, so counting it would count the sky. What says seat 5 is
really there is the engine-side probe: base 5525 (row 5), **456 triangles per
frame over 1300 frames, the same as seat 0**. It is a fully drawn Yoshi that is
black, which is what was asked for.

### Screenshots

`C:\tmp\pal16\status_shots\`, with a `README.txt` beside them carrying the
colour assignment and the census. Five 512x384 captures:

    still_p0_parent.bmp     the still run, parent window -- six of seven seats
                            in one frame, seat 5 black
    still_p1_child.bmp      the still run, a child window
    still_p4_child.bmp      the still run, another child
    walking_p0_parent.bmp   the walking run, parent
    walking_p1_child.bmp    the walking run, a child

The still run is `PAL16_STILL=1`: nobody holds a direction, so all seven stay in
the spawn ring and one camera sees the field. The walking run scatters them and
a single camera sees three or four, which is why both are kept -- the walking
run is where the lockstep sweep and the movement assertion come from.

I have not judged these beyond "distinct", which is the census above and a count
rather than an opinion. The look is the owner's call.

ONE HONEST NOTE. Every CHILD window's capture is byte-identical to every other
child's; only the parent's differs. Children all render the same camera in a
headless selftest run. That is a harness property and not this change -- the
same pattern is in the walking run and in the baseline -- and it does not weaken
the colour result, which was read independently inside all seven processes and
agreed in all seven. But it does mean "two windows" here is the parent and a
child, not seven genuinely different viewpoints.

---

## GAPS, said plainly

**Everything below is stated because it is NOT proven, not because it failed.**

1. **Loopback only.** Every run here is seven processes on one box through the
   comms loopback. Nothing was taken over the relay, which is the same limit
   every VS proof in this tree has had.

2. **Every child window renders the same camera** in a headless selftest, so
   the seven captures are two distinct images, not seven. Pre-existing, not
   this lane's, and it does not touch the colour result (read independently in
   all seven processes, agreed in all seven). Worth a lane of its own if the
   owner wants seven real viewpoints in a capture.

3. **Seat 5's blackness is proven by the probe, not by the census.** A pure
   black body ramp cannot be told from the background by counting pixels. The
   engine-side evidence is strong (base 5525, 456 tris/frame over 1300 frames)
   but the picture is the owner's to confirm.

4. **The twelve default colours are authored, not derived.** There is nothing
   in the cartridge to derive a fifth VS Yoshi from. They are checked to be
   pairwise distinct in body ramp and shoe pair (`test_rows16`), and that is a
   different claim from "they look good together at speed on a small screen".
   Changing one is a one-line edit to `kVsDefaultBody` / `kVsDefaultShoes`.

5. **The belly ramp 12..15 is untouched by decision.** See the measurement
   section: those four indices are what actually borders the body, and they are
   byte-identical in all four ROM rows, so every retail VS Yoshi shares a white
   belly. Report (c) asks for them to be tinted on its face. That is a
   deliberate departure from the cartridge and is the owner's call, not mine;
   the edit is two lines in `kBodyIdx` plus accepting that the identity test
   would no longer hold against the ROM's own rows.

6. **A spawn past the fourth entrance record is still packed wrong upstream.**
   The repair pass fixes the palette row and the player number for any slot
   whose `mPlayerNo` disagrees with its index, but the ROM's own loop packs
   `i << 6`, and at `i == 4` that sets bit 8 and corrupts the SUBLEVEL field
   before anything downstream can see it. The four VS arenas carry exactly four
   player records so the path does not arm today, and it did not arm in any run
   here (zero "came up with mPlayerNo=" lines). It is a real edge and it is not
   this lane's to fix.

7. **The palette VRAM headroom is measured on one arena.** 70,624 bytes free on
   arena 0. Arenas 1..3 and the adventure levels were not measured, and a level
   already near the ceiling gets 384 bytes closer to `LoadTexAndPal`'s
   `Crash()`. The battery boots all 50 mounted levels and all 34 scenes clean on
   this build, which is the broad evidence that nothing was pushed over, but it
   is not the same as a per-level headroom number.

8. **`SM64DS_VS_COLORS` above four fields has never been driven by the lobby.**
   The sixteen-field grammar was already parsed before this lane; what changed
   is that fields 4..15 now reach a real row instead of being dropped. Whether
   the lobby and launcher actually send sixteen fields is a different lane's
   question.

9. **Head models were reasoned about, not separately captured.** Heads sample
   the BODY palette at the body's own base, so they follow the seat for free and
   the captures show heads on all six visible seats. No probe was pointed at a
   head material specifically to confirm the row index there.
