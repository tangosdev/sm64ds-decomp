# WIDE169 - the port's 16:9 widescreen, and why this file exists

The port has had working 16:9 widescreen since 2026-09-02. Nobody wrote it
down. The cost of that showed up immediately: a later scoping effort spent its
whole budget rediscovering the tier, the seam, the guard and the measurements
that already existed, and produced a plan for work that was already shipped.
This file is the record so that does not happen a third time. Read it before
scoping anything that says "widescreen", "aspect ratio", "wider view" or
"ultrawide".

IT IS A RUNTIME SETTING NOW, NOT A COMPILE TIER. That is the one thing to
update in your head if you read an older copy of this file. There is no
`walk_window_wide` executable and no `NTR_WIDE169` build. ONE binary,
`walk_window.exe`, presents either aspect, and which one it presents is decided
at boot by a NUMBER in `settings.json` called `Aspect`. Everything the tier did
still happens, in the same places, for the same reasons; the `#ifdef`s became
`if`s and `ntr::SCREEN_W/SCREEN_H` became `ntr::active_w/active_h`.

Everything below is quoted from the tree with a file:line, or is a number
lifted out of a run log. Where a number in an earlier lane's summary did not
match what the tree or the log actually says, the tree's version is what is
written here and the difference is called out.

The commits that built it, all ancestors of this branch:

    d7bdaa9d7  port: first 16:9 widescreen tier (host-layer, opt-in)
    2350c2cd9  port: widescreen 16:9 second pass -- object cull, stacked minigames
    cd821c4d2  port: the 16:9 cull widen reaches the scene path, once per seed
    (this lane) the runtime plumbing off port-widescreen-toggle (1d5f2f54c,
               9502c9a1a, 7aebef270) merged onto the scene-path cull, with the
               boolean `Widescreen` key replaced by the numeric `Aspect` key

---

## 1. What exists

### 1a. The buffer, and the active extent inside it

`walk_window` links one library, `ntr_wide_rt`, declared at
`port/CMakeLists.txt:132-157`. Its only difference from plain `ntr` is one
line:

    target_compile_definitions(ntr_wide_rt PUBLIC NTR_WIDE_RT)

PUBLIC is doing real work there: because the define is on the library, it also
reaches every source of the executable that links it, which is how
`hal/message_compositor.cpp` and `tests/walk_window.cpp` see the tier's screen
constants without the CMake file having to name them.

On that tier `SCREEN_W` x `SCREEN_H` is 1024x576, the WIDE MAXIMUM, and it is
the row STRIDE and the ALLOCATION -- never the live picture size. The live
picture is `ntr::active_w` x `ntr::active_h`, two integers set once at boot
(`port/ntr/include/ntr/ppu.h:58-96`, `port/ntr/ppu.cpp:20-97`). The rule the
whole port follows, and the rule to apply to any new line of host code:

    a read that means the live screen SIZE  -> active_w / active_h
      (loop bounds, viewport, present source rect, an aspect ratio)
    a read that means the row STRIDE or the buffer ALLOCATION -> SCREEN_W / H

The other tiers are unchanged and still exist for the builds that use them:

    NTR_HIRES    1024 x 768   4x the DS panel, 4:3   (walk_window_hires)
    NTR_HIRES2    512 x 384   2x, 4:3                (no library builds it now)
    NTR_WIDE_RT  1024 x 576   the wide MAXIMUM; active extent chosen at boot
    NTR_WIDE169  1024 x 576   the old fixed 16:9 tier; nothing builds one
    (default)     256 x 192   the DS panel           (ntr, the smokes)

`ntr_2x`, the fixed 512x384 library, is GONE. `walk_window` was its only
consumer and `ntr_wide_rt` with `Aspect` at 0 is byte-for-byte that build.
`NTR_WIDE169` itself is left defined in `ppu.h` -- a fixed-compile wide build is
still expressible -- but nothing in `CMakeLists.txt` builds one, and the runtime
branches below no longer consult it.

### 1b. The Aspect key

`Aspect` is a NUMBER in `settings.json`: WIDTH DIVIDED BY HEIGHT. It is read
once, at boot, through `host_setting_aspect()` in
`port/hal/host_settings.cpp`, which follows the shape of the accessors around it
(`host_setting_run_mode`, `host_setting_camera_mode`, `host_setting_key`) --
`load_once()`, then the stored value, with the environment in front of it.

    { "Aspect": 0 }            native: 512x384, the window the port has shipped
    { "Aspect": 1.7777778 }    16:9:   1024x576
    absent / unparseable /     0, native. A file written before this key
      negative / not a number  existed reads as the shipped look.
    any other positive value   CLAMPED into [1.0, 3.0] and then fitted to the
                               largest rectangle of that ratio inside the
                               1024x576 buffer. 9.0 is 3.0 is 1024x342.

