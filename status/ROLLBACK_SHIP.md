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
if that player is still silent they are dropped and the match goes on. The
default mode is unchanged: "lockstep" is what an absent key reads as, and with
it the rollback code is one cached compare per frame and otherwise dead.

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
  once at load; SM64DS_NETMODE overrides it at transport install. The parent's
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
