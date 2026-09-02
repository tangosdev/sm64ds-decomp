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
shimmers as it rounds to whole pixels.

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

    PROOF_BUILD

### The port battery

    PROOF_BATTERY

### A wide muted session, and the divergence verdict

`port/tools/nametag_proof.sh` is `port/tools/vs16_ladder.sh` with three changes
and nothing else: the tree is this worktree, `SM64DS_VS_NAMES` carries real
nicknames (sixteen fields when the session is wide, four when it is not, since
the grammar settles the shape by comma count), and every window keeps its
`walk_window_selftest.bmp`. Quiet and muted per the standing rule: minimized,
never activated, `SM64DS_VOLUME=0`.

    PROOF_LADDER

### Screenshots

    PROOF_SHOTS

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
- **THE HEAD OFFSET IS A CHOSEN NUMBER.** `NT_HEAD_UP` is +320 world units
  above the actor's position, sized off the follow rig's own +200
  shoulder-height constant in `tests/walk_window.cpp`. It is not read out of the
  player's collision cylinder or model bounds. It looks right at the sizes
  captured; a lane that wants it exact should read the Player's own clsn height.
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