A NUMBER AND NOT A `Widescreen` BOOLEAN, and the reason is worth keeping: a
boolean cannot express ultrawide, and a number means an odd monitor needs no new
mode name, only its own ratio. The boolean this replaced still works as a legacy
alias -- `{ "Widescreen": true }` reads as 16:9 -- but only when `Aspect` is
absent, so a file carrying both is decided by the newer key.

Two environment overrides, for proofs, in front of the file:
`SM64DS_ASPECT` takes the ratio directly and goes through the same sanitiser;
`SM64DS_WIDESCREEN` is the legacy boolean and is consulted only when
`SM64DS_ASPECT` is unset.

`ntr::configure_aspect(host_setting_aspect())` is the single call, the first
statement of `main()` in `port/tests/walk_window.cpp`, before anything touches
the framebuffer. It sets `active_w`, `active_h`, and `ntr::widescreen`, which is
computed FROM THE EXTENT (`active_w * 3 != active_h * 4`) rather than stored from
the argument -- so an `Aspect` of 1.3333 written out longhand correctly reads as
NOT widescreen and the frustum widen declines, because there is nothing to widen.

### 1c. The five behaviours

There are five distinct widescreen BEHAVIOURS in the host layer, spread across
eight files. Every one of them used to sit inside `#ifdef NTR_WIDE169`. They are
compiled UNCONDITIONALLY now, and the 4:3 run is protected two ways rather than
one: most of them self-normalise (the widen factor is exactly 1.0 at 4:3, the
pillarbox margin is exactly 0), and the one that cannot self-normalise -- the
frustum widen, because it is not idempotent -- is guarded by `ntr::widescreen`
at its call sites:

| behaviour | where | how it is off at 4:3 |
|---|---|---|
| the 3D picture widens (Hor+) | `port/ntr/gx.cpp:266-287` | factor is exactly 1.0 |
| the object cull widens to match | `port/hal/camera_bridges.cpp:317-491`, called from `port/tests/walk_window.cpp:11442-11443` and `port/hal/scene_boot.cpp:5980` | `if (ntr::widescreen)` at both call sites |
| the top HUD reanchors instead of stretching | `port/hal/message_compositor.cpp` | spare width is 0, so every band lands where it did |
| the stacked (minigame) bottom panel is pillarboxed | `port/ntr/ppu_sub.cpp` | margin is 0 |
| the stylus maps to the pillarboxed band | `port/hal/sub_screen.cpp` | inverse of the above; clamps to 512x384 |

THE FRUSTUM WIDEN IS THE ONE THAT NEEDS A GUARD AND NOT A SELF-NORMALISING
FACTOR, and section 4 is why: it is a non-idempotent in-place multiply on a ROM
structure. Its scale would be 1.0 at 4:3, so calling it would be harmless in
arithmetic, but the guard means the 4:3 run does not touch the ROM's Clipper at
all, which is the stronger and cheaper statement. It is proved rather than
asserted: a native level-1 run reports `[widen] total calls 0`.

That the 4:3 run is unchanged is now measured directly rather than by
preprocessing, because there is no second build to compare against -- section 5
has the numbers, and they are the same numbers the compile-time tier produced.

## 2. How the widening works, and why it can only live where it lives

The DS builds its 3D projection matrix in ROM code. `G3i::PerspectiveW_` is what
`Camera::Render` hands the field of view and the aspect to, and it divides the
horizontal scale by 0x1555, which is 4/3 in the DS's 12-bit fixed point. That
matrix is byte-frozen source. Nothing in this port is allowed to change it, and
nothing should want to: the whole point of the port is that it IS the decomp.

