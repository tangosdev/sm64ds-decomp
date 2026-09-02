# NAMETAG - names and star counts over the other players' heads

Lane: NAMETAG. Tango's ask: in an online VS match, draw each OTHER player's
username above their head and their star count above the name, in the game's
own Mario-style font rather than a generic one.

Worktree `C:\tmp\nametag`, branch `port/nametag`, base `ad09cd9f5`
(port-mount-noseat-cluster's tip). Own build dir `C:\tmp\nametag\build\port`,
evidence `C:\tmp\nametag-out` and `C:\tmp\nametag\status_shots`. Nothing
pushed, nothing merged.

`git diff --stat ad09cd9f5 -- src/` is EMPTY. No byte of matched source moved.

---

## What shipped

Four files, one of them new.

| file | what changed |
|---|---|
| `port/tests/nametag.h` | **new.** The whole feature: the ROM font decode, the world-to-screen projection, the slot rules and the draw. Header-only because `tests/walk_window.cpp` is the only thing that has an `OvlSurface` to draw on, and both walk_window targets compile that file. |
| `port/tests/walk_window.cpp` | three lines: the include, `nt_stash_view()` where the camera has just been pushed, `nt_draw(surf)` beside the other host overlays. |
| `port/hal/star_flow.cpp` | `port_vs_slot_name` / `port_vs_slot_stars`, so the tag and the end banner read one source for the nickname and one for the score. |
| `port/hal/host_settings.{h,cpp}` | the `NameTags` key, default on, live-reloading, with `SM64DS_NAME_TAGS` as the proof-run override. |

### The glyphs really are the cartridge's

This was the part worth doing properly, and every step is read off matched
`src/` rather than guessed. The full derivation is the banner at the top of
`port/tests/nametag.h`; the short form:

- `LoadFont` (`src/LoadFont.cpp`) and `LoadFont3D` (`src/LoadFont3D.c`) both
  copy **file 0x980e**, 0x4000 bytes, to `func_02054d88()` - engine A's BG
  character base in VRAM. Either arm of `Stage::LoadGraphics2D` gets there, so
  the font is up in a VS match and in adventure alike. The port maps DS VRAM at
  its real address (`ntr/mmio.h` `VRAM_BASE 0x06000000`), so the tag reads the
  same bytes the game's own `Message::AddChar` reads, with no second load and
  no file of our own.
- `Message::AddChar` (`src/_ZN7Message7AddCharEc.cpp`) settles the layout
  exactly: the offset is `((c & 0x1f) + ((c & 0xe0) << 1)) << 5`, it reads
  0x10 words, and the `ip += 0xf8` after the eighth word makes the second tile
  **1024 bytes** further on. That is a 32-tile-wide sheet and an **8x16** 4bpp
  glyph. Nothing here is a guess about the format.
- Widths come from `data_0208f074[c]`, the ROM's own proportional advance
  table, already emitted as real bytes by `port/tools/romdata.py`.
- Character codes are `hal/message_boot.cpp`'s ranges, which that file derived
  off AddChar for the message dumper: digits `0x00..0x09`, `A..Z 0x0a..0x23`,
  `a..z 0x2d..0x46`, space `0x4d`. **The font has lowercase**, so a nickname
  renders as the player typed it.

The atlas is decoded once, the first frame the font is actually in VRAM. The
probe for "actually" is the letter A having ink, not the base pointer being
non-null - `func_02054d88` answers an address in any mode that has one, so a
build that trusted the pointer would cache 96 blank glyphs on frame one and
never look again.

### Where on the screen

There was no world-to-screen helper in this port. `nametag.h` has one, and it
is the renderer's own arithmetic rather than a second opinion:

- **view**: `v' = p * R + T` over `data_0209b3ec`, the `Matrix4x3`
  `View::Render` parks through `CopyToViewMat`. This is the same expression
  `hal/particle_bridges.cpp` already writes out for the fx probe. Its
  translation row is in SCENE units because `Camera::Render` feeds `LookAt_`
  its eye and look-at as `(v + 4) >> 3`, so the actor's Fix12 WORLD position
  crosses that same divide first.
- **projection**: `ntr::gx_debug_proj`, row-vector, which is `ntr/gx.cpp`'s own
  `project()` convention (`mul(mul(v, pos), proj)`).
- **screen**: `ntr/gx.cpp`'s `to_screen`, term for term, viewport included
  (`ntr::gx_debug_viewport`, falling back to full screen when the frame issued
  no viewport command).

Both matrices are **stashed** at the point in the frame loop where the camera
has just been pushed, not read at draw time. By the time an overlay runs the
frame has drawn its 2D layers and whatever projection they left behind is the
live one. One stash covers both camera arms by construction: on the old
hand-tuned rig `data_0209b3ec` is the identity walk_window seeds it with and
the view is folded into the projection `push_camera` makes.

Behind the camera and off-screen are culled. The tag steps up one size when the
player is close and that is the whole distance treatment - a continuous scale
shimmers as it rounds to whole pixels. The threshold is in the clip w the
projection hands back: a four-window arena films its other players at w 100..130
(measured, see the probe below), so an arena tag draws at the readable size and a
body across a course shrinks.

### Which slots, and why it cannot diverge

`data_0209f2d8 == 1` (a VS match), `i != data_0209f250` (never over the local
body), `data_0209f394[i]` non-null, and `data_0209fc5c[i]` non-zero - which is
`Player::Render`'s OWN early-out guard (`src/_ZN6Player6RenderEv.cpp:46`), so a
tag exists exactly when the body under it does.

**Every access in the file is a load.** No write reaches an actor, a pad word,
the heap or a DS global, and the draw target is the composed framebuffer after
the raster owns the frame. That is what makes the lockstep verdict below mean
something: an overlay that touched game state would show up as a divergence.

Outside a VS match `nt_draw` returns on its first compare, which is why no
adventure selftest BMP moves.

---

## Proofs, verbatim

### Build

port/build-port.cmd, the whole guard chain and both configurations:

    closestplayer_guard OK ... inferred_stub_guard OK ... every tool selftest PASS
    dsstate_guard: OK -- 14233 hosted DS symbols all inside .dsstate
                   [0xdcc000, 0xece683), 1058435 bytes captured
    alternatename_guard: OK -- 2200 directive(s) scanned, 1674 fired,
                   23 baseline-known, 0 new defeats (43229 publics)
    gxband_guard: gxbank 3/24 maps (floor 3), 13 members, 36 deltas; dtcm 3/24;
                   vsrank 3/24; vsstar 3/24 maps (floor 3), 2 members, 3 deltas;
                   ready 3/24 -- layout OK
    tailjump_guard: 35 frames, 97 assertions over 24 map(s) -- forms OK
    BUILD_EXIT=0

The gxband row that matters to this lane is **vsstar**: port_vs_slot_stars
reads data_0209f310[slot] over that band, and the guard says the band still
comes out of the linker in ROM order on every hosting target.

### The port battery

python port/tools/battery.py C:\tmp\nametag --skip-build

    levels: 50 mounted, from hal/level_boot.cpp        (every one ok)
    scenes: 34 hosted, from hal/scene_boot.cpp         (every one ok)
    default boot: ok -- a bare launch reaches the TITLE, 300 frames clean,
                  and writes its frame (589,878 bytes, liveness only)
    linkage: 9139 (80.7%)
    ptr_audit: 0 unhosted code pointers
    shipcfg build: ok, walk_window.exe linked in build\port-kit
                  (PORT_ROM_CLEAN, static CRT, 362s)
    shipcfg selftest: ok, rc=0 and walk_window_selftest.bmp written
    skips: level 27 without TTC_MOVING_BEAM, level 45 without GOOMBOSS
           (both pre-existing, both another lane's missing matched body)
    battery: ALL GREEN
    BATTERY_EXIT=0

Fifty level selftests and thirty-four scene selftests are also the evidence for
"nothing outside a VS match changed": every one of them runs the new draw call
and every one of them returns on its first compare.

### A wide muted session, and the divergence verdict

`port/tools/nametag_proof.sh` is `port/tools/vs16_ladder.sh` with three changes
and nothing else: the tree is this worktree, `SM64DS_VS_NAMES` carries real
nicknames (sixteen fields when the session is wide, four when it is not, since
the grammar settles the shape by comma count), and every window keeps its
`walk_window_selftest.bmp`. Quiet and muted per the standing rule: minimized,
never activated, `SM64DS_VOLUME=0`.

sh port/tools/nametag_proof.sh 16 900

    windows 16  ports 46960..46975  child frames 900  parent 2340  map 0
    all 16 exited
    ... 120 pairings, every one rc=0 ...
    pairings with a problem: 0

    [vs] SM64DS_VS_NAMES accepted, 16 fields: [Tango] [Opie] [Hermit] [Vessel]
         [Kappa] [Luma] [Bob] [Ella] [Gus] [Hana] [Iggy] [Jo] [Kit] [Lux]
         [Moe] [Nyx]
    live mask 0xffff
    [tag] the ROM's message font is decoded from VRAM 06000000: 8x16 glyphs,
          32-tile rows, widths from data_0208f074      (in all 16 windows)

**120 pairings, NO DIVERGENCE, over 900 hashed frames each**, with the tags
drawing the whole time. And the narrow arm, `nametag_proof.sh 4 900`:

    windows 4  ports 44380..44383  child frames 900  parent 1620  map 0
    all 4 exited
    6 pairings, every one rc=0
    pairings with a problem: 0

Evidence on disk: C:\tmp\nametag-out\rung16 and \rung4, one directory per
window with its run.log and its BMP.

### Screenshots

sh port/tools/nametag_shots.sh 240 -- four windows, only p0 walking, so
the other three stay in its frame. Each window's own selftest BMP is kept:

    C:\tmp\nametag\status_shots\nametag_p0.bmp   (and .png)
    C:\tmp\nametag\status_shots\nametag_p1.bmp   (and .png)
    C:\tmp\nametag\status_shots\nametag_p2.bmp   (and .png)
    C:\tmp\nametag\status_shots\nametag_p3.bmp   (and .png)

Each carries 2,112 pixels of the star line's own colour (0xFFFFE060, which
nothing else on screen uses), so "the text is in the picture" is a count and
not an opinion. p0's shows three tags -- Vessel, Opie and Hermit, each with its
star line -- over the three remote bodies, and nothing over the local player.

The probe from the same run, which is what says the projection is landing on
bodies rather than anywhere:

    [tag] slot 1 "Opie" 0 STARS at (154,163) w=104.28 scale 2
    [tag] slot 2 "Hermit" 0 STARS at (360,155) w=116.40 scale 2
    [tag] slot 3 "Vessel" 0 STARS at (85,147) w=128.58 scale 2

The owner judges the look; this lane only claims the text is visible.

---

## Gaps, said plainly

- **PUNCTUATION IN A NICKNAME DRAWS AS A SPACE.** The four code ranges above
  are the only ones any matched TU in this tree names, and `SM64DS_VS_NAMES`
  allows `0x20..0x7E`. So `x_1` and `a-b` lose their middle character rather
  than gain a guessed glyph, which would put an arbitrary symbol in a
  stranger's name. `0x24..0x2c` and `0x47..0x4c` are unmapped; the day somebody
  reads them off the font sheet, `nt_code_for` is the one function that
  changes.
- **THE INK IS THE PORT'S, THE SHAPES ARE THE ROM'S.** A 4bpp glyph's non-zero
  indices are the font palette's, which for a tag floating over open world
  would be unreadable against half the sky in the game. Every inked pixel is
  drawn in the tag's colour with a black outline. Deliberate, and the same
  deviation the winner banner already makes.
- **THE SLOT-TO-NICKNAME MAPPING CARRIES THE LOBBY'S KNOWN CAVEAT.** Field n is
  slot n. Above two players the parent assigns child slots from JOIN arrival
  order, which need not match lobby seat order - `hal/star_flow.cpp`'s
  `vs_names_load` banner states this in the lobby spec's words. The end-of-match
  banner already resolves names the same way (`vs_name_for(best)` with a
  `data_0209f310` index), and the tag deliberately shares that rule rather than
  inventing a second one: a tag that disagreed with the winner banner would be
  worse than one that shares its caveat. Both move on the day the lobby sends a
  seat map.
- **THE HEAD OFFSET IS MEASURED, NOT DERIVED.** `NT_HEAD_UP` is +200 world
  units above the actor's position, and where it came from is the game's own
  camera rather than a matched constant: in a live arena the player's feet sit
  at world y 254.0 and `Camera::Render`'s look-at (`cam+0x80`) at 415.2, so the
  ROM films him from 161 units above his feet. `SM64DS_TAG_PROBE=2` prints that
  ruler line, and it is what caught the first pass -- 320 lifted the tag 164 px
  on a 384-row screen, a whole second body above him. It is still a measurement
  of one camera in one arena, not the Player's own collision height; a lane that
  wants it exact should read that out of the class.
- **NOTHING HERE IS PROVEN OVER THE RELAY**, only over loopback, which is the
  same boundary every VS proof in this tree has.
- **THE LAUNCHER DOES NOT WRITE `NameTags` YET.** The game reads it and
  defaults it on, so a settings.json without the key behaves exactly as a
  player expects; adding the checkbox is a launcher-side change this lane did
  not make.

## Teardown hazard

This worktree has `extracted`, `tools\mwccarm`, `tools\bin` and `build\assets`
**junctioned** from `C:\Users\bmanu\Documents\sm64ds-wt\cons`. `git worktree
remove` recurses through junctions and empties the targets, and the ROM dump is
not re-downloadable. Unlink first (`Remove-Item` the four reparse points), or
use `~\.claude\skills\decomp-worktree\wt-remove.ps1`.
