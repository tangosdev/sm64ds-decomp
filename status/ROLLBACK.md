# Rollback netcode feasibility spike

Branch port/rollback-spike off port-mount-noseat-cluster (6fb68f152). Measurement
code is hal/rollback_probe.cpp plus eleven guarded lines in tests/walk_window.cpp,
all behind SM64DS_ROLLBACK_PROBE / SM64DS_ROLLBACK_DET; the default build's frame
loop takes one cached int compare per site and is otherwise unchanged.

All numbers: level 1 (castle grounds), SM64DS_WINDOW_SELFTEST=300 headless,
SM64DS_FAULTS_FATAL=1, SM64DS_VOLUME=0, this desk, Release build, wall time from
QueryPerformanceCounter. Runs are in build/tmp/runA..runG of the worktree.

## 1. Snapshot cost

What one save captures (lk6_savestate.cpp, unchanged):

| region | bytes |
|---|---|
| hosted DS arena (os_arena.cpp, 8 MB fixed) | 8,388,608 |
| .dsstate section (every hosted out-of-arena DS global) | 1,058,435 |
| hardware content stores (palette/video/sprite memory, ntr/io.cpp) | 9,441,280 |
| total | 18,888,323 (18.0 MiB) |

Note the hardware stores are the biggest region, bigger than the arena.

Timed once per frame from frame 30 to 299 (n=270), SM64DS_ROLLBACK_PROBE=2:

| operation | mean ms | p50 | p95 | max |
|---|---|---|---|---|
| lk6_savestate_save as shipped (3 mallocs + 3 copies + 3 frees + a stderr line) | 6.40 | 5.59 | 11.99 | 20.54 |
| lk6_savestate_load as shipped (3 copies back + hw copy_in, which drops the texture decode cache + 5 audio resets + a stderr line) | 2.67 | 1.62 | 6.90 | 16.27 |
| memcpy of the arena only, into a preallocated buffer | 0.68 | 0.40 | 2.86 | 8.11 |
| memcpy of arena + .dsstate + hw copy_out, preallocated | 1.44 | 0.94 | 4.60 | 10.95 |

Reading: the shipped save is slow because it mallocs and frees 18 MB every
call. A ring of preallocated slots costs 1.4 ms per frame for the whole set
(0.9 typical), which fits inside a 33 ms frame with room to spare and inside a
16.7 ms one with less. The p95 and max tails are the OS taking the core, not the
copy (the same tails show on every phase below).

A dirty-page or delta scheme is NOT needed for feasibility. If one were wanted
later: the arena is a single VirtualAlloc at 0x30000000 (os_arena.cpp), so
GetWriteWatch on it is a one-line change that would give a per-page dirty list
and cut the arena copy to the pages the frame touched; .dsstate is a linker
section inside the exe image, which GetWriteWatch cannot watch, so it would stay
a 1 MB straight copy; and the hardware stores would need ntr/io.cpp's reservation
pass to allocate them with MEM_WRITE_WATCH too. None of that is on the critical
path at these numbers.

The restore also drops the ntr texture decode cache (port_hw_regions_copy_in),
so the first rendered frame after a rollback pays a refill. That is a render
cost, not a state cost, and a rollback design can skip the hw copy_in when the
hardware stores did not change between the snapshot and now (their hash was
identical across every run below).

## 2. Re-simulation cost

The port's frame (tests/walk_window.cpp level loop) is already split into
phases with a QPC pair each. SM64DS_ROLLBACK_PROBE=1, 300 frames:

| phase | mean ms | p50 | p95 | max |
|---|---|---|---|---|
| port_actor_tick alone (phases 4/2/3: cleanup, init, behaviour; includes the Player) | 0.124 | 0.081 | 0.192 | 4.84 |
| PH_INPUT (keys, pad, Stage::CheckInput, the actor tick, frame clock, fader) | 0.256 | 0.195 | 0.474 | 4.93 |
| PH_CAMERA (Camera::Behavior, comms dispatch func_0203df40, fan-out) | 0.031 | 0.019 | 0.043 | 1.28 |
| PH_SUBMIT (geometry into gx; contains the four game-state spans below) | 1.252 | 0.923 | 3.531 | 7.75 |
| of which port_stage_advance_anims | 0.000 | | 0.001 | 0.008 |
| of which particle sim + submit | 0.005 | | 0.012 | 0.133 |
| of which port_cylinder_clsn_process | 0.009 | | 0.013 | 0.101 |
| of which port_actor_scene_pass | 0.003 | | 0.007 | 0.054 |
| PH_RASTER (ntr::gx_render, software) | 7.62 | 6.50 | 14.18 | 66.2 |
| PH_BLIT (present) | 0.33 | 0.18 | 0.78 | 9.84 |
| PH_FRAME (whole loop body) | 12.96 | 11.13 | 24.73 | 87.0 |

