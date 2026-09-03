# Rollback netcode: what shipped (port/rollback)

Branch port/rollback, rebased onto port-mount-noseat-cluster at 3d048eb4e
(LAGDELAY, widescreen, health meter, King of the Star and the seat work all
underneath). The spike that sized this is status/ROLLBACK.md; this file is
what was built on its recommendation, what was proven, and what is not
closed. Every number below is from this desk, Release build, the runs kept
under build/rollback_proof/<rung>/p<k>/run.log of the worktree.

## 1. What it is, in one paragraph for the owner

Online play used to work by every copy of the game waiting for every other
copy's button presses before it would draw the next frame, so a slow
connection made everybody's game hitch. With NetMode set to "rollback" in
settings.json, each copy now keeps going on its own: when another player's
input has not arrived yet it makes a guess (they are still doing what they
were doing), and when the real input turns up a moment later and the guess was
wrong, the game silently rewinds a few frames and replays them with the truth.
Your own character always responds the instant you press a button. The other
players' characters occasionally hop a short distance when a guess is
corrected, which is the visible cost of every rollback game ever made. If one
player's connection falls badly behind, the others keep playing at full speed
while the game guesses for up to eight frames, then hold for one second, and
if that player is still silent they are dropped and the match goes on. Since
the owner's decision of 2026-09-03 rollback is the default (section 8): a
copy with no settings.json, or one that never mentions NetMode, plays online
this way, "lockstep" is still there for anyone who writes it, and a session
of more than eight players runs lockstep on its own and says so.

## 2. The pieces

