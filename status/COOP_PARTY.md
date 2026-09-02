# COOP_PARTY - the party tier: a shared, synced world

Lane: ADVENTURE co-op, party tier (A leads co-op; C owns rollback). This is the
DESIGN, drafted while C's rollback ship lane is in flight, so the seam is ready
to co-design the moment rollback reports. Nothing here is built yet.

## The three online tiers, and why party is different

1. VS match = input LOCKSTEP. Every console runs the identical deterministic sim
   of one shared world; only inputs cross the wire; slowest peer stalls the room.
   Shipped (0.3.x). Not the co-op model.
2. Adventure GHOSTS (built, M1+M2, port-coop-m2): each player runs their OWN solo
   game and STATE-BROADCASTS their body; peers draw it as a see-through,
   non-colliding ghost, filtered to same-level presence (peer_visible), joinable
   by room code (lobby session_type "adventure"). No shared world, no determinism
   requirement, one laggy peer only rubber-bands their own ghost.
3. Party (this doc) = a SHARED, SYNCED world for an opt-in group: same doors
   opened, same stars collected, same enemies, shared progress. Members are
   genuinely in one world together, not ghosts.

The party tier's requirement (shared world + shared progress) is exactly a
DETERMINISTIC SHARED SIMULATION, which is what C's rollback netcode provides.
So party co-op = C's rollback infra applied to co-op, NOT a separate mechanism.

## Why party rides rollback, not the ghost broadcast

The ghost broadcast is deliberately approximate (own-body authority, no shared
outcomes). A party needs authoritative shared events: who opened the door, who
took the star, whether an enemy died. Deciding those from a smoothed, late copy
of the other body (the ghost model) makes every such event a guess the two
screens disagree on. Rollback keeps one thing that makes a shared world fair:
both machines run the exact same simulation, proven byte-identical across a
rewind and replay (ROLLBACK.md section 3: 0 arena bytes differ, only the audio
queue, which re-sim mutes).

The rollback spike already measured this is affordable: ~1.4 ms whole-state
snapshot, ~0.3-1.5 ms per re-sim frame, byte-identical retick, hook at the
transport's exchange()/peer_block() pair. The snapshot IS the dsstate save-state
capture (arena + .dsstate + hw regions) A built for save states.

## The seam with rollback (to co-design with C when rollback ships)

A party session is a rollback session seated through the ADVENTURE invite path
rather than the VS lobby:

1. INVITE / SEAT. Reuse the lobby "adventure" session_type + room code that M2
   added, but seat it as a rollback session (NetMode=rollback) instead of the
   state-broadcast ghost path. A party is a small fixed group (2-4 to start), so
   the wide-16 rollback cost concern is smaller than a full VS.
2. SHARED WORLD ENTRY. On join, party members enter the SAME level/sublevel as
   the host (the host's current world), unlike ghosts where each is in their own
   level. The lobby go-plan carries the host's level; joiners boot into it.
   Leaving the party drops you back to your own solo adventure (ghost tier).
3. THE SIM. From there it is rollback: each member predicts remote inputs,
   snapshots per frame, and rolls back on a misprediction (ROLLBACK.md sections
   4-5). Shared outcomes (doors, stars, enemies, coins) are correct because every
   console runs the identical inputs through the identical code.
4. GHOSTS-VS-PARTY BOUNDARY. In the same level you may have BOTH: party members
   (rollback, solid, real shared world) AND non-party ghosts (broadcast,
   see-through, no shared state) passing through. The render path already has the
   two-mode gate; party bodies render solid and collide, ghosts render translucent
   and pass through (port_player_render_hidden + the adventure translucency/hold
   already distinguish per-slot). Party membership is the per-slot flag that
   selects which treatment.

## Open questions for the co-design with C (when rollback ships)

- Wide cost: party size cap where conservative re-sim still fits p95 (ROLLBACK.md
  flags the Render audit + tick-only re-sim as REQUIRED for many players). Start
  small (2-4), measure, raise.
- Distance hybrid (A's earlier proposal): if a party gets large, rollback the
  near/interacting members and ghost-broadcast the distant ones. The broadcast
  half already exists (comms_sync); the boundary is per-member.
- Area change inside the prediction window: ROLLBACK.md risk item, the snapshot
  ring must flush + stall on a level/area change; a party member warping mid-party
  is exactly this and needs a defined handoff.
- Progress ownership: a shared world's persistent save (stars banked to a file).
  Whose save is authoritative for the party, and how a member's solo progress
  reconciles when they leave. Design with C's rollback + the save layer.

## Status

- 2026-09-02: design drafted from the rollback spike + the shipped ghost/invite
  layer, pending C's rollback ship to co-design the seam. Party tier NOT built.
  Ghost tier (see, invite, prox voice off-by-default) IS built and at C's gate.