By the time the projection reaches the host it has stopped being a matrix
construction and become a hardware command. It arrives as a GX `MTX_LOAD_4x4` /
`MTX_LOAD_4x3` into the projection slot, handled at `port/ntr/gx.cpp:587-590`:

    case 0x16: case 0x17: {                    // MTX_LOAD_4x4 / 4x3
        ...
        if (g.mode == MTX_PROJ) g.proj = m;

So the host's options are: rewrite `g.proj` when it lands, or leave `g.proj`
alone and widen at the point where it is used. The port does the second, in
`project()` at `port/ntr/gx.cpp:269-287`:

    if (g.proj.m[3] != 0.0f || g.proj.m[7] != 0.0f || g.proj.m[11] != 0.0f) {
        const float widen =
            (4.0f / 3.0f) * ((float)SCREEN_H / (float)SCREEN_W);
        c.x *= widen;
    }

This is not a stylistic preference. It is the only correct place, for three
reasons that the code's own banner spells out and that hold up on inspection:

Rewriting `g.proj` at load time would be wrong because a projection is not
always loaded whole. The GX command stream also carries `MTX_MULT_4x4/4x3/3x3`
(`gx.cpp:595` onward) and `MTX_PUSH`/`MTX_POP`, so a projection can be built up
or restored by paths that never pass through the load case. A widen written into
the matrix at load would be missed by some of those paths and double-applied by
others. Scaling `c.x` after the transform is naturally idempotent: it happens
once per vertex, on the result, no matter how the matrix got there.

Only `clip.x` is touched, never `clip.y`. That is what makes this Hor plus with
zero stretch. The extra width reveals world that was off the 4:3 frame instead
of stretching the 4:3 frame sideways. The factor is `(4/3) / (SCREEN_W/SCREEN_H)`
which at 1024x576 is 0.75, so what used to sit at the screen edge moves inward
and the new margins are filled with newly visible geometry.

The guard on the three `m` terms is the perspective test. An orthographic or 2D
projection has no vertex-dependent w, so `m[3]`, `m[7]` and `m[11]` are all zero.
Those loads are left exactly as they were, which is how the game's own 2D
geometry avoids being squeezed by a field expansion that only makes sense for a
camera.

## 3. The two halves: picture and cull are separate problems

This is the part that a reader coming to the code fresh gets wrong, so it is the
part to read twice.

Widening the PICTURE (section 2) fires on every vertex of every perspective
draw, everywhere, on every path, with no call site to remember. It is inside the
rasteriser.

Widening the object CULL is a completely separate mechanism, it lives in the hal
layer, and it has to be CALLED BY HAND. `data_0209f43c` (declared at
`port/hal/camera_bridges.cpp:303`) is the ROM's global `Clipper`, the frustum
that `Actor::BeforeBehavior` tests every ambient actor against. An actor whose
`SpawnInfo` carries 0x10000 skips its `Behavior` entirely whenever that test
fails, which on the castle grounds is the authentic dormancy of the birds,
butterflies and fish. That structure is seeded by ROM code:
`Camera::Render` ends in `func_0200d954`, which calls `Clipper::Func_020156DC`
with the camera's own aspect (0x1555 again), and that calls `Func_0201559C`,
which builds the four side planes from a horizontal half-extent taken off byte
0x4c of the Clipper (called `m4c` throughout the code and this document).

If only the picture is widened, the 16:9 build draws a wider view of the world
against a 4:3 cull. An ambient actor standing in the new side margins is drawn
by the rasteriser but has its `Behavior` skipped, because the frustum it is
tested against still thinks the screen ends where the 4:3 screen ended. The fix
is to scale `m4c` by the exact inverse of the picture's 0.75 and rebuild the
planes, which is `hal_camera_widen_frustum` at
`port/hal/camera_bridges.cpp:374-386`:

    long long m4c = data_0209f43c[0x4c / 4];
    data_0209f43c[0x4c / 4] =
        (int)((m4c * (ntr::active_w * 3)) / (ntr::active_h * 4));
    _ZN7Clipper13Func_0201559CEv(&data_0209f43c);

`active_w`/`active_h`, not `SCREEN_W`/`SCREEN_H`, for the same reason `project()`
above reads them: the buffer is the wide maximum on every run and the live
picture is not.

(An earlier summary put this function at `camera_bridges.cpp:297-329`. That range
is the explanatory banner. The function body is :374-386 and the scene-path
variant is :467-491.)

Note that it scales the value that is THERE rather than a literal 0x1555. A
camera or a cutscene that seeds a different aspect stays correct.

There are exactly two call sites.

The LEVEL path calls it from `port/tests/walk_window.cpp:11443`, immediately
after the loop's by-hand `hal_camera_render` and before the actor render bucket.
That position is chosen because the by-hand render is what runs
`func_0200d954` and reseeds `m4c`, so the widen always starts from a known
0x1555 and always lands before anything reads the frustum.

The SCENE path calls `hal_camera_widen_frustum_scene` from `port_scene_tick` in
`port/hal/scene_boot.cpp:5980`, and this is the change that commit cd821c4d2
made. Before it, every 3D scene in the 16:9 build drew a widened picture against
a 4:3 cull, measured as zero reaches on all 27 hosted scene ids.

BOTH CALL SITES ARE WRAPPED IN `if (ntr::widescreen)`. That is what replaced the
`#ifdef NTR_WIDE169` around them when the tier became a setting, and it is not
cosmetic: with the aspect at 0 the ROM's Clipper is left exactly as the ROM
seeded it, on both paths, and the counter says so (`[widen] total calls 0`).

THE SCENE CALL SITS AFTER `port_actor_render`, NOT BEFORE, and that ordering was
measured rather than reasoned. A scene drives no camera by hand. Its `Camera` is
an actor, and it rides the ROM's own render bucket, so the seed of `m4c` happens
INSIDE `port_actor_render`. A call placed before the bucket is wiped by the
reseed inside the bucket, in the same frame, and the whole fix comes out as a
null result that looks like "the change made no difference". The commit message
records this explicitly: placed before the bucket, "the reseed inside the bucket
wipes it in the same frame and the fix reads as a null result."

The other half of the ordering constraint pulls the same way. The consumer of
the cull is `Actor::BeforeBehavior`, which runs in the tick, not the render. So
the level path's real contract is "widen after the seed, read on the NEXT tick",
and after cd821c4d2 the scene path has the same one.

## 4. THE COMPOUNDING TRAP

This is the single most important warning in this file. If you take one thing
away, take this.

THE WIDEN IS NOT IDEMPOTENT. It multiplies `m4c` by 4/3 in place. On the level
path that is safe only because something else guarantees the word is reset to
0x1555 before every call: `Camera::Render` reseeds it every single frame.
Measured on level 1 over 300 frames, `m4c` reads 5461 (0x1555) at every sampled
call (`build/agent_wide1/w_lvl1/run.log`, the `[widen] call N m4c 5461` lines at
calls 1, 64, 128, 192, 256).

The scene path does not come with that guarantee, and measuring all 27 scene ids
found not two shapes but three:

| shape | scenes | `m4c` behaviour |
|---|---|---|
| RESEEDED every frame | 363 (MG_MEMORY2), 372, 373 (MG_JUMP2) and most others | rewritten to 5461 every frame by the render bucket; a per-frame widen is correct, this is the level contract |
| NEVER SEEDED | 4 (STAR_SELECT), 374 (MG_CURLING), the title, the 2D minigames | `m4c` reads 0 on frame 0 and on frame 299; there is no cull to widen |
| SEEDED ONCE at spawn | 361 (MG_CUP), 367 (MG_BOOMBOX) | written once, never again |

The third shape is the trap. A naive per-frame call on a scene seeded once at
spawn compounds. Measured, before the guard existed: 5461, then 7281, then 9708,
and on up to 1540606696 by frame 299. That is an overflowed aspect term feeding
four garbage side planes into a structure the ROM never puts in that state.

The shipped rule is therefore ONCE PER SEED, NOT ONCE PER FRAME, and it is
carried by the function rather than by the call site, at
`port/hal/camera_bridges.cpp:463-485`. It remembers the value it last wrote
(`g_widen_scene_written`). If the word still holds that value, nothing reseeded,
the frustum is already widened, decline. If the word holds something else, a
reseed happened, widen it. If the word holds 0, forget the memory and decline,
so the next scene's first genuine seed is not mistaken for a value this seam
wrote.

On a RESEEDED scene that is a widen every frame, identical to the level path. On
a SEEDED ONCE scene it is one widen that then persists at 7281, which is the
right answer for a Clipper written once. On a NEVER SEEDED scene it is nothing.

The one ambiguity is named in the code rather than hidden: a reseed that happens
to write exactly the value the last widen produced is indistinguishable from no
reseed, and the guard declines it. The direction is deliberate. Declining is a
no-op; widening twice corrupts.

ANYONE ADDING A THIRD CALL SITE MUST RESPECT THIS. The probe's per-frame `m4c`
line exists to catch a violation: a second call on the same seed shows up as a
compounding `m4c` in the log, immediately.

`SM64DS_WIDEN_PROBE` is the instrument for all of this
(`camera_bridges.cpp:335-417`). One environment variable, inert unless set to
something other than 0, and it prints progress every 64 calls rather than only
at a static destructor, because a run that faults or is killed by a timeout
never reaches a destructor and a reachability answer that only exists at a clean
exit is no answer at all.

READ IT OUT OF THE RIGHT FILE. The probe writes to stderr, and on the SCENE path
the port's flight recorder routes stderr into `playlog/play_*.log` in the run's
working directory, not into whatever the harness piped. A scene run whose widen
lines are "missing" from the pipe is almost certainly a harness reading the wrong
file; the level path writes to both.

## 5. What was measured

All runs through `port/tools/mp2_proof.py`'s quiet spawner, `FAULTS_FATAL=1`,
`VOLUME=0`, `MINIMIZED=1`, `SM64DS_*` scrubbed, window minimized and unfocused.

### 5a. ONE BINARY, BOTH ASPECTS

The whole point of the runtime conversion, and the thing to re-run first if any
of this is ever in doubt. Same `walk_window.exe`, 300-frame selftest, the only
difference is `SM64DS_ASPECT`:

| content | aspect 0 | aspect 1.7777778 |
|---|---|---|
| level 1  | 512x384, 2034.6 tris | 1024x576, 2064.0 tris |
| level 13 | 512x384, 1145.3 tris | 1024x576, 1166.1 tris |

Those four triangle means are the compile-time tier's own numbers, to the
decimal, and the selftest positions are identical across the aspect on both
levels (`pos=(-4915200, 2929633, 11141348)` on level 1,
`pos=(-24206229, 8192012, 24214420)` on level 13). The runtime conversion is
therefore not "close to" the tier; it reproduces it.

The dimensions are read out of the selftest BMP header, so the aspect claim is
made in pixels rather than in a log line the code could be wrong about.

### 5b. THE ASPECT KEY

Read from `settings.json` with no environment override, 60-frame runs, BMP
dimensions again:

| `settings.json` | picture |
|---|---|
| `{ "Volume": 100 }` (Aspect absent) | 512x384 |
| `{ "Aspect": 0 }` | 512x384 |
| `{ "Aspect": 1.7777778 }` | 1024x576 |
| `{ "Aspect": 9.0 }` | 1024x342 (clamped to 3.0, rc 0) |
| `{ "Aspect": "wide" }` | 512x384 |
| `{ "Aspect": -2 }` | 512x384 |
| `{ "Widescreen": true }` | 1024x576 (legacy alias) |
| `{ "Aspect": 0, "Widescreen": true }` | 512x384 (newer key wins) |

9.0 CLAMPS RATHER THAN BREAKING, which is the case worth stating: the run exits
0 and draws a picture, at 1024x342, whose ratio is 2.994 and therefore inside
the documented band. The derived side of the fit rounds in the direction that
keeps the delivered ratio no wider than the clamp, which is why it is 342 and
not 340.

### 5c. The compile-time tier's original measurements

Everything from here down was measured against the fixed `NTR_WIDE169` build and
is retained because the mechanism did not change. Logs in
`C:\tmp\wide\build\agent_wide1\`.

REACHABILITY, RE-PROVED ON THE RUNTIME BUILD. At aspect 1.7777778, level 1 and
level 6 each report `[widen] total calls 300, scene frames declined 0` over a
300-frame selftest -- once per frame, not twice -- with `m4c` reading 5461 at
every sampled call (1, 64, 128, 192, 256), which is the non-compounding
signature. At aspect 0 the same level reports `[widen] total calls 0`: the
runtime guard leaves the ROM's Clipper untouched.

THE SCENE PATH STILL FIRES, and all three shapes reproduce at the runtime
aspect over 300 scene frames:

| scene | shape | wide (1.7777778) | native (0) |
|---|---|---|---|
| 363 MG_MEMORY2 | reseeded every frame | 300 calls, 0 declined, post `m4c` 7281 every frame | 0 calls, `m4c` 5461 |
| 361 MG_CUP | seeded once at spawn | 1 call, 299 declined, `m4c` 7281 and HOLDS | 0 calls, `m4c` 5461 |
| 374 MG_CURLING | never seeded | 0 calls, 300 declined, `m4c` 0 | 0 calls, `m4c` 0 |

361 holding at 7281 for 299 frames instead of climbing is the once-per-seed
guard doing its job; see section 4 for what happens without it.

The original tier measurements follow. The level path reaches the widen on every
frame: levels 1, 5, 6, 8
and 13 all report `[widen] total calls 300` for a 300-frame selftest
(`w_lvl1`, `w_l5`, `w_l6`, `w_l8`, `w_l13`). A cutscene reaches it 900 of 900
frames (`w_intro`, `[widen] total calls 900`) because a cutscene runs the level
loop. The scene path, in that same phase-1 build, reached it zero times:
`w_scene4` and `w_scene374` both report `[widen] total calls 0`. That zero is
what cd821c4d2 fixed; after it, the seam is reached on 300 of 300 scene frames
and 900 of 900 on the title, with 11 of the 27 scene ids widening and 16
correctly declining because they never seed a Clipper at all.

THOSE 16 DECLINES ARE CORRECT, NOT A GAP. Their `m4c` is 0. Running
`Func_0201559C` over an all-zero Clipper would build four side planes out of a
zero aspect, which is a state the ROM never puts that structure in. Declining
leaves the structure exactly as the byte-frozen source left it.

ACTOR CULLING IS IDENTICAL between 4:3 and wide on level 1. The two runs
`ab_43_l1` and `ab_wide_l1` produce byte-identical `[census]` blocks: 82 spawned
across 22 classes, 0 skipped, and every per-class count matches (17 x1
BLACK_BRICK_BLOCK through 353 x3 DOOR). Both end on the identical selftest
position `pos=(-4915200, 2929633, 11141348)`. So the widened cull is not
resurrecting or killing actors on level 1; the ambient set behaves the same.

TRIANGLES SUBMITTED go up slightly in wide, which is exactly what a wider field
should do. Means over the `[perf]` samples in each run:

| content | 4:3 | wide |
|---|---|---|
| level 1 | 2034.6 | 2064.0 |
| level 13 | 1145.3 | 1166.1 |

(These are means of the ten `[perf] ... tris N` lines each run emits, not a
per-frame mean over all 300 frames. Recomputed from the logs while writing this
file and they reproduce exactly.)

The probe itself is free: `inert_wide_l1`, the wide build with
`SM64DS_WIDEN_PROBE` unset, has a mean of 2064.0 as well, identical to the run
with the probe on.

WHAT THE SCENE-PATH FIX CHANGES TODAY: NOTHING, and this should be said plainly
rather than dressed up. On the heaviest scene-path content the port hosts,
`SCENE_MG_CUP` (361) and `SCENE_MG_BOOMBOX` (367) at 1806.0 triangles per frame,
then MEMORY2/MCARLO at 1782.1 and JUMP2 at 940.4, triangles submitted are
IDENTICAL with the widen active and with it removed, to the frame. Today's scene
content has no ambient 0x10000 actors sitting in the new side margins, so there
is nothing for a wider frustum to let through. The gap the fix closes is real
and structural, and it is currently harmless. It was closed before scene-path
content that would fall into it exists, not after.

The level path is untouched by that commit: levels 1 and 6 still widen exactly
300 times in 300 frames (not 600) and land on the same selftest positions.

## 6. The HUD, and the one thing that is owed

The DS composes its top-screen HUD in its own 256x192 space. On the 4:3 tiers
`sx == sy` (both 4 at hires, both 2 at hires2), so one integer scale fills the
screen with no distortion and the mapping is simply `x * sx`. At 1024x576 they
differ: `sx` is 4 and `sy` is 3. The naive mapping would stretch the HUD 33%
wider than tall AND smear it across the full 1024, hugging the left edge.

What ships instead is at `port/hal/message_compositor.cpp:1773-1794` (the
constants) and `:1811-1832` (the placement). Scale UNIFORMLY by `sy`, so blocks
stay square and elements stay native size, then place each source column by a
BAND SPLIT on its source x:

    const int uni = sy;                            /* uniform native scale (3) */
    const int margin = ntr::SCREEN_W - 256 * uni;  /* spare width (256) */
    const int band_l = 96, band_r = 160;           /* source-x band splits */

    const int hx0 = !shown3d
                  ? x * uni + margin / 2
                  : (x < band_l) ? x * uni
                  : (x >= band_r) ? x * uni + margin
                                  : x * uni + margin / 2;

So: source x below 96 rides the left edge, 160 and above rides the right edge,
and the middle stays centred. The `!shown3d` arm is a separate case and is
covered below.

WHY A BAND SPLIT AND NOT A PER-ELEMENT RULE. By the time this code runs, the
compositor is holding a composited 256x192 buffer of DS pixels. It has already
lost per-element identity. There is no "this pixel belongs to the coin counter"
available at this point; there is a pixel, its owning layer and its priority.
A true per-element reanchor would have to be done further upstream, where the
elements still exist as elements. The band split is what can be expressed at
this seam.

WHY IT IS FREE TODAY. For every element actually drawn in current play, the band
split and a true per-element rule give BYTE-IDENTICAL placement, a 0 px
difference. Measured by differing the OBJ layer on and off on level 6
(`build/hudband/wide_lvl6` against `build/hudband/wide_lvl6_noobj`) and inverting
the band map on the differing host columns: the HUD's OBJ pixels occupy source x
6-47 (the lives cluster, left band) and 207-247 (the coin and star counters,
right band). Both clusters land where a per-element rule would put them, and the
reason is arithmetic rather than luck. The left cluster starts at source x 6 and
`x * uni` puts it 18 host pixels from the left edge, which is the native margin
(6 x 3). The right cluster ends at source x 247, and `x * uni + margin` puts its
right edge at 1000 on a 1024-wide screen, a 24 pixel margin, which is the native
margin (256 - 248 = 8, times 3). The right band adds a constant, so it preserves
the native right margin exactly. The health meter, at source x 108-147, is
entirely inside the centre band and stays centred, which is where it already
was. (The meter is a BG layer, not OBJ, so it is not in the OBJ-diff measurement
above; its extent is carried from an earlier lane and was not independently
re-measured here.)

WHAT IS OWED, AND WHEN. Two elements that appear in ordinary play STRADDLE a
band boundary, and a band split cuts a straddling element in half with the full
margin opened through the middle of it:

* the red-coin pip row. Pip 8 sits at DS x 85-100, which crosses `band_l` (96).
  Pixels below 96 ride left, pixels at 96 and above stay centred, so the pip is
  torn with 128 px of gap through it.
* the TIME label. It sits at DS x 140-179, which crosses `band_r` (160). Same
  failure, at the other boundary.

Neither is on screen in the runs measured above, which is why the measured
difference is zero. Both appear the moment somebody plays a red-coin star or a
timed level. So the honest statement is: THE BAND SPLIT IS FREE TODAY AND OWED
BEFORE RED COINS OR TIMERS MATTER. The fix is a per-element reanchor done
upstream of the composite, where element identity still exists. It is not urgent
and it is not optional forever.

THE FULL-2D EXCEPTION. A minigame's top screen is a full 2D raster with no 3D
layer behind it (`shown3d` is false). Reanchoring that tears it into vertical
strips with black gaps where the bands separate, which is what a pachinko or
smartball board showed as two dark bars. So a full 2D top screen is PILLARBOXED
instead: centred at the uniform scale with black margins either side, and
`margin / 2` is 128, which is exactly the bottom panel's `pan_x0`, so the two DS
screens line up. The reanchor is for a SPARSE HUD OVER A 3D FIELD, where the 3D
fills the gaps the bands open, and only there.

## 7. The limits, and the one rule you must not break

HEIGHT MUST STAY A WHOLE MULTIPLE OF 192. This is not a preference. At
`port/ntr/ppu_sub.cpp:4182` the stacked-screen layout does

    l.scale = SCREEN_H / SUB_H;

and that single integer is what expresses the whole stacked and band geometry in
whole host rows per DS row. Everything downstream reads it: `l.head_h`,
`l.top_y`, `l.band_y`, `l.bottom_y`, the DIB header, the window size, the BMP
writer and both stylus mappers. A height that is not a whole multiple of 192
makes that integer wrong and every one of those derived numbers wrong with it.
576 is 192 x 3, which is why 576 was chosen.

WIDTH DOES NOT NEED TO BE AN INTEGER MULTIPLE of anything. It happens to be
(1024 is 4 x 256), but nothing depends on that any more: the widescreen paths
carry `pan_x0` and `pan_w` and a separate `uni` scale precisely so the width can
be whatever the aspect wants. This is the first tier where `SCREEN_W/256` (4)
does not equal `SCREEN_H/192` (3), and the code that had to learn that lesson
has already learned it.

STACKED MINIGAME TOP SCREENS ARE LEFT FULL WIDTH. `port/ntr/ppu_sub.cpp:4452-4459`
says why, and it is an honest limitation rather than an oversight: whether a
minigame's upper screen is the widened Hor+ 3D field (which should keep the
extra width) or a 2D DS raster (which wants the same pillarbox the bottom panel
gets) is a per-scene fact the compose function cannot read. Stretching it there
would be wrong for the 3D case. So the compose touches only the panel that is
unambiguously a 256x192 raster, and A 2D-TOP MINIGAME WILL STRETCH. That is a
known, deliberate, flagged gap.

NEVER LINK A SMOKE TEST AGAINST `ntr_wide_rt`. The smokes deliberately keep the
DS-native 256x192 `ntr` library (`port/CMakeLists.txt:120`, with the reasoning
spelled out where `ntr_hires` is declared: "the smokes keep the DS-native lib
(their reference pixel counts depend on 256x192)"). This did not stop mattering
when the tier became runtime-selectable: `ntr_wide_rt`'s framebuffer is 1024x576
no matter what `Aspect` says, and the stride is what a reference pixel count is a
function of. The selftest
BMP probes compare against reference pixel counts that are a function of the
frame's dimensions, so a smoke linked against a wide library fails for a reason
that has nothing to do with what it is testing. This is the same trap recorded
elsewhere as "the selftest BMP is layout dependent": the frame's geometry is
part of the probe.

## 8. What is not done

HIGHER RESOLUTION IS A SEPARATE AXIS FROM WIDER. This gets conflated constantly,
so state it flatly. `NTR_HIRES` is 1024x768 and 4:3: sharper, not wider.
`NTR_WIDE_RT` is a 1024x576 buffer: at 16:9 that is wider than the DS, and at
the same 1024 across as hires, so it is not sharper than hires, it is hires's
width applied to a shorter frame. THE `Aspect` KEY DOES NOT CHANGE THIS -- it
picks a SHAPE inside that buffer, never a bigger one. Wide AND sharp is a bigger
buffer, and it is not hard: raise `SCREEN_H` on the `NTR_WIDE_RT` arm to a taller
whole multiple of 192 (768 is the obvious one, since hires already uses it) and
`SCREEN_W` to match the widest ratio wanted (1366 for 16:9 at 768; the width need
not be an integer multiple, see section 7). `configure_aspect` fits the active
extent inside whatever those two are, so nothing else has to change and no new
mechanism is required. Nobody has done it because nobody asked.

ULTRAWIDE IS NOW EXPRESSIBLE, AND STILL GATED ON THE PER-ELEMENT HUD WORK. The
key can say 2.37 and the fit will deliver a 21:9 picture out of the same buffer;
that half is done, and `Aspect: 9.0` clamping to 1024x342 is the proof the
arithmetic holds past 16:9. What is not done is the HUD.

TRUE ULTRAWIDE BEYOND ABOUT 2.0 IS GATED ON THE PER-ELEMENT HUD WORK. The 3D
side scales fine: section 2's widen is a ratio and does not care how wide the
frame is. The HUD does care. Each band boundary opens a hole of `margin / 2`,
and `margin` grows with the width. At 16:9 that is 256 spare, so 128 px at each
boundary. At 21:9 (1344x576, say) it is 576 spare, so two holes of 288 px each.
An element straddling a boundary is not just torn, it is torn with a gap wider
than the element. So section 6's owed work is a hard prerequisite for ultrawide,
not a nice-to-have.

## 9. Reproducing

    build with port\build-port.cmd (there is one game target, walk_window)

    the aspect A/B, one binary, in pixels:
      SM64DS_LEVEL=1 SM64DS_WINDOW_SELFTEST=300 SM64DS_FRAME_MS=1 walk_window.exe
      SM64DS_ASPECT=1.7777778 <the same>
      -> walk_window_selftest.bmp is 512x384 then 1024x576; the [perf] tris
         means are 2034.6 then 2064.0; the selftest pos line is identical.

    the settings key, with no environment override:
      write settings.json next to the run and unset SM64DS_ASPECT and
      SM64DS_WIDESCREEN. Section 5b is the table of what each value gives.

    reachability and the compounding watch, level path:
      SM64DS_LEVEL=1 SM64DS_WINDOW_SELFTEST=300 SM64DS_WIDEN_PROBE=1
      SM64DS_ASPECT=1.7777778 walk_window.exe
      -> [widen] call N m4c 5461 every 64 calls, [widen] total calls 300 at exit.
         Drop SM64DS_ASPECT and it must be total calls 0.

    reachability and the compounding watch, scene path:
      SM64DS_SCENE=361 SM64DS_SCENE_FRAMES=300 SM64DS_WIDEN_PROBE=1
      SM64DS_ASPECT=1.7777778 walk_window.exe
      -> READ playlog/play_*.log, NOT the piped stderr (section 4).
         [widen] scene fN pre/post m4c ... tris ... every frame.
         m4c must go 5461 -> 7281 ONCE and then hold. If it keeps climbing,
         something added a second call site. See section 4.

    the 4:3 / wide A/B on content:
      the same level at aspect 0 and at 1.7777778; compare the [census] block
      (must be identical) and the [perf] tris means (wide is slightly higher).

    the HUD band evidence:
      build/hudband/shots.py captures the four reference images in
      status_shots/hudband/; build/hudband/nolayer.py is the OBJ-on / OBJ-off
      diff that produced the source-x extents in section 6.