* hal/rollback.cpp: the frame-boundary half. A ring of 12 preallocated
  snapshots (arena 8 MB + .dsstate 1 MB each, 147 MB total), taken once per
  frame after the present and the sound drain and before ++frame. The three
  hardware content stores (palette, video, sprite memory, 9.4 MB) are NOT
  copied per frame: ntr/io.cpp reserves them with MEM_WRITE_WATCH, the
  boundary asks the kernel which pages the frame wrote, and each slot keeps an
  undo log of previous page contents against a shadow image. A rewind
  restores the arena and .dsstate, applies the undo logs newest first, puts
  back the two host counters the hash lines stamp (the seam's round count and
  the divergence detector's frame), rewinds the transport's clock and the
  frame counter, mutes the output stage (sdat/mixer.cpp sd_host_mute) and
  lets the frame loop run again to the present with the host inputs, the
  camera rig, the level model, the rasteriser and the present standing down.
* hal/comms_loopback.cpp, the ROLLBACK LEDGER: the transport half. In
  rollback mode exchange() serves round R the frame it opens, at input delay
  zero. This console's block is real, a live slot whose block has landed is
  real, any other live slot is a prediction (its last confirmed block, heading
  extrapolated at its last per-round delta, frame counter overwritten so the
  ROM's clock check agrees). What was predicted is kept per round; the
  boundary scan (comms_rb_scan) compares confirmed blocks with the guesses and
  names the oldest contradicted round. The window is 8 rounds past the last
  confirmed block (kRbWindow); past it exchange() stalls for 1000 ms
  (kRbGraceMs) and then the parent retires the slot through the same path a
  Bye takes, round-exact: every round from the slot's last confirmed + 1 is
  revised (live mask without it) and re-sent, so every console rolls back to
  that round and re-runs with the slot absent, which the ROM's own unpack turns
  into a zeroed record. A dropped slot's late datagrams are ignored for the
  session so it cannot reseat itself mid-match. The parent's aggregates carry
  the PER-ROUND live mask rather than the parent's current one.
* The tick-only re-sim: port_actor_render_replay (hal/actor_registry.cpp).
  The Render-side audit walked every list-5 Render body reachable in the
  hosted levels and found them to be geometry submission and Render-private
  scratch, except InvisibleSecret (329/330), whose Render decrements the
  countdown its Behavior gates on, and FlameChompFire (271) and
  MrI_Projectile (264), whose Render keeps their particle emitters alive. A
  replayed frame walks list 5 through the ROM's own walker on a one-node view
  and runs only those four; the four Stage::Render game-state spans (BTA
  anims, particle sim, cylinder pushbacks, scene pass) still run.
  SM64DS_ROLLBACK_ACTOR_RENDER=1 is the conservative re-sim (every Render
  body kept, only the rasteriser and the present dropped).
* NetMode: hal/host_settings.cpp reads "NetMode": "lockstep" | "rollback"
  once at load, rollback being the default (an absent file, an unparseable
  file and an absent key all read as rollback; section 8); SM64DS_NETMODE
  overrides it at transport install, and a session wider than
  kRollbackMaxPlayers (8, hal/host_settings.h) runs lockstep. The parent's
  choice travels in the ACCEPT and the roster announce on bit 17 (bit 16 is
  LAGDELAY's report ack), and a child that asked for the other mode adopts the
  parent's, loudly. An older parent sets neither bit and reads as lockstep.
* port/tools/rollback_proof.py: the ladder (section 4). net_proof.py takes
  --netmode rollback to run the lockstep ladder's rungs in the new mode.

## 3. Living beside LAGDELAY

The adaptive input delay and rollback are two answers to the same latency and
they never run together. Rollback forces g_input_delay to 0 at install,
before the sizing is armed, so `g_adaptive_delay` (which needs a nonzero
depth) stays false and none of the sizing wait, the round-0 ack gate or the
late-join refusal (all keyed on a raised depth) engages; the freeze flag is
still recorded on the first exchange, which is harmless. In lockstep nothing
of rollback is reachable: `pipelined()` reads `g_input_delay > 0 ||
g_rollback`, and with the mode off it is the old test. The rebase kept both
sides of every overlap: the rule-3 hold sits under the rollback branch in
pipe_try_broadcast, the child adopts the delay through child_adopt_delay and
then the mode, the close line carries starvedby AND the rollback line, and
exchange() runs the sizing wait and the freeze before it hands off to the
rollback exchange.

## 4. The proof ladder (port/tools/rollback_proof.py)

Every rung launches walk_window.exe headless and muted through
mp2_proof.env_base with SM64DS_NETMODE=rollback, the injected key TOGGLING
every 45 rounds (so guesses go wrong and real rollbacks happen), the state
hash on, and reads the logs. The verdict lines are in build/tmp/ladder3a.log,
ladder3b.log and ladder3c.log of the worktree; the per-window logs under
build/rollback_proof/<rung>/p<k>/run.log. The table in section 4.1 is from
the final binary (the tip named at the top of section 7).

### 4.1 Verdicts and numbers on the final binary

All rows: this desk, every window of a rung on the same CPU (so the per-frame
times of the 4-, 7- and 8-window rungs carry the contention of that many
software rasterisers sharing one machine; the PAIR rows are the ones that
show what one console pays). "rollback event" is restore plus the whole
replay; "per replayed frame" is one re-run frame with the render skipped
(tick-only); "snapshot" is the per-frame cost every frame pays. Round trips
are induced with SM64DS_COMMS_DELAY_MS on every window; the relay rows go
through port/tools/relay/relay.py on 127.0.0.1, a fresh relay per RTT.

| rung | verdict | sweep | session | rollback event ms | per replayed frame ms | snapshot ms |
|---|---|---|---|---|---|---|
| pair_rtt0 | PASS | 1/1 pairings NO DIVERGENCE to round 592 | live=0x3 on 2/2 | n=118 p50 17.2 p95 26.0 max 38.1 | p50 7.6 p95 10.8 | p50 0.57 p95 1.35 |
| pair_rtt40 | PASS | 1/1 pairings NO DIVERGENCE to round 592 | live=0x3 on 2/2 | n=199 p50 23.5 p95 43.6 max 65.7 | p50 7.6 p95 12.4 | p50 0.61 p95 1.70 |
| pair_rtt80 | PASS | 1/1 pairings NO DIVERGENCE to round 592 | live=0x3 on 2/2 | n=191 p50 28.2 p95 57.5 max 66.5 | p50 7.6 p95 11.2 | p50 0.60 p95 1.92 |
| pair_rtt160 | PASS | 1/1 pairings NO DIVERGENCE to round 592 | live=0x3 on 2/2 | n=191 p50 33.5 p95 64.0 max 81.0 | p50 7.0 p95 11.4 | p50 0.62 p95 1.92 |
| vs4_rtt0 | PASS | 6/6 pairings NO DIVERGENCE to round 892 | live=0xf on 4/4 | n=390 p50 41.5 p95 72.2 max 112.1 | p50 18.3 p95 31.4 | p50 0.64 p95 3.28 |
| vs4_rtt40 | PASS | 6/6 pairings NO DIVERGENCE to round 892 | live=0xf on 4/4 | n=551 p50 49.2 p95 91.1 max 143.5 | p50 16.8 p95 30.2 | p50 0.66 p95 3.18 |
| vs4_rtt80 | PASS | 6/6 pairings NO DIVERGENCE to round 892 | live=0xf on 4/4 | n=610 p50 49.5 p95 94.2 max 167.8 | p50 16.0 p95 26.9 | p50 0.67 p95 2.72 |
| vs4_rtt160 | PASS | 6/6 pairings NO DIVERGENCE to round 892 | live=0xf on 4/4 | n=554 p50 61.9 p95 113.6 max 238.5 | p50 14.0 p95 23.1 | p50 0.66 p95 2.32 |
| vs7_rtt0 | PASS | 21/21 pairings NO DIVERGENCE to round 892 | live=0x7f on 7/7 | n=502 p50 52.2 p95 122.3 max 300.9 | p50 22.0 p95 50.9 | p50 0.70 p95 3.17 |
| vs7_rtt40 | PASS | 21/21 pairings NO DIVERGENCE to round 892 | live=0x7f on 7/7 | n=632 p50 69.1 p95 162.4 max 251.9 | p50 24.2 p95 54.1 | p50 0.72 p95 3.80 |
| vs7_rtt80 | PASS | 21/21 pairings NO DIVERGENCE to round 892 | live=0x7f on 7/7 | n=703 p50 74.4 p95 191.5 max 407.4 | p50 24.2 p95 60.3 | p50 0.74 p95 3.60 |
| vs7_rtt160 | PASS | 21/21 pairings NO DIVERGENCE to round 892 | live=0x7f on 7/7 | n=764 p50 75.6 p95 178.1 max 315.5 | p50 21.8 p95 43.0 | p50 0.73 p95 3.31 |
| relay_rtt0 | PASS | 1/1 pairings NO DIVERGENCE to round 592 | live=0x3 on 2/2 | n=118 p50 23.5 p95 51.0 max 72.6 | p50 9.6 p95 19.5 | p50 0.65 p95 2.67 |
| relay_rtt40 | PASS | 1/1 pairings NO DIVERGENCE to round 592 | live=0x3 on 2/2 | n=211 p50 26.3 p95 49.5 max 66.2 | p50 9.1 p95 15.1 | p50 0.65 p95 2.46 |
| relay_rtt80 | PASS | 1/1 pairings NO DIVERGENCE to round 592 | live=0x3 on 2/2 | n=153 p50 34.0 p95 69.1 max 89.0 | p50 8.6 p95 16.5 | p50 0.65 p95 2.30 |
| relay_rtt160 | PASS | 1/1 pairings NO DIVERGENCE to round 592 | live=0x3 on 2/2 | n=182 p50 36.8 p95 66.9 max 149.8 | p50 7.7 p95 12.7 | p50 0.66 p95 2.43 |
| relay4_rtt0 | PASS | 6/6 pairings NO DIVERGENCE to round 892 | live=0xf on 4/4 | n=370 p50 46.2 p95 89.5 max 213.2 | p50 19.2 p95 37.6 | p50 0.74 p95 3.64 |
| relay4_rtt40 | PASS | 6/6 pairings NO DIVERGENCE to round 892 | live=0xf on 4/4 | n=555 p50 51.8 p95 103.1 max 183.5 | p50 17.2 p95 30.6 | p50 0.70 p95 3.05 |
| relay4_rtt80 | PASS | 6/6 pairings NO DIVERGENCE to round 892 | live=0xf on 4/4 | n=610 p50 52.3 p95 102.2 max 177.7 | p50 16.6 p95 30.3 | p50 0.71 p95 3.25 |
| relay4_rtt160 | PASS | 6/6 pairings NO DIVERGENCE to round 892 | live=0xf on 4/4 | n=539 p50 66.3 p95 147.2 max 508.8 | p50 15.0 p95 33.3 | p50 0.70 p95 2.54 |
| relay7_rtt0 | PASS | 21/21 pairings NO DIVERGENCE to round 892 | live=0x7f on 7/7 | n=483 p50 80.0 p95 303.3 max 725.4 | p50 21.3 p95 87.1 | p50 0.77 p95 3.43 |
| relay7_rtt40 | PASS | 21/21 pairings NO DIVERGENCE to round 892 | live=0x7f on 7/7 | n=663 p50 67.4 p95 150.5 max 456.7 | p50 21.9 p95 53.6 | p50 0.78 p95 3.82 |
| relay7_rtt80 | PASS | 21/21 pairings NO DIVERGENCE to round 892 | live=0x7f on 7/7 | n=782 p50 64.9 p95 147.0 max 360.8 | p50 21.1 p95 47.9 | p50 0.79 p95 3.74 |
| relay7_rtt160 | PASS | 21/21 pairings NO DIVERGENCE to round 892 | live=0x7f on 7/7 | n=789 p50 73.8 p95 156.2 max 251.4 | p50 20.5 p95 43.3 | p50 0.75 p95 3.43 |
| det4 | PASS | 3/3 pairings NO DIVERGENCE to round 392 | live=0xf on 4/4 | n=157 p50 35.2 p95 68.7 max 116.6 | p50 15.1 p95 28.1 | p50 0.69 p95 2.77 |
| det8 | PASS | 3/3 pairings NO DIVERGENCE to round 392 | live=0xff on 8/8 | n=230 p50 64.7 p95 159.9 max 264.7 | p50 26.0 p95 73.9 | p50 0.72 p95 3.69 |

Every rung also passed its "local input same-frame" check ([rb-local]: on
every frame a round was served, the local record the game read carried the
key this console pressed that frame, 0 mismatches over every window of every
rung), and every PAIR and RELAY pair row passed "every rewind honoured"
(unrecoverable=0: no rollback ever reached past the ring).

DET verdicts (restore+retick inside the live session, SM64DS_ROLLBACK_DET=150
DET_N=8 on p0 and p1, through the real rollback path with the served blocks):
det4 p0 and p1 IDENTICAL-EXCEPT-SOUNDQUEUE arena=0 dsstate=186 hw=0 with all
186 in the sound queue; det8 p0 and p1 IDENTICAL arena=0 dsstate=0 hw=0.

STALL (three windows, slot 2 asleep 2500 ms at frame 200 with its pump
stopped): parent stalled at round 209, "9 past the last confirmed block of
slot(s) 0x4; stalling (grace 1000 ms)"; "slot 2 retired (grace) as of round
201; 8 round(s) revised, live mask now 0x3"; the sleeper saw the Bye; the
parent and slot 1 each logged exactly one stall episode (stallevents=1), both
finished their budgets at rc 0 (1260 and 600 frames), and the survivors'
sweep is NO DIVERGENCE. Slot 1 never window-dropped the parent during the
parent's own grace stall (section 7 has the fix that made this true).

COST (8 players, VS map 0, the parent forced to rewind 8 rounds every 30
frames through the real path, 600 frames, build/tmp/ladder3d.log; the first
run of this rung, in ladder3b.log, is void: the forcing knob re-fired on the
replayed arrival at the same frame and rewound the parent forever, which is
fixed in hal/rollback.cpp with g_force_last):

| re-sim | sweep | rollback event ms (8 rounds) | per replayed frame ms | snapshot ms | restore ms |
|---|---|---|---|---|---|
| tick-only (default) | 3/3 pairings NO DIVERGENCE to round 592 | n=311 mean 77.7 p50 61.4 p95 174.9 max 326.2 | mean 28.6 p50 23.8 p95 58.4 max 124.2 | p50 0.73 p95 3.91 | p50 0.61 p95 3.23 |
| conservative (SM64DS_ROLLBACK_ACTOR_RENDER=1) | 3/3 pairings NO DIVERGENCE to round 592 | n=306 mean 73.9 p50 57.8 p95 166.9 max 383.7 | mean 27.8 p50 24.1 p95 55.9 max 95.6 | p50 0.73 p95 3.57 | p50 0.64 p95 4.19 |

Reading the cost. An 8-player 8-round rollback costs about 60 ms at the
median and 175 ms at p95 on this desk WITH EIGHT WINDOWS SHARING THE CPU;
the per-replayed-frame p50 of 24 ms against the PAIR rows' 7.6 ms is that
contention plus the 8-Player tick, not the rollback machinery (snapshot and
restore are under a millisecond at p50 in every row). The tick-only and
conservative re-sims are within noise of each other at this scale: what the
replay skips either way is the rasteriser and the present, and the actors'
Render bodies are cheap next to the tick of eight Players. Tick-only stays
the default because it is the one the DET rung proved byte-identical with
the audited list, and the conservative knob is the fallback that needs no
list. Both numbers are for a rewind of the whole 8-round window at once,
which is the worst case; the natural rollbacks in the VS rows above are 2 to
4 rounds deep at the median (rollback event p50 41 to 76 ms for 4 and 7
windows on one desk, 17 to 34 ms for a pair).

### 4.2 The other two ladders

net_proof.py --netmode rollback --only N0,N1,N2,N3,N4,N5: ALL GREEN
(build/tmp/netproof_rb2.log). N0 solo byte-identical and the carrier
silent; N1 loopback forms in loopback mode; N2 direct over this machine's
real LAN address with the parent learning the child's address off the wire;
N3 through the local relay with neither end knowing the other's address; N4
sessions form at RTT 0/40/80/120 with the delay ring never overflowing; N5
survives 80 ms plus 5 percent loss (598 of 600 rounds). N6 (the live VPS
relay) was not run from this desk; N7 measures pipelining against
stop-and-wait, which does not apply to a mode that waits for nothing.

python port/tools/battery.py --skip-build, NetMode absent (lockstep):
battery: ALL GREEN, including the shipping configuration (PORT_ROM_CLEAN,
static CRT, built in 346 s, selftest rc 0) -- build/tmp/battery3.log, and
build/tmp/battery4.log on the final binary. The one red on the way was
romblob_verify in a stale build/port-kit (13 ov002/ov046 symbols from the
merged cons tree not in the kit's baked table); a fresh kit configure cleared
it and it is not this lane's code. No level was touched by this lane; the
battery's level selftests run every mounted level under SM64DS_FAULTS_FATAL=1.

## 5. The 16-player wall, and why it is not this lane's

The wide VS session at 16 players does not survive its second frame on four
of its sixteen consoles, and it does not survive it in LOCKSTEP either. The
control (build/tmp/ctrl16.py, NetMode lockstep, VS map 0, 16 windows, no
rollback code reachable) faults on slots 8, 12, 14 and 15 at frame 2, rc
0xC0000005, every time:

    FAULT code c0000005 at +0x000428f0 accessing ffffe11a
      walker node 3002C2A4 actor 3002C26C id 0x14f

id 0x14f is 335, the MINIMAP. The frame is +0x428f0 = OAM::Render, called
from Minimap::Render (src/_ZN7Minimap6RenderEv.cpp), and the read that faults
is `unk70[idx]` / `MM_VS_PLAYER_ICONS[pl->unk8 + idx * 4]` with idx =
data_0209f250, this console's player number: the ROM's minimap keeps
four-entry position and icon tables (the DS never had more than four
players) and a player number of 8 or more indexes past them into whatever
sits there. Slots 4 to 7 happen to read something harmless. That is a
src/ file this lane may not touch, and the fix (a wide-session Minimap seat
in hal/, or the wide lane clamping the minimap's own-player index) belongs to
the lane that seated 16 players.

So the 16-player restore+retick and cost gates could not be run on a whole
session. The ladder's --wide knob runs DET and COST at 8 players, the widest
session that survives, and section 4.1 carries those numbers; the 16-player
attempt is kept in build/tmp/ladder3c.log for the record (the session
collapses at frame 2 on the four faulting consoles, and every surviving
console's numbers are of a session that fell apart). The rollback code path
does not scale with the player count except in the re-sim (a wide frame
ticks more Players), so the 8-player p95 is the number to plan against until
the minimap is fixed and the 16-player rung can be re-run as it stands.

## 6. The Render-side audit, done by measurement

The spike's tick-only re-sim rests on "no Render body writes state a tick
reads". The first cut of that audit was by reading, and the DET rung showed
it was short: with only the four actors it named in the replay walk, a
4-player restore+retick left one arena byte different on every console, at
Minimap +0x22e (the star-marker blink counter Minimap::Render advances), and
once that was in the walk, 14 to 20 bytes of the UnchainedChomp's chain-link
matrices (ModelAnim::Render through its five +0x370 links advances them, and
its Behavior reads them back). Both were found by bisection, not by reading:
SM64DS_ROLLBACK_REPLAY_ALSO="id,id" adds actors to the replay walk without a
rebuild, and DET at 4 players with the map's census (POWER_STAR, STAR_MARKER,
PLAYER, WATERFALL_MIST, CAP, TREE, COIN, CAMERA, HUD, MINIMAP,
UNCHAINED_CHOMP, AMBIENT_SOUND) split into groups named the chomp in two
rounds. The replay walk now runs 329, 330, 271, 264, 335 and 337. Two other
findings on the way: the walk's actor-id read was unmasked (aliveState sits
in the byte above actorID, so 0x1014f never equalled 335 and the first cut
ran none of its four), and the rig's injected key toggled on a publish
counter of its own, so a replayed frame staged a different local record than
the straight run had; it is keyed on the seam's round count now, which rides
in the snapshot.

The rule this leaves for a new map: run DET on it. An actor whose Render
advances an animation or a counter its tick reads shows up as arena bytes
with its id named, and goes on the list. SM64DS_ROLLBACK_ACTOR_RENDER=1 (the
conservative re-sim, every Render body kept) is the fallback that needs no
list and was also measured byte-identical in the arena (build/tmp/l_det_cons.log).

What the DET verdict counts. The arena and the hardware stores must come
back with 0 bytes different. The .dsstate section has one region a restore
does not put back: the hosted ARM7 sound command queue (data_020a6760's node
pool, the data_020a64a8 batch ring, the data_020a6484.. cursors, the
data_020a50ec sdat bss), which the restore re-seeds the way lk6 does and the
muted replay re-fills; the spike documented it and hal/rollback.cpp names
those bytes (in_sound_queue) so the verdict reads IDENTICAL-EXCEPT-SOUNDQUEUE
rather than failing on audio. Any .dsstate byte outside that queue is a
divergence and fails the rung. With nothing skipped at all
(SM64DS_ROLLBACK_FULL_RESIM=1, build/tmp/l_det_full.log) every window came
back IDENTICAL in all three regions, sound queue included.

## 7. What changed on the way, and what is not closed

Fixes made while proving, all in this branch:

* the mode bit moved from accept bit 16 (LAGDELAY's report ack, which the
  rebase brought in) to bit 17, and it rides on the roster announce as well
  as the unicast accept, because a child seated off the roster announce first
  was reading lockstep and never rolling back (the first PAIR run after the
  rebase: one end at 189 rollbacks, the other at 0);
* a child never window-drops the parent: while the parent grace-drops a
  sleeper it opens no new rounds for up to a second, and a child predicting
  ahead of it overshot the window on slot 0 and left, then re-knocked, seven
  times in the STALL rung. The parent slot on a child is now a wait, never a
  stall, and the "parent went silent" leave is gone (a parent that is truly
  gone is ended by the ROM's own wait bound, as before);
* a grace drop is bounded by the ring and by g_pipe_low, and a dropped slot's
  later datagrams are ignored for the session so it cannot reseat itself
  mid-match as a fresh join;
* the tick-only replay walk masks the actor id (aliveState shares the word),
  and runs the Minimap and the UnchainedChomp (section 6);
* the injected key is keyed on the seam's round count, not a publish counter;
* the DET verdict names the sound command queue instead of failing on it;
* the ladder starts a fresh relay per RTT (the reused relay seated only the
  first RTT's wide session), takes --wide for the wide count, --extra for
  bisecting, and the forcing knob fires once per frame number.

Not closed, in order of weight:

1. THE 16-PLAYER SESSION (section 5). Gates (b) and (c) at 16 Players cannot
   be run until the wide lane's Minimap seat stops faulting at frame 2 on
   player numbers 8 and up, in lockstep as much as in rollback. The 8-player
   numbers stand in; build/tmp/ladder3c.log holds the 16-player attempt.
2. THE TICK-ONLY LIST IS PROVEN PER MAP. VS maps 0 and 1 (the DET and sweep
   maps) are byte-identical with the six ids in the walk; a map with another
   actor whose Render advances something its tick reads will show up in DET
   as arena bytes with the id named. The conservative knob needs no list.
3. THE NUMBERS ARE ONE DESK'S. Every multi-window row shares one CPU; a real
   session has one console per machine and pays the PAIR row's per-frame
   cost with the 4- or 7-player tick, which nothing here measured in
   isolation. The live-internet rung (net_proof N6) was not run.
4. A DEAD PARENT ON A CHILD IS THE ROM'S BOUND. The child waits (about 20 s
   of the ROM's own turns) rather than leaving on a timer of its own; a
   keepalive-based leave would be the next step if that wait is too long in
   practice.
5. AUDIO ACROSS A REWIND. Sounds that started inside the rewound window play
   from where the replay leaves them; the output stage is muted during the
   replay and the queue is re-seeded. Every rollback game has this; nobody
   has listened to it here.

For the owner, in plain English. The rollback mode works: two, four, seven
and eight copies of the game, on this machine and through the relay, at
every fake internet delay we tried, ran the same match byte for byte while
guessing and rewinding hundreds of times each, and every copy's own controls
responded on the same frame they were pressed. When one copy froze for two
and a half seconds the others kept playing, held for one second, dropped it,
and carried on in agreement. Rewinding and replaying the game's memory was
proven exact (the world comes back identical; only the sound queue, which is
re-seeded on purpose, differs). The old lockstep mode is untouched and the
whole normal test battery is green on it. Two things are not done: sixteen
players cannot be tested because the minimap crashes on players nine and up
in the existing sixteen-player mode itself (not this work), and all the
speed numbers were taken with every copy of the game crammed onto one
computer, so real-world numbers will be better than the table but have not
been measured across real machines.

## 8. Rollback is the default (owner's decision, 2026-09-03)

The owner decided that rollback ships ON in the next release. What changed:

* THE DEFAULT. hal/host_settings.cpp sets g_net_mode to rollback with the
  other defaults, before it looks for the file, so settings.json absent,
  unparseable, or without the key all read as rollback. Only the word
  "lockstep" moves it. SM64DS_NETMODE still wins over the file, both ways.
  The "[settings]" boot line follows the file's rule (a setting is said only
  when it is off its default), so it now names lockstep when lockstep was
  chosen, in plain words, and says nothing for rollback.
* THE WIDTH GUARD. Determinism (DET) and cost (COST) are proven only up to
  eight seated players; past that the wide lane's Minimap::Render faults at
  frame 2 on player numbers 8 and up, in lockstep as much as in rollback
  (section 5). So kRollbackMaxPlayers = 8 lives in hal/host_settings.h next
  to the NetMode parse, and the transport's install block, after the env has
  had its say, runs any session opened wider than that in lockstep with one
  line naming the width and the reason:

      [comms:loopback] NetMode rollback asked for a 9-player session, but
      rollback is proven (determinism and cost) only up to 8 players; this
      session runs LOCKSTEP instead

  Every peer reads the same SM64DS_VS_PLAYERS, so every peer lands on the
  same side of the line, and the parent's accept bit 17 carries the mode
  anyway.
* THE LADDER. rollback_proof.py takes --netmode none (no SM64DS_NETMODE
  handed to the game, and there is no settings.json in a proof run, so the
  mode is the built-in default) and a GUARD rung (--only GUARD --wide 9).

Re-proved on the rebuilt binary, exe 63536EEC702E5069, build/tmp of the
worktree (pair_default.log, n0_default.log, guard9.log, battery_netmode.log):

* PAIR with --netmode none: NetMode ROLLBACK printed by 2/2 windows at every
  RTT, 1/1 pairings NO DIVERGENCE to round 592 at RTT 0/40/80/160, [rb-local]
  OK on 2/2, unrecoverable 0 with 119 to 191 rollbacks per window. ALL GREEN
  (20 pass, 0 fail).
* net_proof N0, default mode: SOLO IS BYTE-IDENTICAL, pos=(-4915200,
  2929633, 11141348) as pinned, and the carrier is SILENT. A solo run never
  installs the transport, so the default touches nothing there.
* GUARD at --wide 9: the fallback line printed by 9/9 windows, NetMode
  ROLLBACK by 0/9. ALL GREEN (2 pass, 0 fail).
* battery.py --skip-build: ALL GREEN (build/tmp/battery_netmode2.log):
  every smoke, level and scene selftest ok, default boot to the title ok,
  linkage 9284 (82.0%), ptr_audit 0, the shipping configuration built in
  201 s and its selftest rc 0. The first attempt (battery_netmode.log) went
  red on the same stale build/port-kit romblob_verify the section 4.2 run
  hit (the 13 ov002/ov046 symbols); a fresh kit configure cleared it, as
  before, and nothing in this change touches the kit.
* the boundary at exactly 8 players (build/tmp/guard8.log): NetMode ROLLBACK
  printed by 8/8 windows and the fallback line by none, so the guard is
  strictly "more than eight".

## 9. The re-sim cost lane: rollback affordable while the peer moves

The follow-up to section 4. That table measured rollback events on a
console whose peers stood still and toggled a key; this section measures
what a player's FRAME costs while the remote peer walks and turns, on the
final binary named at the end of this section, and what was done to keep
it under 33 ms. The measurement rig is the MOVE rung of rollback_proof.py
(two windows on VS map 0 through the local relay at RTT 40 and 80 ms, paced,
each peer walking for the whole run: "walk" holds Up on both, "turn"
alternates Up and Left every 45 rounds on opposite phases); the numbers are
the REAL FRAME (loop top to the boundary at which the frame is settled,
pace excluded, the rollback inside it) as hal/rollback.cpp reports it.

### 9.1 What changed

* THE REPLAY DIET (commit "the moving-peer rig and the replay diet"): a
  replayed frame no longer clears the framebuffer, composites engine A,
  fades, draws the overlays, the shadows or the players' host pixels, and
  the last replayed frame is tick-only too (the corrected world is shown by
  the next real frame). The particle SUBMISSION stays in a replayed frame:
  DET on VS map 0 named a 16-bit countdown per 0x78-byte particle entry
  that port_particle_render advances (8 skipped renders, 8 steps short), so
  it belongs with the Stage::Render game-state spans, not with the pixels.
  Per replayed frame p50 went from about 7.6 ms (section 4.1, PAIR rows) to
  1.3 to 2.4 ms (table below).
* HEADING EXTRAPOLATION AND REPEAT-LAST were already the predictor
  (comms_loopback.cpp rb_predict: the last confirmed block repeated, the
  camera heading at +0x0B carried forward at its last per-round delta) and
  they stay. The transport now counts WHICH block bytes a contradicted
  guess missed on: with the peer turning, 95 to 98 percent of mispredicted
  blocks differ ONLY in the heading, by 1 to 16 units out of 65536 most of
  the time (buckets on the close line); the key misses are the 45-round
  toggles, and the stylus and payload bytes never miss. The confirmed
  headings of a whole turn run were replayed offline (build/tmp/pred_eval.py
  on the [rbh] trace) against flat repeat, first-order (shipped), second-
  order, damped second-order, sign-gated and two-delta-average predictors
  at 2, 3 and 5 rounds of lag: first-order is the best of the family (740
  of 896 rounds still mispredict at lag 3, against 751 flat and 760
  second-order), because the ROM's camera yaw moves by an integer amount
  that jitters by a few units every frame while it turns, and a byte-exact
  simulation must roll back on a one-unit miss. So the rollback RATE while
  a peer turns is a property of the ROM's camera math (9 to 14 a second
  here, most of them 2 to 3 frames deep), and the lever is the cost per
  rollback, which the diet took to about 4 to 8 ms per event.
* THE PER-FRAME BUDGET (hal/rollback.cpp g_budget_ms,
  SM64DS_ROLLBACK_BUDGET_MS, default 33, 0 = off): the count spread
  (SM64DS_ROLLBACK_SPREAD, default 0) by time. At every replayed frame's
  boundary, if the frame's elapsed work plus what a rendered frame costs on
  this machine (the loop body of the frames that rendered, smoothed, load
  outliers over a second excluded) would pass the budget, the next
  replayed frame renders, presents and paces like a real one, and the
  replay goes on in the next frame. The real frame stays open across a
  replay and closes at the presented boundary, so a split replay is
  measured as the chunks the player sees. At 33 ms the budget NEVER FIRED
  on this desk in a pair (0 splits in every MOVE window: a whole 3-frame
  replay plus a rendered frame is about 8 + 6 ms); forced to 12 ms on the
  turn run at RTT 80 it split 406 and 402 replays and held the real frame
  p95 at 14.5 and 18.9 ms (build/tmp/move_budget12.log, sweep NO
  DIVERGENCE; the "rollback event" stat then spans the paced chunks and
  reads 170 to 245 ms, which is wall time across presented frames, not
  work). It is the cap for a loaded machine, and it costs nothing when the
  machine is not.
* THE SOUND DRAIN was the biggest non-rollback spike: in an early walk run
  (build/tmp/move_repro.log, zero rollbacks) the real frame p95 was 84 to
  90 ms, all of it in sdat_host_tick. SM64DS_SND_SLOW_MS=<n> now names any
  drain over n ms by its parts, and on the final binary the spikes are 38
  to 49 ms, one or two per 900 frames, ENTIRELY in sd_out_push (the
  waveOut write path; the ARM9 sound frame and the queue drain are 0 ms).
  Not fixed here: it is hal/sdat's output stage, not the rollback, and it
  is what a player pays with or without rollback. The split of that push
  between the mix render and waveOutWrite is on the same line for the lane
  that takes it.
* DET NAMES WHAT IT FINDS. The verdict prints every .dsstate range with a
  byte outside the sound command queue (marked), and for an arena byte the
  aligned word in both worlds and whether either reads as a host address.
  On VS map 3 the bytes outside the queue were the ARM9's own voice
  bookkeeping (data_020a4d54 the FREE voice list, data_020a4d60 the ACTIVE
  one, data_020a4d6c the 32 x 0x1c voice nodes; named through the linker
  map): a voice the hosted ARM7 was still playing in the straight run is
  reported finished to the re-run, whose consumer was re-seeded, so the
  ARM9 frees it a frame earlier. That is the audio-across-a-rewind question
  of section 7 item 5 showing up as three lists, not the game's world, and
  in_sound_queue names them now.

### 9.2 Numbers: the moving peer, final binary, this desk

Before is the checkpoint binary with the diet and no budget
(build/tmp/move_nobudget.log, SM64DS_ROLLBACK_BUDGET_MS=0); after is the
final binary with the budget at its default (build/tmp/move_final.log).
Both windows of a pair on one CPU with the relay; p0 is the parent. The
pre-diet numbers are build/tmp/move_before.log (turn at RTT 40: real frame
p50 22 to 23, p95 103 to 110 ms; the RTT 80 turn session did not form in
that run).

| rung | window | real frame p50 / p95 / max ms (before) | (after) | rollbacks/s (after) | frames per event | per replayed frame p50 ms |
|---|---|---|---|---|---|---|
| walk rtt40 | p0 | 7.6 / 9.1 / 13.8 | 11.4 / 18.9 / 44.6 | 0.11 | 3.2 | 2.4 |
| walk rtt40 | p1 | 7.8 / 9.0 / 11.6 | 11.4 / 18.0 / 28.2 | 0.13 | 3.0 | 2.2 |
| walk rtt80 | p0 | 6.8 / 7.4 / 19.7 | 6.9 / 13.0 / 42.8 | 0.10 | 5.6 | 1.3 |
| walk rtt80 | p1 | 6.8 / 7.3 / 10.4 | 6.9 / 14.5 / 37.3 | 0 | - | - |
| turn rtt40 | p0 | 10.9 / 12.0 / 14.8 | 12.8 / 15.2 / 19.1 | 13.9 | 3.0 | 1.7 |
| turn rtt40 | p1 | 7.8 / 11.2 / 15.2 | 10.2 / 13.6 / 17.9 | 14.3 | 2.0 | 1.9 |
| turn rtt80 | p0 | 6.9 / 19.0 / 28.7 | 9.0 / 18.2 / 35.6 | 9.3 | 4.7 | 1.7 |
| turn rtt80 | p1 | 7.2 / 10.1 / 13.1 | 8.3 / 11.5 / 19.5 | 13.8 | 2.0 | 1.7 |

Reading it: every window's real-frame p95 is under 20 ms with the peer
moving, against the 33 ms target, at both round trips. The before and
after columns are the same binary family minutes apart and differ by desk
noise (the after run's walk rows carry the sound-drain spikes named above:
the 44 ms max is one waveOut push), not by the budget, which never fired.
The rollback rate while the peer turns (9 to 14 a second) is the ROM's
camera jitter, per 9.1; at 1.3 to 2.4 ms per replayed frame and 2 to 5
frames per event it costs 4 to 8 ms per event at p50.

### 9.3 DET per arena, final binary (build/tmp/det_maps5.log, 4 players, paced, rewinding 9)

| arena | p0 | p1 | sweep |
|---|---|---|---|
| VS map 0 (det4) | DIVERGED arena=1 dsstate=0 hw=0 | DIVERGED arena=1 (dsstate 187, all sound queue) hw=0 | 3/3 NO DIVERGENCE |
| VS map 1 | IDENTICAL-EXCEPT-SOUNDQUEUE arena=0 dsstate=203 hw=0 | IDENTICAL arena=0 dsstate=0 hw=0 | 3/3 NO DIVERGENCE |
| VS map 2 | IDENTICAL-EXCEPT-SOUNDQUEUE arena=0 dsstate=186 hw=0 | same | 3/3 NO DIVERGENCE |
| VS map 3 | IDENTICAL-EXCEPT-SOUNDQUEUE arena=0 dsstate=108 hw=0 | same | 3/3 NO DIVERGENCE |
| VS map 0, 8 players (det8) | IDENTICAL arena=0 dsstate=0 hw=0 | IDENTICAL-EXCEPT-SOUNDQUEUE arena=0 dsstate=359 hw=0 | 3/3 NO DIVERGENCE |

THE MAP 0 BYTE, FOUND AND FIXED. One arena byte, arena +0xe091c, the low
byte of a word 0x4d1f7bXX, came back 0x20 or 0x40 apart between the
straight run and the re-run, on some windows of some runs of VS map 0
only, harmlessly (the sweep of every run was NO DIVERGENCE on every
pairing throughout). Two things were wrong before this could be named.

First, the heap-block namer this section's word diagnostic grew (the
"host pointer" guess above was checked, and was wrong) never actually
ran: its search loop's own entry condition was false at the loop's
starting value on every call, so the diagnostic printed nothing on any
DET run from the day it was written -- a C `for` loop checks its
condition before the first pass, and `b + 0x10 <= w + 4` is false at
`b == w`. Rewritten as a loop that checks the starting word too
(hal/rollback.cpp name_word), it named the block on the first re-run:
arena +0xe0810, size 0x1f0, magic 0x5544 (an ExpandingHeapAllocator used
node), the diff sitting at +0x10c into the payload.

Second, that block is not an actor's -- name_arena_addr's own
render-list walk placed the nearest actor 0xa626c away, and the payload's
own first words carry no vtable, so it was never going to be fixed by
adding an id to the tick-only replay list. SM64DS_ROLLBACK_ACTOR_RENDER=1
(the conservative re-sim, every actor Render body kept) reproduced the
same byte just as often as the default -- direct proof the actor list was
never the lever. A one-shot trace bracketing every rb_skip_render()-gated
call by name (SM64DS_ROLLBACK_TRACE_E091C=1, since removed) walked the
word through a frame and found the write sitting inside
hal_render_player_world, specifically hal_player_texseq_head: Player::
Render's per-character eye-blink material update (TextureSequence::Update
onto "the CURRENT head model's components", the comment's own words,
"everyone gets, all characters"). VS map 0's four windows in this rig all
play the same character, so all four Players' head models are the SAME
loaded asset, and its ModelComponents flags word is shared state a later
call reads (whether the blink shows) -- exactly section 6's rule for an
actor's Render, on a Player instead of an actor, so not on the actor
list. hal_render_player_world -- draw and all -- stood down under
rb_skip_render() every replayed frame, tick-only or conservative, so the
shared flag fell behind by whatever a rollback's replay skipped.

THE FIX. hal_player_texseq_tick (hal/player_bridges.cpp) pulls the two
state-mutating calls (texseq_body, texseq_head) out from under the draw,
behind the same port_player_render_hidden gate Player::Render's own hide
logic uses, and tests/walk_window.cpp now calls it on every live peer
(local and remote) every tick, replayed or not, beside the particle
submission's own carve-out -- while UpdateVerts, ModelAnim::Render and
the wing model stay under rb_skip_render(), because DET showed they
write nothing a tick reads. Re-proved on the fixed binary: three straight
IDENTICAL restore+retick pairs at 4 players on map 0 alone
(build/tmp/det_map0_fix1.log, det_map0_fix2.log, det_map0_fix4.log,
arena=0 dsstate=0 every time -- no sound-queue bytes pending either), and
one combined run across all four VS arenas plus the 8-player rung
(build/tmp/det_allmaps_fix1.log): ALL GREEN, 25 pass 0 fail, arena=0 on
every window of every map, dsstate explained entirely by the sound queue
where it is nonzero at all. PAIR and one MOVE arm were re-run on the same
binary (section 4.1 and 9.2's numbers stand; both green, no regression),
and the battery is ALL GREEN.

### 9.4 Before/after, the fix that closed it

Same rig as 9.3 (4 players, VS map 0, paced, rewinding 9 frames), before
the fix (the checkpoint binary, section 9.3's own numbers) and after
(build/tmp/det_map0_fix1.log, det_map0_fix2.log, det_map0_fix4.log, and
the map0 row of build/tmp/det_allmaps_fix1.log, which also carries maps 1
to 3 and the 8-player rung on the same fixed binary).

| run | before | after |
|---|---|---|
| map0 det4 p0 | DIVERGED arena=1 dsstate=0 hw=0 | BYTE-IDENTICAL arena=0 dsstate=0 hw=0 (x3), IDENTICAL-EXCEPT-SOUNDQUEUE arena=0 dsstate=187 (x1) |
| map0 det4 p1 | DIVERGED arena=1 dsstate=187 (soundqueue) hw=0 | BYTE-IDENTICAL arena=0 dsstate=0 hw=0 (x3), IDENTICAL-EXCEPT-SOUNDQUEUE arena=0 dsstate=187 (x1) |
| map0 det4 sweep | 3/3 NO DIVERGENCE | 3/3 NO DIVERGENCE, every run |
| map1 / map2 / map3 det4 | already arena=0 (never the byte) | arena=0, unchanged |
| map0 det8 (8 players) | IDENTICAL arena=0 / IDENTICAL-EXCEPT-SOUNDQUEUE arena=0 | IDENTICAL arena=0 / IDENTICAL arena=0 |
| combined 4-arena + det8 run | -- | ALL GREEN, 25 pass 0 fail (build/tmp/det_allmaps_fix1.log) |
| PAIR (4 RTTs) | section 4.1's own numbers, unaffected | ALL GREEN, 20 pass 0 fail, re-run on the fixed binary (build/tmp/pair_rtt40_retest.log) |
| MOVE, one arm (walk+turn, RTT 40) | section 9.2's own numbers, unaffected | ALL GREEN, 10 pass 0 fail, sweep NO DIVERGENCE both profiles (build/tmp/move_fix1.log) |
| battery.py --skip-build | ALL GREEN before this lane touched anything | ALL GREEN after (build/tmp/battery_fix1.log) |

For the owner, in plain English. One byte inside the game's memory came
back slightly different after a rewind on the very first VS course only,
never on the others, and it never once threw two players' games out of
sync -- the earlier proof runs already showed that, which is why this was
the one open item and not a red light. What it turned out to be: when
several players are using the same character, the game keeps one shared
copy of that character's face, and a rewind was skipping the tiny "which
eye-blink frame is showing" update on the copies of other players while
correcting a guess, so that shared copy could end up one blink-frame off
from what it would have been. Purely cosmetic, never seen by any game
logic, and now fixed: that update runs on every tick again, rewound or
not, the same way the sparkle and fire effects already did. Proved twice
more on map 0 alone and once across all four courses at once, all clean;
the multiplayer and moving-peer checks and the full test battery were
run again on the same build and are still green.

### 9.5 The rebase, and a build-cache scare that was not a bug

Two more things happened closing this out, both worth naming so nobody
re-chases them.

REBASED ONTO CONS 1192d9cee. This branch's base (f1b05e8fe) predated
SEATCAMERATAG (1192d9cee, camera tag 333 seated by address); the two
commits between them touch no file this lane owns
(port/hal/rollback.cpp, player_bridges.cpp, tests/walk_window.cpp,
status/ROLLBACK_SHIP.md), so `git rebase 1192d9cee` replayed clean, no
conflicts. It also explains a battery red that showed up first: a stale
build/port-kit (a long-lived worktree binary directory, not rebuilt in
this session before now) had a baked romblob table one rebase behind and
romblob_verify failed on CameraTag_SpawnInfo not being in it -- a
build-kit staleness, not code, and `rm -rf build/port-kit` before the
battery's next run cleared it the way section 4.2 and 8's own notes said
a fresh kit configure would.

A SECOND, LOUDER RED, AND WHERE IT ACTUALLY WAS. Deleting build/port-kit
uncovered a real-looking one: default boot (a bare launch, no
SM64DS_* env, which is ROLLBACK by section 8's default) failed rc=1 at
frame 0, reproducibly, while NetMode lockstep on the same binary ran 300
frames clean. Bisecting by hand (checking out the pre-fix files, stubbing
hal_player_texseq_tick's body, adding entry/exit traces) chased it
through several contradictory results -- a stub passed, a fully-traced
real body passed and was never even entered, the untouched committed code
failed -- because none of those rebuilds ever cleared build/port's own
incremental object cache, which had accumulated four back-to-back
edit/rebuild cycles in the same directory. `rm -rf build/port` and one
clean rebuild of the exact committed tree (no edits at all) ran default
boot clean 6/6, and DET (all four arenas + det8), PAIR and one MOVE arm
all re-passed on that same clean binary (build/tmp/det_clean_final.log,
pair_clean_final.log, move_clean_final.log). The lesson for the next
session bisecting a boot-time crash in this worktree: wipe build/port (or
build/port-kit) before trusting a rebuild that follows several rapid
in-place edits, or a stale object file can look exactly like a real
regression.

THE FINAL NUMBERS, fully clean binary, exe hash in build_clean.log,
rebased onto 1192d9cee:

* DET, all four VS arenas plus det8: ALL GREEN, 25 pass 0 fail, arena=0
  on every window of every map (build/tmp/det_clean_final.log).
* PAIR, all four RTTs: ALL GREEN, 20 pass 0 fail, sweep NO DIVERGENCE
  (build/tmp/pair_clean_final.log). An earlier PAIR attempt on this desk
  read one FAIL at RTT 40 from a stray walk_window.exe left running out
  of an unrelated worktree (C:\tmp\oc) colliding on the PID-derived port
  range; re-run alone, clean, it passed -- a desk-sharing artifact, not
  this branch's.
* MOVE, one arm (walk + turn, RTT 40): ALL GREEN, 10 pass 0 fail, sweep
  NO DIVERGENCE both profiles (build/tmp/move_clean_final.log).
* battery.py --skip-build, build/port-kit rebuilt clean: ALL GREEN --
  every smoke, level and scene selftest ok, default boot ok (300 frames,
  clean), linkage 9487 (83.7%), ptr_audit 0, shipcfg build and selftest
  ok (build/tmp/battery_clean_final.log).

Rollback is still the shipped default (section 8); nothing about that
decision changed here, only the one open item under it.
