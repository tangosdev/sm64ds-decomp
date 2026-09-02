# ADVENTURE - single-player adventure, played together as ghosts

Lane: ADVENTURE. Tango's ask (2026-09-02): play the single-player adventure
online. Everyone in adventure mode sees see-through ghost versions of the other
players in the same level (no collision, no interference) and can chat. Players
can add each other to a PARTY so their instances sync: same world state, shared
progress in the level. Build on the 16P wire, the name tags, and the
lockstep/sync layer.

Worktree `C:\tmp\adv`, branch `port-adventure-ghosts`, base `f8a0c7846`
(port-mount-noseat-cluster's tip, the 0.3.2 line + name tags). Own build dir
`C:\tmp\adv\build\port`. Nothing pushed, nothing merged. This is host-layer
work only: `git diff --stat <base> -- src/` stays EMPTY, no byte of matched
source moves, same discipline the VS16 and NAMETAG lanes held.

---

## The load-bearing decision: adventure is NOT the versus engine

The shipped 16-player VS match is **input lockstep** (see `status/VS16.md`,
`port/hal/comms_seam.h:166-170`). Every instance runs the identical
deterministic ROM simulation of the whole world and all sixteen players; the
only thing on the wire each frame is a 0x20-byte block that is essentially
controller input. All instances stay bit-identical; the proof regime is "NO
DIVERGENCE." This has two properties that make it the WRONG base for adventure:

1. Everyone must be doing the same synchronized thing. There is no per-player
   authority for gameplay; one shared deterministic world is recomputed on
   every machine. Sixteen players exploring freely and doing DIFFERENT things
   is not representable on this model.
2. The slowest player stalls everyone. `exchange()` only completes when every
   live peer's frame has arrived; the parent's frame counter is the one clock.
   A hiccup on any link freezes the room, and a real desync drops the whole
   session to solo.

So adventure mode uses the **state-broadcast** model instead, the one already
prototyped in `port/hal/comms_sync.cpp`: each player runs their OWN solo game,
authoritative for their own body only, and broadcasts where that body is and
what it is doing. Everyone else draws those broadcasts as ghosts. Nobody's game
depends on anyone else's inputs, so there is no bit-identical-determinism
requirement, no whole-session desync, and no slowest-player stall. This is the
Elden Ring / FromSoft shape Tango named: you play your own game, you see faint
ghosts of others near you.

`comms_sync.cpp`'s own banner already states the model: owner-authoritative per
body, "NO ROLLBACK. NO RESIMULATION. NO INPUT PREDICTION," corrects only remote
bodies, never the local one. Today it is a cosmetic smoother layered on top of
lockstep and carries only position/yaw/anim. Adventure mode promotes that same
per-body-authority idea into a standalone session type and extends what it
carries (character, palette, spawn/despawn, level id).

## Two tiers

- **GHOSTS (base tier, this lane's target).** Everyone in the same level sees
  translucent, walk-through, non-interfering versions of the others, with name
  tags and chat. NO shared world. Each player's world is entirely their own;
  the only thing shared is "here is my body, drawn faintly in your world."
- **PARTY (tight tier, a later phase, NOT this lane).** Opt-in. Members' worlds
  share state and progress. This is the genuinely hard problem: it needs an
  authority model for the shared world (whose game is the source of truth for a
  door that got opened, a star that got collected, an enemy that died). Scoped
  separately, after ghosts are proven. Do not promise it in the same breath.

## What already exists and gets reused (verified in-tree)

| piece | where | reuse for ghosts |
|---|---|---|
| state-broadcast of own body, apply remote snapshots | `port/hal/comms_sync.cpp` | the ghost position/pose channel, already built (pos/yaw/anim); extend to carry character + palette + level id |
| the network carrier + relay + lobby | `comms_loopback.cpp`, `tools/relay/relay.py`, `tools/lobby/` | connect + slot assignment; adventure needs a "presence room" contract, not a "match" contract |
| remote players are real Player bodies | per-slot `data_0209f394[16]` | the thing a ghost IS: a Player body you chose to draw translucent and not collide with |
| see-through rendering | `ModelBase::ApplyOpacity` / `mOpacity`, `hal_render_player_world` in `player_bridges.cpp` | draw the ghost at reduced alpha |
| no-collision / no-interference | per-body disable-interaction state: clear `+0x713` mIsBodyClsnEnabled, set `+0x2ec |= 4`, `+0x709` mIsNoControl (see `status/VSMERCY.md`, `player_fields.h`) | hold remote bodies disabled; re-assert after ROM `ChangeState` re-arms them |
| the player-vs-player collision gate | `src/func_ov002_020d869c.cpp`, gated `src/func_ov002_020d82f0.c` | the exact entry to neutralize per-mode |
| label/overlay projection to a head | `port/tests/nametag.h` `nt_project` | ghost name labels and chat bubbles, drop-in |

## What is genuinely new to build

1. An **adventure-ghost mode flag**, distinct from the VS flag `data_0209f2d8==1`
   (several draw/collide gates key off VS mode, so ghost mode must be its own
   scope, not a reuse of the VS flag).
2. A **ghost spawn/despawn path**: in your own solo level, spawn a puppet body
   for each remote player present in the same level, and remove it when they
   leave or change level. This is NOT the VS match spawn (which places all
   players in one shared session); it is per-remote-player, driven by presence.
3. **Presence room** on the lobby/relay: a persistent "who is in which level and
   where" channel rather than a one-shot match seating. Carries level id so we
   only ghost players who are in the same level.
4. **In-game text chat**: the current chat is lobby-only and never reaches a
   running game. A new in-match text channel (small, on the aux UDP path) plus
   a host-side overlay (reusing the nametag font/draw).
5. **Proximity voice**: fully greenfield, large, its own phase. The one useful
   primitive is that every body's world position is already available for
   distance attenuation, and the ROM sound API takes a `Vector3` for positional
   playback. Not in the base slice.

## Milestones

- **M1 (this slice): two players, same level, ghosts.** Two solo instances,
  connected, each renders the other as a see-through non-colliding body with a
  name tag. No shared world, no chat yet. Proves the whole base architecture on
  the smallest slice, entirely from patterns already proven in the tree.
- M2: N players, presence room with level-id filtering, spawn/despawn as
  players enter and leave a level.
- M3: in-game text chat.
- M4: proximity voice.
- M5+ (separate lane): PARTY world-sync.

## Status

- 2026-09-02: lane opened, design recorded. Base tier = ghosts over your own
  game; party = later phase. Starting M1.