Where the split is. The tick side is PH_INPUT + PH_CAMERA, about 0.29 ms mean
(0.21 typical). The render side is PH_SUBMIT + PH_RASTER + PH_BLIT, but
PH_SUBMIT is not pure drawing: four pieces of game logic live in it because the
ROM runs them from Stage::Render, and each one writes game state (BTA texture
animation counters, the particle simulation, the cylinder collision pushbacks,
the scene tree housekeeping). Together they cost under 0.02 ms. The rest of
PH_SUBMIT (about 1.2 ms) is actors' Render bodies and the level model going into
gx. Whether any actor's Render writes state that its Behavior later reads (a
frustum "on screen" flag is the classic case) was NOT audited in this spike, so
there are two honest re-sim numbers:

* conservative re-sim, everything except the rasteriser and the present:
  about 1.55 ms per frame mean, 1.14 typical, 4.0 at p95. This is the mode the
  determinism check below actually ran and proved.
* tick-only re-sim, if the Render audit passes and only the four named spans
  are kept: about 0.3 ms per frame mean, 0.23 typical, 0.5 at p95.

How many rollback frames fit. A normal frame is about 13 ms mean (11 typical),
dominated by the software rasteriser. Against a 16.7 ms budget that leaves
about 3.7 ms, or about 2.2 ms after a 1.5 ms restore:

* conservative: 1 to 2 re-sim frames per frame at the mean, 2 at typical cost.
* tick-only: about 7 re-sim frames at the mean, 9 at typical cost.

Against the 33.3 ms the 3D levels are actually paced to (data_0208ee44 = 2,
frame_pace) there is about 20 ms spare, minus the restore:

* conservative: about 12 re-sim frames.
* tick-only: about 60.

Two caveats. These are castle grounds with one Player and W held; a VS stage
with four Players and a crowd of actors will tick slower, and the number to
plan around is the p95 not the mean. And the OS tails (max 66 ms on the
rasteriser) are why a rollback design must be allowed to spread a long re-sim
over two frames rather than assume every frame gets its full budget.

## 3. Determinism check

SM64DS_ROLLBACK_DET=F: at the end of frame F save; run N more frames; hash the
arena, .dsstate and the hardware stores and keep a copy; lk6_savestate_load;
rewind the frame counter to F; run the same N frames again (the selftest's
inputs are a function of the frame number, so they repeat); hash and compare.
Four runs: F=60 N=8 with the re-run rendering normally; F=60 N=8 with the
rasteriser and present skipped during the re-run (SM64DS_ROLLBACK_DET_SKIP=1,
what a real rollback would do); F=120 N=30 skipped, twice (once with the
.dsstate images dumped for a symbol-level diff).

Result, every run:

* arena: 0 differing bytes over 8 and over 30 frames. Every actor, the heaps,
  the Player, the particle RNG, the allocator cursor: byte-identical to never
  having rolled back.
* hardware stores: 0 differing bytes.
* .dsstate: 62 to 64 differing bytes over 8 frames, 417 over 30, and every one
  of them maps (walk_window.map) to the hosted ARM7 SOUND COMMAND QUEUE:

  | symbol | bytes (30-frame run) | what it is |
  |---|---|---|
  | data_020a6760 | 398 | the 256 x 0x18 command node pool (player_bridges.cpp) |
  | data_020a64a8 | 8 | the 9-slot batch ring |
  | data_020a50ec | 5 | sdat sound bss (sdat/sound_bss.cpp) |
  | data_020a6484 / 6488 / 6494 / 64a4 | 6 | the queue's in-flight and reclaimed counters |

  That is the restore doing what it is documented to do: lk6_savestate_load
  calls sd_seq_reset, sd_mix_reset, sd_consumer_reset, sd_waves_reset and
  sd_sdat_reseat, which silence the mixer and re-seed the command queue (the
  queue is described half by DS globals and half by host cursors, and only the
  DS half is in the snapshot). The probe confirms it: diffing the world right
  after the restore against the save-time bytes shows 0 arena bytes, 0 hw
  bytes and 14 to 17 .dsstate bytes moved, all in the same queue. From there
  the re-run re-triggers sounds into a reset queue, so the queue's contents
  drift from the straight run's. Nothing in the game read the queue back in
  these runs; if it had, the arena would have diverged.

Verdict: the game simulation is deterministic across restore-and-re-tick; the
only thing that is not byte-exact is audio, which a rollback design mutes
during re-sim anyway. Nothing diverged in the RNG, the ntr GPU state, or any
host-side static outside the snapshot regions, on this path. Two host-side
things that a shipping design still has to handle by hand because the snapshot
does not cover them: the frame loop's own pointers (player, c, cam, g_mc; the
existing ss_reseat lambda re-derives them, and they are unchanged by an
in-level restore), and the analog/freecam camera rig's host-side yaw and pivot
(fc_yaw, an_pivot), which the selftest does not use (CAM_DS) and which write
into the local record's heading field every frame.

## 4. The input record and the hook point

The wire block is 0x20 bytes per player per frame (comms_seam.h):
+0 flags, +2 frame counter (the parent is the clock), +6 key word, +8/+9/+A
stylus x/y/touch, +B heading, +D player count, +E reserved, +F 17 bytes of
game payload. Three of those are continuous, not buttons: the stylus, the
heading (Camera::Behavior writes it each frame through func_0203dafc), and the
payload bytes (written by WarpPlayer, Stage::Render and the func_02020028..
func_020201c0 setters; per-player flags such as character and cap state, sparse
and event-driven).

Where the bytes flow, in src/func_0203ea5c.c (the ROM's lockstep, linked):

* :184-201 stage the local block at data_020a1020 from the local record
  data_020a1040 (which src/func_0203df40.c:31-39 filled from KEYINPUT and the
  touch panel this frame).
* :223 and :236 func_020406b4(&data_020a1020, &sp2e): publish and poll. Hosted
  by hal/comms_seam.cpp:225, which calls the installed transport's exchange().
  The ROM spins on it inside the :172 while loop, sleeping through
  func_02042778 at :444/:447 (which is where the host pump runs), for up to
  sp4 = 0x4B0 turns (0x12C in the info mode, :158/:160).
* :255 func_0204068c(0): the parent's block, for the player count.
* :289-301 the per-slot loop: func_0204068c(aid) for each slot, unpacked into
  the 0x24-byte record data_020a1154[aid]. Hosted by comms_seam.cpp:238 which
  calls the transport's peer_block(aid).
* :418 the clock check: every live record's frame counter (unk0) must equal
  my own slot's, else flag bit 2 and keep waiting.
* :514 the session drop: sp4 reached 0, data_020a0f04 = 0, solo from here on.
* Then, back in walk_window.cpp right after func_0203df40 returns,
  port::comms_fanout() runs the ROM's own func_0203bb60 and func_0203bc7c,
  which turn the four records into TouchInfo[4] and PadData[4]
  (data_020a0e58), and THAT is what every actor's input read comes from.

Ordering inside the port's frame: the actor tick runs in PH_INPUT and the
exchange runs after it in PH_CAMERA, so round R's records feed frame R+1's
tick. A snapshot "for round R" is therefore the world at the frame boundary
after frame R, which is exactly where the probe's hook sits (after
editor_channel_drain, before ++frame).

Can the host substitute a predicted record and later replace it? Yes, and the
hook is exactly ONE place: the transport's exchange()/peer_block() pair, i.e.
the pipelined path in hal/comms_loopback.cpp exchange() (around :2268-2330,
the block that today does `if (!s || (s->mask & g_live) != g_live) { ++g_pipe_starved; return 0; }`).
Nothing above the seam has to change:

* exchange() for round R, when a live slot's block is missing, fills that
  slot's entry in g_latched with a PREDICTED block instead of returning 0: a
  copy of the slot's last confirmed block with +2 (frame counter) overwritten to
  round R's expected value so :418 agrees, and the flag word carrying 0x8000
  and 0x4000 the way a real round-complete block does. It records which slots
  of round R were predicted. It returns 1. The ROM unpacks it at :289-301,
  the fan-out hands it to the actors, and the frame proceeds with no wait.
* when the real block for round R arrives in service() (the pump), the
  transport compares it with what it predicted. Equal: nothing to do, mark
  confirmed. Different: store the real block in the round ring (PipeRound,
  kPipeDepth = 64, already exists) and raise "rollback needed to round R".
* the ROLLBACK ITSELF cannot run inside exchange(), because exchange() is
  called from the middle of a frame. It runs at the frame boundary in the host
  loop: restore the ring snapshot for round R, set the transport to REPLAY mode
  (peer_block answers out of the confirmed ring for rounds R+1..now and
  exchange returns 1 without publishing), run the frame body for those rounds
  with audio muted and the rasteriser and present skipped (the
  rb_resim_skip_render guard in this spike is that switch), then back to live.
  The frame loop's `frame` counter rewinds the same way the probe does.
* the local player's block is applied the frame it is read, as the ROM already
  does at delay 0 (its own record goes straight into data_020a1154[my slot]);
  the pipelined input delay stays a knob and is 0 in this mode.

One ROM detail to keep: the parent still aggregates and re-broadcasts, so a
child sees another child's input one relay hop later than the parent does.
Rollback absorbs that, but the prediction window has to be sized for the
child-to-child path, not the child-to-parent one.

## 5. Design for the shipping lane, and the recommendation

Snapshot ring: 16 slots of {arena, .dsstate, hw} preallocated once (about
300 MB; or 12 slots at 226 MB). One snapshot per frame at the frame boundary,
1.4 ms mean. 16 frames is 533 ms at 30 fps, comfortably past the p95 relay
round trip the loopback file measured (410 ms, 13 frames). Skip the hw copy
when the hardware stores' hash has not moved since the last slot (it never
moved in any run here; it moves on an area change).

Prediction rule: per remote slot, "same as the last confirmed block" for the
key word, stylus, touch and payload; for the heading, last confirmed heading
plus the last confirmed per-frame delta (it is the remote camera's yaw and
moves smoothly while their camera turns, so a straight repeat mispredicts on
every frame the remote is turning, and a rollback per frame is affordable at
33 ms but wasteful).

Confirm/rollback step, once per received round at the frame boundary: compare
real blocks with predicted; if any differ, restore the oldest mispredicted
round's snapshot and re-run up to the present with confirmed blocks where they
exist and fresh predictions where they do not. Cost per rollback of k frames:
about 1.5 ms restore plus k times 1.5 ms (conservative) or k times 0.3 ms
(tick-only, after the Render audit). At the relay's typical 3-frame latency
that is 6 ms conservative, 2.5 ms tick-only, per rollback.

Max prediction window: 8 frames predicted ahead of the last confirmed round
(267 ms), the same figure as kInputDelayMax. Past it: STALL for that slot's
session up to a short grace (say 1 second, 30 frames, counted and reported like
g_pipe_starved is today), then DROP the slot (the transport already handles a
kTypeBye leaving the live mask, comms_loopback.cpp:1384). Stall rather than
drop first, because a drop in a VS match is a scoreboard event; the ROM's own
bound (0x4B0 turns, about 20 wall seconds) never fires in this mode because
exchange() never returns 0 for a predicted round, so the host owns the drop
decision and has to make one.

What the remote player sees on a misprediction: their character was drawn for
a few frames where the guess put them; after the rollback and re-sim they are
drawn where they really are. At 3 frames of latency and running speed that is
a jump of about 3 frames of movement, a small snap. Hits, star steals, cap
blocks and coin pickups are never approximate: after the re-sim every console
has run the identical inputs through the identical code, so the arena is
identical (section 3), and a hit that was shown for two frames and then
un-happened is the visible cost of rollback everywhere it is used.

Risks, named:

* the wire block carries continuous fields (heading, stylus), so mispredictions
  are frequent whenever a remote is turning the camera or dragging the stylus.
  The rollback budget above assumes that; it is a cost, not a correctness
  problem.
* the ROM's own session-drop bound (:514) is bypassed because the game never
  waits; the host has to reproduce a drop path that leaves the ROM's records
  in the state the ROM's own drop would (data_020a0f04 = 0 and the record
  flags), or a stalled peer becomes a ghost the ROM still counts.
* the parent-as-clock rule: rollback does not change who is the clock, but it
  does mean the parent can be ahead of a child by the prediction window, and
  the parent's aggregate for round R may now be re-sent with corrected blocks.
  The relay path must carry "block for round R, revised", which the current
  datagram does (round-numbered blocks), but the child's ring must accept a
  revision for a round it has already consumed.
* the 16-player wide session: 16 remote slots means up to 16 independent
  mispredictions per frame; the snapshot cost does not scale with players, the
  re-sim cost does (a wide VS frame ticks 16 Players), and a wide session on a
  relay will have one slow peer most of the time. The drop rule matters more
  there than for a pair.
* the Render-side audit (section 2) decides which re-sim number is real. Until
  it is done, budget the conservative one.
* audio during re-sim is muted and the queue re-seeded; sounds triggered inside
  the rolled-back window are lost or doubled. Every rollback game has this.
* snapshots do not cover host-side state: the camera rig's fc_yaw and an_pivot
  in walk_window.cpp, the level_boot.cpp file-handle table and entrance cache
  (fine in-level, wrong across an area change), and the comms ring itself. An
  area change inside the prediction window must flush the ring and stall.

The other route: per-player ownership with state broadcast. This is what
hal/comms_sync.cpp already does for adventure ghosts: each console simulates
its own body from its own input with zero latency, publishes its position and
animation at 30 Hz, and applies every peer's view of that peer's body with a
lerp band and a snap threshold. No snapshots, no re-sim, no determinism
requirement, and it already runs. What it costs in VS is that the game logic
that involves two players becomes approximate, because each console decides
those events from a smoothed, late copy of the other body:

* hits and pushbacks between players (the cylinder pass): each side sees the
  collision at a different time and place; both may think they landed the hit,
  or neither.
* star steals: the star is taken from whoever the local console thinks is
  holding it; two consoles can disagree about who has it, and the scoreboard
  has to be reconciled by an owner rule (the host's word wins), which is a
  visible correction.
* cap blocks and character switches: a remote's cap state arrives late; for a
  few frames the local console fights them with the wrong rules.
* coins: whoever touches a coin first on THEIR console gets it; two consoles
  can both award the same coin, and the VS score needs a host-authoritative
  reconciliation pass that does not exist today.
* everything driven by the shared timer and the ROM's VS logic (match end,
  ranking) stays lockstep-free only if the host broadcasts it, which is more
  ownership code per feature.

Recommendation, in plain English.

Go with rollback. Here is why. The whole point of this game's online play is
that two people are in the same world at the same time, bumping into each
other, grabbing the same star, punching each other off the same ledge. The
ownership route makes each of those moments a guess, and the guesses disagree
between the two screens, so you would get "I clearly hit him first" arguments
and stars that change hands after the fact. Rollback keeps the one thing that
makes this game fair: both machines run the exact same match, and we now have
the measurement that proves it, byte for byte, across a rewind and a replay.

What the measurements say it costs: taking a full copy of the game's memory
every frame is about a millisecond and a half. Rewinding and replaying three
frames (the normal internet delay) is about six milliseconds today, and about
two and a half after one more piece of work. The game has about twenty
milliseconds to spare each frame, so this fits with room left over. The
rasteriser is where the frame time actually goes, and rollback does not touch
it.

What the player experiences: your own character responds the instant you
press a button, always. The other player's character occasionally hops a
short distance when the game learns what they really did. If someone's
connection falls far behind, everybody else keeps playing at full speed for up
to a quarter of a second of guessing, then the game holds for that person for
a moment, and if they do not come back, they are dropped and the match goes
on. Nobody waits on anybody the way the current lockstep does.

What it needs built: a ring of memory snapshots, the guessing and checking
logic in one file (the transport), a rewind-and-replay loop in the frame loop
using the same restore that F9 already uses, and a stall-or-drop rule. Plus
one audit of the drawing code to unlock the cheaper replay. The ghost layer
that already exists stays useful for the adventure mode, where nobody is
scoring.

## What was not measured, and why

* A VS stage with four (or sixteen) Players. The wide conductor and the VS
  scenes exist, but this spike ran the level-1 selftest as instructed; the
  re-sim numbers above are for one Player and the castle grounds' actors.
* The tick-only re-sim (skipping the geometry submission) as a determinism
  run. Guarding the 550-line submit block in walk_window.cpp for a one-off was
  more surgery than a spike should do to that file; the conservative mode was
  proven instead and the Render audit is named as the follow-up.
* A real two-instance session over the loopback transport with prediction.
  No prediction code was written; the hook point was located, not exercised.
* GetWriteWatch dirty-page snapshotting. Not needed at these numbers.

## Reproducing

    build with port\build-port.cmd, then from an empty directory:
    SM64DS_LEVEL=1 SM64DS_WINDOW_SELFTEST=300 SM64DS_FAULTS_FATAL=1 SM64DS_NO_FOCUS=1 SM64DS_VOLUME=0
      SM64DS_ROLLBACK_PROBE=1  walk_window.exe      phase timing summary on stderr, [rb-probe]
      SM64DS_ROLLBACK_PROBE=2  walk_window.exe      plus save/restore/copy timing
      SM64DS_ROLLBACK_DET=60   walk_window.exe      determinism, full re-run, [rb-det]
      SM64DS_ROLLBACK_DET=120 SM64DS_ROLLBACK_DET_N=30 SM64DS_ROLLBACK_DET_SKIP=1 SM64DS_ROLLBACK_DET_DUMP=1
                               walk_window.exe      30-frame window, raster skipped, .dsstate images dumped

The symbol-level diff of the dumped images against build/port/walk_window.map
was a ten-line Python one-off (parse the map's `addr symbol` rows, bisect each
differing offset plus the printed base); it is not checked in.
